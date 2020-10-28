#include "customdoubleeditor.h"
#include "commonfunction.h"
#include <QDoubleValidator>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStyle>
#include <QDebug>
#include <QMouseEvent>

CustomDoubleEditor::CustomDoubleEditor(QWidget *parent)
    : QLineEdit(parent)
    , m_min(std::numeric_limits<double>::min())
    , m_max(std::numeric_limits<double>::max())
    , m_lastValue(0.f)
    , m_dec(0)
{
    //不用QDoubleValidator的原因是界面可以输入小数点开头的数字
//    setValidator(new QDoubleValidator(std::numeric_limits<double>::min(), std::numeric_limits<double>::max(), 10, this));


    //double精度最大只能达到15-16位有效数字，确定的为15位，所以选择最多输入16个字符
    setMaxLength(16);

    setDecimals(8);

    setProperty("outOfRange", false);

    setAttribute(Qt::WA_StyledBackground);

    connect(this, &CustomDoubleEditor::returnPressed, this, &CustomDoubleEditor::onEditFinished);
    connect(this, &CustomDoubleEditor::editingFinished, this, &CustomDoubleEditor::onEditFinished);
    connect(this, &CustomDoubleEditor::textChanged, this, &CustomDoubleEditor::onTextChanged);
}

double CustomDoubleEditor::validValue() const
{
    double val = value();
    bool isOutofRange = (val > m_max || val < m_min);
    if(isOutofRange){
        val = m_max;
    }
    return val;
}

double CustomDoubleEditor::value() const
{
    return text().toDouble();
}

void CustomDoubleEditor::setValue(double value)
{
    QString srcText = CommonFunction::toString(value, m_dec);//QString::number(value, 'f', m_dec);
//    qDebug() << "====" << value << srcText;
    if(srcText.length() > maxLength()){
        srcText = srcText.left(maxLength());
    }
    if(srcText.contains('.')){
        QRegularExpression rx("(\\.){0,1}0+$");
        srcText.replace(rx, "");
    }
    setText(srcText);
}

void CustomDoubleEditor::setRange(double min, double max)
{
    m_min = min;
    m_max = max;
    jugeIsOutOfRange();
}

void CustomDoubleEditor::setMin(double min)
{
    m_min = min;
    jugeIsOutOfRange();
}

void CustomDoubleEditor::setMax(double max)
{
    m_max = max;
    jugeIsOutOfRange();
}

double CustomDoubleEditor::min() const
{
    return m_min;
}

double CustomDoubleEditor::max() const
{
    return m_max;
}

void CustomDoubleEditor::setDecimals(int count)
{
    m_dec = count;
    if(validator()){
        delete validator();
    }
//    QRegularExpression regExpresstion(QString("^((-?[0])|(-?[1-9][0-9]{0,8}))(?:\\.\\d{0,%1})?$|(^\\t?$)|(^[+-]?[\\d]+([\\.][0-9]{0,%2})?([Ee][+-]?[\\d]+)?$)").arg(count).arg(count));
//    QRegularExpression regExpresstion(QString("^$|(((([1-9]\\d{0,15})|0)(\\.\\d{0,%1})?)|10000000000000000|((([1-9]\\d{0,15})|0)(\\.\\d+)?e[+-]?\\d+))").arg(count));
    QRegularExpression regExpresstion(QString("^-?(([1-9]\\d{0,15})|(0))(((\\.\\d{0,%1})?)|((\\.\\d{1,%2})?([Ee][+-]?\\d{0,8})?))").arg(count).arg(count));
    if(count == 0){
        regExpresstion.setPattern(QString("^(-?[0]|-?[1-9][0-9]{0,15})"));
    }
    setValidator(new QRegularExpressionValidator(regExpresstion, this));
}

bool CustomDoubleEditor::isOutOfRange() const
{
    return property("outOfRange").toBool();
}

void CustomDoubleEditor::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    if(text().isEmpty()){
        setValue(m_lastValue);
    }
    home(false);
}

void CustomDoubleEditor::mouseDoubleClickEvent(QMouseEvent *e)
{
    selectAll();
    e->accept();
}

void CustomDoubleEditor::jugeIsOutOfRange()
{
    double currentValue = value();
    bool isOutofRange = (currentValue > m_max || currentValue < m_min);
    updateOutOfRangeState(isOutofRange);
}

void CustomDoubleEditor::onTextChanged(const QString &text)
{
    if(!text.isEmpty())
        m_lastValue = value();
    jugeIsOutOfRange();
    if(!isOutOfRange()){
        emit sigValidDataChanged(m_lastValue);
    }
}

void CustomDoubleEditor::updateOutOfRangeState(bool isOutOfRange)
{
    bool old = property("outOfRange").toBool();
    if(isOutOfRange != old){
        QString t = text();
        setProperty("outOfRange", isOutOfRange);
        style()->unpolish(this);
        style()->polish(this);
    }
}

void CustomDoubleEditor::onEditFinished()
{
    QString currentText = text();
    if(currentText.endsWith('.')){
        currentText.chop(1);
    }
    setText(currentText);
    double val = value();
    bool isOutofRange = (val > m_max || val < m_min);
    if(!isOutofRange){
        emit sigValidDataEditFinished(val);
    }
}
