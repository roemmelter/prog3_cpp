// (c) by Stefan Roettger, licensed under MIT license
// based on the Qt code editor example

#ifndef GLVERTEX_QT_CODEEDITOR_H
#define GLVERTEX_QT_CODEEDITOR_H

#include <QPainter>
#include <QTextBlock>
#include <QPlainTextEdit>

class lgl_Qt_CodeEditor;

class lgl_Qt_LineNumberArea : public QWidget
{
public:

   lgl_Qt_LineNumberArea(lgl_Qt_CodeEditor *editor = NULL);

   QSize sizeHint() const;

protected:

   void paintEvent(QPaintEvent *event);

private:

   lgl_Qt_CodeEditor *editor_;
};

class lgl_Qt_CodeEditor : public QPlainTextEdit
{
   Q_OBJECT

public:

   lgl_Qt_CodeEditor(QWidget *parent = NULL);

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

inline lgl_Qt_CodeEditor::lgl_Qt_CodeEditor(QWidget *parent)
   : QPlainTextEdit(parent)
{
   lineNumberArea_ = new lgl_Qt_LineNumberArea(this);

   connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
   connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));

   updateLineNumberAreaWidth(0);

   pointSize_ = document()->defaultFont().pointSize();

   setLineWrapMode(QPlainTextEdit::NoWrap);
}

inline void lgl_Qt_CodeEditor::setFontZoom(float zoom)
{
   QFont font = document()->defaultFont();
   font.setPointSize(pointSize_*zoom+0.5f);
   document()->setDefaultFont(font);
}

inline void lgl_Qt_CodeEditor::highlightLine(int number, QColor color)
{
   QList<QTextEdit::ExtraSelection> selections;

   if (number > 0)
   {
      QTextEdit::ExtraSelection selection;
      selection.format.setBackground(color);
      QTextCursor cursor(document()->findBlockByLineNumber(number-1));
      cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
      selection.cursor = cursor;
      selections.append(selection);
   }

   setExtraSelections(selections);
}

inline void lgl_Qt_CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
   QPainter painter(lineNumberArea_);
   painter.fillRect(event->rect(), Qt::lightGray);

   int lineNumber = 1;

   QTextBlock start = document()->findBlockByLineNumber(0);
   QTextBlock block = firstVisibleBlock();

   while (start.blockNumber() != block.blockNumber())
   {
      bool separator = start.text().contains("---");
      start = start.next();

      if (separator) lineNumber = 1;
      else lineNumber++;
   }

   int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
   int bottom = top + (int)blockBoundingRect(block).height();

   while (block.isValid() && top<=event->rect().bottom())
   {
      bool separator = block.text().contains("---");

      if (!separator)
         if (block.isVisible() && bottom>=event->rect().top())
         {
            QString number = QString::number(lineNumber);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea_->width(), fontMetrics().height(), Qt::AlignRight, number);
         }

      block = block.next();
      top = bottom;
      bottom = top + (int)blockBoundingRect(block).height();

      if (separator) lineNumber = 1;
      else lineNumber++;
   }
}

inline int lgl_Qt_CodeEditor::lineNumberAreaWidth()
{
   int digits = 1;
   int max = qMax(1, blockCount());
   while (max >= 10)
   {
      max /= 10;
      digits++;
   }

   int space = 3 + fontMetrics().averageCharWidth() * digits;

   return(space);
}

inline void lgl_Qt_CodeEditor::resizeEvent(QResizeEvent *event)
{
   QPlainTextEdit::resizeEvent(event);

   QRect rect = contentsRect();
   lineNumberArea_->setGeometry(QRect(rect.left(), rect.top(), lineNumberAreaWidth(), rect.height()));
}

inline void lgl_Qt_CodeEditor::updateLineNumberAreaWidth(int newBlockCount)
{
   setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

inline void lgl_Qt_CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
   if (dy)
      lineNumberArea_->scroll(0, dy);
   else
      lineNumberArea_->update(0, rect.y(), lineNumberArea_->width(), rect.height());

   if (rect.contains(viewport()->rect()))
      updateLineNumberAreaWidth(0);
}

inline lgl_Qt_LineNumberArea::lgl_Qt_LineNumberArea(lgl_Qt_CodeEditor *editor)
   : QWidget(editor)
{
   editor_ = editor;
}

inline QSize lgl_Qt_LineNumberArea::sizeHint() const
{
   return QSize(editor_->lineNumberAreaWidth(), 0);
}

inline void lgl_Qt_LineNumberArea::paintEvent(QPaintEvent *event)
{
   editor_->lineNumberAreaPaintEvent(event);
}

#endif
