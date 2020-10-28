#include "customtoolbox.h"
#include <QToolButton>
#include <QVBoxLayout>
#include <QList>
#include <QScrollArea>
#include <QDebug>
#include <QVBoxLayout>

struct ItemData : QObjectUserData{
    QIcon icon;
    QIcon expandIcon;
    ItemData(const QIcon& icon, const QIcon& expandIcon)
        : icon(icon)
        , expandIcon(expandIcon)
    {}
};

class CustomToolBox::CustomToolBoxPrivate{
public:
    CustomToolBoxPrivate(CustomToolBox* q)
        : q_ptr(q)
        , layout(nullptr)
        , area(nullptr)
        , itemSpacing(6)
    {}

    void init()
    {
        Q_Q(CustomToolBox);
        area = new QScrollArea(q);
        area->setObjectName("toolbox_area");
        QWidget* widget = new QWidget(area);
        widget->setObjectName("acra_content_widget");
        QVBoxLayout* v = new QVBoxLayout(q);
        v->setContentsMargins(0, 0, 0, 0);
        v->setSpacing(0);
        area->setWidget(widget);
        area->setWidgetResizable(true);
        area->setFrameStyle(QFrame::NoFrame);
        v->addWidget(area);

        layout = new QVBoxLayout(widget);
        layout->setAlignment(Qt::AlignTop);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(itemSpacing);
    }

private:
    void _q_buttonClicked()
    {
        Q_Q(CustomToolBox);
        QObject* from = q->sender();
        QToolButton* btn = dynamic_cast<QToolButton*>(from);

        //获取索引
        int nVirsualIdx = 0;
        for(int i = 0; i < buttons.size(); i++){
            if(buttons.at(i).first == btn){
                nVirsualIdx = i;
                break;
            }
        }

        //获取显示的widget
        auto widget = buttons[nVirsualIdx].second;
        bool expanded = widget->isVisible();

        //互斥非当前项
        if(!expanded){
            for(int i = 0; i < buttons.size(); i++){
                if(buttons.at(i).first != btn){
                    ItemData* data = dynamic_cast<ItemData*>(buttons[i].first->userData(Qt::UserRole));
                    buttons[i].first->setIcon(data->icon);
                    buttons[i].second->setVisible(false);
                }
            }
            int widgetHeight = q->height() - btn->height() * buttons.size()\
                    - (buttons.size() - 1) * layout->spacing()\
                    - q->layout()->contentsMargins().top() - q->layout()->contentsMargins().bottom()
                    - layout->contentsMargins().top() - layout->contentsMargins().bottom();
            widget->setFixedHeight(widgetHeight);
        }
        widget->setVisible(!expanded);
        ItemData* data = dynamic_cast<ItemData*>(btn->userData(Qt::UserRole));
        btn->setIcon(expanded ? data->icon : data->expandIcon);

        if(!expanded)
            emit q->itemExpanded(nVirsualIdx);
    }

private:
    Q_DECLARE_PUBLIC(CustomToolBox)
    CustomToolBox* q_ptr;

private:
    QVBoxLayout* layout;
    QList<QPair<QToolButton*, QWidget*>> buttons;
    QScrollArea* area;
    int itemSpacing;
};



CustomToolBox::CustomToolBox(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new CustomToolBoxPrivate(this))
{
    Q_D(CustomToolBox);
    d->init();
}

int CustomToolBox::addItem(QWidget *widget, const QString &text)
{
    return addItem(widget, QIcon(), QIcon(), text);
}

int CustomToolBox::addItem(QWidget *widget, const QIcon &icon, const QIcon &expandedIcon, const QString &text)
{
    return insertItem(-1, widget, icon, expandedIcon, text);
}

int CustomToolBox::insertItem(int idx, QWidget *widget, const QString &text)
{
    return insertItem(idx, widget, QIcon(), QIcon(), text);
}

int CustomToolBox::insertItem(int idx, QWidget *widget, const QIcon &icon, const QIcon &expanedIcon, const QString &text)
{
    Q_D(CustomToolBox);
    if(!widget || idx > d->buttons.size())
        return -1;
    QToolButton* btn = new QToolButton(this);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btn->setObjectName("customtoolbox_button");
    btn->setText(text);
    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btn->setIcon(icon);
    btn->setUserData(Qt::UserRole, new ItemData{icon, expanedIcon});

    connect(btn, SIGNAL(clicked()), this, SLOT(_q_buttonClicked()));

    //    connect(btn, &QToolButton::clicked, [this,d]{
    //        QToolButton* btn = dynamic_cast<QToolButton*>(sender());
    //        int btnIdx = d->layout->indexOf(btn);
    //        int nVirsualIdx = btnIdx / 2;
    //        auto area = d->buttons[nVirsualIdx].second;
    //        bool expanded = area->isVisible();
    //        area->setVisible(!expanded);
    //        ItemData* data = dynamic_cast<ItemData*>(btn->userData(Qt::UserRole));
    //        btn->setIcon(expanded ? data->icon : data->expandIcon);
    //        if(!expanded)
    //            emit itemExpanded(nVirsualIdx);
    //    });

//    QScrollArea* area = new QScrollArea(this);
//    area->setObjectName("customtoolbox_scrollarea");
//    area->setWidget(widget);
//    area->setWidgetResizable(false);
//    area->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
//    area->hide();
//    area->setFrameStyle(QFrame::NoFrame);
    widget->hide();

    QVBoxLayout* tempLay = new QVBoxLayout;
    tempLay->setSpacing(0);
    tempLay->addWidget(btn);
    tempLay->addWidget(widget);

    int nRealButtonIndex = idx * 2;
    if(idx < 0)
    {
        d->layout->addLayout(tempLay);
        d->buttons.append(qMakePair(btn,widget));
    }
    else
    {
        d->layout->insertLayout(idx, tempLay);
        d->buttons.insert(idx, qMakePair(btn,widget));
    }
    btn->show();
    return idx < 0 ? d->buttons.size() - 1 : idx;
}

int CustomToolBox::indexOf(QWidget *widget) const
{
    const Q_D(CustomToolBox);
    int nIdx = -1;
    for(int i = 0; i < d->buttons.size(); i++){
        if(d->buttons.at(i).second == widget)
        {
            nIdx = i;
            break;
        }
    }
    return nIdx;
}

int CustomToolBox::count() const
{
    const Q_D(CustomToolBox);
    return d->buttons.size();
}

bool CustomToolBox::isItemExpanded(int idx) const
{
    const Q_D(CustomToolBox);
    Q_ASSERT(idx >= 0 && idx < d->buttons.size());
    return d->buttons.at(idx).second->isVisible();
}

bool CustomToolBox::isItemEnabled(int nIdx) const
{
    const Q_D(CustomToolBox);
    Q_ASSERT(nIdx >= 0 && nIdx < d->buttons.size());
    return d->buttons.at(nIdx).first->isEnabled();
}

void CustomToolBox::setItemToolTip(int idx, const QString &text)
{
    Q_D(CustomToolBox);
    Q_ASSERT(idx >= 0 && idx < d->buttons.size());
    d->buttons[idx].first->setToolTip(text);
}

void CustomToolBox::setItemText(int idx, const QString &text)
{
    Q_D(CustomToolBox);
    Q_ASSERT(idx >= 0 && idx < d->buttons.size());
    d->buttons[idx].first->setText(text);
}

void CustomToolBox::setItemIcon(int idx, const QIcon &icon)
{
    Q_D(CustomToolBox);
    Q_ASSERT(idx >= 0 && idx < d->buttons.size());
    d->buttons[idx].first->setIcon(icon);
}

void CustomToolBox::setItemEnabled(int idx, bool enabled)
{
    Q_D(CustomToolBox);
    Q_ASSERT(idx >= 0 && idx < d->buttons.size());
    d->buttons[idx].first->setEnabled(enabled);
}

void CustomToolBox::setItemExpanded(int idx, bool expanded)
{
    Q_D(CustomToolBox);
    Q_ASSERT(idx >= 0 && idx < d->buttons.size());
    auto widget = d->buttons[idx].second;
    auto btn  = d->buttons[idx].first;
    widget->setVisible(true);
    ItemData* data = dynamic_cast<ItemData*>(btn->userData(Qt::UserRole));
    btn->setIcon(expanded ? data->icon : data->expandIcon);
}

void CustomToolBox::setItemSpacing(int spacing)
{
    Q_D(CustomToolBox);
    d->itemSpacing = spacing;
    d->layout->setSpacing(spacing);
}

int CustomToolBox::itemSpacing() const
{
    const Q_D(CustomToolBox);
    return d->itemSpacing;
}

void CustomToolBox::expandItem(int idx)
{
    Q_D(CustomToolBox);
    Q_ASSERT(idx >= 0 && idx < d->buttons.size());
    d->buttons[idx].first->click();
}


#include "debug/moc_customtoolbox.cpp"
