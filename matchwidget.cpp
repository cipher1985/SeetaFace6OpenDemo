#include "matchwidget.h"
#include "ui_matchwidget.h"

#include <QMessageBox>
#include <QFileDialog>

MatchWidget::MatchWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MatchWidget)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::Tool);

    m_sf6Match = new SeetaFace6(this);
    m_sf6Match->initFaceDetector();
    m_sf6Match->initFaceLandmarker();

    on_comboBox_match_mode_currentIndexChanged(
        ui->comboBox_match_mode->currentIndex());
    ui->label_show->installEventFilter(this);
}

MatchWidget::~MatchWidget()
{
    delete ui;
}

void MatchWidget::setTestImage(QImage img)
{
    if(!m_mutex.tryLock())
        return;
    m_testImg = img.copy();
    m_mutex.unlock();
}

QVector<float> MatchWidget::detectFeature(QImage img)
{
    QVector<float> feature;
    SeetaImageData simg = SeetaFace6::createFromImage(img);
    //检测人脸位置
    SeetaFaceInfo info = m_sf6Match->detectMaxFace(simg);
    if(info.score == 0) {
        SeetaFace6::releaseSeetaImage(simg);
        return feature;
    }
    //检测关键点
    std::vector<SeetaPointF> points = m_sf6Match->markFace(simg, info.pos);
    if(points.size() == 0) {
        SeetaFace6::releaseSeetaImage(simg);
        return feature;
    }
    //检测特征点
    feature = m_sf6Match->detectFaceFeature(simg, points);
    SeetaFace6::releaseSeetaImage(simg);
    return feature;
}

void MatchWidget::updateShowImage()
{
    int currentRow = ui->listWidget_face->currentRow();
    if(currentRow >= 0) {
        QImage img = m_regList.at(currentRow).image;
        ui->label_show->setPixmap(QPixmap::fromImage(
            img.scaled(ui->label_show->size(), Qt::KeepAspectRatio)));
    }
}

void MatchWidget::on_pushButton_clear_clicked()
{
    if(m_regList.isEmpty())
        return;
    int ret = QMessageBox::information(this, u8"提示",
        u8"确定清空所有注册信息吗?", u8"取消", u8"确定");
    if(ret != 1)
        return;
    m_regList.clear();
    ui->listWidget_face->clear();
}

void MatchWidget::on_pushButton_delete_clicked()
{
    int index = ui->listWidget_face->currentRow();
    if(index < 0)
        return;
    m_regList.removeAt(index);
    ui->listWidget_face->removeItemWidget(
        ui->listWidget_face->takeItem(index));
}

void MatchWidget::on_pushButton_register_clicked()
{
    QFileDialog dlg;
    dlg.setNameFilter(u8"图片文件 (*.bmp *.png *.jpg *.jpeg)");
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setFileMode(QFileDialog::ExistingFiles);
    dlg.setWindowTitle(u8"选择人脸图像文件");
    int ret = dlg.exec();
    if (ret != QDialog::Accepted)
        return;
    QStringList files = dlg.selectedFiles();
    if(files.count() == 0)
        return;
    //注册人脸
    QMutexLocker locker(&m_mutex);
    for(auto& i : files) {
        RegData data;
        data.fileName = QFileInfo(i).absoluteFilePath();
        data.image = QImage(data.fileName);
        if(data.image.isNull())
            continue;
        data.feature = detectFeature(data.image);
        if(data.feature.isEmpty())
            continue;
        m_regList.append(data);
        ui->listWidget_face->addItem(data.fileName);
    }
}

void MatchWidget::on_listWidget_face_currentRowChanged(int currentRow)
{
    if(currentRow < 0) {
        ui->label_show->setPixmap(QPixmap());
        ui->label_show->setText(u8"图像显示区域");
        return;
    }
    updateShowImage();
}


void MatchWidget::on_comboBox_match_mode_currentIndexChanged(int index)
{
    SeetaFace6::FaceRecognizerMode mode = (SeetaFace6::FaceRecognizerMode)index;
    QMutexLocker locker(&m_mutex);
    m_sf6Match->initFaceRecognizer(mode);
    //重新检测特征点
    for(auto& i : m_regList)
        i.feature = detectFeature(i.image);
}

void MatchWidget::on_pushButton_match_clicked()
{
    QMutexLocker locker(&m_mutex);
    if(m_regList.count() == 0) {
        ui->label_show->setPixmap(QPixmap());
        ui->label_show->setText(u8"未注册图像");
        return;
    }
    if(m_testImg.isNull()) {
        ui->label_show->setPixmap(QPixmap());
        ui->label_show->setText(u8"无采集图像");
        return;
    }
    QVector<float> feature = detectFeature(m_testImg);
    if(feature.count() == 0) {
        ui->label_show->setPixmap(QPixmap());
        ui->label_show->setText(u8"未检测到有效人脸信息");
        return;
    }
    int index = -1;
    float maxScore = 0;
    for(int i = 0; i < m_regList.count(); ++i) {
        float score = m_sf6Match->matchFeature(
            feature, m_regList.at(i).feature);
        if(score > maxScore) {
            maxScore = score;
            index = i;
        }
    }
    double thr = ui->doubleSpinBox_thr->value();
    if(index < 0)  {
        ui->label_show->setPixmap(QPixmap());
        ui->label_show->setText(u8"未检测到匹配人脸");
        return;
    }
    if(maxScore < thr)  {
        ui->label_show->setPixmap(QPixmap());
        ui->label_show->setText(
            QString(u8"未检测到匹配人脸[%1]").arg(
            QString::number(maxScore, 'f', 2)));
        return;
    }
    //找到匹配人脸
    ui->listWidget_face->setCurrentRow(index);
    QMessageBox::information(this, u8"提示",
        QString(u8"识别到匹配的人脸[%1]").arg(
            QString::number(maxScore, 'f', 2)), u8"确定");
}

bool MatchWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Resize)
        updateShowImage();
    return QWidget::eventFilter(watched, event);
}

