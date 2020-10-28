#ifndef CUSTOMDOUBLEEDITOR_H
#define CUSTOMDOUBLEEDITOR_H

#include <QLineEdit>

//这里只有一句代码扩展但是也用继承的原因是在填写范围和提示时区分和QLineEdit的区别
class CustomDoubleEditor : public QLineEdit
{
    Q_OBJECT

public:
    CustomDoubleEditor(QWidget* parent = nullptr);

    double validValue()const;
    double value()const;
    void setValue(double value);

    void setRange(double min, double max);
    void setMin(double min);
    void setMax(double max);
    double min()const;
    double max()const;

    void setDecimals(int count);

    bool isOutOfRange()const;

protected:
    virtual void focusOutEvent(QFocusEvent* e)override;
    virtual void mouseDoubleClickEvent(QMouseEvent* e)override;

private:
    void jugeIsOutOfRange();
    void updateOutOfRangeState(bool isOutOfRange);

signals:
    void sigValidDataEditFinished(double value);
    void sigValidDataChanged(double val);

private slots:
    void onEditFinished();
    void onTextChanged(const QString& text);

private:
    double m_min;
    double m_max;
    double m_lastValue;
    int m_dec;
};

#endif // CUSTOMDOUBLEEDITOR_H
