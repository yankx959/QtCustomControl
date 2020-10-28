#ifndef WHITEBOARDDLG_H
#define WHITEBOARDDLG_H

#include <QDialog>

class WhiteBoardDlg : public QDialog
{
    Q_OBJECT

public:
    explicit WhiteBoardDlg(QWidget *parent = 0);
    ~WhiteBoardDlg();

protected:
    virtual bool eventFilter(QObject * o, QEvent * e)override;
    virtual void showEvent(QShowEvent* e)override;
    virtual void resizeEvent(QResizeEvent* re)override;

public slots:
    //撤销
    void undo();

    //取消撤销
    void redo();

    //清空
    void clear();

    //选择颜色
    void selectColor();

    //保存  format 图片格式 参考QImage save format
    void save(const QString& filePath, const QString& format);

private:
    class WhiteBoardDlgPrivate;
    Q_DECLARE_PRIVATE(WhiteBoardDlg)
    WhiteBoardDlgPrivate* d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_buttonGroupClicked(int))
    Q_PRIVATE_SLOT(d_func(), void _q_colorSelected(QColor))
};

#endif // WHITEBOARDDLG_H
