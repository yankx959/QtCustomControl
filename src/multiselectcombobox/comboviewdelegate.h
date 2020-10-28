#ifndef COMBOVIEWDELEGATE_H
#define COMBOVIEWDELEGATE_H

#include <QStyledItemDelegate>

enum {
    MultiSelectComboDataRole = Qt::UserRole + 100,
    RowHoverRole,
};

struct stCheckBoxInfo {
    QSize imageSize;
    QString imagePath;
    QString checkedImagePath;
    QString unSelectedPath;
    QString unSelectedCheckedPath;
    bool isChecked{false};
};
Q_DECLARE_METATYPE(stCheckBoxInfo)

class ComboViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboViewDelegate(QObject *parent = nullptr);
    ~ComboViewDelegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    
signals:
    void sigClicked(const QModelIndex& idx);
    void sigItemClicked(const QModelIndex& idx);
    
private:
    QRect calcCheckBoxRect(const QRect &itemRC, const QSize &size) const;
};

#endif // COMBOVIEWDELEGATE_H
