#ifndef CIRCLEMENU_H
#define CIRCLEMENU_H

#include <QFrame>

class QParallelAnimationGroup;
class QVariantAnimation;
class CircleMenuPrivate;
class CircleMenu : public QFrame
{
    Q_OBJECT

public:
    enum Area{
        RightBottom,
        Right,
        RightTop,
        LeftTop,
        Left,
        LeftBottom,

        Unknown,
    };

public:
    explicit CircleMenu(QWidget *parent = nullptr);
    virtual ~CircleMenu()override;

    //0~1
    void setRotateSpeed(double speed);

    void setRotateAnimationEnabled(bool enabled);
    bool rotateAnimationEnabled()const;

    void setZoomAnimationEnabled(bool enabled);
    bool zoomAnimationEnabled()const;

    void setOutCircleRadius(int radius);

    void setAnimationDuration(int duration);
    int animationDuration()const;

    QVector<QImage> images()const;

    //右下逆时针到左下
    void setAreas(const QVector<QImage>& images, const QStringList& tips);

    void setShowPopup(bool isPopup);
    bool isShowPopup()const;

    void setMoveable(bool moveable);
    bool moveable()const;

    void setArrowVisible(bool visible);
    bool arrowVisible()const;

    void setInsideCircleTransparent(bool trans);
    bool insideCircleTransparent()const;

    void setArrowColor(const QColor& color);
    QColor arrowColor()const;

    void setPieBursh(const QBrush& brush);
    QBrush pieBrush()const;

    void setOutCircleBrush(const QBrush& brush);
    QBrush outCircleBrush()const;

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual bool event(QEvent *event) override;

private:

signals:
    void sigClicked(Area area);
    void sigAreaChanged(Area area);

public:
    void show();

private:
    CircleMenuPrivate* d_ptr;
    Q_DECLARE_PRIVATE(CircleMenu)
};

#endif // CIRCLEMENU_H
