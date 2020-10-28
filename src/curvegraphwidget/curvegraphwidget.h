#ifndef CURVEGRAPHWIDGET_H
#define CURVEGRAPHWIDGET_H

#include <QFrame>

using Points = QVector<double>;
class QCPRange;
class CurveGraphWidgetPrivate;
class CurveGraphWidget : public QFrame
{
    Q_OBJECT
public:
    explicit CurveGraphWidget(QWidget *parent = nullptr);
    ~CurveGraphWidget();

    void addCurve(int type, const QString& name = QString());

    void addCurve(const QVector<int> &types, const QStringList& names);

    int curveCount()const;

    //添加数据，不会重绘，不会调整坐标轴，
    //需要重绘手动调用redrawCurve
    //需要调整坐标轴调用adjustAxesSize
    void addData(int type, const Points& x, const Points& y);

    void addData(int type, double x, double y);

    void setCurveData(int type, const Points& x, const Points& y);

    void setCurveData(const QMap<int, QPair<Points, Points>>& data);

    void setCurvePen(int type, const QPen& pen);

    void setCurveBrush(int type, const QBrush& brush);

    void adjustAxesSize();

    void clear(int type);

    void clearAll();

    //重绘
    void redrawCurve();

    void setAxisText(bool isX, const QString& text);

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent* e)override;
    virtual void enterEvent(QEvent* e)override;
    virtual void focusOutEvent(QFocusEvent* e)override;

signals:


public slots:
    void onLegendClicked(int id);
    void onCurveSelectionChanged();

protected slots:
    void setCurveVisible(int id, bool visible);

private:
    CurveGraphWidgetPrivate* d_ptr;
    Q_DECLARE_PRIVATE(CurveGraphWidget)
};

#endif // CURVEGRAPHWIDGET_H
