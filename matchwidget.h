#ifndef MATCHWIDGET_H
#define MATCHWIDGET_H

#include <QWidget>
#include <QMutex>
#include <seetaface6open.h>

namespace Ui {
class MatchWidget;
}

class MatchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MatchWidget(QWidget *parent = nullptr);
    ~MatchWidget();
    void setTestImage(QImage img);
    QVector<float> detectFeature(QImage img);
    void updateShowImage();
private slots:
    void on_pushButton_clear_clicked();
    void on_pushButton_delete_clicked();
    void on_pushButton_register_clicked();
    void on_listWidget_face_currentRowChanged(int currentRow);
    void on_comboBox_match_mode_currentIndexChanged(int index);
    void on_pushButton_match_clicked();
private:
    virtual bool eventFilter(QObject *watched, QEvent *event);
    Ui::MatchWidget *ui;
    SeetaFace6* m_sf6Match{};
    QImage m_testImg;
    QRecursiveMutex m_mutex;
    struct RegData
    {
        QString fileName;
        QImage image;
        QVector<float> feature;
    };
    QList<RegData> m_regList;
};

#endif // MATCHWIDGET_H
