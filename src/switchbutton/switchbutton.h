#pragma once

#include <QtWidgets/QWidget>

class QPropertyAnimation;
class SwitchButton : public QWidget
{
	Q_OBJECT

public:
	SwitchButton(QWidget *parent = Q_NULLPTR);

	void setRectBrush(const QBrush& openBrush, const QBrush& closeBrush);

	void setCircleBrush(const QBrush& brush);

	void setDuration(int nDura);

    void setCheckedNoAni(bool checked);

    void setChecked(bool checked);

    bool checked()const;

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void resizeEvent(QResizeEvent *event);

private:
	bool inLegalRange(const QPoint& pos);
	void drawRoundRect(QPainter& painter);
	void drawCircle(QPainter& painter);
	void changeState();

signals:
	void sig_switch(bool bOpen);

private slots:
	void onValueChanged(const QVariant& value);
	void onFinished();

private:
	bool m_bOpen;
	QBrush m_rectOpenBrush;
	QBrush m_rectCloseBrush;
	QBrush m_rectCurBrush;
	QBrush m_circleBrush;
	QRectF m_circleRect;
	float m_fRadius;
	QPropertyAnimation* m_pAni;
};
