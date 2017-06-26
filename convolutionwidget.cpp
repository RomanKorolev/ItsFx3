#include <QPainter>
#include "convolutionwidget.h"

using namespace std;

ConvolutionWidget::ConvolutionWidget(QWidget *parent) : QWidget(parent)
{
    colors.resize(255);
    for ( size_t i = 0; i < colors.size(); i++ ) {
        colors[i] = QColor( i, i, i, 255 );
    }
    conv_paint.resize(1);
    conv_paint[0].resize(1);
}

void ConvolutionWidget::SetConvolution(ConvResult *convolution)
{
    lock_guard<mutex> lock(mtx);
    this->conv = convolution;
    float min = conv->min;
    float max = conv->max;

    min = max * 0.90;

    float len = max - min;
    float color_range = 250.0;
    float coef = color_range / len;

    conv_paint.resize( conv->data.size() );
    for ( size_t a = 0; a < conv->data.size(); a++ ) {
        std::vector<float>& raw = conv->data[a];
        conv_paint[a].resize( raw.size() );
        for( size_t p = 0; p < raw.size(); p++ ) {
            int color_idx = (int)(  (raw[p] - min) * coef  );
            conv_paint[a][p] = color_idx >= 0 ? colors[ color_idx ] : colors[ 0 ];
        }
    }

}

void ConvolutionWidget::paintEvent(QPaintEvent* /*event*/)
{
    lock_guard<mutex> lock(mtx);

    QPainter painter( this );
    float W = width();
    float W2 = W/2.0f;
    float H = height();
    float H2 = H/2.0f;

    painter.setPen( QPen( Qt::gray, 2, Qt::SolidLine) );
    painter.drawRect( 0, 0, W, H );

    painter.setPen( QPen( Qt::green, 1, Qt::DotLine) );
    painter.drawLine( W2, 0, W2, H );

    painter.drawLine( 0, H2, W, H2 );

    int Ycnt = conv_paint.size();
    int Xcnt = conv_paint[0].size();

    float Xstep = W / (float)Xcnt;
    float Ystep = H / (float)Ycnt;

    //fprintf(stderr, "%d %d\n", Xcnt, Ycnt );

    for ( size_t a = 0; a < Ycnt; a++ ) {

        std::vector<QColor>& raw = conv_paint[a];
        int Y0 = Ystep * a;
        int Y1 = Ystep * (a+1);

        for( size_t p = 0; p < Xcnt; p++ ) {
            QColor& color = raw[p];
            painter.setPen( QPen( color, 1, Qt::SolidLine) );
            painter.setBrush( QBrush( color ) );

            painter.drawRect( Xstep*p, Y0, Xstep*(p+1), Y1 );
        }
    }

//    QPoint center( W2, H2 );
//    QColor color( Qt::red );
//    painter.setPen( QPen( color, 4, Qt::SolidLine) );
//    painter.setBrush( QBrush( color ) );
//    painter.drawEllipse( center, 4, 4 );

//    float perc = 0.05;
//    int rad = H*perc;
//    color = QColor(128, 128, 255, 32);
//    painter.setPen( QPen( color, 1, Qt::SolidLine) );
//    painter.setBrush( QBrush(color) );
//    painter.drawEllipse( center, rad, rad );
}