#include "customcombobox.h"
#include "comboviewdelegate.h"
#include <QAbstractItemView>
#include <QDebug>

class CustomComboBoxPrivate{
    Q_DECLARE_PUBLIC(CustomComboBox)

public:
    CustomComboBoxPrivate(CustomComboBox* q)
        : q_ptr(q){}

private:
    CustomComboBox* q_ptr;
    bool callParenthidePopup = true;
    bool canMultiSelect = false;
};

CustomComboBox::CustomComboBox(QWidget *parent)
    : QComboBox(parent)
    , d_ptr(new CustomComboBoxPrivate(this))
{
    this->view()->viewport()->installEventFilter(this);
    this->view()->setItemDelegate(new ComboViewDelegate(this));
}

CustomComboBox::~CustomComboBox()
{
    delete d_ptr;
}

bool CustomComboBox::canMultiSelect() const
{
    return d_func()->canMultiSelect;
}

QList<int> CustomComboBox::selectedItems() const
{
    QList<int> ret;
    for(int i = 0; i < count(); ++i){
        if(itemData(i, MultiSelectComboDataRole).isValid()
       && itemData(i, MultiSelectComboDataRole).canConvert<stCheckBoxInfo>()){
            if(itemData(i, MultiSelectComboDataRole).value<stCheckBoxInfo>().isChecked){
                ret << i;
            }
        }
    }
    return ret;
}

void CustomComboBox::setMultiSelectEnabled(bool multi)
{
    if(d_func()->canMultiSelect == multi){
        return ;
    }
    d_func()->canMultiSelect = multi;
    QVariant var;
    d_func()->callParenthidePopup = !multi;
    if(multi){
        stCheckBoxInfo info;
        info.checkedImagePath = ":/image/combochecked.png";
        info.imagePath = ":/image/combounchecked.png";
        info.unSelectedPath = ":/image/combouncheckedb.png";
        info.unSelectedCheckedPath = ":/image/combocheckedb.png";
        info.imageSize = QImage(info.imagePath).size();
        info.isChecked = false;
        var = QVariant::fromValue(info);
    }
    for(int i = 0; i < count(); ++i){
        setItemData(i, var, MultiSelectComboDataRole);
    }
    if(count() && var.isValid()){
        stCheckBoxInfo info = var.value<stCheckBoxInfo>();
        info.isChecked = true;
        setItemData(0, QVariant::fromValue(info), MultiSelectComboDataRole);
    }
}

void CustomComboBox::setPopupHidden(bool hide)
{
    d_func()->callParenthidePopup = hide;
    hidePopup();
}

void CustomComboBox::hidePopup()
{
    if(!d_func()->canMultiSelect){
        QComboBox::hidePopup();
        return ;
    }
    if(d_func()->callParenthidePopup) {
        QComboBox::hidePopup();
        d_func()->callParenthidePopup = false;
    }
}
