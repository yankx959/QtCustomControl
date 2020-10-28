#include "switchbutton.h"
#include <QPropertyAnimation>
#include <QPainter>
#include <cmath>
#include <QMouseEvent>

SwitchButton::SwitchButton(QWidget *parent)
	: QWidget(parent)
	, m_rectOpenBrush(QColor(53, 141, 255))
	, m_rectCloseBrush(Qt::gray)
	, m_rectCurBrush(m_rectCloseBrush)
	, m_circleBrush(Qt::white)
	, m_bOpen(false)
	, m_fRadius(0.0f)
{
	setMouseTracking(true);
	setProperty("circlePos", QPointF((float)height() * 0.05, (float)height() * 0.05));
	m_pAni = new QPropertyAnimation(this);
	m_pAni->setTargetObject(this);
	m_pAni->setPropertyName("circlePos");
	m_pAni->setStartValue(QPointF((float)height() * 0.05, (float)height() * 0.05));
	m_pAni->setEndValue(QPointF((float)width() - (float)height() * 0.95, (float)height() * 0.05));
	m_pAni->setDuration(200);

	connect(m_pAni, &QPropertyAnimation::valueChanged, this, &SwitchButton::onValueChanged);
	connect(m_pAni, &QPropertyAnimation::finished, this, &SwitchButton::onFinished);

	resize(180, 38);
}

void SwitchButton::setRectBrush(const QBrush& openBrush, const QBrush& closeBrush)
{
	m_rectOpenBrush = openBrush;
	m_rectCloseBrush = closeBrush;
	update();
}

void SwitchButton::setCircleBrush(const QBrush& brush)
{
	m_circleBrush = brush;
	update();
}

void SwitchButton::setDuration(int nDura)
{
	m_pAni->setDuration(nDura);
}

void SwitchButton::setCheckedNoAni(bool checked)
{
    m_rectCurBrush = checked ? m_rectOpenBrush : m_rectCloseBrush;
    m_bOpen = checked;
    QPointF pos = m_bOpen ? m_pAni->endValue().toPointF() : m_pAni->startValue().toPointF();
    m_circleRect.setX(pos.x());
    m_circleRect.setY(pos.y());
    m_circleRect.setSize(QSize(m_fRadius * 1.8, m_fRadius * 1.8));
    update();
}

void SwitchButton::setChecked(bool checked)
{
    m_bOpen = checked;
    changeState();
}

bool SwitchButton::checked() const
{
    return m_bOpen;
}

void SwitchButton::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	drawRoundRect(painter);
	drawCircle(painter);
}

void SwitchButton::mousePressEvent(QMouseEvent *event)
{
	if (!inLegalRange(event->pos()))
	{
		return;
	}
	m_bOpen = !m_bOpen;
	changeState();
}

void SwitchButton::mouseMoveEvent(QMouseEvent *event)
{
	if (inLegalRange(event->pos()))
	{
		setCursor(QCursor(Qt::PointingHandCursor));
	}
	else
	{
		setCursor(QCursor(Qt::ArrowCursor));
	}
}

void SwitchButton::resizeEvent(QResizeEvent *event)
{
	m_fRadius = (float)height() / 2;
	m_pAni->setStartValue(QPointF(m_fRadius * 0.1, m_fRadius * 0.1));
	m_pAni->setEndValue(QPointF((float)width() - m_fRadius * 1.9, m_fRadius * 0.1));
	if (m_pAni->state() == QPropertyAnimation::Stopped)
	{
		m_circleRect.setX(m_bOpen ? ((float)width() - m_fRadius * 1.9) : m_fRadius * 0.1);
		m_circleRect.setY(m_fRadius * 0.1);
		m_circleRect.setSize(QSize(m_fRadius * 1.8, m_fRadius * 1.8));
	}
}

bool SwitchButton::inLegalRange(const QPoint& pos)
{
	QPointF leftCenter(m_fRadius, m_fRadius);
	QPointF rightCenter(width() - m_fRadius, m_fRadius);
	if (pos.x() < m_fRadius)
	{
		QPointF rePos = pos - leftCenter;
		float fDis = sqrt(pow(rePos.x(), 2) + pow(rePos.y(), 2));
		if (fDis > m_fRadius)
		{
			return false;
		}
	}
	else if (pos.x() > width() - m_fRadius)
	{
		QPointF rePos = pos - rightCenter;
		float fDis = sqrt(pow(rePos.x(), 2) + pow(rePos.y(), 2));
		if (fDis > m_fRadius)
		{
			return false;
		}
	}
	return true;
}

void SwitchButton::drawRoundRect(QPainter& painter)
{
	painter.save();

	painter.setBrush(m_rectCurBrush);
	painter.drawRoundedRect(rect(), m_fRadius, m_fRadius);

	painter.restore();
}

void SwitchButton::drawCircle(QPainter& painter)
{
	painter.save();

	painter.setBrush(m_circleBrush);
	painter.drawEllipse(m_circleRect);

	painter.restore();
}

void SwitchButton::changeState()
{
	m_pAni->setDirection(m_bOpen ? QPropertyAnimation::Forward : QPropertyAnimation::Backward);
	m_pAni->start();
	update();
}

void SwitchButton::onValueChanged(const QVariant& value)
{
	QPointF pos = value.toPointF();
	m_circleRect.setX(pos.x());
	m_circleRect.setY(pos.y());
	m_circleRect.setSize(QSize(m_fRadius * 1.8, m_fRadius * 1.8));
	update();
}

void SwitchButton::onFinished()
{
	m_rectCurBrush = m_bOpen ? m_rectOpenBrush : m_rectCloseBrush;
	update();
}
