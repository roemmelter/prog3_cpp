// (c) by Stefan Roettger, licensed under MIT license
// based on the Qt code editor example

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPainter>
#include <QTextBlock>
#include <QPlainTextEdit>

class CodeEditor;

class LineNumberArea : public QWidget
{
public:

   LineNumberArea(CodeEditor *editor = NULL);

   QSize sizeHint() const;

protected:

   void paintEvent(QPaintEvent *event);

private:

   CodeEditor *editor_;
};

class CodeEditor : public QPlainTextEdit
{
   Q_OBJECT

public:

   CodeEditor(QWidget *parent = NULL);

   void setText(const QString &text)
   {
      setPlainText(text);
   }

   void setFontZoom(float zoom = 1.0f);
   void highlightLine(int number = 0, QColor color = QColor(255,255,127));

   void lineNumberAreaPaintEvent(QPaintEvent *event);
   int lineNumberAreaWidth();

protected:

   void resizeEvent(QResizeEvent *event);

private slots:

   void updateLineNumberAreaWidth(int newBlockCount);
   void updateLineNumberArea(const QRect &rect, int dy);

private:

   QWidget *lineNumberArea_;
   int pointSize_;
};

#endif
