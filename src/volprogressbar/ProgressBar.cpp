#include "ProgressBar.h"
#include <QPainter>
#include <QMouseEvent>
#include <QToolTip>
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>

ProgressBar::ProgressBar(QWidget *parent)
    : QWidget(parent)
    , m_wgtTip(nullptr)
{
    initUISetting();
    initValue();
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::setMaxValue(int nValue)
{
    if (nValue < m_range.m_nMin)
    {
        nValue = m_range.m_nMin;
    }
    m_range.m_nMax = nValue;
    updateValue(0);
}

int ProgressBar::maxValue() const
{
    return m_range.m_nMax;
}

void ProgressBar::setMinValue(int nValue)
{
    if (nValue > m_range.m_nMax)
    {
        nValue = m_range.m_nMax;
    }
    m_range.m_nMin = nValue;
    updateValue(0);
}

int ProgressBar::minValue() const
{
    return m_range.m_nMin;
}

void ProgressBar::setRange(int nMin, int nMax)
{
    if (nMax < nMin)
    {
        nMax = nMin;
    }
    m_range.m_nMin = nMin;
    m_range.m_nMax = nMax;

    updateValue(0);
}

void ProgressBar::setValue(int nValue)
{
    updateValue(nValue);
}

int ProgressBar::value() const
{
    return m_nCurValue;
}

void ProgressBar::setBKRectBrush(const QBrush& brush)
{
    if(m_bkRectBrush == brush){
        return ;
    }
    m_bkRectBrush = brush;
    update();
}

QBrush ProgressBar::backRectBrush() const
{
    return m_bkRectBrush;
}

void ProgressBar::setProcessRectBrush(const QBrush& brush)
{
    if(m_processRectBrush == brush){
        return ;
    }
    m_processRectBrush = brush;
    update();
}


QBrush ProgressBar::processRectBrush() const
{
    return m_processRectBrush;
}

void ProgressBar::setCircleBrush(const QBrush& brush)
{
    if(m_circleBrush == brush){
        return ;
    }
    m_circleBrush = brush;
    update();
}

QBrush ProgressBar::circleBrush() const
{
    return m_circleBrush;
}

void ProgressBar::setProcHeight(double dbHeight)
{
    if(qFuzzyCompare(m_dbHeight, dbHeight)){
        return ;
    }
    m_dbHeight = dbHeight;
    update();
}

void ProgressBar::setRadius(double dbRadius)
{
    if(qFuzzyCompare(m_dbRadius, dbRadius)){
        return ;
    }
    m_dbRadius = dbRadius;
    update();
}

void ProgressBar::setTipInfoEnabled(bool enabled)
{
    if(m_bTipEnabled == enabled){
        return ;
    }
    m_bTipEnabled = enabled;
    update();
}

void ProgressBar::setTipInfoType(TipType t)
{
    if(m_tipType == t){
        return ;
    }
    m_tipType = t;
    update();
}

void ProgressBar::setAlwaysShowCircle(bool show)
{
    if(show == m_alwaysShowCircle){
        return ;
    }
    m_alwaysShowCircle = show;
    update();
}

void ProgressBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackRect(painter);
    drawProcess(painter);
    drawCurPosCircle(painter);
}

void ProgressBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bPressed)
    {
        m_dbCurPercent = (event->pos().x() - m_dbRadius) * 1.0 / (width() - 2 * m_dbRadius);
        m_dbCurPercent = std::min(1.0, m_dbCurPercent);
        m_dbCurPercent = std::max(0.0, m_dbCurPercent);
        m_nCurValue = (m_range.m_nMax - m_range.m_nMin) * m_dbCurPercent;
        emit sigValueChanged(m_nCurValue);
        update();
    }
    if(rect().contains(event->pos()))
    {
        if(!m_wgtTip->isVisible())
            m_wgtTip->setVisible(true);
        updateTipString(event->pos());
        updateTipPos(event->pos());
    }
    else
    {
        m_wgtTip->hide();
    }
    QWidget::mouseMoveEvent(event);
}

void ProgressBar::mousePressEvent(QMouseEvent *event)
{
    m_bPressed = true;
    m_dbCurPercent = (event->pos().x() - m_dbRadius) * 1.0 / (width() - 2 * m_dbRadius);
    m_nCurValue = (m_range.m_nMax - m_range.m_nMin) * m_dbCurPercent;
    updateTipPos(event->pos());
    emit sigValueChanged(m_nCurValue);
    update();
}

void ProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
    m_bPressed = false;
    //    QWidget::mouseReleaseEvent(event);
}

void ProgressBar::enterEvent(QEvent *event)
{
    m_bDrawCirckle = true;
    if(!m_wgtTip->isVisible())
    {
        m_wgtTip->show();
        m_wgtTip->raise();
    }
    //    qDebug() << "enter enter[======================";
    update();
}

void ProgressBar::leaveEvent(QEvent *event)
{
    m_bDrawCirckle = false;
    QPoint curSorPos = mapFromGlobal(QCursor::pos());
    if(!this->rect().contains(curSorPos))
        m_wgtTip->hide();
    update();
}

bool ProgressBar::eventFilter(QObject * o, QEvent * e)
{
    if(o == m_wgtTip)
    {
        if(e->type() == QEvent::Paint)
        {
            QPainter painter(m_wgtTip);
            painter.setRenderHints(QPainter::Antialiasing);
            painter.setPen(QPen(Qt::red, 1));
            painter.drawText(m_wgtTip->rect(), Qt::AlignCenter, m_tipString);
            return true;
        }
    }
    return QWidget::eventFilter(o, e);
}

void ProgressBar::drawBackRect(QPainter& painter)
{
    painter.save();

    painter.fillRect(QRectF(m_dbRadius, (height() - m_dbHeight) / 2, width()-m_dbRadius, m_dbHeight), m_bkRectBrush);

    painter.restore();
}

void ProgressBar::drawProcess(QPainter& painter)
{
    painter.save();
    QRectF rectf(m_dbRadius, (height() - m_dbHeight) / 2, (width() - m_dbRadius) * m_dbCurPercent, m_dbHeight);
    painter.fillRect(rectf, m_processRectBrush);

    painter.restore();
}

void ProgressBar::drawCurPosCircle(QPainter& painter)
{
    if (!m_alwaysShowCircle && !m_bDrawCirckle)
    {
        return;
    }

    painter.save();

    painter.setBrush(m_circleBrush);

    double dbCurCenterX = (width() - 2 * m_dbRadius) * m_dbCurPercent + m_dbRadius;
    if (dbCurCenterX < m_dbRadius)
        dbCurCenterX = m_dbRadius;
    else if (dbCurCenterX > width() - m_dbRadius)
        dbCurCenterX = width() - m_dbRadius;
    painter.drawEllipse(QPointF(dbCurCenterX, height() / 2), m_dbRadius, m_dbRadius);

    painter.restore();
}

void ProgressBar::updateValue(int nCurValue)
{
    if(m_nCurValue == nCurValue){
        return ;
    }
    m_nCurValue = nCurValue;
    if (m_range.m_nMax == m_range.m_nMin)
        m_dbCurPercent = 0;
    else
        m_dbCurPercent = m_nCurValue * 1.0 / (m_range.m_nMax - m_range.m_nMin);
    update();
}

void ProgressBar::initValue()
{
    m_range = { 0, 0 };
    m_nCurValue = 0;
    m_dbCurPercent = 0;

    m_dbHeight = 5;
    m_dbRadius = 7;

    m_bkRectBrush = std::move(QBrush(QColor(78, 78, 78)));
    m_processRectBrush = std::move(QBrush(QColor(26, 158, 255)));
    m_circleBrush = std::move(QBrush(Qt::white));

    m_bDrawCirckle = false;
    m_bPressed = false;

    m_bTipEnabled = false;
    m_alwaysShowCircle = false;
}

void ProgressBar::initUISetting()
{
    setCursor(QCursor(Qt::PointingHandCursor));
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_wgtTip = new QWidget(this);
    m_wgtTip->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    m_wgtTip->setAttribute(Qt::WA_TranslucentBackground);
    m_wgtTip->setFocusPolicy(Qt::NoFocus);
    m_wgtTip->setFixedSize(80, 28);
    m_wgtTip->installEventFilter(this);
}

void ProgressBar::updateTipPos(const QPoint &pos)
{
    if(!m_bTipEnabled)
        return ;
    QPoint global = this->mapToGlobal(pos);
    QPoint thisGPos = this->mapToGlobal(QPoint(0, 0));
    QPoint target(global.x() - m_wgtTip->width() / 2, thisGPos.y() - m_wgtTip->height());
    m_wgtTip->move(target);
    m_wgtTip->update();
}

void ProgressBar::updateTipString(const QPoint &pos)
{
    double percent = (pos.x() - m_dbRadius) * 1.0 / (width() - 2 * m_dbRadius);
    int nShowValue = (m_range.m_nMax - m_range.m_nMin) * percent;

    if(nShowValue < 0)
        nShowValue = 0;
    else if(nShowValue > m_range.m_nMax)
        nShowValue = m_range.m_nMax;

    switch (m_tipType) {
    case TimeTip:
    {
        int nMiniute = nShowValue / 60.0;
        if(nMiniute >= 0)
        {
            int nHour = nMiniute / 60.0;
            if(nHour >= 1)
            {
                int nShowMin = nMiniute % 60;
                m_tipString = QString("%1:%2:%3").arg(nHour).arg(nShowMin, 2, 10, QChar('0')).arg(nShowValue % 60, 2, 10, QChar('0'));// QString::number(nHour) + ":" + QString::number(nShowMin) + ":" + QString::number();
            }
            else
            {
                m_tipString = QString("%1:%2").arg(nMiniute).arg(nShowValue % 60, 2, 10, QChar('0'));
            }
        }
        else
        {
            m_tipString = QString::number(nShowValue % 60);
        }
    }
        break;
    case VolumnTip:
        m_tipString = QString::number(nShowValue) + "%";
        break;
    default:
        break;
    }
}
