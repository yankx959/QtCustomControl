#pragma once

#include <QtWidgets/QWidget>
#include <QPainter>

class RoundProgress : public QWidget
{
	Q_OBJECT

public:
	RoundProgress(QWidget *parent = Q_NULLPTR);

	void setProcessColor(const QColor& color);
	void setLineProcessColor(const QColor& color);
	void setInsideCircleBrush(const QBrush& brush);
	void setMidCircleBrush(const QBrush& brush);
	void setTextFont(const QFont& font);
	void setTextColor(const QColor& color);
	void setValue(int nValue);
    int value()const;

protected:
	virtual void paintEvent(QPaintEvent *event);

private:
	void drawInCircle(QPainter& painter);
	void drawMidCircle(QPainter& painter);
	void drawArcProcess(QPainter& painter);
	void drawOutCircle(QPainter& painter);
	void drawInText(QPainter& painter);
	void drawProcess(QPainter& painter);

private:
	int m_nCurrentValue = 0;
	QColor m_ProcessColor;
	QColor m_LineProcessColor;
	QBrush m_InsideCircleBrush;
	QBrush m_MidCircleBrush;
	QFont m_TextFont;
	QColor m_TextColor;
	int m_minWidth;
};
