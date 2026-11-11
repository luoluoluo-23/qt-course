#include "replacedialog.h"
#include "ui_replacedialog.h"

#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextDocument>
#include <QMessageBox>
#include <QApplication>

ReplaceDialog::ReplaceDialog(QPlainTextEdit *editor, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
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

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

bool ReplaceDialog::findNext(bool showTip)
{
    if (m_editor == nullptr)
        return false;

    const QString target = ui->lineEdit->text();
    if (target.isEmpty()) {
        if (showTip)
            QMessageBox::information(this, "替换", "请输入要查找的内容。");
        return false;
    }

    QTextDocument::FindFlags flags;
    if (ui->radioButton->isChecked())                 // 向上
        flags |= QTextDocument::FindBackward;
    if (ui->checkBox->isChecked())                    // 区分大小写
        flags |= QTextDocument::FindCaseSensitively;

    if (m_editor->find(target, flags))
        return true;

    // 找不到就绕回文档开头（或末尾）继续，实现“连续查找”
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(ui->radioButton->isChecked() ? QTextCursor::End
                                                     : QTextCursor::Start);
    m_editor->setTextCursor(cursor);

    if (m_editor->find(target, flags)) {
        QApplication::beep();
        return true;
    }

    if (showTip)
        QMessageBox::information(this, "替换", QString("找不到“%1”。").arg(target));

    return false;
}

bool ReplaceDialog::replaceCurrent()
{
    const QString target = ui->lineEdit->text();
    const QString replacement = ui->lineEdit_2->text();

    QTextCursor cursor = m_editor->textCursor();
    if (!cursor.hasSelection())
        return false;

    Qt::CaseSensitivity cs = ui->checkBox->isChecked() ? Qt::CaseSensitive
                                                       : Qt::CaseInsensitive;

    // 只有选中的内容确实是查找目标时才替换，避免替换掉别的位置
    if (QString::compare(cursor.selectedText(), target, cs) != 0)
        return false;

    cursor.insertText(replacement);
    return true;
}

int ReplaceDialog::replaceAll()
{
    const QString target = ui->lineEdit->text();
    const QString replacement = ui->lineEdit_2->text();

    QTextDocument::FindFlags flags;
    if (ui->checkBox->isChecked())
        flags |= QTextDocument::FindCaseSensitively;

    int count = 0;
    QTextCursor cursor(m_editor->document());
    cursor.movePosition(QTextCursor::Start);

    while (true) {
        QTextCursor found = m_editor->document()->find(target, cursor, flags);
        if (found.isNull())
            break;

        found.insertText(replacement);
        cursor = found;          // 从替换后的位置继续往后找
        ++count;
    }

    return count;
}

void ReplaceDialog::on_pushButton_clicked()
{
    findNext(true);
}

void ReplaceDialog::on_pushButton_2_clicked()
{
    const QString target = ui->lineEdit->text();
    if (target.isEmpty()) {
        QMessageBox::information(this, "替换", "请输入要查找的内容。");
        return;
    }

    // 当前选中的不是查找目标时，先找到一处再替换
    if (!replaceCurrent() && !findNext(true))
        return;

    // 替换完自动跳转到下一处，方便连续操作
    findNext(false);
}

void ReplaceDialog::on_pushButton_3_clicked()
{
    const QString target = ui->lineEdit->text();
    if (target.isEmpty()) {
        QMessageBox::information(this, "替换", "请输入要查找的内容。");
        return;
    }

    int count = replaceAll();

    if (count == 0) {
        QMessageBox::information(this, "替换", QString("找不到“%1”。").arg(target));
    } else {
        QMessageBox::information(this, "替换", QString("共替换了 %1 处。").arg(count));
    }
}

void ReplaceDialog::on_pushButton_4_clicked()
{
    reject();
}
