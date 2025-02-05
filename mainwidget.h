#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <seetaface6open.h>
#include "qtcameracapture.h"

#include "matchwidget.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    void detectFace(QImage img);
    void showDetect();
    QString getGender(SeetaFace6::Gender gender);
    QString getEyeState(SeetaFace6::EyeState state);
    QString getFakeState(SeetaFace6::FakeState state);
Q_SIGNALS:
    void sigDetect(QImage img);
private slots:
    void onDetect(QImage img);
    void on_pushButton_refresh_clicked();
    void on_checkBox_reverse_x_stateChanged(int state);
    void on_checkBox_reverse_y_stateChanged(int state);
    void on_pushButton_capture_clicked();
    void on_pushButton_image_clicked();
    void on_pushButton_detect_clicked();
    void on_comboBox_key_mode_currentIndexChanged(int index);
    void on_comboBox_live_mode_currentIndexChanged(int index);
    void on_pushButton_match_clicked();
private:
    virtual bool eventFilter(QObject *watched, QEvent *event);
    MatchWidget* m_matchWidget{};
    Ui::MainWidget *ui;
    SeetaFace6* m_sf6{};
    QtCameraCapture* m_cam{};
    std::atomic_bool m_reverseX{false};
    std::atomic_bool m_reverseY{false};
    QImage m_showImage;
    bool m_hasFace = false;
    QRecursiveMutex m_mutex;
};
#endif // MAINWIDGET_H
