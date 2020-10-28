#include "idrawitem.h"
#include <QGraphicsScene>

IDrawItem *DrawItemFactory::create(const ItemType &t, QGraphicsScene *scene, QGraphicsItem *parent)
{
    IDrawItem* ret = nullptr;
    switch (t) {
    case ItemType::Rect:
        ret = new RectDrawItem(scene, parent);
        break;
    case ItemType::Clear:
        ret = new ClearDrawItem(scene, parent);
        break;
    case ItemType::Ellipse:
        ret = new EllipseDrawItem(scene, parent);
        break;
    case ItemType::Free:
        ret = new FreeDrawItem(scene, parent);
        break;
    case ItemType::Triangle:
        ret = new TriangleDrawItem(scene, parent);
        break;
    default:
        break;
    }
    return ret;
}

RectDrawItem::RectDrawItem(QGraphicsScene *scene, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , IDrawItemBase(scene)
{

}

void RectDrawItem::startDraw(const QPointF &scenePos)
{
    m_startPos = scenePos;
    setRect(QRectF(scenePos, scenePos));
}

void RectDrawItem::draw(const QPointF &sceneePos)
{
    setRect(getDrawRect(sceneePos));
}

void RectDrawItem::endDraw(const QPointF &scenePos)
{
    setRect(getDrawRect(scenePos));
}

void RectDrawItem::initDrawInfo(const stDrawInfo &info)
{
    setPen(info.pen);
    setBrush(info.brush);
}

QRectF RectDrawItem::getDrawRect(const QPointF &anotherPos) const
{
    QPointF tempTopLeft = m_startPos;
    QPointF tempBottomRight = anotherPos;
    if(m_startPos.x() > anotherPos.x())
    {
        tempTopLeft.setX(anotherPos.x());
        tempBottomRight.setX(m_startPos.x());
    }
    if(m_startPos.y() > anotherPos.y())
    {
        tempTopLeft.setY(anotherPos.y());
        tempBottomRight.setY(m_startPos.y());
    }
    return QRectF(tempTopLeft, tempBottomRight);
}

ClearDrawItem::ClearDrawItem(QGraphicsScene *scene, QGraphicsItem *parent)
    : FreeDrawItem(scene, parent)
{

}

void ClearDrawItem::initDrawInfo(const stDrawInfo &info)
{
    QPen pen(info.pen);
    pen.setColor(info.scene->backgroundBrush().color());
    setPen(pen);
}

void IDrawItemBase::setDrawInfo(const QPen &pen, const QBrush &brush)
{
    m_drawInfo.pen = pen;
    m_drawInfo.brush = brush;
    initDrawInfo(m_drawInfo);
}

void IDrawItemBase::erase()
{
    if(!m_drawInfo.scene)
        return ;
    m_drawInfo.scene->removeItem(item());
}

void IDrawItemBase::add()
{
    m_drawInfo.scene->addItem(item());
}

EllipseDrawItem::EllipseDrawItem(QGraphicsScene *scene, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
    , IDrawItemBase(scene)
{

}

void EllipseDrawItem::startDraw(const QPointF &scenePos)
{
    m_startPos = scenePos;
    setRect(QRectF(scenePos, scenePos));
}

void EllipseDrawItem::draw(const QPointF &sceneePos)
{
    setRect(getDrawRect(sceneePos));
}

void EllipseDrawItem::endDraw(const QPointF &scenePos)
{
    setRect(getDrawRect(scenePos));
}

void EllipseDrawItem::initDrawInfo(const stDrawInfo &info)
{
    setPen(info.pen);
    setBrush(info.brush);
}

QRectF EllipseDrawItem::getDrawRect(const QPointF &anotherPos) const
{
    QPointF tempTopLeft = m_startPos;
    QPointF tempBottomRight = anotherPos;
    if(m_startPos.x() > anotherPos.x())
    {
        tempTopLeft.setX(anotherPos.x());
        tempBottomRight.setX(m_startPos.x());
    }
    if(m_startPos.y() > anotherPos.y())
    {
        tempTopLeft.setY(anotherPos.y());
        tempBottomRight.setY(m_startPos.y());
    }
    return QRectF(tempTopLeft, tempBottomRight);
}

FreeDrawItem::FreeDrawItem(QGraphicsScene *scene, QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
    , IDrawItemBase(scene)
{

}

void FreeDrawItem::startDraw(const QPointF &scenePos)
{
    m_path.moveTo(scenePos);
    setPath(m_path);
}

void FreeDrawItem::draw(const QPointF &scenePos)
{
    m_path.lineTo(scenePos);
    setPath(m_path);
}

void FreeDrawItem::endDraw(const QPointF &scenePos)
{
    m_path.lineTo(scenePos);
    setPath(m_path);
}

void FreeDrawItem::initDrawInfo(const stDrawInfo &info)
{
    setPen(info.pen);
}

TriangleDrawItem::TriangleDrawItem(QGraphicsScene *scene, QGraphicsItem *parent)
    : FreeDrawItem(scene, parent)
{

}

void TriangleDrawItem::startDraw(const QPointF &scenePos)
{
    m_startPos = scenePos;
    m_path.moveTo(m_startPos);
    setPath(m_path);
}

void TriangleDrawItem::draw(const QPointF &scenePos)
{
    QRectF rc = getDrawRect(scenePos);
    m_path = QPainterPath();

    //顶点
    QPointF topCenter = QPointF(rc.center().x(), rc.top());
    m_path.moveTo(topCenter);

    //底点
    m_path.lineTo(rc.bottomRight());
    m_path.lineTo(rc.bottomLeft());
    m_path.lineTo(topCenter);
    setPath(m_path);
}

void TriangleDrawItem::endDraw(const QPointF &scenePos)
{
    draw(scenePos);
}

QRectF TriangleDrawItem::getDrawRect(const QPointF &anotherPos) const
{
    QPointF tempTopLeft = m_startPos;
    QPointF tempBottomRight = anotherPos;
    if(m_startPos.x() > anotherPos.x())
    {
        tempTopLeft.setX(anotherPos.x());
        tempBottomRight.setX(m_startPos.x());
    }
    if(m_startPos.y() > anotherPos.y())
    {
        tempTopLeft.setY(anotherPos.y());
        tempBottomRight.setY(m_startPos.y());
    }
    return QRectF(tempTopLeft, tempBottomRight);
}
