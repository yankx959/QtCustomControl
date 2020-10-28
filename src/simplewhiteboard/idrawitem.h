#ifndef IDRAWITEM_H
#define IDRAWITEM_H

#include <QGraphicsItem>
#include <QPen>
#include <QBrush>

struct stDrawInfo{
    QPen pen;
    QBrush brush;
    QGraphicsScene* scene = nullptr;
};

enum class ItemType{
    Free,
    Rect,
    Triangle,
    Ellipse,
    Clear,
};

class IDrawItem;
class DrawItemFactory{
public:
    static IDrawItem* create(const ItemType& t, QGraphicsScene* scene, QGraphicsItem* parent = nullptr);
};

class IDrawItem
{
public:
    IDrawItem(){}
    virtual ~IDrawItem(){}
    virtual void startDraw(const QPointF& scenePos) = 0;
    virtual void draw(const QPointF& sceneePos) = 0;
    virtual void endDraw(const QPointF& scenePos) = 0;

    //add to scene
    virtual void add() = 0;

    //erase from scene
    virtual void erase() = 0;
    virtual void setDrawInfo(const QPen& pen = QPen(), const QBrush& brush = QBrush()) = 0;
    virtual QPen getPen() = 0;
    virtual QBrush getBrush() = 0;
};

class IDrawItemBase : public IDrawItem{
public:
    IDrawItemBase(QGraphicsScene* scene){m_drawInfo.scene = scene;}
    virtual void setDrawInfo(const QPen& pen = QPen(), const QBrush& brush = QBrush())final;
    virtual QPen getPen(){return m_drawInfo.pen;}
    virtual QBrush getBrush(){return m_drawInfo.brush;}
    virtual void erase();
    virtual void add();

protected:
    virtual QGraphicsItem* item() = 0;

protected:
    virtual void initDrawInfo(const stDrawInfo& info) = 0;

private:
    stDrawInfo m_drawInfo;
};

class RectDrawItem : public QGraphicsRectItem, public IDrawItemBase{
public:
    RectDrawItem (QGraphicsScene* scene, QGraphicsItem* parent = nullptr);

    // IDrawItem interface
public:
    void startDraw(const QPointF &scenePos);
    void draw(const QPointF &sceneePos);
    void endDraw(const QPointF &scenePos);

protected:
    QGraphicsItem* item(){return this;}
    void initDrawInfo(const stDrawInfo& info);

private:
    QRectF getDrawRect(const QPointF& anotherPos)const;

private:
    QPointF m_startPos;
};

class FreeDrawItem : public QGraphicsPathItem, public IDrawItemBase{
public:
    FreeDrawItem (QGraphicsScene* scene, QGraphicsItem* parent = nullptr);

    // IDrawItem interface
public:
    void startDraw(const QPointF &scenePos);
    void draw(const QPointF &scenePos);
    void endDraw(const QPointF &scenePos);

protected:
    QGraphicsItem* item(){return this;}
    void initDrawInfo(const stDrawInfo& info);

private:
    QPainterPath m_path;
};

class TriangleDrawItem : public FreeDrawItem{
public:
    TriangleDrawItem (QGraphicsScene* scene, QGraphicsItem* parent = nullptr);

    // IDrawItem interface
public:
    void startDraw(const QPointF &scenePos);
    void draw(const QPointF &scenePos);
    void endDraw(const QPointF &scenePos);

private:
    QRectF getDrawRect(const QPointF &anotherPos) const;

private:
    QPointF m_startPos;
    QPainterPath m_path;
};

class ClearDrawItem : public FreeDrawItem{
public:
    ClearDrawItem (QGraphicsScene* scene, QGraphicsItem* parent = nullptr);

    // IDrawItem interface
protected:
    void initDrawInfo(const stDrawInfo& info);
    QGraphicsItem* item(){return this;}
};

class EllipseDrawItem : public QGraphicsEllipseItem, public IDrawItemBase{
public:
    EllipseDrawItem(QGraphicsScene* scene, QGraphicsItem* parent = nullptr);

public:
    void startDraw(const QPointF &scenePos);
    void draw(const QPointF &sceneePos);
    void endDraw(const QPointF &scenePos);

protected:
    QGraphicsItem* item(){return this;}
    virtual void initDrawInfo(const stDrawInfo& info);

private:
    QRectF getDrawRect(const QPointF& anotherPos)const;

private:
    QPointF m_startPos;
};

#endif // IDRAWITEM_H
