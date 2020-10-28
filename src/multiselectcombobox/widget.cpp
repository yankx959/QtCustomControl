#include "widget.h"
#include "customcombobox.h"
#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QAbstractItemView>

CustomComboBox* combo = nullptr;

widget::widget(QWidget *parent) : QWidget(parent)
{
    combo = new CustomComboBox(this);
    combo->addItems({"1", "2", "3"});
    combo->setMultiSelectEnabled(true);
    combo->setFocus();
    qApp->installEventFilter(this);
}

bool widget::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease){
        if(combo->view()->isVisible()){
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            if(!combo->view()->rect().contains(me->pos())){
                combo->setPopupHidden(true);
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}
