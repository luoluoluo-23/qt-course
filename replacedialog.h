#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

class QPlainTextEdit;

namespace Ui {
class ReplaceDialog;
}

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    /// editor 为要操作的文本编辑器，parent 为父窗口
    explicit ReplaceDialog(QPlainTextEdit *editor = nullptr, QWidget *parent = nullptr);
    ~ReplaceDialog();

private slots:
    void on_pushButton_clicked();      // 查找下一个

    void on_pushButton_2_clicked();    // 替换

    void on_pushButton_3_clicked();    // 全部替换

    void on_pushButton_4_clicked();    // 取消

private:
    /// 查找下一个；showTip 为 true 时在没找到的情况下弹提示框
    bool findNext(bool showTip);

    /// 替换当前选中的内容，返回是否真的替换了
    bool replaceCurrent();

    /// 替换全部，返回替换的处数
    int replaceAll();

    Ui::ReplaceDialog *ui;
    QPlainTextEdit *m_editor;
};

#endif // REPLACEDIALOG_H
