#include "roundbutton.h"
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent> 
#include <QDebug>

RoundBtn::RoundBtn(QWidget *parent)
	: QWidget(parent)
	, m_circleBrush(QColor(Qt::gray))
	, m_nRaduis(0)
    , m_direction(Unknown)
{
	m_listText << QStringLiteral("↑") << QStringLiteral("←")
		<< QStringLiteral("↓") << QStringLiteral("→");
	this->setMouseTracking(true);

	QRadialGradient RadialGradient1;
	RadialGradient1.setColorAt(0, Qt::green);
	RadialGradient1.setColorAt(1, Qt::red);

	QRadialGradient RadialGradient2;
	RadialGradient2.setColorAt(0, Qt::white);
	RadialGradient2.setColorAt(1, Qt::black);

	QRadialGradient RadialGradient3;
	RadialGradient3.setColorAt(0, Qt::yellow);
	RadialGradient3.setColorAt(1, Qt::blue);

	QRadialGradient RadialGradient4;
	RadialGradient4.setColorAt(0, Qt::cyan);
	RadialGradient4.setColorAt(1, Qt::magenta);

	m_vPieBrush << RadialGradient1 << RadialGradient2
		<< RadialGradient3 << RadialGradient4;
}

void RoundBtn::setInCircleBrush(const QBrush& brush)
{
	m_circleBrush = brush;
	update();
}

void RoundBtn::setPieBrush(Direction directtion, const QColor& outColor, const QColor& inColor)
{
    if (directtion == Unknown)
	{
		return;
	}
	m_vPieBrush[directtion].setColorAt(0, inColor);
	m_vPieBrush[directtion].setColorAt(1, outColor);
	update();
}

void RoundBtn::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	m_nRaduis = qMin(this->height(), this->width()) / 2.1;
	painter.setRenderHint(QPainter::Antialiasing);
	painter.translate(this->width() / 2, this->height() / 2);
	drawPie(painter);
	drawText(painter);
	drawInCircle(painter);
}

void RoundBtn::mousePressEvent(QMouseEvent * ev)
{
    m_direction = getDiretion(ev->pos());
	m_bPressed = true;
	update();
}

void RoundBtn::mouseMoveEvent(QMouseEvent *event)
{
    m_direction = getDiretion(event->pos());
	update();
}

void RoundBtn::mouseReleaseEvent(QMouseEvent *event)
{
	m_bPressed = false;
    m_direction = getDiretion(event->pos());
	update();
    if (m_direction == Unknown){
        return;
    }
    emit sigClicked(m_direction);
}

void RoundBtn::drawPie(QPainter& painter)
{
	painter.save();
	int nAngle = 45;
	for (int i = 0; i < 4; i++)
	{
		QRadialGradient RadialGradient = m_vPieBrush.at(i);
		RadialGradient.setCenter(0, 0);
		RadialGradient.setCenterRadius(m_nRaduis);
		RadialGradient.setFocalPoint(0, 0);
        if (m_direction == i)
		{
			RadialGradient.setFocalRadius(m_bPressed ? m_nRaduis / 1.8 : m_nRaduis / 1.2);
		}
		else
		{
			RadialGradient.setFocalRadius(m_nRaduis / 2);
		}
		painter.setBrush(RadialGradient);
		painter.drawPie(QRect(0 - m_nRaduis, 0 - m_nRaduis, m_nRaduis * 2, m_nRaduis * 2), nAngle * 16, 90 * 16);
		nAngle += 90;
	}
	painter.restore();
}

void RoundBtn::drawText(QPainter& painter)
{
	painter.save();
	QFont font = this->font();
	font.setPixelSize(m_nRaduis / 8);
	painter.setFont(font);
	QRect rectTop(0 - m_nRaduis, 0 - m_nRaduis, m_nRaduis * 2, m_nRaduis / 2);
	QRect rectLeft(0 - m_nRaduis, 0 - m_nRaduis, m_nRaduis /2 , m_nRaduis * 2);
	QRect rectButtom(0 - m_nRaduis, m_nRaduis / 2, m_nRaduis * 2, m_nRaduis / 2);
	QRect rectRight(m_nRaduis / 2, 0 - m_nRaduis, m_nRaduis / 2, m_nRaduis * 2);
	QRect rects[] = {rectTop, rectLeft, rectButtom, rectRight};
	for (int i = 0; i < m_listText.size(); i++)
	{
		painter.drawText(rects[i], Qt::AlignCenter, m_listText.at(i));
	}
	painter.restore();
}

void RoundBtn::drawInCircle(QPainter& painter)
{
	painter.save();
	painter.setBrush(m_circleBrush);
	painter.drawEllipse(QPoint(0, 0), m_nRaduis / 2, m_nRaduis / 2);
	painter.restore();
}

RoundBtn::Direction RoundBtn::getDiretion(const QPoint& pos)
{
	//计算点距离中心点距离
	QPoint centerPos(width() / 2, height() / 2);
	QPoint relativePos = pos - centerPos;
	int nDistance = std::sqrt(std::pow(relativePos.x(), 2) + std::pow(relativePos.y(), 2));
	
	//点击点位于内圆或者无效区域
	if (nDistance <= m_nRaduis / 2 || nDistance > m_nRaduis)
	{
        return Unknown;
	}
	//判读tan = 0;
	if (relativePos.x() == 0)
	{
		if (relativePos.y() >= 0)
		{
            return Bottom;
		}
		else
		{
            return Top;
		}
	}
	//斜率
	float k = (float)relativePos.y() / relativePos.x();
	if (k < 1 && k > -1)
	{
		if ((relativePos.y() <= 0 && relativePos.x() > 0)
			|| (relativePos.y() >=0 && relativePos.x() > 0))
		{
            return Right;
		}
		if ((relativePos.x() < 0 && relativePos.y() <= 0)
			|| (relativePos.x() < 0 && relativePos.y() >=0 ))
		{
            return Left;
		}
	}
	if (  k > 1|| k < -1 )
	{
		if (relativePos.y() > 0)
		{
            return Bottom;
		}
		else
		{
            return Top;
		}
	}
    return Unknown;
}
