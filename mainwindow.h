#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QCloseEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    /// 关闭窗口前检查文件是否已保存
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_actionAbout_triggered();

    void on_actionFind_triggered();

    void on_actionReplace_triggered();

    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSaveAs_triggered();

    void on_actionSave_triggered();

    void on_actionExit_triggered();

    void on_plainTextEdit_textChanged();

    void on_actionUndo_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionRedo_triggered();

    void on_actionSelectall_triggered();

    void on_plainTextEdit_copyAvailable(bool b);

    void on_plainTextEdit_redoAvailable(bool b);

    void on_plainTextEdit_undoAvailable(bool b);

    void on_actionFont_triggered();

    void on_actionFontBackgroundColor_triggered();

    void on_actionFontColor_triggered();

    void on_actionLineWrap_triggered();

    void on_actionrow_triggered();

    void on_actionBackgourndColor_triggered();

    void on_actionToolbar_triggered();

    void on_actionStatusbar_triggered();

private:
    /// 刷新状态栏的行列号、字符数和行数
    void updateStatusBar();

    /// 把当前编辑器中的内容写入指定文件
    bool saveToFile(const QString &fileName);

    /// 读取文件内容到编辑器
    void loadFromFile(const QString &fileName);

    /// 若文件被修改过则询问用户是否保存，返回 false 表示用户取消了操作
    bool userEditConfirmed();

    Ui::MainWindow *ui;

    QLabel statusCursorLabel;
    QLabel statusLabel;

    QString filePath;
    bool textChanged;
};
#endif // MAINWINDOW_H
