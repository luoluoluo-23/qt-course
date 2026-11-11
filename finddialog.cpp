#include "finddialog.h"
#include "ui_finddialog.h"

#include <QPlainTextEdit>
#include <QTextCursor>
#include <QMessageBox>
#include <QApplication>

FindDialog::FindDialog(QPlainTextEdit *editor, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindDialog)
    , m_editor(editor)
{
    ui->setupUi(this);

    // 默认向下查找
    ui->radioButton_2->setChecked(true);

    // 如果编辑器里选中了一段文字，直接填到查找框里
    if (m_editor != nullptr) {
        QString selected = m_editor->textCursor().selectedText();
        if (!selected.isEmpty() && !selected.contains(QChar::ParagraphSeparator)) {
            ui->lineEdit->setText(selected);
        }
    }

    ui->lineEdit->setFocus();
    ui->lineEdit->selectAll();
}

FindDialog::~FindDialog()
{
    delete ui;
}

bool FindDialog::findNext(bool showTip)
{
    if (m_editor == nullptr)
        return false;

    const QString target = ui->lineEdit->text();
    if (target.isEmpty()) {
        if (showTip)
            QMessageBox::information(this, "查找", "请输入要查找的内容。");
        return false;
    }

    QTextDocument::FindFlags flags;
    if (ui->radioButton->isChecked())                 // 向上
        flags |= QTextDocument::FindBackward;
    if (ui->checkBox->isChecked())                    // 区分大小写
        flags |= QTextDocument::FindCaseSensitively;

    // 先按当前光标位置找一次
    if (m_editor->find(target, flags))
        return true;

    // 没找到就绕回文档开头（或末尾）再找一次，实现“连续查找”
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(ui->radioButton->isChecked() ? QTextCursor::End
                                                     : QTextCursor::Start);
    m_editor->setTextCursor(cursor);

    if (m_editor->find(target, flags)) {
        QApplication::beep();
        return true;
    }

    if (showTip)
        QMessageBox::information(this, "查找", QString("找不到“%1”。").arg(target));

    return false;
}

void FindDialog::on_btFindnext_clicked()
{
    findNext(true);
}

void FindDialog::on_pushButton_clicked()
{
    reject();
}
