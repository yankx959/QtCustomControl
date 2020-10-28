#include "checkbleheaderview.h"
#include "switchbutton.h"
#include "circlemenu.h"
#include "curvegraphwidget.h"
#include "customtoolbox.h"
#include "flowlayout.h"
#include "hoverrowtablewidget.h"
#include "widget.h"
#include "roundbutton.h"
#include "roundprogressbar.h"
#include "whiteboarddlg.h"
#include "trapetabbar.h"
#include "ProgressBar.h"
#include "waveprogressbar.h"
#include <QTableWidget>
#include <QApplication>
#include <QFileInfo>
#include <QLinearGradient>
#include <QPushButton>
#include <QMetaEnum>
#include <QMessageBox>
#include <QTimer>

void test_checkableHeaderView();

void test_switchButton();

void test_circlemenu();

void test_curveWidget();

void test_customToolBox();

void test_flowLayout();

void test_hoverRowTable();

void test_multiSelectComboBox();

void test_roundButton();

void test_roundProgressBar();

void test_simpleWhiteBoard();

void test_trapeTabBar();

void test_volProgressBar();

void test_waveProgressBar();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    test_checkableHeaderView();
    test_switchButton();
    test_circlemenu();
    test_curveWidget();
    test_customToolBox();
    test_flowLayout();
    test_hoverRowTable();
    test_multiSelectComboBox();
    test_roundButton();
    test_roundProgressBar();
    test_simpleWhiteBoard();
    test_trapeTabBar();
    test_volProgressBar();
    test_waveProgressBar();
    return a.exec();
}

void test_checkableHeaderView()
{
    QTableWidget* w = new QTableWidget;
    w->setColumnCount(2);
    w->setRowCount(2);

    CheckbleHeaderView* view = new CheckbleHeaderView(Qt::Horizontal);
    view->setCheckbleColumn({{0, {true, "1", Qt::AlignCenter}}});
    w->setHorizontalHeader(view);

    w->resize(400, 400);
    w->setWindowTitle(__FUNCTION__);
    w->show();
}

void test_switchButton()
{
    SwitchButton* btn = new SwitchButton;
    btn->setRectBrush(Qt::red, Qt::green);
    btn->setCircleBrush(Qt::yellow);
    btn->resize(400, 200);
    btn->setWindowTitle(__FUNCTION__);
    btn->show();
}

void test_circlemenu()
{
    CircleMenu* circleMenu = new CircleMenu;
    circleMenu->setArrowVisible(true);
    circleMenu->setRotateSpeed(0.6);
    circleMenu->setAnimationDuration(200);
    circleMenu->setRotateAnimationEnabled(true);

    circleMenu->setShowPopup(false);

    QVector<QImage> images;
    QStringList tips;
    for(int i = 0; i < 6; ++i){
        QString imagePath = QString(":/image/img%1").arg(i+1);
        images << QImage(imagePath);
        tips << QFileInfo(imagePath).baseName();
    }
    circleMenu->setAreas(images, tips);
    circleMenu->setWindowTitle(__FUNCTION__);
    circleMenu->show();
}

void test_curveWidget()
{
    CurveGraphWidget* curveWidget = new CurveGraphWidget;
    curveWidget->addCurve({0, 1, 2}, {"1", "2", "3"});
    curveWidget->addData(0, {0, 1, 2, 3, 4}, {1, 2, 3, 4, 5});
    curveWidget->addData(1, {0, 1, 2, 3, 4}, {3, 4, 5, 6, 7});
    curveWidget->addData(2, {0, 1, 2, 3, 4}, {5, 6, 7, 8, 9});
    curveWidget->setAxisText(true, "x");
    curveWidget->setAxisText(false, "y");
    for(int i = 0; i < curveWidget->curveCount(); ++i){
        QLinearGradient line;
        line.setColorAt(0, Qt::GlobalColor(Qt::red + i));
        line.setColorAt(1, Qt::transparent);
        curveWidget->setCurveBrush(i, QBrush(line));
    }
    curveWidget->setStyleSheet("QPushButton{background-color:rgba(255, 0, 0, 120); "
                               "border:1px solid black;"
                               "border-radius:3px;"
                               "min-width:60px;"
                               "}"
                               "QFrame#frame_legend_container{"
                               "margin-right:30px;"
                               "}");
    curveWidget->resize(700, 500);
    curveWidget->adjustAxesSize();
    curveWidget->setWindowTitle(__FUNCTION__);
    curveWidget->show();
}

void test_customToolBox()
{
    CustomToolBox* toolBox = new CustomToolBox;
    for(int i = 0; i < 10; ++i){
        toolBox->addItem(new QWidget, QString::number(i));
    }
    toolBox->resize(400, 600);
    toolBox->setWindowTitle(__FUNCTION__);
    toolBox->show();
}

void test_flowLayout()
{
    QWidget* widget = new QWidget;
    FlowLayout* flow = new FlowLayout(widget);
    for(int i = 0; i < 10; ++i){
        QPushButton* btn = new QPushButton(widget);
        btn->setText(QString::number(i));
        flow->addWidget(btn);
    }
    widget->resize(400, 600);
    widget->setWindowTitle(__FUNCTION__);
    widget->show();
}

void test_hoverRowTable()
{
    HoverRowTableWidget* w = new HoverRowTableWidget;
    w->setHoverRowEnabled(true);
    w->setColumnCount(3);
    w->setRowCount(3);
    for(int r = 0; r < w->rowCount(); ++r){
        for(int c = 0; c < w->columnCount(); ++c){
            w->setItem(r, c, new QTableWidgetItem);
        }
    }
    w->horizontalHeader()->setStretchLastSection(true);
    w->resize(400, 400);
    w->setWindowTitle(__FUNCTION__);
    w->show();
}

void test_multiSelectComboBox()
{
    widget* w = new widget;
    w->resize(400, 100);
    w->setWindowTitle(__FUNCTION__);
    w->show();
}

void test_roundButton()
{
    RoundBtn* btn = new RoundBtn;
    btn->connect(btn, &RoundBtn::sigClicked, btn, [=](RoundBtn::Direction dir){
        auto var = QMetaEnum::fromType<decltype(dir)>();
        QMessageBox::information(btn, "clicked", var.valueToKey(dir));
    });
    btn->resize(400, 400);
    btn->setWindowTitle(__FUNCTION__);
    btn->show();
}

void test_roundProgressBar()
{
    RoundProgress* w = new RoundProgress;
    QTimer* t = new QTimer;
    t->setInterval(100);
    t->connect(t, &QTimer::timeout, [=]{
        int value = w->value() + 1;
        if(value > 100){
            value = 0;
        }
        w->setValue(value);
    });
    t->start();
    w->resize(400, 400);
    w->setWindowTitle(__FUNCTION__);
    w->show();
}

void test_simpleWhiteBoard()
{
    WhiteBoardDlg* w = new WhiteBoardDlg;
    w->resize(400, 400);
    w->setWindowTitle(__FUNCTION__);
    w->show();
}

void test_trapeTabBar()
{
    TrapeTabBar* tabBar = new TrapeTabBar;
    tabBar->addTab("1");
    tabBar->addTab("23333");
    //设置宽度统一，否则自适应
//    tabBar->setUniformWidth(80);
    //设置三角形长度
    tabBar->setTriangleLength(10);
    //设置左边距
    tabBar->setLeftMargin(30);
    //设置文本边距
    tabBar->setTextMargin(10);
    //设置Y
    tabBar->setStartY(5);
    //设置梯形间距
    tabBar->setSpacing(0);

    tabBar->connect(tabBar, &TrapeTabBar::sigCurrentIndexChanged, [=](int idx){
        QMessageBox::information(tabBar, "index", tabBar->text(idx));
    });
    tabBar->resize(400, 40);
    tabBar->setWindowTitle(__FUNCTION__);
    tabBar->show();
}

void test_volProgressBar()
{
    ProgressBar* w = new ProgressBar;
    w->setTipInfoEnabled(true);
    w->setTipInfoType(VolumnTip);
    w->setBKRectBrush(Qt::green);
    QPalette pal = w->palette();
    pal.setBrush(QPalette::Window, Qt::red);
    w->setPalette(pal);
    w->setAutoFillBackground(true);
    w->setRange(0, 100);
    QTimer* t = new QTimer;
    t->setInterval(100);
    t->connect(t, &QTimer::timeout, [=]{
        int value = w->value() + 1;
        if(value > 100){
            value = 0;
        }
        w->setValue(value);
    });
    t->start();
    w->resize(400, 30);
    w->setWindowTitle(__FUNCTION__);
    w->show();
}

void test_waveProgressBar()
{
    WaveProgress* w = new WaveProgress;
    w->setWaveBrush(Qt::red);
    w->setShadowWaveBrush(Qt::green);
    w->setRange(0, 100);
    QTimer* t = new QTimer;
    t->setInterval(100);
    t->connect(t, &QTimer::timeout, [=]{
        int value = w->value() + 1;
        if(value > 100){
            value = 0;
        }
        w->setValue(value);
    });
    t->start();
    w->resize(400, 400);
    w->setWindowTitle(__FUNCTION__);
    w->show();
}
