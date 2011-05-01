#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QtGui>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QToolButton;
QT_END_NAMESPACE

namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void chooseSource();

    void on_tlbSrcimg_clicked();

    void on_dbsScaled_valueChanged(double arg1);
    void lblDstimgContextMenu(const QPoint& pos);

    void on_pushButton_clicked();

private:
    Ui::Widget *ui;

    QImage srcImage; //input image
    QImage *dstImage; //result image after filter
    uchar *srcimgdata; //src Image ARGB data
    uchar *dstimgdata; //src Image ARGB data

    void chooseImage(const QString &title, QImage *image, QToolButton *button);
    void loadImage(const QString &fileName, QImage *image, QToolButton *button);
    int cpu_means(uchar* inbuf, uchar* outbuf, int n);

    int imgheight;
    int imgwidth;

};

#endif // WIDGET_H
