#include "whiteboarddlg.h"
#include "ui_whiteboarddlg.h"
#include "idrawitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QColorDialog>
#include <QDebug>

class WhiteBoardDlg::WhiteBoardDlgPrivate{
public:
    WhiteBoardDlgPrivate(WhiteBoardDlg* q)
        : q_ptr(q)
        , scene(nullptr)
        , curItemType(ItemType::Free)
        , isLeftbuttonPressed(false)
        , currentColor(Qt::red)
    {}

    void init()
    {
        Q_Q(WhiteBoardDlg);
        ui.setupUi(q);
        q->setWindowTitle(QStringLiteral("白板"));
        q->resize(1135, 690);

        scene = new QGraphicsScene(q);
        scene->installEventFilter(q);
        scene->setBackgroundBrush(QBrush(Qt::white));
        scene->setSceneRect(0, 0, ui.graphicsView->width(), ui.graphicsView->height());
        ui.graphicsView->setScene(scene);
//        ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//        ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        //init slider
        ui.slider_pen_width->setAlwaysShowCircle(true);
        ui.slider_pen_width->setFixedHeight(22);
        ui.slider_pen_width->setRadius(10);
        ui.slider_pen_width->setProcHeight(3);
        ui.slider_pen_width->setCircleBrush(QBrush(QColor(Qt::white)));
        ui.slider_pen_width->setProcessRectBrush(QBrush(QColor(111,127,143)));
        ui.slider_pen_width->setBKRectBrush(QBrush(QColor(211,218,234)));
        ui.slider_pen_width->setFixedWidth(100);
        ui.slider_pen_width->setRange(0, 36);
        ui.slider_pen_width->setValue(12);
        ui.spin_pen_width->setRange(0, 36);
        ui.spin_pen_width->setValue(12);

        ui.buttonGroup->setId(ui.btn_pen, int(ItemType::Free));
        ui.buttonGroup->setId(ui.btn_eraser, int(ItemType::Clear));
        ui.buttonGroup->setId(ui.btn_ell, int(ItemType::Ellipse));
        ui.buttonGroup->setId(ui.btn_rect, int(ItemType::Rect));
        ui.buttonGroup->setId(ui.btn_triangle, int(ItemType::Triangle));

        changeColorLabel(currentColor);
    }

    void startDraw(const QPointF& scenePos)
    {
        if(curItemType == ItemType::Clear && items.size() == 0)
            return;
        IDrawItem* item = DrawItemFactory::create(curItemType, scene);
        QPen pen(currentColor, ui.spin_pen_width->value());
        pen.setJoinStyle(Qt::MiterJoin);
        item->setDrawInfo(pen);
        item->add();
        items << item;
        item->startDraw(scenePos);
    }

    void draw(const QPointF& scenePos)
    {
        if(items.size() == 0)
            return ;
        auto& lastItem = items.last();
        lastItem->draw(scenePos);
    }

    void endDraw(const QPointF& scenePos)
    {
        if(items.size() == 0)
            return ;
        auto& lastItem = items.last();
        lastItem->endDraw(scenePos);
    }

    void clearCacheItems()
    {
        qDeleteAll(cacheItems);
        cacheItems.clear();
    }

    void clearItems()
    {
        for(auto& p : items)
            p->erase();
        qDeleteAll(items);
        items.clear();
    }

    void changeColorLabel(const QColor& color)
    {
        QPixmap pix(19, 19);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        painter.setRenderHints(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(color));
        painter.drawRoundedRect(pix.rect(), 3, 3);
        ui.label_color->setPixmap(pix);
    }

    void save(QPaintDevice *io)
    {
        QPainter painter(io);
        scene->render(&painter, scene->sceneRect(), scene->sceneRect());
    }

private:
    void _q_buttonGroupClicked(int id)
    {
        curItemType = static_cast<ItemType>(id);
    }

    void _q_colorSelected(QColor color)
    {
        currentColor = color;
        changeColorLabel(color);
    }

private:
    Q_DECLARE_PUBLIC(WhiteBoardDlg)
    WhiteBoardDlg* q_ptr;

private:
    Ui::WhiteBoardDlg ui;
    QGraphicsScene* scene;
    bool isLeftbuttonPressed;
    QColor currentColor;
    ItemType curItemType;
    QList<IDrawItem*> items;
    QList<IDrawItem*> cacheItems;  //缓存撤销后的item 用户恢复， 当不做恢复，继续画图时清空
};

WhiteBoardDlg::WhiteBoardDlg(QWidget *parent) :
    QDialog(parent),
    d_ptr(new WhiteBoardDlgPrivate(this))
{
    Q_D(WhiteBoardDlg);
    d->init();

    connect(d->ui.buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(_q_buttonGroupClicked(int)));
    connect(d->ui.btn_undo, SIGNAL(clicked()), this, SLOT(undo()));
    connect(d->ui.btn_delete, SIGNAL(clicked()), this, SLOT(clear()));
    connect(d->ui.btn_palette, SIGNAL(clicked()), this, SLOT(selectColor()));
    connect(d->ui.spin_pen_width, SIGNAL(valueChanged(int)), d->ui.slider_pen_width, SLOT(setValue(int)));
    connect(d->ui.slider_pen_width, &ProgressBar::sigValueChanged, [d](qint64 value){
        d->ui.spin_pen_width->setValue(value);
    });
}

WhiteBoardDlg::~WhiteBoardDlg()
{

}

bool WhiteBoardDlg::eventFilter(QObject *o, QEvent *e)
{
    Q_D(WhiteBoardDlg);
    if(o == d->scene)
    {
        switch (e->type()) {
        case QEvent::GraphicsSceneMousePress:
        {
            QGraphicsSceneMouseEvent* me = dynamic_cast<QGraphicsSceneMouseEvent*>(e);
            if(me->button() == Qt::LeftButton)
            {
                d->isLeftbuttonPressed = true;
                d->clearCacheItems();
                d->startDraw(me->scenePos());
            }
        }
            break;
        case QEvent::GraphicsSceneMouseMove:
        {
            if(d->isLeftbuttonPressed)
            {
                QGraphicsSceneMouseEvent* me = dynamic_cast<QGraphicsSceneMouseEvent*>(e);
                d->draw(me->scenePos());
            }
        }
            break;
        case QEvent::GraphicsSceneMouseRelease:
        {
            QGraphicsSceneMouseEvent* me = dynamic_cast<QGraphicsSceneMouseEvent*>(e);
            if(me->button() == Qt::LeftButton)
            {
                d->isLeftbuttonPressed = false;
                d->endDraw(me->scenePos());
            }
        }
            break;
        case QEvent::KeyPress:
        {
            QKeyEvent* keye = dynamic_cast<QKeyEvent*>(e);
            if(keye->matches(QKeySequence::Undo))
            {
                undo();
            }
            else if(keye->matches(QKeySequence::Redo))
            {
                redo();
            }
            else if(keye->matches(QKeySequence::Save))
            {
                save(QApplication::applicationDirPath() + "/temp/WhiteBoard.png", "PNG");
            }
        }
            break;
        default:
            break;
        }
    }
    return QDialog::eventFilter(o, e);
}

void WhiteBoardDlg::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    Q_D(WhiteBoardDlg);
    d->scene->setSceneRect(QRectF(0, 0, d->ui.graphicsView->width(), d->ui.graphicsView->height()));
}

void WhiteBoardDlg::resizeEvent(QResizeEvent *re)
{
    QDialog::resizeEvent(re);
    Q_D(WhiteBoardDlg);
    d->scene->setSceneRect(QRectF(0, 0, d->ui.graphicsView->width(), d->ui.graphicsView->height()));
    qDebug() << d->scene->sceneRect();
    qDebug() << "view" << d->ui.graphicsView->rect();
}

void WhiteBoardDlg::undo()
{
    Q_D(WhiteBoardDlg);
    if(d->items.size())
    {
        auto&& last = d->items.last();
        d->cacheItems.push_front(last);
        d->items.pop_back();
        last->erase();
    }
}

void WhiteBoardDlg::redo()
{
    Q_D(WhiteBoardDlg);
    if(d->cacheItems.size())
    {
        auto&& first = d->cacheItems.first();
        first->add();
        d->items << first;
        d->cacheItems.pop_front();
    }
}

void WhiteBoardDlg::clear()
{
    Q_D(WhiteBoardDlg);
    d->clearCacheItems();
    d->clearItems();
}

void WhiteBoardDlg::selectColor()
{
    QColorDialog dlg(this);
    connect(&dlg, SIGNAL(colorSelected(QColor)), this, SLOT(_q_colorSelected(QColor)));
    dlg.exec();
}

void WhiteBoardDlg::save(const QString &filePath, const QString &format)
{
    Q_D(WhiteBoardDlg);
    QImage img(d->scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    d->save(&img);
    img.save(filePath, format.toStdString().c_str());
}
#ifdef QT_DEBUG
#include "debug/moc_whiteboarddlg.cpp"
#else
#include "release/moc_whiteboarddlg.cpp"
#endif
