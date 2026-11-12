#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "searchdialog.h"
#include "replacedialog.h"
#include "codeeditor.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileInfo>
#include <QCloseEvent>
#include <QClipboard>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    textChanged = false;
    filePath = "";

    statusLabel.setMaximumWidth(150);
    statusLabel.setText("length：" + QString::number(0) + "  lines:  " + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);

    statusCursorLabel.setMaximumWidth(150);
    statusCursorLabel.setText("Ln：" + QString::number(0) + "  Col:  " + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    QLabel *author = new QLabel(ui->statusbar);
    author->setText(tr("luoluoluo"));
    ui->statusbar->addPermanentWidget(author);

    // 这些 Action 用来开关某个功能，做成可勾选的
    ui->actionToolbar->setCheckable(true);
    ui->actionToolbar->setChecked(true);
    ui->actionStatusbar->setCheckable(true);
    ui->actionStatusbar->setChecked(true);
    ui->actionLineWrap->setCheckable(true);
    ui->actionLineWrap->setChecked(false);
    ui->actionrow->setCheckable(true);
    ui->actionrow->setChecked(false);

    // 剪切、复制、粘贴、撤销、重做在启动时都没有可操作的内容
    ui->actionCopy->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());

    connect(ui->plainTextEdit, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::updateStatusBar);
    connect(QApplication::clipboard(), &QClipboard::dataChanged,
            this, [this]() {
                ui->actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
            });

    on_actionNew_triggered();
    updateStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ------------------------------------------------------------------ 文件操作

void MainWindow::on_actionNew_triggered()
{
    if (!userEditConfirmed())
        return;

    filePath = "";
    ui->plainTextEdit->clear();
    this->setWindowTitle(tr("新建文本文件-编辑器"));

    textChanged = false;
}

void MainWindow::on_actionOpen_triggered()
{
    if (!userEditConfirmed())
        return;

    QString filename = QFileDialog::getOpenFileName(this, "打开文件", ".",
                                                    tr("Text files (*.txt) ;; All(*.*)"));

    // 用户点了取消
    if (filename.isEmpty())
        return;

    loadFromFile(filename);
}

void MainWindow::on_actionSave_triggered()
{
    // 还没有关联文件（新建后从未保存过），走“另存为”
    if (filePath.isEmpty()) {
        on_actionSaveAs_triggered();
        return;
    }

    saveToFile(filePath);
}

void MainWindow::on_actionSaveAs_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "保存文件",
                                                    filePath.isEmpty() ? QString() : filePath,
                                                    tr("Text files (*.txt) ;; All(*.*)"));

    // 用户点了取消
    if (filename.isEmpty())
        return;

    saveToFile(filename);
}

void MainWindow::on_actionExit_triggered()
{
    // close() 会触发 closeEvent()，在那里统一做未保存提示
    close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (userEditConfirmed()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::saveToFile(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "保存文件",
                             QString("无法写入文件：\n%1").arg(fileName));
        return false;
    }

    QTextStream out(&file);
    out << ui->plainTextEdit->toPlainText();
    file.flush();
    file.close();

    filePath = fileName;
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());

    textChanged = false;
    return true;
}

void MainWindow::loadFromFile(const QString &fileName)
{
    QFile file(fileName);

    if (!file.exists()) {
        QMessageBox::warning(this, "打开文件",
                             QString("文件不存在：\n%1").arg(fileName));
        return;
    }

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "打开文件",
                             QString("无法读取文件：\n%1").arg(fileName));
        return;
    }

    QTextStream in(&file);
    QString text = in.readAll();
    file.close();

    // 用 setPlainText 覆盖原有内容，而不是插到光标处
    ui->plainTextEdit->setPlainText(text);

    filePath = fileName;
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());

    textChanged = false;
}

bool MainWindow::userEditConfirmed()
{
    if (!textChanged)
        return true;

    QString path = (filePath != "") ? filePath : "无标题.txt";

    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Question);
    msg.setWindowTitle("保存修改");
    msg.setText(QString("是否将更改保存到\n") + "\"" + path + "\" ?");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Yes);
    int r = msg.exec();

    switch (r) {
    case QMessageBox::Yes:
        on_actionSave_triggered();
        // 如果用户在弹出的“另存为”里点了取消，就不算保存成功
        return !textChanged;
    case QMessageBox::No:
        textChanged = false;
        return true;
    case QMessageBox::Cancel:
    default:
        return false;
    }
}

// ------------------------------------------------------------------ 编辑操作

void MainWindow::on_plainTextEdit_textChanged()
{
    if (!textChanged) {
        this->setWindowTitle("*" + this->windowTitle());
        textChanged = true;
    }

    updateStatusBar();
}

void MainWindow::on_actionUndo_triggered()
{
    ui->plainTextEdit->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    ui->plainTextEdit->redo();
}

void MainWindow::on_actionCut_triggered()
{
    ui->plainTextEdit->cut();
}

void MainWindow::on_actionCopy_triggered()
{
    ui->plainTextEdit->copy();
}

void MainWindow::on_actionPaste_triggered()
{
    ui->plainTextEdit->paste();
}

void MainWindow::on_actionSelectall_triggered()
{
    ui->plainTextEdit->selectAll();
}

void MainWindow::on_plainTextEdit_copyAvailable(bool b)
{
    ui->actionCopy->setEnabled(b);
    ui->actionCut->setEnabled(b);
}

void MainWindow::on_plainTextEdit_redoAvailable(bool b)
{
    ui->actionRedo->setEnabled(b);
}

void MainWindow::on_plainTextEdit_undoAvailable(bool b)
{
    ui->actionUndo->setEnabled(b);
}

// --------------------------------------------------------------- 查找与替换

void MainWindow::on_actionFind_triggered()
{
    SearchDialog dlg(ui->plainTextEdit, this);
    dlg.exec();
}

void MainWindow::on_actionReplace_triggered()
{
    ReplaceDialog dlg(ui->plainTextEdit, this);
    dlg.exec();
}

// --------------------------------------------------------------- 格式与外观

void MainWindow::on_actionFont_triggered()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, ui->plainTextEdit->font(), this, "选择字体");

    if (ok) {
        ui->plainTextEdit->setFont(font);
    }
}

void MainWindow::on_actionFontColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "选择字体颜色");

    if (!color.isValid())
        return;

    // 只改选中文字的前景色；没有选中时对之后输入的文字生效
    QTextCharFormat fmt;
    fmt.setForeground(color);
    ui->plainTextEdit->mergeCurrentCharFormat(fmt);
}

void MainWindow::on_actionFontBackgroundColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::yellow, this, "选择字体背景色");

    if (!color.isValid())
        return;

    QTextCharFormat fmt;
    fmt.setBackground(color);
    ui->plainTextEdit->mergeCurrentCharFormat(fmt);
}

void MainWindow::on_actionBackgourndColor_triggered()
{
    // 改的是整个编辑器的底色，用调色板实现，不会覆盖字体颜色
    QPalette pal = ui->plainTextEdit->palette();
    QColor color = QColorDialog::getColor(pal.color(QPalette::Base), this, "选择编辑器背景色");

    if (!color.isValid())
        return;

    pal.setColor(QPalette::Base, color);
    ui->plainTextEdit->setPalette(pal);
}

void MainWindow::on_actionLineWrap_triggered()
{
    if (ui->plainTextEdit->lineWrapMode() == QPlainTextEdit::NoWrap) {
        ui->plainTextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        ui->actionLineWrap->setChecked(true);
    } else {
        ui->plainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        ui->actionLineWrap->setChecked(false);
    }
}

void MainWindow::on_actionrow_triggered()
{
    ui->plainTextEdit->setLineNumberVisible(ui->actionrow->isChecked());
}

// ------------------------------------------------------------------ 窗口与状态栏

void MainWindow::on_actionToolbar_triggered()
{
    ui->toolBar->setVisible(ui->actionToolbar->isChecked());
}

void MainWindow::on_actionStatusbar_triggered()
{
    ui->statusbar->setVisible(ui->actionStatusbar->isChecked());
}

void MainWindow::updateStatusBar()
{
    statusCursorLabel.setText(QString("Ln：%1  Col:  %2")
                              .arg(ui->plainTextEdit->currentLineNumber())
                              .arg(ui->plainTextEdit->currentColumnNumber()));

    const QString text = ui->plainTextEdit->toPlainText();
    statusLabel.setText(QString("length：%1  lines:  %2")
                        .arg(text.length())
                        .arg(ui->plainTextEdit->blockCount()));
}

// ---------------------------------------------------------------------- 关于

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg;
    dlg.exec();
}
