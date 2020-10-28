#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class widget : public QWidget
{
    Q_OBJECT
public:
    explicit widget(QWidget *parent = nullptr);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event)override;

signals:

};

#endif // WIDGET_H
