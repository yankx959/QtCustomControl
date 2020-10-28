#include "checkbleheaderview.h"
#include <QMap>
#include <QStyleOptionButton>
#include <QCheckBox>
#include <QMouseEvent>
#include <QPainter>
#include <QSize>
#include <QDebug>

struct stColumnData{
    bool canTristate;
    Qt::CheckState state;
    QString strText;
    Qt::Alignment al;
    stColumnData(){
        canTristate = false;
        state = Qt::Unchecked;
        al = Qt::AlignLeft | Qt::AlignVCenter;
    }

    stColumnData(bool canTristate, const QString& text, Qt::Alignment al)
        : state(Qt::Unchecked)
        , strText(text)
        , canTristate(canTristate)
        , al(al)
    {}
};

class CheckbleHeaderViewPrivate{
public:
    explicit CheckbleHeaderViewPrivate(CheckbleHeaderView* q)
        : q_ptr(q)
        , m_nCurrentPressColumn(-1)
    {
    }

    bool contains(int nCol, const QPoint& pos)
    {
        if(!m_mapColumn2Data.contains(nCol))
        {
            return false;
        }
        Q_Q(CheckbleHeaderView);
        int nX = q->sectionPosition(nCol);
        int size = q->sectionSize(nCol);
        QRect rc(nX, 0, size, q->height());
        QCheckBox check;
        QStyleOptionButton opt = getStyleOption(&check, nCol, m_mapColumn2Data.value(nCol), rc);
        QRect rcIndicator = q->style()->subElementRect(QStyle::SE_CheckBoxIndicator, &opt, &check);
        return rcIndicator.contains(pos);
    }

    QStyleOptionButton getStyleOption(QCheckBox* check, int logicalIndex, const stColumnData& columnData, const QRect& rect)const
    {
        const Q_Q(CheckbleHeaderView);
        QStyleOptionButton opt;
        opt.initFrom(check);
        opt.rect = rect;
        opt.text = columnData.strText;
        if(m_nCurrentPressColumn == logicalIndex)
        {
            opt.state |= QStyle::State_Sunken;
        }
        if(columnData.state == Qt::PartiallyChecked)
        {
            opt.state |= QStyle::State_NoChange;
        }
        else
        {
            opt.state |= columnData.state == Qt::Checked ? QStyle::State_On : QStyle::State_Off;
        }
        if(q->testAttribute(Qt::WA_Hover) && q->underMouse())
        {
            opt.state |= QStyle::State_MouseOver;
        }
        //根据对齐方式重新计算rect
        reCalcOptRect(check, columnData.al, opt);
        return opt;
    }

    void reCalcOptRect(QCheckBox* check, Qt::Alignment al, QStyleOptionButton& opt)const
    {
        const Q_Q(CheckbleHeaderView);
        if(al & Qt::AlignLeft)
        {
            ;
        }
        if(al & Qt::AlignVCenter)
        {
            ;
        }
        if(al & Qt::AlignTop)
        {
            opt.rect.setY(0);
        }
        if(al & Qt::AlignBottom)
        {
            int nHeight = q->style()->subElementRect(QStyle::SE_CheckBoxClickRect, &opt, check).height();
            opt.rect.setY(opt.rect.y() + opt.rect.height() - nHeight);
        }
        if(al & Qt::AlignRight)
        {
            int nWidth = q->style()->subElementRect(QStyle::SE_CheckBoxClickRect, &opt, check).width();
            opt.rect.setX(opt.rect.x() + opt.rect.width() - nWidth);
        }
        if(al & Qt::AlignHCenter)
        {
            int nWidth = q->style()->subElementRect(QStyle::SE_CheckBoxClickRect, &opt, check).width();
            opt.rect.setX(opt.rect.x() + (opt.rect.width() - nWidth) / 2);
        }
    }

private:
    Q_DECLARE_PUBLIC(CheckbleHeaderView)
    CheckbleHeaderView* q_ptr;
    int m_nCurrentPressColumn;
    QMap<int, stColumnData> m_mapColumn2Data;
};

CheckbleHeaderView::CheckbleHeaderView(Qt::Orientation orition, QWidget *parent) :
    QHeaderView(orition, parent),
    d_ptr(new CheckbleHeaderViewPrivate(this))
{

}

CheckbleHeaderView::~CheckbleHeaderView()
{
    delete d_ptr;
}

void CheckbleHeaderView::setCheckbleColumn(const QMap<int, stCheckbleColumnSettings> &columns)
{
    Q_D(CheckbleHeaderView);
    d->m_mapColumn2Data.clear();
    for(auto it = columns.begin(); it != columns.end(); it++)
    {
        d->m_mapColumn2Data[it.key()] = {it.value().canTristate, it.value().text, it.value().al};
    }
    update();
}

QMap<int, stCheckbleColumnSettings> CheckbleHeaderView::getCheckableColumn() const
{
    stCheckbleColumnSettings temp;
    QMap<int, stCheckbleColumnSettings> ret;
    for(auto it = d_func()->m_mapColumn2Data.begin(); it != d_func()->m_mapColumn2Data.end(); ++it){
        temp.al = it->al;
        temp.canTristate = it->canTristate;
        temp.text = it->strText;
        ret[it.key()] = temp;;
    }
    return ret;
}

int CheckbleHeaderView::getCheckState(int col) const
{
    int ret = -1;
    if(d_func()->m_mapColumn2Data.contains(col)){
        ret = d_func()->m_mapColumn2Data.value(col).state;
    }
    return ret;
}

void CheckbleHeaderView::setChecked(int col, int state)
{
    Q_D(CheckbleHeaderView);
    if(!d->m_mapColumn2Data.contains(col))
    {
        return ;
    }
    auto& columnData = d->m_mapColumn2Data[col];
    columnData.state = Qt::CheckState(state);
    //调用此函数重绘
    updateSection(col);
}

void CheckbleHeaderView::mousePressEvent(QMouseEvent *e)
{
    QHeaderView::mousePressEvent(e);
    if(e->button() != Qt::LeftButton)
        return ;
    Q_D(CheckbleHeaderView);
    int nIdx = logicalIndexAt(e->pos());
    if(d->m_mapColumn2Data.contains(nIdx))
    {
        d->m_nCurrentPressColumn = nIdx;
    }
}

void CheckbleHeaderView::mouseReleaseEvent(QMouseEvent *e)
{
    QHeaderView::mouseReleaseEvent(e);
    if(e->button() != Qt::LeftButton)
        return ;
    int nIdx = logicalIndexAt(e->pos());
    Q_D(CheckbleHeaderView);
    if(!d->m_mapColumn2Data.contains(nIdx))
    {
        return ;
    }
    auto& columnData = d->m_mapColumn2Data[nIdx];

    //判断是否在checkbox的点击范围内
    if(!d->contains(nIdx, e->pos()))
    {
        d->m_nCurrentPressColumn = -1;
        return;
    }
    if(columnData.canTristate)
    {
        if(columnData.state == Qt::Unchecked)
        {
            columnData.state = Qt::PartiallyChecked;
        }
        else
        {
            columnData.state = columnData.state == Qt::Checked ? Qt::Unchecked : Qt::Checked;
        }
    }
    else
    {
        columnData.state = columnData.state == Qt::Unchecked ? Qt::Checked : Qt::Unchecked;
    }
    d->m_nCurrentPressColumn = -1;
    //更新界面
    updateSection(nIdx);
    emit sigStateChanged(nIdx, columnData.state);
}

void CheckbleHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    const Q_D(CheckbleHeaderView);
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    //判断该列是否支持复选
    if(!d->m_mapColumn2Data.contains(logicalIndex))
    {
        return;
    }
    const auto& columnData = d->m_mapColumn2Data.value(logicalIndex);
    QCheckBox check((QWidget*)this);
    QStyleOptionButton opt = d->getStyleOption(&check, logicalIndex, columnData, rect);
    style()->drawControl(QStyle::CE_CheckBox, &opt, painter, &check);
}
