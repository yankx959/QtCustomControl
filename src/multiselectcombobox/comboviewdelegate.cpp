#include "comboviewdelegate.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QAbstractItemModel>

ComboViewDelegate::ComboViewDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

ComboViewDelegate::~ComboViewDelegate()
{
}

void ComboViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    QModelIndex idx(index);
//    auto model = const_cast<QAbstractItemModel*>(idx.model());
//    if(!model->data(idx, ComboItemDataRole).isValid()
//     || model->data(idx, Qt::DisplayRole).toString() != model->data(idx, ComboItemDataRole).toString()){
//        model->setData(idx, idx.data(Qt::DisplayRole), ComboItemDataRole);
//    }
//    model->setData(idx, QString(), Qt::DisplayRole);
//    QStyledItemDelegate::paint(painter, option, idx);

//    painter->save();
//    if(option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver){
//        painter->setPen(StyleHelper::getSelectedColor(StyleItemType::ListView));
//    }
//    else{
//        painter->setPen(StyleHelper::getColor(StyleItemType::ListView));
//    }
//    painter->drawText(option.rect.adjusted(leflSpacing, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, idx.data(ComboItemDataRole).toString());
    QStyledItemDelegate::paint(painter, option, index);
    painter->save();
    if(index.data(MultiSelectComboDataRole).isValid()
    && index.data(MultiSelectComboDataRole).canConvert<stCheckBoxInfo>()) {
        auto info = index.data(MultiSelectComboDataRole).value<stCheckBoxInfo>();

        QString path;
        if(info.isChecked){
            path = index.data(RowHoverRole).toBool() ? info.checkedImagePath : info.unSelectedCheckedPath;
        }
        else{
            path = index.data(RowHoverRole).toBool() ? info.imagePath : info.unSelectedPath;
        }

//        qDebug() << "draw" << index.row() << index.data(RowHoverRole) << path;

        QPixmap pix(path);
        QRect rc = calcCheckBoxRect(option.rect, pix.size());
        painter->drawPixmap(rc, pix);
    }

//    if(model->data(idx, Qt::DisplayRole).toString() != model->data(idx, ComboItemDataRole).toString()){
//        model->setData(idx, idx.data(ComboItemDataRole), Qt::DisplayRole);
//    }
    painter->restore();
}

bool ComboViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        if(index.data(MultiSelectComboDataRole).isValid()
         &&index.data(MultiSelectComboDataRole).canConvert<stCheckBoxInfo>()) {
            auto info = index.data(MultiSelectComboDataRole).value<stCheckBoxInfo>();
            info.isChecked = !info.isChecked;
            model->setData(index, QVariant::fromValue(info), MultiSelectComboDataRole);
            return true;
        }
        break;
    }
    case QEvent::MouseMove:{
        if(index.data(MultiSelectComboDataRole).isValid()
         &&index.data(MultiSelectComboDataRole).canConvert<stCheckBoxInfo>()) {
            model->setData(index, true, RowHoverRole);
            for(int i = 0; i < model->rowCount(); ++i){
                if(i == index.row()){
                    continue ;
                }
                model->setData(model->index(i, index.column()), false, RowHoverRole);
            }
            return true;
        }
        break;
    }
    default:
        break;
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QRect ComboViewDelegate::calcCheckBoxRect(const QRect &itemRC, const QSize &size) const
{
    int spacing = (itemRC.height() - size.height()) / 2;
    return QRect(itemRC.right() - spacing - size.width(), spacing + itemRC.y()
                 , size.width(), size.height());
}
