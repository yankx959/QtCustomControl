#pragma once

#include <QtWidgets/QWidget>
#include <QRadialGradient>
#include <QPair>

class RoundBtn : public QWidget
{
	Q_OBJECT

public:
    enum Direction {
        Top,
        Left,
        Bottom,
        Right,
        Unknown
	};
    Q_ENUM(Direction)

public:
	RoundBtn(QWidget *parent = Q_NULLPTR);
	void setInCircleBrush(const QBrush& brush);
    void setPieBrush(Direction directtion, const QColor& outColor, const QColor& inColor);

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent * ev);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

private:
	void drawPie(QPainter& painter);
	void drawText(QPainter& painter);
	void drawInCircle(QPainter& painter);
    Direction getDiretion(const QPoint& pos);

signals:
    void sigClicked(Direction diretion);

private:
	QStringList m_listText;
	int m_nRaduis;
	QVector<QRadialGradient> m_vPieBrush;
    Direction m_direction;
	bool m_bPressed;
	QBrush m_circleBrush;
};
