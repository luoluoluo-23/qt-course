#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

class QPlainTextEdit;

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    /// editor 为要查找的文本编辑器，parent 为父窗口
    explicit SearchDialog(QPlainTextEdit *editor = nullptr, QWidget *parent = nullptr);
    ~SearchDialog();

private slots:
    void on_btFindnext_clicked();

    void on_pushButton_clicked();

private:
    /// 查找下一个；showTip 为 true 时在没找到的情况下弹提示框
    bool findNext(bool showTip);

    Ui::SearchDialog *ui;
    QPlainTextEdit *m_editor;
};

#endif // SEARCHDIALOG_H
