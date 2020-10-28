#pragma once

#include <QWidget>
#include <QBrush>

enum TipType{
    TimeTip,
    VolumnTip,
};

class QLabel;
class ProgressBar : public QWidget
{
	Q_OBJECT

public:
    ProgressBar(QWidget *parent = nullptr);
    ~ProgressBar();

	void setMaxValue(int nValue);
    int maxValue()const;

	void setMinValue(int nValue);
    int minValue()const;

	void setRange(int nMin, int nMax);

    int value()const;

	void setBKRectBrush(const QBrush& brush);
    QBrush backRectBrush()const;

	void setProcessRectBrush(const QBrush& brush);
    QBrush processRectBrush()const;

	void setCircleBrush(const QBrush& brush);
    QBrush circleBrush()const;

	void setProcHeight(double dbHeight);
	void setRadius(double dbRadius);

    void setTipInfoEnabled(bool enabled);
    void setTipInfoType(TipType t);

    void setAlwaysShowCircle(bool show);

protected:
	virtual void paintEvent(QPaintEvent *event)override;
	virtual void mouseMoveEvent(QMouseEvent *event)override;
	virtual void mousePressEvent(QMouseEvent *event)override;
	virtual void mouseReleaseEvent(QMouseEvent *event)override;
	virtual void enterEvent(QEvent *event)override;
	virtual void leaveEvent(QEvent *event)override;
    virtual bool eventFilter(QObject *o, QEvent *e)override;

private:
	void drawBackRect(QPainter& painter);
	void drawProcess(QPainter& painter);
	void drawCurPosCircle(QPainter& painter);

private:
	void updateValue(int nCurValue);
	void initValue();
	void initUISetting();
    void updateTipPos(const QPoint& pos);
    void updateTipString(const QPoint& pos);

signals:
    void sigValueChanged(qint64 nValue);

public slots:
    void setValue(int nValue);

private:
	struct stRange
	{
		int m_nMin;
		int m_nMax;
	}m_range;

	int m_nCurValue;
	double m_dbCurPercent;

	double m_dbHeight;
	double m_dbRadius;

	QBrush m_bkRectBrush;
	QBrush m_processRectBrush;
	QBrush m_circleBrush;

	bool m_bDrawCirckle;
	bool m_bPressed;

    QString m_tipString;
    bool m_bTipEnabled;

    TipType m_tipType;

    QWidget* m_wgtTip;

    bool m_alwaysShowCircle;
};
