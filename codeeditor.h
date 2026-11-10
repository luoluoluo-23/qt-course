#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class CodeEditor;

/**
 * @brief 左侧行号区域。
 *
 * 只负责把绘制工作转交给 CodeEditor，宽度由 CodeEditor 计算。
 * 类名与参考资料 3（Qt Code Editor Example）保持一致。
 */
class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor);

    QSize sizeHint() const override;
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditor *m_codeEditor;
};

/**
 * @brief 带行号的 QPlainTextEdit。
 *
 * 从 QPlainTextEdit 派生，在左侧绘制行号，可通过 setLineNumberVisible() 显示/隐藏。
 * 在 Qt Designer 中把 plainTextEdit 提升（promote）为本类即可，无需改动界面布局。
 */
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    /// 行号区域宽度（按最大行号的位数计算，至少 3 位）
    int lineNumberAreaWidth() const;

    /// 供 LineNumberArea 调用：在行号区域上绘制行号
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    /// 显示/隐藏行号
    void setLineNumberVisible(bool visible);
    bool isLineNumberVisible() const { return m_lineNumberVisible; }

    /// 当前光标所在行（从 1 开始）与列（从 1 开始）
    int currentLineNumber() const;
    int currentColumnNumber() const;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    LineNumberArea *m_lineNumberArea;
    bool m_lineNumberVisible;
};

#endif // CODEEDITOR_H
