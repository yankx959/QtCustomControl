#ifndef CUSTOMTOOLBOX_H
#define CUSTOMTOOLBOX_H

#include <QWidget>
#include <QScopedPointer>

class CustomToolBox : public QWidget
{
    Q_OBJECT
public:
    explicit CustomToolBox(QWidget *parent = 0);

    int addItem(QWidget* widget, const QString& text);
    int addItem(QWidget *widget, const QIcon& icon, const QIcon& expandedIcon,const QString &text);
    int insertItem(int idx, QWidget* widget, const QString& text);
    int insertItem(int idx, QWidget* widget, const QIcon& icon, const QIcon& expanedIcon, const QString& text);
    int indexOf(QWidget* widget)const;
    int count()const;

    bool isItemExpanded(int idx)const;
    bool isItemEnabled(int nIdx)const;
    void setItemToolTip(int idx, const QString& text);
    void setItemText(int idx, const QString& text);
    void setItemIcon(int idx, const QIcon& icon);
    void setItemEnabled(int idx, bool enabled);
    void setItemExpanded(int idx, bool expanded);

    void setItemSpacing(int spacing);
    int itemSpacing()const;

signals:
    void itemExpanded(int idx);

public slots:
    void expandItem(int idx);

private:
    class CustomToolBoxPrivate;
    Q_DECLARE_PRIVATE(CustomToolBox)
    QScopedPointer<CustomToolBoxPrivate> d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_buttonClicked())
};

#endif // CUSTOMTOOLBOX_H
