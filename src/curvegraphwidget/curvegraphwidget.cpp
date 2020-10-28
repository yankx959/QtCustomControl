#include "curvegraphwidget.h"
#include "qcustomplot.h"
#include <QPushButton>
#include <QButtonGroup>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMap>
#include <QDebug>
#include <functional>
#include <QShortcut>

struct stGraphData{
    QPen pen;
    QBrush brush;
    QCPGraph* graph = nullptr;
    QCPItemText* yItemItem = nullptr;
    friend bool operator==(const stGraphData& left, const stGraphData& right){
        return left.graph == right.graph;
    }
};

static const int TickUpper = 5;

class CurveGraphWidgetPrivate{
public:
    CurveGraphWidgetPrivate(CurveGraphWidget* q)
        : q_ptr(q)
        , plot(nullptr)
        , legends(nullptr)
        , vboxLegend(nullptr)
        , frameLegendContainer(nullptr)
        , xItemText(nullptr)
        , tracker(nullptr)
    {}

private:
    void init();

    void updateGraphBrush(QCPGraph* graph, const QBrush &bru = QBrush());

    void refreshSelection();

    bool dataIsEmpty()const;

    void updateTicker();

    void setTrackerVisible(bool visible);

    QCPItemText* createItemText();

    void handleMouseMove(QMouseEvent* me);

    void updateTracker(const QPoint& mousePos);

private:
    CurveGraphWidget* q_ptr;
    Q_DECLARE_PUBLIC(CurveGraphWidget)

private:
    QCustomPlot* plot;
    QButtonGroup* legends;
    QVBoxLayout* vboxLegend;
    QFrame* frameLegendContainer;
    QMap<int, stGraphData> curves;

    int lastCheckedID = -1;
    int lastBeforeToTopIndex = -1;

    QCPItemText* xItemText;
    QCPItemTracer* tracker;

    QCPItemLine* xLine = 0;
    QCPItemLine* yLine = 0;
};

CurveGraphWidget::CurveGraphWidget(QWidget *parent)
    : QFrame(parent)
    , d_ptr(new CurveGraphWidgetPrivate(this))
{
    d_func()->init();
}

CurveGraphWidget::~CurveGraphWidget()
{

}

void CurveGraphWidget::addCurve(int type, const QString &name)
{
    Q_D(CurveGraphWidget);
    if(d->curves.find(type) == d->curves.end()){
        d->plot->addLayer(name);
        QCPLayer* layer = d->plot->layer(name);
        layer->setMode(QCPLayer::lmBuffered);
        d->plot->setCurrentLayer(layer);
        QCPGraph* graph = d->plot->addGraph();
        graph->setName(name);
        d->curves[type].graph = graph;

        d->curves[type].yItemItem = d->createItemText();
        d->curves[type].yItemItem->setPen(QPen(Qt::black, 1));
        d->curves[type].yItemItem->setColor(Qt::white);

        QPushButton* btnLegend = new QPushButton(d->frameLegendContainer);
        btnLegend->setCheckable(true);
        btnLegend->setAutoExclusive(false);
        btnLegend->setObjectName("legend_" + name);
        btnLegend->setText(name);
        btnLegend->setMouseTracking(true);
        d->vboxLegend->addWidget(btnLegend);
        d->legends->addButton(btnLegend, type);
    }
}

void CurveGraphWidget::addCurve(const QVector<int> &types, const QStringList &names)
{
    Q_D(CurveGraphWidget);
    for(int i = 0; i < types.size() && i < names.size(); ++i){
        addCurve(types.at(i), names.at(i));
    }
}

int CurveGraphWidget::curveCount() const
{
    return d_func()->curves.size();
}

void CurveGraphWidget::addData(int type, const Points &x, const Points &y)
{
    if(x.size() != y.size()){
        return ;
    }
    Q_D(CurveGraphWidget);
    auto it = d->curves.find(type);
    if(it != d->curves.end()){
        if(x.size() == 1){
            addData(type, x.at(0), y.at(0));
        }
        else{
            it.value().graph->addData(x, y);
        }
    }
}

void CurveGraphWidget::addData(int type, double x, double y)
{
    Q_D(CurveGraphWidget);
    auto it = d->curves.find(type);
    if(it != d->curves.end()){
        it.value().graph->addData(x, y);
    }
}

void CurveGraphWidget::setCurveData(int type, const Points &x, const Points &y)
{
    Q_D(CurveGraphWidget);
    auto it = d->curves.find(type);
    if(it == d->curves.end()){
        return ;
    }
    it.value().graph->setData(x, y);
}

void CurveGraphWidget::setCurvePen(int type, const QPen &pen)
{
    Q_D(CurveGraphWidget);
    auto it = d->curves.find(type);
    if(it != d->curves.end()){
        it.value().pen = pen;
        it.value().graph->setPen(pen);
        d->plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void CurveGraphWidget::setCurveBrush(int type, const QBrush &brush)
{
    Q_D(CurveGraphWidget);
    auto it = d->curves.find(type);
    if(it != d->curves.end()){
        it.value().brush = brush;
        it.value().graph->setBrush(brush);
        d->updateGraphBrush(it.value().graph);
        d->plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void CurveGraphWidget::adjustAxesSize()
{
    Q_D(CurveGraphWidget);
    if(!d->dataIsEmpty()){
        d->plot->xAxis->rescale(true);
        d->plot->yAxis->rescale(true);
        d->plot->replot();
    }
}

void CurveGraphWidget::clear(int type)
{
    Q_D(CurveGraphWidget);
    auto it = d->curves.find(type);
    if(it != d->curves.end()){
        it.value().graph->data().data()->clear();
        it.value().graph->layer()->replot();
    }
}

void CurveGraphWidget::clearAll()
{
    Q_D(CurveGraphWidget);
    for(auto it = d->curves.begin(); it != d->curves.end(); ++it){
        it.value().graph->data().data()->clear();
    }
    d->plot->replot();
}

void CurveGraphWidget::redrawCurve()
{
    d_func()->plot->replot();
}

void CurveGraphWidget::setAxisText(bool isX, const QString &text)
{
    Q_D(CurveGraphWidget);
    isX ? d->plot->xAxis->setLabel(text) : d->plot->yAxis->setLabel(text);
    d->plot->replot(QCustomPlot::rpQueuedReplot);
}

void CurveGraphWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if(event->oldSize().height() == event->size().height()){
        return ;
    }
    Q_D(CurveGraphWidget);
    for(auto graphData : d->curves){
        d->updateGraphBrush(graphData.graph);
    }
    d->plot->replot(QCustomPlot::rpQueuedReplot);
}

void CurveGraphWidget::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    Q_D(CurveGraphWidget);
    for(auto graphData : d->curves){
        d->updateGraphBrush(graphData.graph);
    }
    d->plot->replot(QCustomPlot::rpQueuedReplot);
}

void CurveGraphWidget::enterEvent(QEvent *e)
{
    QFrame::enterEvent(e);
    setFocus();
}

void CurveGraphWidget::focusOutEvent(QFocusEvent *e)
{
    QFrame::focusOutEvent(e);
}

void CurveGraphWidget::onLegendClicked(int id)
{
    Q_D(CurveGraphWidget);
    auto graphData = d->curves.value(id);
    auto graph = graphData.graph;
    if(!d->legends->button(id)->isChecked()){
        graph->setSelection(QCPDataSelection());
    }
    else{
        graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        for(auto temp : d->curves){
            if(temp.graph == graph){
                continue ;
            }
            d->legends->button(d->curves.key(temp))->setChecked(false);
            temp.graph->setSelection(QCPDataSelection());
        }
    }
    d->refreshSelection();
    d->plot->replot(QCustomPlot::rpQueuedReplot);
}

void CurveGraphWidget::onCurveSelectionChanged()
{
    Q_D(CurveGraphWidget);
    for(auto graphData : d->curves){
        if(graphData.graph->selected()){
            d->legends->button(d->curves.key(graphData))->setChecked(true);
        }
        else{
            d->legends->button(d->curves.key(graphData))->setChecked(false);
        }
    }
    d->refreshSelection();
}

void CurveGraphWidget::setCurveVisible(int id, bool visible)
{
    Q_D(CurveGraphWidget);
    auto it = d->curves.find(id);
    if(it != d->curves.end()){
        it.value().graph->setVisible(visible);
        d->plot->replot();
        d->legends->button(id)->setVisible(visible);
        if(!visible){
            d->legends->button(id)->setChecked(false);
            if(d->tracker->graph() == it.value().graph){
                d->setTrackerVisible(false);
            }
        }
        adjustAxesSize();
    }
}

void CurveGraphWidgetPrivate::init()
{
    Q_Q(CurveGraphWidget);

    QFrame* framePlotContainer = new QFrame(q);
    framePlotContainer->setObjectName("frame_plot_container");
    QVBoxLayout* vPlotContainer = new QVBoxLayout(framePlotContainer);
    vPlotContainer->setMargin(0);
    vPlotContainer->setSpacing(0);
    plot = new QCustomPlot(q);
    plot->legend->setVisible(false);
    plot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iRangeDrag);
    plot->xAxis->setVisible(true);
    plot->yAxis->setVisible(true);
    plot->xAxis->setLabel("迭代步数");
    //    plot->xAxis->setBasePen(QPen(Qt::black, 1));
    //    plot->yAxis->setBasePen(QPen(Qt::black, 1));
    plot->xAxis->setSubTicks(false);
    plot->xAxis->setTickLength(0);
    plot->yAxis->setSubTicks(false);
    plot->yAxis->setTickLength(0);
    //    plot->xAxis->ticker()->setTickCount(5);
    //    plot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);
    plot->xAxis->setNumberFormat("f");
    plot->xAxis->setNumberPrecision(0);
    //    plot->xAxis->grid()->setPen(QPen(Qt::gray, 1, Qt::DotLine));
    //    plot->yAxis->grid()->setPen(QPen(Qt::gray, 1, Qt::DotLine));
    //    plot->xAxis->grid()->setSubGridPen(QPen(Qt::gray, 1, Qt::DotLine));
    //    plot->yAxis->grid()->setSubGridPen(QPen(Qt::gray, 1, Qt::DotLine));
    plot->xAxis->grid()->setSubGridVisible(true);
    plot->yAxis->grid()->setSubGridVisible(true);
    plot->axisRect()->setMargins(QMargins(0, 0, 0 ,0));

    plot->selectionRect()->setPen(QPen(QColor(0, 0, 100), 1));
    plot->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));//设置选框的样式：半透明浅蓝
    plot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
    plot->setMouseTracking(true);

    xItemText = createItemText();

    tracker = new QCPItemTracer(plot);
    tracker->setInterpolating(false);
    tracker->setStyle(QCPItemTracer::tsPlus);//游标样式：十字星、圆圈、方框等

    xLine = new QCPItemLine(plot);
    xLine->start->setType(QCPItemPosition::ptAxisRectRatio);
    xLine->end->setType(QCPItemPosition::ptAxisRectRatio);
    yLine = new QCPItemLine(plot);
    xLine->setVisible(false);
    yLine->setVisible(false);
    yLine->start->setType(QCPItemPosition::ptAxisRectRatio);
    yLine->end->setType(QCPItemPosition::ptAxisRectRatio);

    //这里调用重绘是因为如果鼠标点了一下，但是没有移动，会造成界面存在一个鼠标点
    q->connect(plot->selectionRect(), &QCPSelectionRect::canceled, q, [this]{
        plot->replot();
    });

    vPlotContainer->addWidget(plot);

    QShortcut* shortCut = new QShortcut(QKeySequence("Ctrl+F"), q);
    shortCut->setContext(Qt::WidgetWithChildrenShortcut);
    q->connect(shortCut, &QShortcut::activated, q, [this]{
        if(!dataIsEmpty()){
            q_func()->adjustAxesSize();
        }
    });

    QShortcut* shortCutQ = new QShortcut(QKeySequence("Ctrl+Q"), q);
    shortCutQ->setContext(Qt::WidgetWithChildrenShortcut);
    q->connect(shortCutQ, &QShortcut::activated, q, [this]{
        if(auto btn = legends->checkedButton()){
            btn->click();
        }
    });

    q->connect(plot, &QCustomPlot::mouseRelease, q, [this]{
        q_func()->setFocus();
    });

    q->connect(plot, &QCustomPlot::afterMouseMove, q, [this](QMouseEvent* me){
        handleMouseMove(me);
    });

    q->connect(plot, &QCustomPlot::selectionChangedByUser, q, &CurveGraphWidget::onCurveSelectionChanged);

    frameLegendContainer = new QFrame(q);
    frameLegendContainer->setMouseTracking(true);
    frameLegendContainer->setObjectName("frame_legend_container");
    vboxLegend = new QVBoxLayout(frameLegendContainer);
    vboxLegend->setMargin(0);
    vboxLegend->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    legends = new QButtonGroup(q);
    legends->setExclusive(false);
    q->connect(legends, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), q, &CurveGraphWidget::onLegendClicked);

    QHBoxLayout* h = new QHBoxLayout(q);
    h->setSpacing(0);
    h->setContentsMargins(1, 0, 1, 1);
    h->addWidget(framePlotContainer, 1);
    //    h->addWidget(frameLegendContainer);

    QHBoxLayout* hp = new QHBoxLayout(plot);
    hp->setMargin(0);
    hp->addStretch();
    hp->addWidget(frameLegendContainer);

    setTrackerVisible(false);
}

void CurveGraphWidgetPrivate::updateGraphBrush(QCPGraph *graph, const QBrush& bru)
{
    QBrush brush = bru.style() == Qt::NoBrush ? graph->brush() : bru;
    if(brush.style() == Qt::NoBrush){
        return ;
    }
    if(brush.gradient()){
        QLinearGradient* linear = static_cast<QLinearGradient*>(const_cast<QGradient*>(brush.gradient()));
        if(linear){
            linear->setStart(QPoint(0, plot->yAxis->coordToPixel(plot->yAxis->range().upper)));
            linear->setFinalStop(QPoint(0, plot->yAxis->coordToPixel(plot->yAxis->range().lower)));
            brush = *linear;
        }
    }
    graph->setBrush(brush);
}

void CurveGraphWidgetPrivate::refreshSelection()
{
    if(lastCheckedID != -1){
        auto it = curves.find(lastCheckedID);
        if(it != curves.end()){
            auto graphData = it.value();
            graphData.graph->setPen(graphData.pen);
            //        updateGraphBrush(graphData.graph, graphData.brush);

            if(lastBeforeToTopIndex != -1){
                //恢复上次选中曲线
                plot->moveLayer(plot->layer(plot->layerCount() - 1), plot->layer(lastBeforeToTopIndex), QCustomPlot::limBelow);
            }
        }
        else{
            qDebug() << "not find" << lastCheckedID;
        }
    }
    lastCheckedID = legends->checkedId();
    if(!curves.contains(lastCheckedID)){
        setTrackerVisible(false);
        return ;
    }
    setTrackerVisible(true);

    auto graphData = curves.value(lastCheckedID);
    lastBeforeToTopIndex = graphData.graph->layer()->index();

    //选中曲线置顶
    plot->moveLayer(graphData.graph->layer(), plot->layer(plot->layerCount() - 1));

    QPen pen = graphData.pen;
    pen.setColor(pen.color());
    pen.setWidth(3);
    graphData.graph->selectionDecorator()->setPen(pen);
    QBrush brush = graphData.brush;
    if(const QLinearGradient* linear = static_cast<const QLinearGradient*>(brush.gradient())){
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(linear->stops().begin()->second);
    }
    auto color = brush.color();
    color.setAlpha(80);
    brush.setColor(color);
    //    graphData.graph->selectionDecorator()->setBrush(brush);
}

bool CurveGraphWidgetPrivate::dataIsEmpty() const
{
    bool empty = true;
    for(auto it = curves.begin(); it != curves.end(); ++it){
        if(it.value().graph->dataCount()){
            empty = false;
            break;
        }
    }
    return empty;
}

void CurveGraphWidgetPrivate::updateTicker()
{
    int rangeSize = plot->xAxis->range().size();
    if(rangeSize < TickUpper){
        plot->xAxis->ticker()->setTickCount(std::min(TickUpper, std::max(1, rangeSize)));
    }
    else{
        plot->xAxis->ticker()->setTickCount(std::min(TickUpper, std::max(TickUpper, rangeSize)));
    }
    plot->replot();
}

void CurveGraphWidgetPrivate::setTrackerVisible(bool visible)
{
    tracker->setVisible(visible);
    xItemText->setVisible(visible);
    xLine->setVisible(visible);
    yLine->setVisible(visible);
    for(auto graphData : curves){
        if(graphData.graph->selected()){
            graphData.yItemItem->setVisible(true);
            tracker->setGraph(graphData.graph);
        }
        else{
            graphData.yItemItem->setVisible(false);
        }
    }
    updateTracker(plot->mapFromGlobal(QCursor::pos()));
}

QCPItemText *CurveGraphWidgetPrivate::createItemText()
{
    QCPItemText* ret = new QCPItemText(plot);

    ret->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);//文字布局：顶、左对齐

    ret->position->setType(QCPItemPosition::ptAxisRectRatio);//位置类型（当前轴范围的比例为单位/实际坐标为单位）

    ret->setFont(QFont(q_func()->font())); //字体大小

    ret->setPadding(QMargins(2, 2, 2, 2));//文字距离边框几个像素
    ret->setVisible(false);
    return ret;
}

void CurveGraphWidgetPrivate::handleMouseMove(QMouseEvent *me)
{
    updateTracker(me->pos());
}

void CurveGraphWidgetPrivate::updateTracker(const QPoint &mousePos)
{
    if(tracker->visible())//游标使能
    {
        double x = plot->xAxis->pixelToCoord(mousePos.x());//鼠标点的像素坐标转plot坐标
        double y = plot->yAxis->pixelToCoord(mousePos.y());
        if(x < plot->xAxis->range().lower || x > plot->xAxis->range().upper
                || y < plot->yAxis->range().lower || y > plot->yAxis->range().upper){
            return ;
        }

        int y0 = plot->yAxis->coordToPixel(plot->yAxis->range().lower);
        int itemHeight = QFontMetrics(xItemText->font()).height() + xItemText->padding().top() * 2 + 2;
        y0 -= itemHeight;
        double xItemY = 1 - (plot->yAxis->pixelToCoord(y0) - plot->yAxis->range().lower) / plot->yAxis->range().size();
        //计算游标X值对应的所有曲线的Y值
        //目前只追踪一条曲线，后续如果支持多选，可以修改此处代码，同时显示多条曲线的Y值
        for(auto graphData : curves)
        {
            if(graphData.yItemItem->visible()){
                xItemText->setVisible(true);

                tracker->setGraphKey(x);//设置游标的X值
                tracker->updatePosition();

                double traceX = tracker->position->key();

                int xItemWidth = QFontMetrics(xItemText->font()).width(QString::number(int(traceX))) + xItemText->padding().left() * 2;
                double xItemCoordRange = plot->xAxis->pixelToCoord(plot->xAxis->coordToPixel(plot->xAxis->range().lower) + xItemWidth)
                        - plot->xAxis->range().lower;

                //搜索左边离traceX最近的key对应的点，详情参考findBegin函数的帮助
//                QCPDataContainer<QCPGraphData>::const_iterator coorPoint = graphData.graph->data().data()->findBegin(traceX, true);//true代表向左搜索
//                qDebug() << QString("graph对应的Y值是") << coorPoint->value << xItemY;

                xItemText->position->setCoords(std::max(double(0), (traceX - plot->xAxis->range().lower - xItemCoordRange) / plot->xAxis->range().size()),
                                               xItemY);

                xItemText->setText(QString::number(int(traceX)));//游标文本框，指示游标的X值

                double traceY = tracker->position->value();

                double trackerPTY = 1 - (traceY - plot->yAxis->range().lower) / plot->yAxis->range().size();

                graphData.yItemItem->position->setCoords(0,
                    std::min(xItemY, trackerPTY));
                graphData.yItemItem->setText(QString::number(traceY));//游标文本框，指示游标的Y值


                xLine->start->setCoords((traceX - plot->xAxis->range().lower) / plot->xAxis->range().size(), 0);
                xLine->end->setCoords(xLine->start->coords().x(), 1);

                yLine->start->setCoords(0, trackerPTY);
                yLine->end->setCoords(1, yLine->start->coords().y());
            }
        }
        plot->replot();
    }
}
