#ifndef CUSTOMSPINBOX_H
#define CUSTOMSPINBOX_H

#include <QSpinBox>

class CustomSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    CustomSpinBox(QWidget* parent = nullptr);

    void setValidateRange(int min, int max);

    void setValidateMin(int min);
    void setValidateMax(int max);

    bool isOutOfRange()const;

protected:
    virtual void wheelEvent(QWheelEvent* e)override;
    void focusInEvent(QFocusEvent *event) override;

private:
    void updateOutofRangestate(bool isOutOfRange);
    bool judgeIsOutOfRange();

signals:
    void sigValidValueChanged(int val);
    void sigValidValueEditFinished(int val);
    void sigFocusIn();

private slots:
    void onValueChanged(int val);
    void onEditFinished();

private:
    int m_validateMin;
    int m_validateMax;
};

#endif // CUSTOMSPINBOX_H
