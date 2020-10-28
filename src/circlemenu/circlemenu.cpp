#include "circlemenu.h"
#include <QPainter>
#include <cmath>
#include <QVariantAnimation>
#include <QParallelAnimationGroup>
#include <QDebug>
#include <QApplication>
#include <QMouseEvent>
#include <QPainterPath>
#include <QBitmap>
#include <QToolTip>

const double PI = std::acos(-1);

class CircleMenuPrivate{
public:
    CircleMenuPrivate(CircleMenu* q)
        : q_ptr(q)
        , isMenuVisible(false)
        , aniForZoom(nullptr)
        , aniForRotate(nullptr)
        , aniGroup(new QParallelAnimationGroup(q))
        , angle(0)
        , diameter(0)
        , imageSize{26, 26}
        , margin(4)
        , canMove(true)
        , moved(false)
        , pressed(false)
        , needDrawArrow(true)
        , arrowColor{Qt::black}
        , outCircleBrush{QColor{192,212,204}}
        , incircleTranparent(true)
        , pieBrush{QColor{153, 189, 221}}
        , animationDuration(150)
        , smallDiameter(0)
        , isPopup(false)
        , rotateSpeed(0.3)
        , currentArea(CircleMenu::Unknown)
        , pressedMovedArea(CircleMenu::Unknown)
        , pressPosIsInSmallCircle(false)
    {

    }

private:
    void init();

    CircleMenu::Area calcArea(const QPoint& pos)const;
    int calcInCircleRadius()const;

    void clearMouseInfo();

    void drawCircle(QPainter* painter);

    void drawPie(QPainter* painter);

    void drawArrow(QPainter* painter);

    void drawImages(QPainter* painter);

    void showPopup();

    void hidePopup();

    void updateTip(const QPoint &gpos);

private:
    CircleMenu* q_ptr;
    Q_DECLARE_PUBLIC(CircleMenu)

private:
    bool isMenuVisible;
    QVariantAnimation* aniForZoom;
    QVariantAnimation* aniForRotate;
    QParallelAnimationGroup* aniGroup;

    QVector<QImage> images;
    int angle = 0;
    int diameter;

    QSize imageSize;
    int margin;
    bool canMove;
    bool moved;
    bool pressed;
    bool needDrawArrow;
    QPoint startPos;
    QColor arrowColor;
    QBrush outCircleBrush;
    bool incircleTranparent;
    QBrush pieBrush;
    int animationDuration;
    int smallDiameter;
    bool isPopup;
    double rotateSpeed;
    bool pressPosIsInSmallCircle;
    CircleMenu::Area currentArea;
    CircleMenu::Area pressedMovedArea;

    QStringList tips;
};

CircleMenu::CircleMenu(QWidget *parent)
    : QFrame(parent)
    , d_ptr(new CircleMenuPrivate(this))
{
    d_func()->init();
}

CircleMenu::~CircleMenu()
{
    delete d_ptr;
}

void CircleMenu::setRotateSpeed(double speed)
{
    Q_D(CircleMenu);
    d->rotateSpeed = speed;
    if(d->aniForRotate){
        d->aniForRotate->setStartValue(int(d->rotateSpeed * -360));
    }
}

void CircleMenu::setRotateAnimationEnabled(bool enabled)
{
#ifndef Q_OS_LINUX
    Q_D(CircleMenu);
    if(enabled){
        if(d->aniForRotate){
            return ;
        }
        else{
            d->aniForRotate = new QVariantAnimation(this);
            d->aniForRotate->setStartValue(int(-360 * d->rotateSpeed));
            d->aniForRotate->setEndValue(0);
            d->aniForRotate->setDuration(d->animationDuration);

            connect(d->aniForRotate, &QVariantAnimation::valueChanged, this, [d, this](const QVariant& v){
                d->angle = v.toInt();
                update();
            });
            d->aniGroup->addAnimation(d->aniForRotate);
        }
    }
    else{
        if(d->aniForRotate){
            d->aniGroup->takeAnimation(d->aniGroup->indexOfAnimation(d->aniForRotate));
            delete d->aniForRotate;
            d->aniForRotate = nullptr;
        }
    }
#endif
}

bool CircleMenu::rotateAnimationEnabled() const
{
    return d_func()->aniForRotate;
}

void CircleMenu::setZoomAnimationEnabled(bool enabled)
{
#ifndef Q_OS_LINUX
    Q_D(CircleMenu);
    if(enabled){
        if(d->aniForZoom){
            return ;
        }
        else{
            d->aniForZoom = new QVariantAnimation(this);
            d->aniForZoom->setStartValue(d->smallDiameter);
            d->aniForZoom->setEndValue(d->diameter);
            d->aniForZoom->setDuration(d->animationDuration);

            connect(d->aniForZoom, &QVariantAnimation::valueChanged, [d, this](const QVariant& v){
                d->diameter = v.toInt();
                update();
            });
            d->aniGroup->addAnimation(d->aniForZoom);
        }
    }
    else{
        if(d->aniForZoom){
            d->aniGroup->takeAnimation(d->aniGroup->indexOfAnimation(d->aniForZoom));
            delete d->aniForZoom;
            d->aniForZoom = nullptr;
        }
    }
#endif
}

bool CircleMenu::zoomAnimationEnabled() const
{
    return d_func()->aniForZoom;
}

void CircleMenu::setOutCircleRadius(int radius)
{
    d_func()->diameter = radius * 2;
    d_func()->smallDiameter = d_func()->calcInCircleRadius();
    update();
}

void CircleMenu::setAnimationDuration(int duration)
{
    Q_D(CircleMenu);
    d->animationDuration = duration;
    if(d->aniForZoom){
        d->aniForZoom->setDuration(duration);
    }
    if(d->aniForRotate){
        d->aniForRotate->setDuration(duration);
    }
}

int CircleMenu::animationDuration() const
{
    return d_func()->animationDuration;
}

QVector<QImage> CircleMenu::images() const
{
    return d_func()->images;
}

void CircleMenu::setAreas(const QVector<QImage> &images, const QStringList &tips)
{
    Q_D(CircleMenu);
    d->images.clear();
    int size = std::min(6, images.size());
    d->images.resize(size);
    std::copy(images.begin(), images.begin() + size, d->images.begin());
    if(d->images.size()){
        d->imageSize = d->images.at(0).size();
    }
    d->tips = tips;
    update();
}

void CircleMenu::setShowPopup(bool isPopup)
{
    d_func()->isPopup = isPopup;
    if(d_func()->isPopup){
        d_func()->hidePopup();
    }
}

bool CircleMenu::isShowPopup() const
{
    return d_func()->isPopup;
}

void CircleMenu::setMoveable(bool moveable)
{
    d_func()->canMove = moveable;
}

bool CircleMenu::moveable() const
{
    return d_func()->canMove;
}

void CircleMenu::setArrowVisible(bool visible)
{
    d_func()->needDrawArrow = visible;
    update();
}

bool CircleMenu::arrowVisible() const
{
    return d_func()->needDrawArrow;
}

void CircleMenu::setInsideCircleTransparent(bool trans)
{
    d_func()->incircleTranparent = trans;
    update();
}

bool CircleMenu::insideCircleTransparent() const
{
    return d_func()->incircleTranparent;
}

void CircleMenu::setArrowColor(const QColor &color)
{
    d_func()->arrowColor = color;
    update();
}

QColor CircleMenu::arrowColor() const
{
    return d_func()->arrowColor;
}

void CircleMenu::setPieBursh(const QBrush &brush)
{
    d_func()->pieBrush = brush;
    update();
}

QBrush CircleMenu::pieBrush() const
{
    return d_func()->pieBrush;
}

void CircleMenu::setOutCircleBrush(const QBrush &brush)
{
    d_func()->outCircleBrush = brush;
    update();
}

QBrush CircleMenu::outCircleBrush() const
{
    return d_func()->outCircleBrush;
}

void CircleMenu::show()
{
    QWidget::show();
    setFocus();
    if(d_func()->isPopup){
        d_func()->showPopup();
    }
}

void CircleMenu::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    Q_D(CircleMenu);
    QPainter painter(this);
#ifndef Q_OS_LINUX
    if(d->isPopup){
        painter.fillRect(rect(), QBrush(QColor(0, 0, 0, 1)));
    }
#endif

    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    //    painter.fillRect(rect(), QBrush(QColor(255, 0, 0, 1)));
    QPointF center(width() / 2.0, height() / 2.0);
    painter.translate(center);
    painter.rotate(d->angle);

    d->drawCircle(&painter);
    d->drawPie(&painter);
    d->drawArrow(&painter);
    d->drawImages(&painter);
}

void CircleMenu::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(CircleMenu);
    if(d->canMove && d->pressed && d->pressPosIsInSmallCircle){
        this->move(this->pos() + event->pos() - d->startPos);
        d->moved = true;
    }
    if(d->isMenuVisible){
        auto old = d->currentArea;
        d->currentArea = d->calcArea(event->pos());
        update();
        if(!d->pressed){
            d->updateTip(event->globalPos());
        }
        if(d->pressed && old != d->currentArea && !d->pressPosIsInSmallCircle){
            d->pressedMovedArea = d->currentArea;
            emit sigAreaChanged(d->currentArea);
        }
    }
}

void CircleMenu::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton){
        return ;
    }
    Q_D(CircleMenu);
    d->pressed = true;
    d->startPos = event->pos();

    QPointF center(width() / 2.0, height() / 2.0);
    double distance = std::sqrt(std::pow(center.x() - d->startPos.x(), 2) + std::pow(center.y() - d->startPos.y(), 2));
    d->pressPosIsInSmallCircle = distance < d->calcInCircleRadius();
}

void CircleMenu::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(CircleMenu);
    d->pressed = false;
    if(event->button() != Qt::LeftButton){
        return ;
    }
    if(d->isMenuVisible){
        auto old = d->pressedMovedArea;
        d->pressedMovedArea = d->calcArea(event->pos());
        if(old != d->pressedMovedArea){
            emit sigAreaChanged(d->pressedMovedArea);
        }
        emit sigClicked(d->pressedMovedArea);
    }
    if(!d->isPopup && !d->moved){
        QPoint rela = event->pos() - QPoint(width() / 2, height() / 2);
        int dis = std::sqrt(rela.x() * rela.x() + rela.y() * rela.y());
        if(dis > d->smallDiameter / 2){
            return ;
        }
        d->isMenuVisible = !d->isMenuVisible;
        d->aniGroup->setDirection(d->isMenuVisible ? QVariantAnimation::Forward : QVariantAnimation::Backward);
        d->aniGroup->start();
    }
    d->moved = false;
}

bool CircleMenu::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::FocusOut:
    case QEvent::WindowDeactivate:
        if(d_func()->isPopup){
            d_func()->hidePopup();
        }
        break;
    case QEvent::Leave:
        d_func()->currentArea = Unknown;
        update();
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

void CircleMenuPrivate::init()
{
    Q_Q(CircleMenu);
    q->setFixedSize(180, 180);
    diameter = 150;
    smallDiameter = calcInCircleRadius() * 2;
    q->setMouseTracking(true);

    q->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
#ifndef Q_OS_LINUX
    q->setAttribute(Qt::WA_TranslucentBackground);
#else
    q->setFocusPolicy(Qt::NoFocus);
    QBitmap mask(q->width(), q->height());
    mask.clear();
    QPainter painter(&mask);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush(Qt::red);
    painter.drawEllipse(q->rect().center(), diameter / 2 - 1, diameter / 2 - 1);
    q->setMask(mask);
#endif
    q->setRotateAnimationEnabled(true);
    q->setZoomAnimationEnabled(true);
}

CircleMenu::Area CircleMenuPrivate::calcArea(const QPoint &pos) const
{
    const Q_Q(CircleMenu);
    CircleMenu::Area ret = CircleMenu::Unknown;
    QPoint center{q->width() / 2, q->height() / 2};
    QPoint temp = pos - center;
    //计算距离圆心距离
    int dis = std::sqrt(std::pow(temp.x(), 2) + std::pow(temp.y(), 2));

    //如果在内圆或者边界外
    if(dis < calcInCircleRadius() || dis > diameter / 2){
        return ret;
    }
    //右侧
    if(temp.x() > 0){
        if(temp.y() > temp.x() * std::tan(30 * (PI / 180))){
            ret = CircleMenu::RightBottom;
        }
        else if(temp.y() < temp.x() * std::tan(-30 *(PI / 180))){
            ret = CircleMenu::RightTop;
        }
        else{
            ret = CircleMenu::Right;
        }
    }
    else{
        if(temp.y() < temp.x() * std::tan(30 * (PI / 180))){
            ret = CircleMenu::LeftTop;
        }
        else if(temp.y() > temp.x() * std::tan(-30 *(PI / 180))){
            ret = CircleMenu::LeftBottom;
        }
        else{
            ret = CircleMenu::Left;
        }
    }
    return ret;
}

int CircleMenuPrivate::calcInCircleRadius() const
{
    return diameter / 2 - imageSize.width() - margin * 2;
}

void CircleMenuPrivate::clearMouseInfo()
{
    pressed = false;
    moved   = false;
}

void CircleMenuPrivate::drawCircle(QPainter *painter)
{
    painter->save();
    QPainterPath path;
    if(isMenuVisible || aniGroup->state() == QParallelAnimationGroup::Running){
        path.addEllipse(QPointF(0, 0), diameter / 2, diameter / 2);
    }
    double smallRadius = calcInCircleRadius();
    {
        painter->setPen(QPen(Qt::NoPen));
        painter->setBrush(QBrush(QColor(210,217,226)));
        if(!path.isEmpty()){
            path.addEllipse(QPointF(0, 0), smallRadius, smallRadius);

            //这里画一个透明度防止鼠标穿透
#ifndef Q_OS_LINUX
            painter->setBrush(QBrush(QColor(0, 0, 0, 1)));
#else
            painter->setBrush(QColor(240, 240, 240));
#endif
            painter->drawEllipse(QPointF(0, 0), smallRadius, smallRadius);
        }
        else{
            painter->drawEllipse(QPointF(0, 0), smallDiameter / 2, smallDiameter / 2);
        }
    }
    painter->fillPath(path, outCircleBrush);
    painter->restore();
}

void CircleMenuPrivate::drawPie(QPainter *painter)
{
    Q_Q(CircleMenu);
    painter->save();
    if(currentArea != CircleMenu::Unknown){
        double radius = diameter / 2;
        int start = 270 + currentArea * 60;
        painter->setPen(Qt::NoPen);
        QRadialGradient radial(0, 0, radius);
        radial.setColorAt(0, Qt::transparent);
        radial.setColorAt(calcInCircleRadius() / radius, Qt::transparent);
        if(pieBrush.gradient()){
            double db = calcInCircleRadius() / radius + 0.001;
            for(auto&& p : pieBrush.gradient()->stops()){
                radial.setColorAt(db + p.first * (1 - db), p.second);
            }
        }
        else{
            radial.setColorAt(calcInCircleRadius() / radius + 0.001, pieBrush.color());
            radial.setColorAt(1, pieBrush.color());
        }

        painter->setBrush(radial);

        //这里偏移坐标是因为painter偏移过
        painter->drawPie(QRect(-radius, -radius, diameter, diameter), start * 16, 60 * 16);
    }
    painter->restore();
}

void CircleMenuPrivate::drawArrow(QPainter *painter)
{
    if(!isMenuVisible && aniGroup->state() != QParallelAnimationGroup::Running){
        return ;
    }
    painter->save();
    if(needDrawArrow){
        painter->setPen(QPen(arrowColor, 1));
        painter->setBrush(arrowColor);
        painter->rotate(30);
        double smallRadius = calcInCircleRadius();
        for(int i = 0; i < 6; ++i){
            painter->drawLine(0, 5, 0, smallRadius * 0.7);
            QPolygonF ploy{{{0.0, smallRadius * 0.8}, {-4.0, smallRadius * 0.7}, {4, smallRadius * 0.7}}};
            painter->drawPolygon(ploy);
            painter->rotate(60);
        }
    }
    painter->restore();
}

void CircleMenuPrivate::drawImages(QPainter *painter)
{
    if(!isMenuVisible && aniGroup->state() != QParallelAnimationGroup::Running){
        return ;
    }
    painter->save();
    painter->setBrush(Qt::NoBrush);
    double smallRadius = calcInCircleRadius();
    double radius = diameter / 2;
    double span = radius - smallRadius;

    int w1 = std::sqrt(imageSize.width() * imageSize.width() + imageSize.height() * imageSize.height());
    int startAngle = 60;
    for(int i = 0; i < 6; ++i){
        double x = 0;
        double y = 0;
        switch (startAngle) {
        case 60:
            x = std::cos(startAngle * (PI / 180)) * (radius - (span - w1) / 2);
            y = std::sin(startAngle * (PI / 180)) * (radius - (span - w1) / 2);
            x -= imageSize.width();
            y -= imageSize.height();
            break;
        case 0:
            x = smallRadius + (span - imageSize.width()) / 2;
            y -= imageSize.height() / 2;
            break;
        case -60:
            x = std::cos(startAngle * (PI / 180)) * (radius - (span - w1) / 2);
            y = std::sin(startAngle * (PI / 180)) * (radius - (span - w1) / 2);
            x -= imageSize.width();
            break;
        case -120:
            x = std::cos(startAngle * (PI / 180)) * (radius - (span - w1) / 2);
            y = std::sin(startAngle * (PI / 180)) * (radius - (span - w1) / 2);
            break;
        case -180:
            x = -radius + (span - imageSize.width()) / 2;
            y -= imageSize.height() / 2;
            break;
        case -240:
            x = std::cos(startAngle * (PI / 180)) * (radius - (span - w1) / 2);
            y = std::sin(startAngle * (PI / 180)) * (radius - (span - w1) / 2);
            y -= imageSize.height();
            break;
        default:
            break;
        }
        painter->drawImage(QRect(x, y, imageSize.width(), imageSize.height()), images.at(i));
        startAngle -= 60;
    }
    painter->restore();
}

void CircleMenuPrivate::showPopup()
{
    isMenuVisible = true;
    aniGroup->setDirection(QAbstractAnimation::Forward);
    aniGroup->start();
}

void CircleMenuPrivate::hidePopup()
{
    isMenuVisible = false;
    angle = 0;
    if(aniForZoom){
        diameter = aniForZoom->startValue().toInt();
    }
    q_func()->repaint();
    q_func()->hide();
    //    aniGroup->setDirection(QAbstractAnimation::Backward);
    //    aniGroup->start();
}

void CircleMenuPrivate::updateTip(const QPoint& gpos)
{
    if(currentArea == CircleMenu::Unknown){
        QToolTip::hideText();
    }
    if(tips.size() <= currentArea){
        return ;
    }
    Q_Q(CircleMenu);
    QToolTip::showText(gpos, tips.at(currentArea));
}
