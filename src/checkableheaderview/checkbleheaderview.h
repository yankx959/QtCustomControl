#ifndef CHECKBLEHEADERVIEW_H
#define CHECKBLEHEADERVIEW_H

#include <QHeaderView>
#include <QMap>
#include <QPair>

struct stCheckbleColumnSettings{
    bool canTristate;   //是否支持三态
    QString text;       //显示的文本
    Qt::Alignment al;   //对齐方式
    stCheckbleColumnSettings(){
        canTristate = false;
        al = Qt::AlignCenter;
    }
    stCheckbleColumnSettings(bool canTristate, const QString& text, Qt::Alignment al = Qt::AlignLeft | Qt::AlignVCenter)
        : canTristate(canTristate), text(text), al(al){}
};

class CheckbleHeaderViewPrivate;
class CheckbleHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    explicit CheckbleHeaderView(Qt::Orientation orition, QWidget *parent = 0);
    ~CheckbleHeaderView();

    void setCheckbleColumn(const QMap<int,stCheckbleColumnSettings>& columns);
    QMap<int,stCheckbleColumnSettings> getCheckableColumn()const;

    int getCheckState(int col)const;

signals:
    void sigStateChanged(int nCol, int state);

public slots:
    void setChecked(int col, int state);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    // QHeaderView interface
protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

private:
    Q_DECLARE_PRIVATE(CheckbleHeaderView)
    CheckbleHeaderViewPrivate* d_ptr;
};

#endif // CHECKBLEHEADERVIEW_H
