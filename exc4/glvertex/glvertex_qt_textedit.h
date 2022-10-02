// (c) by Stefan Roettger, licensed under MIT license

#ifndef GLVERTEX_QT_TEXTEDIT_H
#define GLVERTEX_QT_TEXTEDIT_H

#include "glvertex_qt.h"
#include "glvertex_qt_codeeditor.h"

class lgl_Qt_TextEdit: public lgl_Qt_CodeEditor
{
public:

   //! default ctor
   lgl_Qt_TextEdit(QString name, QString code_suffix, QString text_suffix = "", QWidget *parent = NULL)
      : lgl_Qt_CodeEditor(parent),
        name_(name),
        code_suffix_(code_suffix),
        text_suffix_(text_suffix)
   {}

   //! dtor
   virtual ~lgl_Qt_TextEdit()
   {}

   void read(QString filename)
   {
      QString code = lglReadTextFile(filename.toStdString().c_str()).c_str();
      setPlainText(code);
   }

   void write(QString filename)
   {
      QString code = toPlainText();

      QFile file(filename);
      if (file.open(QIODevice::WriteOnly))
      {
         file.write(code.toLatin1());
         file.close();
      }
   }

   void write()
   {
      if (filename_ == "") return;
      if (text_suffix_ == "") return;

      QString f = filename_;
      int p1 = f.lastIndexOf(".");
      if (p1 >= 0) f.truncate(p1);
      int p2 = f.lastIndexOf(".");
      if (p2 >= 0) f.truncate(p2);
      f += "."+name_;
      f += "."+text_suffix_;

      write(f);
   }

   void load(QString filename, bool extra = true)
   {
      filename_ = filename;
      if (filename_ == "") return;

      sync();

      QString text1 = loadFrom(extra_);
      QString text2 = loadFrom(code_);

      if (text1!="" && extra)
         setPlainText(text1);
      else
         setPlainText(text2);
   }

   void sync()
   {
      if (filename_ == "") return;

      QString f = filename_;
      int p1 = f.lastIndexOf(".");
      if (p1 >= 0) f.truncate(p1);
      int p2 = f.lastIndexOf(".");
      if (p2 >= 0) f.truncate(p2);
      f += "."+name_;
      f += "."+code_suffix_;

      extra_ = lglReadTextFile(f.toStdString().c_str()).c_str();
      code_ = lglReadTextFile(filename_.toStdString().c_str()).c_str();
   }

   void revert()
   {
      load(filename_, false);
   }

   void save(bool extra = true)
   {
      if (filename_ == "") return;

      sync();

      loadFrom(extra_);
      loadFrom(code_);

      QString f = filename_;
      int p1 = f.lastIndexOf(".");
      if (p1 >= 0) f.truncate(p1);
      int p2 = f.lastIndexOf(".");
      if (p2 >= 0) f.truncate(p2);
      f += "."+name_;
      f += "."+code_suffix_;

      QString code1 = extra_;
      saveTo(code1);
      QFile file(f);
      if (file.open(QIODevice::WriteOnly))
      {
         file.write(code1.toLatin1());
         file.close();
      }

      if (!extra)
      {
         QString code2 = code_;
         saveTo(code2);
         QFile file(filename_);
         if (file.open(QIODevice::WriteOnly))
         {
            file.write(code2.toLatin1());
            file.close();
         }
      }
   }

protected:

   QString toCode()
   {
      return(toCode(toPlainText(), name_));
   }

   void saveTo(QString &code)
   {
      code = toCode(toPlainText(), name_) + code;
   }

   QString loadFrom(QString &code)
   {
      return(fromCode(code, name_));
   }

   QString toCode(QString text, QString name)
   {
      static const QString codestart = "const char";

      QString code;

      if (text=="" || text=="\n")
      {
         code = codestart+" "+name+"[] = \"\"";
      }
      else
      {
         code = codestart+" "+name+"[] =\n";

         for (int p1=0, p2=0; p2<text.size(); p2++)
            if (text[p2]=='\n' || p2+1==text.size())
            {
               QString line = text.mid(p1, p2-p1);
               code += " \""+line+"\\n\"";
               if (p2+1 < text.size()) code += "\n";
               p1 = p2+1;
            }
      }

      code += ";\n\n";

      return(code);
   }

   QString fromCode(QString &code, QString name)
   {
      static const QString codestart = "const char";

      QString text;

      int p = 0;

      while (p < code.size())
      {
         int p1 = code.indexOf(codestart, p);
         if (p1 < 0) break;

         int start = p1;

         p = p1+codestart.size();

         while (p<code.size() && code[p]==' ')
            p++;

         int p2 = code.indexOf("[]", p);
         if (p2 < 0) continue;

         QString string = code.mid(p, p2-p);
         if (string != name) continue;

         p = p2+1;

         p1 = code.indexOf("\"", p);
         if (p1 < 0) break;

         while (p1<code.size() && code[p1]=='\"')
         {
            p = p1+1;

            int p2 = code.indexOf("\"", p);
            if (p2 < 0) break;

            QString line = code.mid(p, p2-p);

            if (line.endsWith("\\n"))
               line.chop(2);

            text += line + "\n";

            p = p2+1;

            while (p<code.size() && code[p]!='\"' && code[p]!=';')
               p++;

            p1 = p;
         }

         p = p1;

         if (code[p] != ';')
         {
            text = "";
            break;
         }

         while (p<code.size() && code[p]!='\n')
            p++;

         int stop = p+1;

         while (stop<code.size() && (code[stop]=='\n' || code[stop]=='\r'))
            stop++;

         code = code.mid(0, start) + code.mid(stop);

         break;
      }

      if (text == "\n")
         text = "";

      return(text);
   }

   QString name_;
   QString filename_;
   QString code_suffix_;
   QString text_suffix_;

   QString code_;
   QString extra_;
};

#endif
