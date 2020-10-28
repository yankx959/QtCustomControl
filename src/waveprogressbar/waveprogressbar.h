#pragma once

#include <QWidget>

class QTimer;
class WaveProgress : public QWidget
{
	Q_OBJECT

public:
	WaveProgress(QWidget *parent = nullptr);
	~WaveProgress();

	void setMaxValue(double dbMax);
	void setMinValue(double dbMin);

	void setRange(double dbMin, double dbMax);

	void setAtt(double dbAtt);

    //...PI
	void setDensity(int nDensity);
    double value()const;

	void setDuration(int nMesc);
    int duration();

	void setBorderWidth(int nWidth);
	void setBorderColor(const QColor& color);

	void setWaveBrush(const QBrush& brush);
	void setShadowWaveBrush(const QBrush& brush);

	void setTextFont(const QFont& font);
	void setTextColor(const QColor& color);
	void setTextWidth(int nWidth);

protected:
	virtual void paintEvent(QPaintEvent *event)override;
	virtual void resizeEvent(QResizeEvent *event)override;

public slots:
    void setValue(double dbValue);

private:
	struct stRange
	{
		double m_dbMin;
		double m_dbMax;
	}m_range;
	double m_dbCurrentValue;
	double m_dbOffsetPhase;
	double m_dbAtt;
	double m_dbCycle;
	int m_nDensity;
	QTimer* m_pTimer;

	QPoint m_posCenter;
	float m_fRadius;

	QBrush m_waveBrush;
	QBrush m_shadowWaveBrush;
	int m_nBorderWidth;
	QColor m_borderColor;

	QFont m_textFont;
	QColor m_textColor;
	int m_nTextWidth;
};
