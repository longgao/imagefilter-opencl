#include "widget.h"
#include "ui_widget.h"

static const QSize resultSize(400, 400);

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    srcimgdata = 0;
    dstimgdata = 0;
    dstImage = 0;

    //intitialize srcimg and dstimg
    ui->tlbSrcimg->setIconSize(resultSize);
    loadImage("lenna.jpg", &srcImage, ui->tlbSrcimg);

    //define popup menu in dstimage area
    ui->lblDstimg->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->lblDstimg, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(lblDstimgContextMenu(const QPoint&)));

}

Widget::~Widget()
{
    if (dstImage)
        delete dstImage;

    if (srcimgdata)
        delete srcimgdata;

    if (dstimgdata)
        delete dstimgdata;

    delete ui;
}

void Widget::lblDstimgContextMenu(const QPoint& pos) // this is a slot
{
    // for most widgets
    QPoint globalPos = ui->lblDstimg->mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    QMenu myMenu;
    myMenu.addAction("Save image");
    // ...

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        // save image to file
        QString fileName = QFileDialog::getSaveFileName(this,tr("Save File"),
                                                        "./",tr("Images (*.png *.bmp *.jpg)"));
        if (!fileName.isEmpty()&&dstImage)
            dstImage->save(fileName);
    }

}

void Widget::chooseImage(const QString &title, QImage *image,
                         QToolButton *button)
{
    QString fileName = QFileDialog::getOpenFileName(this, title);
    if (!fileName.isEmpty())
        loadImage(fileName, image, button);
}

void Widget::loadImage(const QString &fileName, QImage *image,
                       QToolButton *button)
{
    image->load(fileName);

    imgwidth = image->width();
    imgheight = image->height();

    //ARGB format, fixedImage is only used to display in screen.
    QImage fixedImage(resultSize, QImage::Format_ARGB32);
    QPainter painter(&fixedImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(fixedImage.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    //scale image
    painter.scale(ui->dbsScaled->value(), ui->dbsScaled->value());
    painter.drawImage(QPoint(0,0), *image);

    painter.end();
    button->setIcon(QPixmap::fromImage(fixedImage));

    //*image = fixedImage;

    //show image imgwidth/height in label
    ui->lblImgheightvalue->setText(QString::number(imgheight));
    ui->lblImgwidthvalue->setText(QString::number(imgwidth));

    if(imgheight>0&&imgwidth>0)
    {
        if (dstImage)
        {
            delete dstImage;
            dstImage = 0;
        }
        dstImage = new QImage(imgwidth,imgheight,QImage::Format_ARGB32);
        dstImage->fill(qRgba(0, 0, 0, 0));
        //ui->lblDstimg->setPixmap(QPixmap::fromImage(*dstImage));

        if(srcimgdata)
        {
            delete srcimgdata;
            srcimgdata =0;
        }

        if(dstimgdata)
        {
            delete dstimgdata;
            dstimgdata =0;
        }
        srcimgdata = new uchar[imgwidth*imgheight*4];
        dstimgdata = new uchar[imgwidth*imgheight*4];

        int i, j;
        for(i = 0; i < imgheight; i++)
        {
            for(j = 0; j < imgwidth; j++)
            {
                QRgb color = srcImage.pixel(j, i);
                srcimgdata[i*imgwidth*4+j*4] = qAlpha(color);
                // qDebug()<<srcimgdata[i*imgimgwidth*4+j]<<" "<<i<<" "<<j;
                srcimgdata[i*imgwidth*4+j*4+1] = qRed(color);
                srcimgdata[i*imgwidth*4+j*4+2] = qGreen(color);
                srcimgdata[i*imgwidth*4+j*4+3] = qBlue(color);

            }

        }

    }

}


void Widget::chooseSource()
{
    chooseImage(tr("Choose Source Image"), &srcImage, ui->tlbSrcimg);
}

void Widget::on_tlbSrcimg_clicked()
{
    chooseSource();
}

void Widget::on_dbsScaled_valueChanged(double arg1)
{
    QImage fixedImage(resultSize, QImage::Format_ARGB32);
    QImage fixedImage1(resultSize, QImage::Format_ARGB32);

    QPainter painter(&fixedImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(fixedImage.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    //scale image
    painter.scale(ui->dbsScaled->value(), ui->dbsScaled->value());
    painter.drawImage(QPoint(0,0), srcImage);

    painter.end();

    //dst image
    QPainter painter1(&fixedImage1);
    painter1.setCompositionMode(QPainter::CompositionMode_Source);
    painter1.fillRect(fixedImage1.rect(), Qt::transparent);
    painter1.setCompositionMode(QPainter::CompositionMode_SourceOver);

    //scale image
    painter1.scale(ui->dbsScaled->value(), ui->dbsScaled->value());
    painter1.drawImage(QPoint(0,0), *dstImage);

    painter1.end();
    ui->tlbSrcimg->setIcon(QPixmap::fromImage(fixedImage));
    ui->lblDstimg->setPixmap(QPixmap::fromImage(fixedImage1));
}

int Widget::cpu_means(uchar* inbuf, uchar* outbuf, int n)
{
    int i, j, k, t1, t2;

    k = (n-1)/2;
    int k2 = n*n;

    for(j = 0; j < imgheight; j++)
    {
        for(i = 0; i < imgwidth; i++)
        {
            if(i < k || j < k || i > imgwidth - k -1 || j > imgheight - k -1)
            {
                outbuf[i*4 + j*imgwidth*4] = inbuf[i*4 + j*imgwidth*4];
                outbuf[i*4 + j*imgwidth*4 + 1] = inbuf[i*4 + j*imgwidth*4 + 1];
                outbuf[i*4 + j*imgwidth*4 + 2] = inbuf[i*4 + j*imgwidth*4 + 2];
                outbuf[i*4 + j*imgwidth*4 + 3] = inbuf[i*4 + j*imgwidth*4 + 3];
                continue;
            }
            int b, g, r,a;
            b = g = r = a = 0;
            for(t1 = j - k; t1 <= j + k; t1++)
            {
                for(t2 = i - k; t2 <= i + k; t2++)
                {
                    b += inbuf[t2*4 + t1*imgwidth*4];
                    g += inbuf[t2*4 + t1*imgwidth*4 + 1];
                    r += inbuf[t2*4 + t1*imgwidth*4 + 2];
                    a += inbuf[t2*4 + t1*imgwidth*4 + 3];

                }
            }

            b = b/k2;
            g = g/k2;
            r = r/k2;
            a = a/k2;

            outbuf[i*4 + j*imgwidth*4] = (unsigned char)b;
            outbuf[i*4 + j*imgwidth*4 + 1] = (unsigned char)g;
            outbuf[i*4 + j*imgwidth*4 + 2] = (unsigned char)r;
            outbuf[i*4 + j*imgwidth*4 + 3] = (unsigned char)a;
        }
    }

    return 0;
}

void Widget::on_pushButton_clicked()
{
    cpu_means(srcimgdata, dstimgdata, 3);
    int i, j;
    qDebug()<<imgwidth<<" "<<imgheight;
    for(i = 0; i < imgheight; i++)
    {
        for(j = 0; j < imgwidth; j++)
        {
            QRgb color = qRgba(dstimgdata[i*imgwidth*4+j*4+1],dstimgdata[i*imgwidth*4+j*4+2],dstimgdata[i*imgwidth*4+j*4+3],dstimgdata[i*imgwidth*4+j*4]);

//            if(i==0&&j==1)
//                qDebug()<<srcimgdata[0]<<" : "<<srcimgdata[1]<<" : "<<srcimgdata[2]<<":"<<srcimgdata[3];
            dstImage->setPixel(j,i,color);
        }

    }

    //dst image
    QImage fixedImage1(resultSize, QImage::Format_ARGB32);
    QPainter painter1(&fixedImage1);
    painter1.setCompositionMode(QPainter::CompositionMode_Source);
    painter1.fillRect(fixedImage1.rect(), Qt::transparent);
    painter1.setCompositionMode(QPainter::CompositionMode_SourceOver);

    //scale image
    painter1.scale(ui->dbsScaled->value(), ui->dbsScaled->value());
    painter1.drawImage(QPoint(0,0), *dstImage);

    painter1.end();
    ui->lblDstimg->setPixmap(QPixmap::fromImage(fixedImage1));
}
