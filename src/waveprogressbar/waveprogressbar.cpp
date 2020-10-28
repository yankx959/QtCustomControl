#include "waveprogressbar.h"
#include <QPainterPath>
#include <QPainter>
#include <QTimer>
#include <QBitmap>

#define _USE_MATH_DEFINES
#include <math.h>

WaveProgress::WaveProgress(QWidget *parent)
	: QWidget(parent)
{
	m_range.m_dbMin = .0f;
	m_range.m_dbMax = 100;
	
	m_nDensity = 1;
	m_dbAtt = 10;
	m_dbOffsetPhase = .0f;
	m_dbCurrentValue = 50;

	m_borderColor = std::move(QColor(Qt::red));
	m_nBorderWidth = 1;
	m_waveBrush = std::move(QColor(255, 0, 0, 180));
	m_shadowWaveBrush = std::move(QColor(255, 0, 0, 100));

	m_textFont = font();
	m_textColor = std::move(QColor(Qt::black));
	m_nTextWidth = 1;

	m_pTimer = new QTimer(this);
	connect(m_pTimer, &QTimer::timeout, [this]() {
		m_dbOffsetPhase += 0.5;
 //		m_dbCurrentValue += 1;
		if (m_dbCurrentValue >= m_range.m_dbMax)
		{
			m_dbCurrentValue = m_range.m_dbMin;
		}
		update();
	});
	setDuration(100);
}

WaveProgress::~WaveProgress()
{
}

void WaveProgress::setMaxValue(double dbMax)
{
	if (dbMax <= m_range.m_dbMin)
	{
		return;
	}
	m_range.m_dbMax = dbMax;
	if (m_dbCurrentValue > dbMax)
	{
		m_dbCurrentValue = dbMax;
	}
	update();
}

void WaveProgress::setMinValue(double dbMin)
{
	if (dbMin >= m_range.m_dbMax)
	{
		return;
	}
	m_range.m_dbMin = dbMin;
	if (m_dbCurrentValue < dbMin)
	{
		m_dbCurrentValue = dbMin;
	}
	update();
}

void WaveProgress::setRange(double dbMin, double dbMax)
{
	if (dbMax <= dbMin)
	{
		return;
	}
	m_range.m_dbMin = dbMin;
	m_range.m_dbMax = dbMax;
}

void WaveProgress::setValue(double dbValue)
{
	if (dbValue > m_range.m_dbMax || dbValue < m_range.m_dbMin)
	{
		return;
	}
	m_dbCurrentValue = dbValue;
	update();
}

void WaveProgress::setAtt(double dbAtt)
{
	m_dbAtt = dbAtt;
	update();
}

void WaveProgress::setDensity(int nDensity)
{
	m_nDensity = nDensity;
    update();
}

double WaveProgress::value() const
{
    return m_dbCurrentValue;
}

void WaveProgress::setDuration(int nMesc)
{
	m_pTimer->start(nMesc);
}

int WaveProgress::duration()
{
	return m_pTimer->interval();
}

void WaveProgress::setBorderWidth(int nWidth)
{
	m_nBorderWidth = std::max(nWidth, 0);
	update();
}

void WaveProgress::setBorderColor(const QColor& color)
{
	m_borderColor = color;
	update();
}

void WaveProgress::setWaveBrush(const QBrush& brush)
{
	m_waveBrush = brush;
	update();
}

void WaveProgress::setShadowWaveBrush(const QBrush& brush)
{
	m_shadowWaveBrush = brush;
	update();
}

void WaveProgress::setTextFont(const QFont& font)
{
	m_textFont = font;
	update();
}

void WaveProgress::setTextColor(const QColor& color)
{
	m_textColor = color;
	update();
}

void WaveProgress::setTextWidth(int nWidth)
{
	m_nTextWidth = std::max(nWidth, 1);
	update();
}

void WaveProgress::paintEvent(QPaintEvent *event)
{
	double dbPercent = m_dbCurrentValue / (m_range.m_dbMax - m_range.m_dbMin);

	double dbYValue = (1 - dbPercent) * height();
	m_dbCycle = m_nDensity * M_PI / m_fRadius;
	m_dbAtt = m_fRadius / 5;

	QPainterPath path;
	path.moveTo(0, height());

	QPainterPath path2;
	path2.moveTo(0, height());
	for (int x = 0; x <= width(); x++)
	{
		double dbY = m_dbAtt * sin(m_dbCycle * x + m_dbOffsetPhase) + dbYValue;
		double dbY2 = m_dbAtt * cos(m_dbCycle * x + m_dbOffsetPhase) + dbYValue;

		path2.lineTo(x, dbY2);

		if (m_dbCurrentValue == m_range.m_dbMax)
		{
			dbY = 0;
		}
		path.lineTo(x, dbY);
	}
	path.lineTo(width(), height());
	path2.lineTo(width(), height());

	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	
	QPainterPath pathCircle;
	pathCircle.addEllipse(m_posCenter, m_fRadius, m_fRadius);

	painter.save();
	if(m_nBorderWidth > 0)
	{
		QPen circlePen(m_borderColor, m_nBorderWidth);
		painter.setPen(circlePen);
	}
	painter.drawPath(pathCircle);
	painter.restore();

	painter.setBrush(m_shadowWaveBrush);
	painter.drawPath(pathCircle.intersected(path));

	painter.setBrush(m_waveBrush);
	painter.drawPath(pathCircle.intersected(path2));

	
	m_textFont.setPointSize(m_fRadius / 4);
	QString strText = QString("%1%").arg(int(dbPercent * 100));
	painter.setFont(m_textFont);
	painter.setPen(QPen(m_textColor, m_nTextWidth));
	painter.drawText(rect(), Qt::AlignCenter, strText);
}

void WaveProgress::resizeEvent(QResizeEvent *event)
{
	m_posCenter.setX(width() / 2);
	m_posCenter.setY(height() / 2);
	m_fRadius = std::min(width(), height()) / 2.0;
}
