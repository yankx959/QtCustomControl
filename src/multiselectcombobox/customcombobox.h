#pragma once

#include <QComboBox>
#include <QApplication>
#include <QMouseEvent>

class CustomComboBoxPrivate;
class CustomComboBox : public QComboBox
{
    Q_OBJECT
public:
    CustomComboBox(QWidget *parent = nullptr);
    ~CustomComboBox();

    bool canMultiSelect()const;

    QList<int> selectedItems()const;

    void setMultiSelectEnabled(bool multi);

    void setPopupHidden(bool hide = true);

private:
    void hidePopup() override;

private:
    CustomComboBoxPrivate* d_ptr;
    Q_DECLARE_PRIVATE(CustomComboBox)
};
