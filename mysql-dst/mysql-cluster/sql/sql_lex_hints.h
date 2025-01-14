/*
   Copyright (c) 2014, 2015, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */


/* A lexical scanner for optimizer hints pseudo-commentary syntax */

#ifndef SQL_LEX_HINTS_ICLUDED
#define SQL_LEX_HINTS_ICLUDED

#include "m_ctype.h"
#include "sql_chars.h"
#include "sql_lex_hash.h"
#include "sql_alloc.h"
#include "lex_symbol.h"
#include "sql_class.h"
#include "sql_hints.yy.h"

struct sql_digest_state;

void hint_lex_init_maps(charset_info_st *cs, hint_lex_char_classes *hint_map);

/// Lexical scanner for hint comments.
///
/// When the main lexical scanner recognizes the "/*+" delimiter, it calls
/// the hint parser (HINT_PARSER_parse) to consume the rest of hint tokens
/// including the */ delimiter. The hint parser uses Hint_scanner as its own
/// lexer to scan hint-specific tokens.
class Hint_scanner : public Sql_alloc
{
  THD *thd;
  const CHARSET_INFO *cs;
  bool is_ansi_quotes;
  size_t lineno;
  const hint_lex_char_classes *char_classes;

  const char *input_buf;
  const char *input_buf_end;

  const char *ptr;

  int prev_token;

  /**
    Digest buffer interface to append tokens.
  */
  sql_digest_state *digest_state;

public:
  /**
    Current token (yytext) origin in the input_buf
  */
  const char *raw_yytext;
  /**
    Current token pointer (may be converted allocated string outside input_buf
  */
  const char *yytext;
  /**
    Length of the current token (see yytext)
  */
  size_t yyleng;

  bool has_hints; ///< True if a hint comment is not empty (has any hints).

public:
  Hint_scanner(THD *thd, size_t lineno_arg, const char *buf, size_t len,
               sql_digest_state *digest_state_arg);
  size_t get_lineno() const { return lineno; }
  const char *get_ptr() const { return ptr; }
  sql_digest_state *get_digest() { return digest_state; }
  void syntax_warning(const char *msg) const;

  int get_next_token()
  {
    DBUG_ENTER("Hint_scanner::get_next_token");
    prev_token= scan();
    add_hint_token_digest();
    DBUG_RETURN(prev_token);
  }

protected:

  int scan()
  {
    int whitespaces= 0;
    for (;;)
    {
      start_token();
      switch (peek_class()) {
      case HINT_CHR_NL:
        skip_newline();
        whitespaces++;
        continue;
      case HINT_CHR_SPACE:
        skip_byte();
        whitespaces++;
        continue;
      case HINT_CHR_DIGIT:
        return scan_number_or_ident();
      case HINT_CHR_IDENT:
        return scan_ident_or_keyword();
      case HINT_CHR_MB:
        return scan_ident();
      case HINT_CHR_BACKQUOTE:
        return scan_quoted_ident<HINT_CHR_BACKQUOTE>();
      case HINT_CHR_DOUBLEQUOTE:
        return scan_quoted_ident<HINT_CHR_DOUBLEQUOTE>();
      case HINT_CHR_ASTERISK:
        if (peek_class2() == HINT_CHR_SLASH)
        {
          ptr+= 2; // skip "*/"
          input_buf_end= ptr;
          return HINT_CLOSE;
        }
        else
          return get_byte();
      case HINT_CHR_AT:
        if (prev_token == '(' ||
            (prev_token == HINT_ARG_IDENT && whitespaces == 0))
          return scan_query_block_name();
        else
          return get_byte();
      case HINT_CHR_EOF:
        return 0;
      default:
        return get_byte();
      }
    }
  }

  template <hint_lex_char_classes Quote>
  int scan_quoted_ident()
  {
    DBUG_ASSERT(Quote == HINT_CHR_BACKQUOTE || Quote == HINT_CHR_DOUBLEQUOTE);
    DBUG_ASSERT(*ptr == '`' || *ptr == '"');

    if (Quote == HINT_CHR_DOUBLEQUOTE && !is_ansi_quotes)
      return get_byte();

    skip_byte(); // skip opening quote sign
    start_token(); // reset yytext & yyleng

    size_t double_separators= 0;

    for (;;)
    {
      hint_lex_char_classes chr_class= peek_class();
      switch (chr_class) {
      case HINT_CHR_NL:
        skip_newline();
        continue;
      case HINT_CHR_MB:
        if (skip_mb())
          return HINT_ERROR;
        continue;
      case HINT_CHR_ASTERISK:
        if (peek_class2() == HINT_CHR_SLASH)
          return HINT_ERROR; // we don't support "*/" inside quoted identifiers
        skip_byte();
        continue;
      case HINT_CHR_EOF:
        return HINT_ERROR;
      case Quote:
        if (peek_class2() == Quote)
        {
          skip_byte(); // skip quote
          skip_byte(); // skip quote
          double_separators++;
          continue;
        }
        else
        {
          if (yyleng == 0)
            return HINT_ERROR; // empty quoted identifier

          ptr++; // skip closing quote

          if (thd->charset_is_system_charset && double_separators == 0)
            return HINT_ARG_IDENT;

          LEX_STRING s;
          if (!thd->charset_is_system_charset)
          {
            if (thd->convert_string(&s, system_charset_info, yytext, yyleng,
                                    thd->charset()))
              return HINT_ERROR; // OOM etc.
          }
          else
          {
            DBUG_ASSERT(0 < double_separators && double_separators < yyleng);
            s.length= yyleng - double_separators;
            s.str= (char *) thd->alloc(s.length);
            if (s.str == NULL)
              return HINT_ERROR; // OOM
          }
          if (double_separators > 0)
            compact<Quote>(&s, yytext, yyleng, double_separators);

          raw_yytext= yytext;
          yytext= s.str;
          yyleng= s.length;
          return HINT_ARG_IDENT;
        }
      default:
        skip_byte();
      }
    }
  }

  int scan_ident()
  {
    for (;;)
    {
      hint_lex_char_classes chr_class= peek_class();
      switch (chr_class) {
      case HINT_CHR_IDENT:
      case HINT_CHR_DIGIT:
        skip_byte();
        continue;
      case HINT_CHR_MB:
        if (skip_mb())
          return HINT_ERROR;
        continue;
      case HINT_CHR_EOF:
      default:
        return HINT_ARG_IDENT;
      }
    }
  }


  int scan_query_block_name()
  {
    DBUG_ASSERT(*ptr == '@');

    skip_byte(); // skip '@'
    start_token();

    switch (peek_class()) {
    case HINT_CHR_IDENT:
    case HINT_CHR_DIGIT:
    case HINT_CHR_MB:
      return scan_ident() == HINT_ARG_IDENT ? HINT_ARG_QB_NAME : HINT_ERROR;
    case HINT_CHR_BACKQUOTE:
      return scan_quoted_ident<HINT_CHR_BACKQUOTE>() == HINT_ARG_IDENT ?
          HINT_ARG_QB_NAME : HINT_ERROR;
    case HINT_CHR_DOUBLEQUOTE:
      return scan_quoted_ident<HINT_CHR_DOUBLEQUOTE>() == HINT_ARG_IDENT ?
          HINT_ARG_QB_NAME : HINT_ERROR;
    default:
      return HINT_ERROR;
    }
  }


  int scan_ident_or_keyword()
  {
    for (;;)
    {
      switch (peek_class()) {
      case HINT_CHR_IDENT:
      case HINT_CHR_DIGIT:
        skip_byte();
        continue;
      case HINT_CHR_MB:
        return scan_ident();
      case HINT_CHR_EOF:
      default:
        const SYMBOL *symbol=
            Lex_hash::hint_keywords.get_hash_symbol(yytext, yyleng);
        if (symbol) // keyword
        {
          /*
            Override the yytext pointer to the short-living buffer with a
            long-living pointer to the same text (don't need to allocate a
            keyword string since symbol array is a global constant).
          */
          yytext= symbol->name;
          DBUG_ASSERT(yyleng == symbol->length);

          return symbol->tok;
        }

        yytext= thd->strmake(yytext, yyleng);
        return HINT_ARG_IDENT;
      }
    }
  }

  int scan_number_or_ident()
  {
    for (;;)
    {
      switch (peek_class()) {
      case HINT_CHR_DIGIT:
        skip_byte();
        continue;
      case HINT_CHR_IDENT:
      case HINT_CHR_MB:
        return scan_ident();
      case HINT_CHR_EOF:
      default:
        return HINT_ARG_NUMBER;
      }
    }
  }

  bool eof() const
  {
    DBUG_ASSERT(ptr <= input_buf_end);
    return ptr >= input_buf_end;
  }

  char peek_byte() const
  {
    DBUG_ASSERT(!eof());
    return *ptr;
  }

  hint_lex_char_classes peek_class() const
  {
    return eof() ? HINT_CHR_EOF : char_classes[static_cast<uchar>(peek_byte())];
  }

  hint_lex_char_classes peek_class2() const
  {
    DBUG_ASSERT(ptr + 1 <= input_buf_end);
    return ptr + 1 >= input_buf_end ?
        HINT_CHR_EOF : char_classes[static_cast<uchar>(ptr[1])];
  }

  void skip_newline()
  {
    DBUG_ASSERT(!eof() && peek_byte() == '\n');
    skip_byte();
    lineno++;
  }

  uchar get_byte()
  {
    DBUG_ASSERT(!eof());
    char ret= *ptr;
    yyleng++;
    ptr++;
    return ret;
  }

  void skip_byte()
  {
    DBUG_ASSERT(!eof());
    yyleng++;
    ptr++;
  }

  bool skip_mb()
  {
    size_t len = my_ismbchar(cs, ptr, input_buf_end);
    if (len == 0)
    {
      ptr++;
      yyleng++;
      return true;
    }
    ptr+= len;
    yyleng+= len;
    return false;
  }

  void start_token()
  {
    yytext= raw_yytext= ptr;
    yyleng= 0;
  }


  template <hint_lex_char_classes Separator>
  void compact(LEX_STRING *to, const char *from, size_t len, size_t doubles)
  {
    DBUG_ASSERT(doubles > 0);

    size_t d= doubles;
    char *t= to->str;
    for (const char *s= from, *end= from + len; s < end; )
    {
      switch (char_classes[(uchar) *s]) {
      case HINT_CHR_MB:
        {
          size_t len = my_ismbchar(cs, s, end);
          DBUG_ASSERT(len > 1);
          memcpy(t, s, len);
          t+= len;
          s+= len;
        }
        continue;
      case Separator:
        DBUG_ASSERT(char_classes[(uchar) *s] == Separator);
        *t++= *s++;
        s++; //skip the 2nd separator
        d--;
        if (d == 0)
        {
          memcpy(t, s, end - s);
          to->length= len - doubles;
          return;
        }
        continue;
      case HINT_CHR_EOF:
        DBUG_ASSERT(0);
        to->length= 0;
        return;
      default:
        *t++= *s++;
      }
    }
    DBUG_ASSERT(0);
    to->length= 0;
    return;
  }

  void add_hint_token_digest();

private:
  /**
    Helper function to check digest buffer for overflow before adding tokens.

    @param token        A token number to add.
  */
  void add_digest(uint token)
  {
    if (digest_state == NULL)
      return; // Digest buffer is full.

    YYSTYPE fake_yylvalue;
    /*
      YYSTYPE::LEX_STRING is designed to accept non-constant "char *": there is
      a consideration, that the lexer returns MEM_ROOT-allocated string values
      there, and the rest of server is welcome to modify that strings inplace
      (ind it does that in a few rare cases).
      The digest calculator never modify YYSTYPE::LEX_STRING::str data, so
      it is not practical to add extra memory allocation there: const_cast is
      enough.
    */
    fake_yylvalue.lex_str.str= const_cast<char *>(yytext);
    fake_yylvalue.lex_str.length= yyleng;

    digest_state= digest_add_token(digest_state, token, &fake_yylvalue);
  }
};



inline int HINT_PARSER_lex(YYSTYPE *yylval, Hint_scanner *scanner)
{
  int ret= scanner->get_next_token();
  yylval->hint_string.str= scanner->yytext;
  yylval->hint_string.length= scanner->yyleng;
  return ret;
}


void HINT_PARSER_error(THD *, Hint_scanner *, PT_hint_list **, const char *msg);


#endif /* SQL_LEX_HINTS_ICLUDED */
 