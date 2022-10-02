// (c) by Stefan Roettger, licensed under MIT license
// based on the Qt code editor example

#include "codeeditor.h"

CodeEditor::CodeEditor(QWidget *parent)
   : QPlainTextEdit(parent)
{
   lineNumberArea_ = new LineNumberArea(this);

   connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
   connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));

   updateLineNumberAreaWidth(0);

   pointSize_ = document()->defaultFont().pointSize();
}

void CodeEditor::setFontZoom(float zoom)
{
   QFont font = document()->defaultFont();
   font.setPointSize(pointSize_*zoom+0.5f);
   document()->setDefaultFont(font);
}

void CodeEditor::highlightLine(int number, QColor color)
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

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
   QPainter painter(lineNumberArea_);
   painter.fillRect(event->rect(), Qt::lightGray);

   QTextBlock block = firstVisibleBlock();
   int blockNumber = block.blockNumber();
   int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
   int bottom = top + (int)blockBoundingRect(block).height();

   while (block.isValid() && top<=event->rect().bottom())
   {
      bool separator = block.text().contains("---");

      if (!separator)
         if (block.isVisible() && bottom>=event->rect().top())
         {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea_->width(), fontMetrics().height(), Qt::AlignRight, number);
         }

      block = block.next();
      top = bottom;
      bottom = top + (int)blockBoundingRect(block).height();

      if (separator) blockNumber = 0;
      else blockNumber++;
   }
}

int CodeEditor::lineNumberAreaWidth()
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

void CodeEditor::resizeEvent(QResizeEvent *event)
{
   QPlainTextEdit::resizeEvent(event);

   QRect rect = contentsRect();
   lineNumberArea_->setGeometry(QRect(rect.left(), rect.top(), lineNumberAreaWidth(), rect.height()));
}

void CodeEditor::updateLineNumberAreaWidth(int newBlockCount)
{
   setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
   if (dy)
      lineNumberArea_->scroll(0, dy);
   else
      lineNumberArea_->update(0, rect.y(), lineNumberArea_->width(), rect.height());

   if (rect.contains(viewport()->rect()))
      updateLineNumberAreaWidth(0);
}

LineNumberArea::LineNumberArea(CodeEditor *editor)
   : QWidget(editor)
{
   editor_ = editor;
}

QSize LineNumberArea::sizeHint() const
{
   return QSize(editor_->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
   editor_->lineNumberAreaPaintEvent(event);
}
