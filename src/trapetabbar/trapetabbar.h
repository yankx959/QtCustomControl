#ifndef TRAPETABBAR_H
#define TRAPETABBAR_H

#include <QWidget>

class TrapeTabBar : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int spacing READ spacing WRITE  setSpacing)
    Q_PROPERTY(int triangleLength READ triangleLength WRITE setTriangleLength)
    Q_PROPERTY(int leftMargin READ leftMargin WRITE setLeftMargin)
    Q_PROPERTY(int uniformWidth READ uniformWidth WRITE setUniformWidth)
    Q_PROPERTY(int textMargin READ textMargin WRITE setTextMargin)
    Q_PROPERTY(int startY READ startY WRITE setStartY)

public:
    explicit TrapeTabBar(QWidget *parent = nullptr);
    ~TrapeTabBar();

public:
    int spacing()const;
    void setSpacing(int space);

    void addTab(const QString& text);
    void insertTab(int idx, const QString& text);

    QString text(int idx)const;
    void setText(int idx, const QString& text);

    void setTriangleLength(int len);
    int triangleLength()const;

    void setLeftMargin(int margin);
    int leftMargin()const;

    void setTextMargin(int margin);
    int textMargin()const;

    void setUniformWidth(int width);
    int uniformWidth()const;

    int indexOf(const QString& text);
    int count()const;

    int currentIndex()const;
    void setCurrentIndex(int idx);

    int startY()const;
    void setStartY(int y);

protected:
    virtual void paintEvent(QPaintEvent* e)override;
    virtual void mousePressEvent(QMouseEvent* me)override;

signals:
    void sigCurrentIndexChanged(int idx);

private:
    class TrapeTabBarPrivate;
    TrapeTabBarPrivate* d_ptr;
    Q_DECLARE_PRIVATE(TrapeTabBar)
};

#endif // TRAPETABBAR_H
