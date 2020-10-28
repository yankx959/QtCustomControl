#include "trapetabbar.h"
#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>

class TrapeTabBar::TrapeTabBarPrivate{
    Q_DECLARE_PUBLIC(TrapeTabBar)

private:
    TrapeTabBarPrivate(TrapeTabBar* q)
        : q_ptr(q)
        , uniformWidth(-1)
        , spacing(0)
        , leftMargin(0)
        , triangleLength(-1)
        , textMargin(5)
        , currentIndex(0)
        , startY(0){}


    QVector<int> calcWidths(const QStyleOption &opt)const;

    int currentTriangleLength()const;

private:
    TrapeTabBar* q_ptr;

private:
    QStringList texts;
    int uniformWidth;
    int spacing;
    int leftMargin;
    int triangleLength;
    int textMargin;
    int currentIndex;
    int startY;
};

TrapeTabBar::TrapeTabBar(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new TrapeTabBarPrivate(this))
{
}

TrapeTabBar::~TrapeTabBar()
{
    delete d_ptr;
}

int TrapeTabBar::spacing() const
{
    return d_func()->spacing;
}

void TrapeTabBar::setSpacing(int space)
{
    if(d_func()->spacing != space){
        d_func()->spacing = space;
        update();
    }
}

void TrapeTabBar::addTab(const QString &text)
{
    d_func()->texts.append(text);
    update();
}

void TrapeTabBar::insertTab(int idx, const QString &text)
{
    Q_D(TrapeTabBar);
    Q_ASSERT(idx >=0 && idx < d->texts.size());
    d->texts.insert(idx, text);
    update();
}

QString TrapeTabBar::text(int idx) const
{
    Q_ASSERT(idx >=0 && idx < d_func()->texts.size());
    return d_func()->texts.at(idx);
}

void TrapeTabBar::setText(int idx, const QString &text)
{
    Q_ASSERT(idx >=0 && idx < d_func()->texts.size());
    d_func()->texts[idx] = text;
    update();
}

void TrapeTabBar::setTriangleLength(int len)
{
    if(d_func()->triangleLength != len){
        d_func()->triangleLength = len;
        update();
    }
}

int TrapeTabBar::triangleLength() const
{
    return d_func()->triangleLength;
}

void TrapeTabBar::setLeftMargin(int margin)
{
    if(d_func()->leftMargin != margin){
        d_func()->leftMargin = margin;
        update();
    }
}

int TrapeTabBar::leftMargin() const
{
    return d_func()->leftMargin;
}

void TrapeTabBar::setTextMargin(int margin)
{
    if(d_func()->textMargin != margin){
        d_func()->textMargin = margin;
        update();
    }
}

int TrapeTabBar::textMargin() const
{
    return d_func()->textMargin;
}

void TrapeTabBar::setUniformWidth(int width)
{
    if(d_func()->uniformWidth != width){
        d_func()->uniformWidth = width;
        update();
    }
}

int TrapeTabBar::uniformWidth() const
{
    return d_func()->uniformWidth;
}

int TrapeTabBar::count() const
{
    return d_func()->texts.size();
}

int TrapeTabBar::currentIndex() const
{
    return d_func()->currentIndex;
}

void TrapeTabBar::setCurrentIndex(int idx)
{
    if(d_func()->currentIndex != idx){
        d_func()->currentIndex = idx;
        emit sigCurrentIndexChanged(idx);
        update();
    }
}

int TrapeTabBar::startY() const
{
    return d_func()->startY;
}

void TrapeTabBar::setStartY(int y)
{
    d_func()->startY = y;
    update();
}

void TrapeTabBar::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    Q_D(TrapeTabBar);

    int startX = d->leftMargin;
    int right = startX;
    int triangleLen = d->triangleLength < 0 ? height() : d->triangleLength;

    QVector<int> widths = d->calcWidths(opt);
    right += std::accumulate(widths.begin(), widths.end(), 0);
    right += (d->texts.size() - 1) * d->spacing;
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::gray));

    int selectIndexLeft = 0;
    for(int i = d->texts.size() - 1; i >= 0; --i){
        if(i == d->currentIndex){
            selectIndexLeft = right - widths.at(i);
            right -= (widths.at(i) + d->spacing);
            continue;
        }
        int curLeft = right - widths.at(i);
        QPolygon ploy;
        ploy << QPoint{right, height()} << QPoint{right - triangleLen, d->startY}
             << QPoint{curLeft, d->startY} << QPoint{curLeft, height()};

        painter.setPen(Qt::NoPen);
        painter.drawPolygon(ploy);

        painter.setPen(QPen(QColor(Qt::blue), 1));
        painter.drawText(QRect(QPoint(curLeft + d->textMargin, d->startY), QSize(right - d->textMargin * 2 - triangleLen, height() - d->startY))
                         , d->texts.at(i)
                         , Qt::AlignLeft | Qt::AlignVCenter);

        right -= (widths.at(i) + d->spacing);
    }
    painter.setBrush(QBrush(Qt::red));

    painter.setPen(Qt::NoPen);
//    painter.setBrush(Qt::red);

    int selectIndexRight = selectIndexLeft + widths.at(d->currentIndex);
    QPolygon ploy;
    ploy << QPoint{selectIndexRight, height()} << QPoint{selectIndexRight - triangleLen, d->startY}
         << QPoint{selectIndexLeft, d->startY} << QPoint{selectIndexLeft, height()};
    painter.drawPolygon(ploy);

    painter.setPen(QPen(QColor(Qt::blue), 1));
    painter.drawText(QRect(QPoint(selectIndexLeft + d->textMargin, d->startY), QSize(selectIndexRight - d->textMargin * 2 - triangleLen, height() - d->startY))
                     , d->texts.at(d->currentIndex)
                     , Qt::AlignLeft | Qt::AlignVCenter);
    painter.restore();
}

void TrapeTabBar::mousePressEvent(QMouseEvent *me)
{
    QWidget::mousePressEvent(me);
    Q_D(TrapeTabBar);
    QStyleOption opt;
    opt.init(this);
    QVector<int> widths = d->calcWidths(opt);

    int triangleLen = d->currentTriangleLength();
    int curLeft = d->leftMargin;
    QVector<QRect> rects;
    for(int width : widths){
        rects << QRect{QPoint{curLeft, d->startY}, QSize(width - triangleLen, height() - d->startY)};
        curLeft += (width + d->spacing);
    }
    for(int i = 0; i < rects.size(); ++i){
        if(rects.at(i).contains(me->pos())){
            if(d->currentIndex != i){
                d->currentIndex = i;
                update();
                emit sigCurrentIndexChanged(i);
                break;
            }
        }
    }
}

QVector<int> TrapeTabBar::TrapeTabBarPrivate::calcWidths(const QStyleOption& opt)const
{
    QVector<int> widths;
    int triangleLen = currentTriangleLength();
    if(uniformWidth > 0){
        for(int i = 0; i < texts.size(); ++i){
            widths.append(uniformWidth + triangleLen);
        }
    }
    else{
        for(int i = 0; i < texts.size(); ++i){
            int tabWidth = 0;
            tabWidth += 2 * textMargin;
            tabWidth += opt.fontMetrics.width(texts.at(i));
            tabWidth += triangleLen;
            widths << tabWidth;
        }
    }
    return widths;
}

int TrapeTabBar::TrapeTabBarPrivate::currentTriangleLength() const
{
    return triangleLength < 0 ? q_func()->height() : triangleLength;
}
