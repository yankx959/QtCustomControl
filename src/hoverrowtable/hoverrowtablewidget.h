#ifndef HOVERROWTABLEWIDGET_H
#define HOVERROWTABLEWIDGET_H

#include <QTableWidget>

struct stDelegateControlInfo;
class HoverRowTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    HoverRowTableWidget(QWidget* parent = nullptr);
    virtual ~HoverRowTableWidget();

    void setHoverRowEnabled(bool enabled);

    void setDelegateControlInfo(const QMap<int, stDelegateControlInfo>& infos);

    void setDelegateControlInfo(int col, const stDelegateControlInfo& info);

    void setAlwaysMouseTracking(bool b);

protected:
    virtual void mouseMoveEvent(QMouseEvent* me)override;
    virtual void leaveEvent(QEvent* e)override;
    virtual void focusOutEvent(QFocusEvent* e);

signals:
    void sigCellLeave(int row, int col);

private slots:
    void onCellEntered(int row, int col);

private:
    int m_lastHoverRow;
    int m_lastHoverCol;
    bool m_isAlwayMouseTracking;
};

#endif // HOVERROWTABLEWIDGET_H
