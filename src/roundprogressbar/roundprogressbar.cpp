#include "roundprogressbar.h"
#include <QConicalGradient> 

RoundProgress::RoundProgress(QWidget *parent)
	: QWidget(parent)
	, m_ProcessColor(106, 201, 241)
	, m_LineProcessColor(Qt::blue)
	, m_InsideCircleBrush(QBrush(QColor(210, 148, 240)))
	, m_TextColor(Qt::white)
{
	QConicalGradient bkColor(0, 0, 0);
	bkColor.setColorAt(0, Qt::red);
	bkColor.setColorAt(1, Qt::yellow);
	m_MidCircleBrush = std::move(bkColor);
	m_TextFont = font();
}

void RoundProgress::setProcessColor(const QColor& color)
{
	m_ProcessColor = color;
	update();
}

void RoundProgress::setLineProcessColor(const QColor& color)
{
	m_LineProcessColor = color;
	update();
}

void RoundProgress::setInsideCircleBrush(const QBrush& brush)
{
	m_InsideCircleBrush = brush;
	update();
}

void RoundProgress::setMidCircleBrush(const QBrush& brush)
{
	m_MidCircleBrush = brush;
	update();
}

void RoundProgress::setTextFont(const QFont& font)
{
	m_TextFont = font;
	update();
}

void RoundProgress::setTextColor(const QColor& color)
{
	m_TextColor = color;
	update();
}

void RoundProgress::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	//初始化设置
	painter.setRenderHint(QPainter::Antialiasing);
	painter.translate(width() / 2, height() / 2);
	m_minWidth = qMin(width(), height());

	drawProcess(painter);
//	drawOutCircle(painter);
	drawArcProcess(painter);
	drawMidCircle(painter);
	drawInCircle(painter);
	drawInText(painter);
}

void RoundProgress::setValue(int nValue)
{
	m_nCurrentValue = nValue;
    update();
}

int RoundProgress::value() const
{
    return m_nCurrentValue;
}

void RoundProgress::drawInCircle(QPainter& painter)
{
	painter.save();
	
	painter.setBrush(m_InsideCircleBrush);
	painter.setPen(Qt::NoPen);
	int nW = m_minWidth / 3.5;
	painter.drawEllipse(QPoint(0, 0), nW, nW);
	
	painter.restore();
}

void RoundProgress::drawMidCircle(QPainter& painter)
{
	painter.save();
	
	painter.setBrush(m_MidCircleBrush);
	painter.setPen(Qt::NoPen);
	int nW = m_minWidth / 3;
	painter.drawEllipse(QPoint(0, 0), nW, nW);
	
	painter.restore();
}

void RoundProgress::drawArcProcess(QPainter& painter)
{
	if (m_nCurrentValue < 1)
	{
		return;
	}
	painter.save();

	//半径
	int nW = m_minWidth / 2.7;
	
	//转换坐标系为Y轴朝上，确保进度条绘制方向与外环一致
    //这里偏移了起始和终止角度，用于和外环对齐
    painter.rotate(61);
	QTransform tranform = painter.transform();
	tranform.rotate(180, Qt::XAxis);
	painter.setTransform(tranform);
	QPen pen;
	pen.setWidth(nW / 10);
	pen.setColor(m_ProcessColor);
	painter.setPen(pen);
    int nEndAngle = (360 - 30 * 2) / 100 * m_nCurrentValue - 7;
    painter.drawArc(QRectF(0 - nW, 0 - nW, nW * 2, nW * 2), 61 * 16, nEndAngle * 16);
	
	painter.restore();
}

void RoundProgress::drawOutCircle(QPainter& painter)
{
	painter.save();
	
	painter.setBrush(QBrush(QColor(106, 201, 241)));
	painter.setPen(Qt::NoPen);
	int nW = qMin(width(), height()) / 2.6;
	painter.drawEllipse(QPoint(0, 0), nW, nW);
	
	painter.restore();
}

void RoundProgress::drawInText(QPainter& painter)
{
	painter.save();
	
	QPen pen;
	pen.setColor(m_TextColor);
	int nL = m_minWidth / 3.5;
	m_TextFont.setPixelSize(nL / 2);
	painter.setFont(m_TextFont);
	painter.setPen(pen);
	QString strText = QString::number(m_nCurrentValue) + "%";
	
	QRectF rect(0 - nL, 0 - nL, nL * 2, nL * 2);
	painter.drawText(rect, Qt::AlignCenter, strText);
	
	painter.restore();
}

void RoundProgress::drawProcess(QPainter& painter)
{
	painter.save();

    painter.rotate(30);
	QPen pen;
	pen.setWidth(m_minWidth / 85);
	pen.setColor(m_LineProcessColor);
    painter.setPen(pen);

	double dbAngle = (double)(360 - 30 * 2) / 100;
	int nEndY = m_minWidth / 2.4;
	int nStartY = m_minWidth / 2.05;

	for (int i = 0; i < m_nCurrentValue; i++)
	{
        painter.drawLine(0, nStartY, 0, nEndY);
		painter.rotate(dbAngle);
	}

	pen.setColor(Qt::gray);
	painter.setPen(pen);
	for (int i = 0; i < 100 - m_nCurrentValue; i++)
	{
		painter.drawLine(0, nStartY, 0, nEndY);
		painter.rotate(dbAngle);
	}

	painter.restore();
}
