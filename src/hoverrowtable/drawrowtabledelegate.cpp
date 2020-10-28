#include "drawrowtabledelegate.h"
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QMap>
#include <QPainter>
#include <QDebug>
#include <QComboBox>
#include <QApplication>
#include <QCheckBox>

void DrawRowTableDelegate::setColumnInputTypes(const QMap<int, stDelegateControlInfo> &types)
{
//    if(!combo){

//        combo = new QComboBox;
//    }
//    combo->setStyleSheet("background-color:transparent;color:black;border:none;");
    m_inputTypes = types;
}

void DrawRowTableDelegate::setColumnInputType(int col, const stDelegateControlInfo &type)
{
    m_inputTypes[col] = type;
}

void DrawRowTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    qDebug() << option.state;
    if(!index.data(Qt::BackgroundRole).isValid()){
        QStyledItemDelegate::paint(painter, option, index);
    }

    QModelIndex idxCol0 = index.model()->index(index.row(), 0);
    //背景
    if((option.state & QStyle::State_Selected)
    || idxCol0.data(RowHoverRole).isValid() && idxCol0.data(RowHoverRole).toBool()){
        if(1){
            bool hover = idxCol0.data(RowHoverRole).isValid() && idxCol0.data(RowHoverRole).toBool();
            drawRowBackground(painter, option, index,
                              (hover && !option.state.testFlag(QStyle::State_Selected)) ? QBrush(Qt::blue)
                                                            : QBrush(Qt::black));
            if(index.data(Qt::CheckStateRole).isValid()){
                drawCheckIndicator(painter, option, index);
            }
            QString text = index.data(Qt::DisplayRole).toString();
            if(!text.isEmpty()){
                text = option.fontMetrics.elidedText(text, Qt::ElideRight, option.rect.width());
                painter->setFont(option.font);
                painter->setPen(QPen(QColor(Qt::white), 1));
                painter->drawText(option.rect.adjusted(0, -1, 0, -1), Qt::AlignCenter, text);
            }
        }
    }

    //这里手动绘制是因为样式表里面设置了item的交替背景，导致setBackground设置的背景绘制不出来
    if(index.data(Qt::BackgroundRole).isValid()){
        QBrush brush = index.data(Qt::BackgroundRole).value<QBrush>();
        painter->save();
        painter->fillRect(option.rect, brush);
        painter->setRenderHints(QPainter::Antialiasing);
        painter->setFont(option.font);
        if(!option.state.testFlag(QStyle::State_Enabled)){
            QPen pen = painter->pen();
            pen.setColor(QColor(160, 160, 164));
            painter->setPen(pen);
        }
        painter->drawText(option.rect.adjusted(0, -1, 0, -1), Qt::AlignCenter, index.data().toString());
        if(index.data(Qt::CheckStateRole).isValid()){
            drawCheckIndicator(painter, option, index);
        }
        painter->restore();
    }

    auto it = m_inputTypes.find(index.column());
    if(it != m_inputTypes.end() && it.value().type == ColumnInputType::ComboBox){
        painter->save();

        QString imageDir = QApplication::applicationDirPath();
        QPixmap arrow = QPixmap(imageDir + "/control/downarrow.png");
        if(option.state.testFlag(QStyle::State_Selected)){
            arrow = QPixmap(imageDir + "/control/downarrowselected.png");
        }
        //多减一个像素与editor对齐
        int arrowRCRight = arrow.width() + 10 - 1;
        QRect arrowRect = QRect(option.rect.right() - arrowRCRight,
                                option.rect.y() + (option.rect.height() - arrow.height())/2,
                                arrow.width(), arrow.height());
        painter->drawPixmap(arrowRect, arrow);
        QVariant var = index.data(ComboCurrentTextRole);
        if(var.isValid()){
            QString text = var.toString();
            text = option.fontMetrics.elidedText(text, Qt::ElideRight, option.rect.width() - arrowRCRight);
            painter->setFont(option.font);
            bool selected = option.state & QStyle::State_Selected;
            painter->setPen(QPen(selected ? QColor(Qt::white)
                                          : QColor(Qt::black)));
            painter->drawText(option.rect.adjusted(0, 0, -arrowRCRight, 0), Qt::AlignCenter, text);
        }
        painter->restore();
    }
    //draw error border
    if(index.data(RowBorderColorRole).isValid() && index.data(RowBorderColorRole).canConvert<QColor>()){
        QColor borderColor = index.data(RowBorderColorRole).value<QColor>();
        drawRowLines(painter, option, index, borderColor);
    }

    if(0){
        return ;
    }
    if((option.state & QStyle::State_Selected)
    || (idxCol0.data(RowHoverRole).isValid() && idxCol0.data(RowHoverRole).toBool())){
        QColor color = Qt::red;
        drawRowLines(painter, option, index, color);
    }
}

QWidget *DrawRowTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget* ret = nullptr;
    auto it = m_inputTypes.find(index.column());
    if(it != m_inputTypes.end()){
        switch (it.value().type) {
        case ColumnInputType::Double:
        {
            QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
            editor->setDecimals(8);
            editor->setStyleSheet("padding-left:0px; min-height:0px; max-height:900px;");
            editor->setRange(it.value().validMin, it.value().validMax);
            editor->setToolTip(QString("[%1-%2]").arg(it.value().validMin).arg(it.value().validMax));
            editor->setAlignment(static_cast<Qt::Alignment>(index.data(Qt::TextAlignmentRole).toInt()));
//            qDebug() << editor->alignment();
            ret = editor;
        }
            break;
        case ColumnInputType::Int:
        {
            QDoubleSpinBox* spin = new QDoubleSpinBox(parent);
            spin->setStyleSheet("padding-left:0px; min-height:0px; max-height:900px;");
            spin->setAlignment(static_cast<Qt::Alignment>(index.data(Qt::TextAlignmentRole).toInt()));
            spin->setRange(it.value().validMin, it.value().validMax);
            spin->setToolTip(QString("[%1-%2]").arg(int(it.value().validMin)).arg(int(it.value().validMax)));
            spin->setDecimals(0);
            ret = spin;
        }
            break;
        case ColumnInputType::String:
        {
            QLineEdit* edit = new QLineEdit(parent);
            edit->setStyleSheet("padding-left:0px; min-height:0px; max-height:900px;");
            if(it.value().disableChinese){
                QRegularExpression reg("[a-zA-Z0-9_]+");
                edit->setValidator(new QRegularExpressionValidator(reg, edit));
            }
            edit->setAlignment(static_cast<Qt::Alignment>(index.data(Qt::TextAlignmentRole).toInt()));
            ret = edit;
        }
            break;
        case ColumnInputType::ComboBox:
        {
            QComboBox* combo = new QComboBox(parent);
            for(auto&& pair : it.value().comboItems){
                combo->addItem(pair.first, pair.second);
            }
            combo->setStyleSheet("padding-left:0px; min-height:0px; max-height:900px;");
            ret = combo;
        }
            break;
        case ColumnInputType::Null:
        {
            ret = nullptr;
        }
            break;
        default:
            ret = QStyledItemDelegate::createEditor(parent, option, index);
            break;
        }
    }
    else{
        ret = QStyledItemDelegate::createEditor(parent, option, index);
    }
    return ret;
}

void DrawRowTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    auto it = m_inputTypes.find(index.column());
    if(it != m_inputTypes.end()){
        switch (it.value().type) {
        case ColumnInputType::Double:
        {
            QDoubleSpinBox* dbEditor = qobject_cast<QDoubleSpinBox*>(editor);
            dbEditor->setValue(index.data(Qt::DisplayRole).toDouble());
        }
            break;
        case ColumnInputType::Int:
        {
            QDoubleSpinBox* intEditor = qobject_cast<QDoubleSpinBox*>(editor);
            intEditor->setValue(index.data(Qt::DisplayRole).toInt());
        }
            break;
        case ColumnInputType::String:
        {
            QLineEdit* strEditor = qobject_cast<QLineEdit*>(editor);
            strEditor->setText(index.data(Qt::DisplayRole).toString());
        }
            break;
        case ColumnInputType::ComboBox:
        {
            QComboBox* combo = qobject_cast<QComboBox*>(editor);
            QVariant var = index.data(ComboCurrentTextRole);
            combo->setCurrentIndex(var.isValid() ? combo->findText(var.toString()) : -1);
        }
            break;
        default:
            QStyledItemDelegate::setEditorData(editor, index);
            break;
        }
    }
    else{
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void DrawRowTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    auto it = m_inputTypes.find(index.column());
    if(it != m_inputTypes.end()){
        switch (it.value().type) {
        case ColumnInputType::Double:
        {
            QDoubleSpinBox* dbEditor = qobject_cast<QDoubleSpinBox*>(editor);
            double val = dbEditor->value();
            QString text = QString::number(val);
            model->setData(index, text);
        }
            break;
        case ColumnInputType::Int:
        {
            QDoubleSpinBox* intEditor = qobject_cast<QDoubleSpinBox*>(editor);
            int val = intEditor->value();
            model->setData(index, val);
        }
            break;
        case ColumnInputType::String:
        {
            QLineEdit* strEditor = qobject_cast<QLineEdit*>(editor);
            model->setData(index, strEditor->text());
        }
            break;
        case ColumnInputType::ComboBox:
        {
            QComboBox* combo = qobject_cast<QComboBox*>(editor);
            model->setData(index, combo->currentText(), ComboCurrentTextRole);
//            model->setData(index, combo->currentText(), Qt::DisplayRole);

//            qDebug() << "set model data" << index.data();
            QVariant var;
            if(combo->currentIndex() != -1){
                var = combo->currentData();
            }
            model->setData(index, var, DelegateSpecialValueRole);
        }
            break;
        default:
            QStyledItemDelegate::setModelData(editor, model, index);
            break;
        }
    }
    else{
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void DrawRowTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    int col = index.column();
//    auto it = m_inputTypes.find(col);
//    if(it != m_inputTypes.end() && it.value().type == ColumnInputType::ComboBox){
        editor->setGeometry(option.rect);
//    }
//    else{
//        QStyledItemDelegate::updateEditorGeometry(editor, option, index);
        //    }
}

QVector<QLineF> DrawRowTableDelegate::getRowLines(const QStyleOptionViewItem &option, const QModelIndex& index) const
{
    QVector<QLineF> lines;
    lines << QLineF{option.rect.topLeft(), option.rect.topRight()}
          << QLineF{option.rect.bottomLeft(), option.rect.bottomRight()};
    if(index.column() == 0){
        lines << QLineF{option.rect.topLeft(), option.rect.bottomLeft()};
    }
    else if(index.column() == index.model()->columnCount() - 1){
        lines << QLineF{option.rect.topRight(), option.rect.bottomRight()};
    }
    return lines;
}

void DrawRowTableDelegate::drawRowLines(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex& index, const QColor &lineColor) const
{
    painter->save();
    QVector<QLineF> lines = getRowLines(option, index);
    painter->setPen(QPen(lineColor, 1));
    painter->drawLines(lines);
    painter->restore();
}

void DrawRowTableDelegate::drawRowBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, const QBrush &brush) const
{
    Q_UNUSED(index);
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(brush);
    painter->drawRect(option.rect);
    painter->restore();
}

void DrawRowTableDelegate::drawCheckIndicator(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex& index) const
{
    QStyleOptionButton opt;
    opt.initFrom(option.widget);
    opt.rect = option.rect;
    //获取表格勾选框大小和位置
    QRect indicatorRC = qApp->style()->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &opt, option.widget);
    if(!m_pCheckBox){
        m_pCheckBox = new QCheckBox(const_cast<QWidget*>(option.widget));
    }
    opt.initFrom(m_pCheckBox);
    //与表头对齐
    opt.rect = QRect(indicatorRC.x() + 2, indicatorRC.y(), indicatorRC.width(), indicatorRC.height());
    int state = index.data(Qt::CheckStateRole).value<Qt::CheckState>();
    opt.state = option.state;
    if(state == Qt::Checked){
        opt.state |= QStyle::State_On;
    }
    else{
        opt.state |= QStyle::State_Off;
    }
    opt.state = opt.state &~ QStyle::State_MouseOver;
//            qDebug() << option.state.testFlag(QStyle::State_MouseOver) << opt.state.testFlag(QStyle::State_MouseOver);
    opt.text = index.data().toString();
    qApp->style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &opt, painter, m_pCheckBox);
}

