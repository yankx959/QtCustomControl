#include "customspinbox.h"
#include <QStyle>
#include <QWheelEvent>

CustomSpinBox::CustomSpinBox(QWidget *parent)
    : QSpinBox(parent)
{
    setButtonSymbols(QAbstractSpinBox::NoButtons);
    setProperty("outOfRange", false);
//    setFocusPolicy(Qt::StrongFocus);
    connect(this, static_cast<void(CustomSpinBox::*)(int)>(&CustomSpinBox::valueChanged), this, &CustomSpinBox::onValueChanged);
    connect(this, &CustomSpinBox::editingFinished, this, &CustomSpinBox::onEditFinished);
}

void CustomSpinBox::setValidateRange(int min, int max)
{
    m_validateMin = min;
    m_validateMax = max;
    judgeIsOutOfRange();
}

void CustomSpinBox::setValidateMin(int min)
{
    m_validateMin = min;
    judgeIsOutOfRange();
}

void CustomSpinBox::setValidateMax(int max)
{
    m_validateMax = max;
    judgeIsOutOfRange();
}

bool CustomSpinBox::isOutOfRange() const
{
    return property("outOfRange").toBool();
}

void CustomSpinBox::wheelEvent(QWheelEvent *e)
{
    e->ignore();
}
void CustomSpinBox::focusInEvent(QFocusEvent *event)
{
    emit sigFocusIn();
    QSpinBox::focusInEvent(event);
}

void CustomSpinBox::updateOutofRangestate(bool isOutOfRange)
{
    bool old = property("outOfRange").toBool();
    if(isOutOfRange != old){
        setProperty("outOfRange", isOutOfRange);
        style()->unpolish(this);
        style()->polish(this);
    }
}

bool CustomSpinBox::judgeIsOutOfRange()
{
    bool outOfRange = value() > m_validateMax || value() < m_validateMin;
    updateOutofRangestate(outOfRange);
    return outOfRange;
}

void CustomSpinBox::onValueChanged(int val)
{
    if(!judgeIsOutOfRange()){
        emit sigValidValueChanged(val);
    }
}

void CustomSpinBox::onEditFinished()
{
    if(!judgeIsOutOfRange()){
        emit sigValidValueEditFinished(value());
    }
}
