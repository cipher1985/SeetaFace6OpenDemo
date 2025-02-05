#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    m_matchWidget = new MatchWidget(this);

    m_sf6 = new SeetaFace6(this);
    m_sf6->initFaceDetector();
    m_sf6->initAgePredictor();
    m_sf6->initEyeStateDetector();
    m_sf6->initGenderPredictor();
    m_sf6->initMaskDetector();
    m_sf6->initBlurDetector();
    m_sf6->initEstimateAngle();
    m_sf6->initFaceLandmarker();

    m_cam = new QtCameraCapture(this);
    connect(m_cam, &QtCameraCapture::sigCaptureFrame,
        this, [this](QImage image){
        image = image.mirrored(m_reverseX, !m_reverseY);
        emit sigDetect(image);
    });

    connect(this, &MainWidget::sigDetect, this,
        &MainWidget::onDetect, Qt::QueuedConnection);
    on_pushButton_refresh_clicked();
    ui->plainTextEdit_state->clear();
    ui->label_show_image->installEventFilter(this);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::onDetect(QImage img)
{
    m_matchWidget->setTestImage(img);
    detectFace(img);
    showDetect();
}

void MainWidget::detectFace(QImage img)
{
    bool showAge = (ui->checkBox_age->checkState() == Qt::Checked);
    bool showGender = (ui->checkBox_gender->checkState() == Qt::Checked);
    bool showEye = (ui->checkBox_eye->checkState() == Qt::Checked);
    int keyMode = ui->comboBox_key_mode->currentIndex();
    bool showMask = (ui->checkBox_mask->checkState() == Qt::Checked);
    bool showBlur = (ui->checkBox_blur->checkState() == Qt::Checked);
    bool showAngle = (ui->checkBox_angle->checkState() == Qt::Checked);
    int liveMode = ui->comboBox_live_mode->currentIndex();

    QMutexLocker locker(&m_mutex);
    //检测人脸
    m_showImage = img.copy();
    SeetaImageData simg = SeetaFace6::createFromImage(m_showImage);
    SeetaFaceInfo info = m_sf6->detectMaxFace(simg);
    if(info.score == 0) {
        m_hasFace = false;
        SeetaFace6::releaseSeetaImage(simg);
        return;
    }
    //绘制人脸位置
    QPainter painter(&m_showImage);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QBrush(Qt::red), 3));
    QRect rt(info.pos.x, info.pos.y, info.pos.width, info.pos.height);
    painter.drawRect(rt);
    //检测关键点
    std::vector<SeetaPointF> points = m_sf6->markFace(simg, info.pos);
    if(keyMode > 0) {
        //显示关键点
        painter.setBrush(Qt::green);
        painter.setPen(Qt::NoPen);
        for(size_t i = 0; i < points.size(); ++i) {
            QPoint pt(points[i].x, points[i].y);
            painter.drawEllipse(pt, 1, 1);
            //painter.drawText(pt, QString::number(i + 1));
        }
    }
    painter.end();
    QString state;
    //检测年龄
    if(showAge) {
        int age = m_sf6->predictAge(simg, points);
        state.append(QString(u8"年龄：%1 岁\n").arg(age));
    }
    //检测年龄
    if(showGender) {
        SeetaFace6::Gender gender = m_sf6->predictGender(simg, points);
        state.append(QString(u8"性别：%1\n").arg(getGender(gender)));
    }
    //检测眼睛
    if(showEye) {
        SeetaFace6::SEyeState eyeState = m_sf6->detectEyeState(simg, points);
        state.append(QString(u8"左眼：%1\n").arg(getEyeState(eyeState.left)));
        state.append(QString(u8"右眼：%1\n").arg(getEyeState(eyeState.right)));
    }
    //检测口罩
    if(showMask) {
        bool hasMask = m_sf6->detectMask(simg, info.pos);
        state.append(QString(u8"口罩：%1\n").arg(hasMask ? u8"有" : u8"无"));
    }
    //检测模糊
    if(showBlur) {
        bool isBlur = m_sf6->detectIsBlur(simg, points);
        state.append(QString(u8"清晰：%1\n").arg(isBlur ? u8"否" : u8"是"));
    }
    //头像角度
    if(showAngle) {
        QVector3D pos = m_sf6->estimateAngle(simg, info.pos);
        state.append(QString(u8"俯仰角X：%1°\n翻滚角Y：%2°\n偏航角Z：%3°\n").arg(
            QString::number(pos.x(), 'f', 2),
            QString::number(pos.y(), 'f', 2),
            QString::number(pos.z(), 'f', 2)));
    }
    //活体检测
    if(liveMode > 0) {
        SeetaFace6::FakeState fakeState;
        if(m_cam->isOpen()) {
            fakeState = m_sf6->predictFakeVideo(simg, info.pos, points, !m_hasFace);
        } else {
            fakeState = m_sf6->predictFake(simg, info.pos, points);
        }
        state.append(QString(u8"活体状态：%1\n").arg(getFakeState(fakeState)));
    }

    ui->plainTextEdit_state->setPlainText(state);
    SeetaFace6::releaseSeetaImage(simg);
    m_hasFace = true;
}

void MainWidget::showDetect()
{
    if(m_showImage.isNull())
        return;
    ui->label_show_image->setPixmap(QPixmap::fromImage(
        m_showImage.scaled(ui->label_show_image->size(), Qt::KeepAspectRatio)));
}

QString MainWidget::getGender(SeetaFace6::Gender gender)
{
    QString strGender;
    switch (gender) {
    case SeetaFace6::GD_MALE:
        strGender = u8"男性";
        break;
    case SeetaFace6::GD_FEMALE:
        strGender = u8"女性";
        break;
    default:
        strGender = u8"未知";
        break;
    }
    return strGender;
}

QString MainWidget::getEyeState(SeetaFace6::EyeState state)
{
    QString strState;
    switch (state) {
    case SeetaFace6::SES_CLOSE:
        strState = u8"闭合";
        break;
    case SeetaFace6::SES_OPEN:
        strState = u8"睁开";
        break;
    case SeetaFace6::SES_NONE:
        strState = u8"没有";
        break;
    default:
        strState = u8"未知";
        break;
    }
    return strState;
}

QString MainWidget::getFakeState(SeetaFace6::FakeState state)
{
    QString strState;
    switch (state) {
    case SeetaFace6::FS_REAL:
        strState = u8"真脸";
        break;
    case SeetaFace6::FS_FAKE:
        strState = u8"假脸";
        break;
    case SeetaFace6::FS_FAILED:
        strState = u8"失败";
        break;
    case SeetaFace6::FS_DETECTING:
        strState = u8"检测";
        break;
    default:
        strState = u8"未知";
        break;
    }
    return strState;
}

void MainWidget::on_pushButton_refresh_clicked()
{
    ui->comboBox_cam->clear();
    QList<QtCameraCapture::Info> infos = QtCameraCapture::infoList();
    for(auto& i : infos) {
        ui->comboBox_cam->addItem(i.name);
    }
}

void MainWidget::on_checkBox_reverse_x_stateChanged(int state)
{
    m_reverseX = (state == Qt::Checked);
}

void MainWidget::on_checkBox_reverse_y_stateChanged(int state)
{
    m_reverseY = (state == Qt::Checked);
}

void MainWidget::on_pushButton_capture_clicked()
{
    int index = ui->comboBox_cam->currentIndex();
    if(!m_cam->open(index))
        QMessageBox::information(this, u8"提示", u8"无法打开摄像头", u8"确定");
}

void MainWidget::on_pushButton_image_clicked()
{
    QFileDialog dlg;
    dlg.setNameFilter(u8"图片文件 (*.bmp *.png *.jpg *.jpeg)");
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setWindowTitle(u8"选择检测图像文件");
    int ret = dlg.exec();
    if (ret != QDialog::Accepted)
        return;
    QStringList files = dlg.selectedFiles();
    if(files.count() == 0)
        return;
    QString fileName = files.at(0);
    QImage img(fileName);
    if(img.isNull())
        return;
    m_cam->close();
    ui->lineEdit_image->setText(fileName);
    emit sigDetect(img);
}

void MainWidget::on_pushButton_detect_clicked()
{
    QString fileName = ui->lineEdit_image->text();
    QImage img(fileName);
    if(img.isNull())
        return;
    m_cam->close();
    emit sigDetect(img);
}


void MainWidget::on_comboBox_key_mode_currentIndexChanged(int index)
{
    if(index == 0)
        return;
    QMutexLocker locker(&m_mutex);
    SeetaFace6::FaceLandmarkerMode mode = (SeetaFace6::FaceLandmarkerMode)(index - 1);
    m_sf6->initFaceLandmarker(mode);
    bool isMark68 = (mode == SeetaFace6::FLM_Mark68);
    ui->checkBox_age->setEnabled(!isMark68);
    ui->checkBox_gender->setEnabled(!isMark68);
    ui->checkBox_eye->setEnabled(!isMark68);
    ui->checkBox_blur->setEnabled(isMark68);
    if(isMark68) {
        ui->checkBox_age->setCheckState(Qt::Unchecked);
        ui->checkBox_gender->setCheckState(Qt::Unchecked);
        ui->checkBox_eye->setCheckState(Qt::Unchecked);
    } else {
        ui->checkBox_blur->setCheckState(Qt::Unchecked);
    }
}

void MainWidget::on_comboBox_live_mode_currentIndexChanged(int index)
{
    if(index == 0)
        return;
    QMutexLocker locker(&m_mutex);
    SeetaFace6::FakeFaceMode mode = (SeetaFace6::FakeFaceMode)(index - 1);
    m_sf6->initFakeFaceDetector(mode);
    m_sf6->setFakePredictVideoCount(10);
}

void MainWidget::on_pushButton_match_clicked()
{
    m_matchWidget->show();
    m_matchWidget->move(this->x() + this->width() + 1, this->y());
}

bool MainWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Resize)
        showDetect();
    return QWidget::eventFilter(watched, event);
}

