#include "rateplot.h"
#include <QDebug>
#include "qwt_scale_div.h"
rateplot::rateplot(QWidget *p)
    : QwtPlot(p)
{
    _plot_curve = new QwtPlotCurve( "name" );
    _plot_curve->attach( this );
    enableAxis(this->xBottom, false);
    enableAxis(this->yLeft, false);
    enableAxis(this->yRight, true);
    setAxisScale(yRight, 0.0, 90, 30);
    setAxisScale(yLeft, 0.0, 90, 30);
    setAxisScale(xTop, 0.0, 256, 10);
    setAxisScale(xBottom, 0.0, 256, 10);
    QwtPlot::setFrameStyle( QFrame::NoFrame );
    QwtPlot::setLineWidth( 0 );
    QwtPlot::setCanvasLineWidth( 2 );
}

void rateplot::PlotSetup(int max_samples)
{
    _max_samples = max_samples;
    for(int i = 0; i< max_samples; i++)
    {
        _x.push_back((double)i);
    }
    _y.reserve(max_samples);
}

void rateplot::setCustomTitle(const QString &t)
{
    QwtText title(t);
    title.setFont(QFont("DejaVu Sans", 9));
    setTitle(title);
}

void rateplot::Clear()
{
    _x.remove(0, _x.size());
    _plot_curve->setRawData(_x.constData(), _y.constData(), 0);
    replot();
}

void rateplot::add2plot(double x)
{
    if(_y.size() >= _max_samples)
    {
        _y.pop_front();
    }
    _y.push_back(x);
    _plot_curve->setRawData(_x.constData(), _y.constData(), _y.size());
    replot();
}
