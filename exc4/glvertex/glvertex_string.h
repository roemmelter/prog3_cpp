// (c) by Stefan Roettger, licensed under MIT license

#ifndef GLVERTEX_STRING_H
#define GLVERTEX_STRING_H

#include <string>
#include <algorithm>

//! LGL string class that extends std::string
class lgl_string: public std::string
{
public:

   lgl_string(const std::string &text = "")
      : std::string(text)
   {}

   lgl_string(const char *text)
      : std::string(text)
   {}

   //! is the string empty?
   bool empty() const
   {
      return(size() == 0);
   }

   //! get prefix before first substring match
   std::string prefix(const std::string &match) const
   {
      size_t pos = find(match);
      if (pos != std::string::npos) return(substr(0, pos));
      return("");
   }

   //! get suffix after last substring match
   std::string suffix(const std::string &match) const
   {
      size_t pos = rfind(match);
      if (pos != std::string::npos) return(substr(pos+match.size(), size()-pos-match.size()));
      return("");
   }

   //! get head before last substring match
   std::string head(const std::string &match) const
   {
      size_t pos = rfind(match);
      if (pos != std::string::npos) return(substr(0, pos));
      return("");
   }

   //! get tail after first substring match
   std::string tail(const std::string &match) const
   {
      size_t pos = find(match);
      if (pos != std::string::npos) return(substr(pos+match.size(), size()-pos-match.size()));
      return("");
   }

   //! match begin
   bool startsWith(const std::string &with) const
   {
      return(find(with) == 0);
   }

   //! match end
   bool endsWith(const std::string &with) const
   {
      return(rfind(with) == size()-with.size());
   }

   //! strip match from begin
   lgl_string strip(const std::string &match) const
   {
      if (startsWith(match)) return(substr(match.size(), size()-match.size()));
      else return(*this);
   }

   //! chop match from end
   lgl_string chop(const std::string &match) const
   {
      if (endsWith(match)) return(substr(0, size()-match.size()));
      else return(*this);
   }

   //! remove fragment between start and stop delimiters
   lgl_string remove(const std::string &start, const std::string &stop)
   {
      size_t pos1 = find(start);
      if (pos1 != std::string::npos)
      {
         size_t pos2 = find(stop, pos1+start.size());
         if (pos2 != std::string::npos)
         {
            lgl_string sub = substr(pos1, pos2-pos1+stop.size());
            replace(pos1, pos2-pos1+stop.size(), "");
            return(sub);
         }
      }

      return("");
   }

   //! convert to lower case
   lgl_string toLower(char separator = '\0') const
   {
      lgl_string s(*this);

      if (separator == '\0')
         std::transform(s.begin(), s.end(), s.begin(), ::tolower);
      else
         for (size_t i=0; i<s.size(); i++)
            if (::isupper(s[i]))
            {
               s[i] = ::tolower(s[i]);

               if (i > 0)
                  if (::islower(s[i-1]))
                     s.insert(i, 1, separator);
            }

      return(s);
   }

   //! convert to upper case
   lgl_string toUpper() const
   {
      lgl_string s(*this);
      std::transform(s.begin(), s.end(), s.begin(), ::toupper);
      return(s);
   }

   //! replace white space
   lgl_string replaceWhiteSpace() const
   {
      lgl_string s(*this);
      std::transform(s.begin(), s.end(), s.begin(), tononwhite);
      return(s);
   }

   //! replace non-printable characters
   lgl_string replaceNonPrintable() const
   {
      lgl_string s(*this);
      std::transform(s.begin(), s.end(), s.begin(), toprint);
      return(s);
   }

   //! replace non-alphanumeric characters
   lgl_string replaceNonAlphaNumeric() const
   {
      lgl_string s(*this);
      std::transform(s.begin(), s.end(), s.begin(), toalnum);
      return(s);
   }

protected:

   static char tononwhite(char c)
   {
      if (::isspace(c)) return('_');
      else return(c);
   }

   static char toprint(char c)
   {
      if (::isprint(c)) return(c);
      else return('_');
   }

   static char toalnum(char c)
   {
      if (::isalnum(c)) return(c);
      else return('_');
   }

public:

   //! replace all occurrences in place
   void replaceAll(const std::string &search, const std::string &with)
   {
      size_t pos = 0;
      while ((pos = find(search, pos)) != std::string::npos)
      {
         replace(pos, search.length(), with);
         pos += with.length();
      }
   }

   //! replace all occurrences in place
   static void stringReplaceAll(std::string &text, const std::string &search, const std::string &with)
   {
      size_t pos = 0;
      while ((pos = text.find(search, pos)) != std::string::npos)
      {
         text.replace(pos, search.length(), with);
         pos += with.length();
      }
   }

};

#endif
