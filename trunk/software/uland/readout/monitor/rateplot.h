#ifndef RATEPLOT_H
#define RATEPLOT_H
#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <QVector>
#include <QObject>
#include <QString>

class rateplot : public QwtPlot
{
    Q_OBJECT
public:
   rateplot(QWidget *p = 0);
   void PlotSetup(int max_samples = 255);
   void Clear();
   void setCustomTitle(const QString &);
   void add2plot(double x);
private:
    QwtPlotCurve* _plot_curve;
    QVector<double> _x;
    QVector<double> _y;
    int _max_samples;
};

#endif // RATEPLOT_H
