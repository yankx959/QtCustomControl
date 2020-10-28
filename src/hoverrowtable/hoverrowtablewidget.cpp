#include "hoverrowtablewidget.h"
#include "drawrowtabledelegate.h"
#include <QHeaderView>
#include <QMouseEvent>
#include <QDebug>
#include <utility>

template<typename Func>
struct FuncGuard{
    Func f;
    FuncGuard(Func&& f):
        f(f){}
    ~FuncGuard(){
        f();
    }
};

template<typename FuncType>
FuncGuard<FuncType> makeFuncGuard(FuncType&& f)
{
    return FuncType(std::forward<FuncType>(f));
}

HoverRowTableWidget::HoverRowTableWidget(QWidget* parent)
    : QTableWidget(parent)
    , m_lastHoverRow(-1)
    , m_lastHoverCol(-1)
    , m_isAlwayMouseTracking(false)
{
    setItemDelegate(new DrawRowTableDelegate(this));
    setHoverRowEnabled(true);
    verticalHeader()->setDefaultSectionSize(30);

    connect(this, &HoverRowTableWidget::cellEntered, this, &HoverRowTableWidget::onCellEntered);
}

HoverRowTableWidget::~HoverRowTableWidget()
{

}

void HoverRowTableWidget::setHoverRowEnabled(bool enabled)
{
    if(m_isAlwayMouseTracking){
        setMouseTracking(true);
    }
    else{
        setMouseTracking(enabled);
    }
//    if(enabled){
//        connect(this, &HoverRowTableWidget::cellEntered, this, &HoverRowTableWidget::onCellEntered, Qt::UniqueConnection);
//    }
//    else{
//        disconnect(this, &HoverRowTableWidget::cellEntered, this, &HoverRowTableWidget::onCellEntered);
//    }
}

void HoverRowTableWidget::setDelegateControlInfo(const QMap<int, stDelegateControlInfo> &infos)
{
    if(DrawRowTableDelegate* delegate = dynamic_cast<DrawRowTableDelegate*>(itemDelegate())){
        delegate->setColumnInputTypes(infos);
    }
}

void HoverRowTableWidget::setDelegateControlInfo(int col, const stDelegateControlInfo &info)
{
    if(DrawRowTableDelegate* delegate = dynamic_cast<DrawRowTableDelegate*>(itemDelegate())){
        delegate->setColumnInputType(col, info);
    }
}

void HoverRowTableWidget::setAlwaysMouseTracking(bool b)
{
    m_isAlwayMouseTracking = b;
    setHoverRowEnabled(true);
}

void HoverRowTableWidget::mouseMoveEvent(QMouseEvent *me)
{
    QTableWidget::mouseMoveEvent(me);
    QModelIndex idx = indexAt(me->pos());
    if(!idx.isValid()){
        onCellEntered(-1, -1);
    }
}

void HoverRowTableWidget::leaveEvent(QEvent *e)
{
    QTableWidget::leaveEvent(e);
    onCellEntered(-1, -1);
}

void HoverRowTableWidget::focusOutEvent(QFocusEvent *e)
{
    QTableWidget::focusOutEvent(e);
    onCellEntered(-1, -1);
}

void HoverRowTableWidget::onCellEntered(int row, int col)
{
    if(m_lastHoverCol != col || m_lastHoverRow != row){
        emit sigCellLeave(m_lastHoverRow, m_lastHoverCol);
    }
    auto t1 = makeFuncGuard([=]{m_lastHoverCol = col;});
    if(m_lastHoverRow == row){
        return ;
    }
    auto t2 = makeFuncGuard([=]{m_lastHoverRow = row;});
    if(!this->hasMouseTracking()){
        return ;
    }
    if(row == -1){
        if(m_lastHoverRow >=0 && m_lastHoverRow < rowCount()){
            item(m_lastHoverRow, 0)->setData(RowHoverRole, false);
            dataChanged(indexFromItem(item(m_lastHoverRow, 0)), indexFromItem(item(m_lastHoverRow, columnCount() - 1)), {RowHoverRole});
        }
        return ;
    }
    if(row >= 0 && row < rowCount()){
        item(row, 0)->setData(RowHoverRole, true);
        dataChanged(indexFromItem(item(row, 0)), indexFromItem(item(row, columnCount() - 1)), {RowHoverRole});
        if(m_lastHoverRow >= 0 && m_lastHoverRow < rowCount()){
            item(m_lastHoverRow, 0)->setData(RowHoverRole, false);
            dataChanged(indexFromItem(item(m_lastHoverRow, 0)), indexFromItem(item(m_lastHoverRow, columnCount() - 1)), {RowHoverRole});
        }
    }
}
