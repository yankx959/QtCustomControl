#ifndef DRAWROWTABLEDELEGATE_H
#define DRAWROWTABLEDELEGATE_H

#include <QStyledItemDelegate>
#include <QVector>
#include <QMap>

enum {
    RowHoverRole = Qt::UserRole + 0x1000,
    ComboCurrentTextRole,
    RowBorderColorRole,
    DelegateSpecialValueRole,
};

//代理输入类型
enum class ColumnInputType{
    Null,
    Double,
    Int,
    String,
    ComboBox,
};

struct stDelegateControlInfo{
    ColumnInputType type;
    double validMin;
    double validMax;
    bool disableChinese;
    QVector<QPair<QString, QVariant>> comboItems;   //text，value
};
Q_DECLARE_METATYPE(stDelegateControlInfo)

class QCheckBox;
class DrawRowTableDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void setColumnInputTypes(const QMap<int, stDelegateControlInfo>& infos);

    void setColumnInputType(int col, const stDelegateControlInfo& info);

    // QAbstractItemDelegate interface
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QVector<QLineF> getRowLines(const QStyleOptionViewItem& option, const QModelIndex &index)const;
    void drawRowLines(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex &index, const QColor& lineColor)const;
    void drawRowBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex &index, const QBrush& brush)const;
    void drawCheckIndicator(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex &index)const;

private:
    QMap<int, stDelegateControlInfo> m_inputTypes;
    mutable QCheckBox* m_pCheckBox = nullptr;
};

#endif // DRAWROWTABLEDELEGATE_H
