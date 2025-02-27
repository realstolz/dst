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

#include "sql_lex_hints.h"
#include "lex_hash.h"
#include "parse_tree_helpers.h"
#include "sql_class.h"

/*
  Generated sources:
*/
#include "sql_yacc.h"
#include "lex_token.h"


/**
  Consrtuctor.

  @param thd_arg          The thread handler.
  @param lineno_arg       The starting line number of a hint string in a query.
  @param buf              The rest of a query buffer with hints at the start.
  @param len              The length of the buf.
  @param digest_state_arg The digest buffer to output scanned token data.
*/
Hint_scanner::Hint_scanner(THD *thd_arg,
                           size_t lineno_arg,
                           const char *buf,
                           size_t len,
                           sql_digest_state *digest_state_arg)
  : thd(thd_arg),
    cs(thd->charset()),
    is_ansi_quotes(thd->variables.sql_mode & MODE_ANSI_QUOTES),
    lineno(lineno_arg),
    char_classes(cs->state_maps->hint_map),
    input_buf(buf),
    input_buf_end(input_buf + len),
    ptr(input_buf + 3), // skip "/*+"
    prev_token(0),
    digest_state(digest_state_arg),
    raw_yytext(ptr),
    yytext(ptr),
    yyleng(0),
    has_hints(false)
{}


void HINT_PARSER_error(THD *thd, Hint_scanner *scanner, PT_hint_list **,
                       const char *msg)
{
  if (strcmp(msg, "syntax error") == 0)
    msg= ER_THD(thd, ER_WARN_OPTIMIZER_HINT_SYNTAX_ERROR);
  scanner->syntax_warning(msg);
}


/**
  @brief Push a warning message into MySQL error stack with line
  and position information.

  This function provides semantic action implementers with a way
  to push the famous "You have a syntax error near..." error
  message into the error stack, which is normally produced only if
  a parse error is discovered internally by the Bison generated
  parser.
*/

void Hint_scanner::syntax_warning(const char *msg) const
{
  /* Push an error into the error stack */
  ErrConvString err(raw_yytext, input_buf_end - raw_yytext,
                    thd->variables.character_set_client);

  push_warning_printf(thd, Sql_condition::SL_WARNING,
                      ER_PARSE_ERROR,  ER_THD(thd, ER_PARSE_ERROR),
                      msg, err.ptr(), lineno);
}


/**
  Add hint tokens to main lexer's digest calculation buffer.

  @note This function adds transformed hint keyword token values with the help
        of the TOK_HINT_ADJUST() adjustment macro.
*/
void Hint_scanner::add_hint_token_digest()
{
  if (digest_state == NULL)
    return; // cant add: digest buffer is full

  if (prev_token == 0 || prev_token == HINT_ERROR)
    return; // nothing to add

  if (prev_token == HINT_CLOSE)
  {
    if (has_hints)
      add_digest(TOK_HINT_COMMENT_CLOSE);
    return;
  }
  if (!has_hints)
  { // the 1st hint in the comment
    add_digest(TOK_HINT_COMMENT_OPEN);
    has_hints= true;
  }

  switch (prev_token) {
  case HINT_ARG_NUMBER:
    add_digest(NUM);
    break;
  case HINT_ARG_IDENT:
    add_digest((peek_class() == HINT_CHR_AT) ? TOK_IDENT_AT : IDENT);
    break;
  case HINT_ARG_QB_NAME:
    add_digest('@');
    add_digest(IDENT);
    break;
  default:
    if (prev_token <= UCHAR_MAX) // Single-char token.
      add_digest(prev_token);
    else // keyword
    {
      /* Make sure this is a known hint keyword: */
      switch (prev_token) {
      case BKA_HINT:
      case BNL_HINT:
      case DUPSWEEDOUT_HINT:
      case FIRSTMATCH_HINT:
      case INTOEXISTS_HINT:
      case LOOSESCAN_HINT:
      case MATERIALIZATION_HINT:
      case MAX_EXECUTION_TIME_HINT:
      case MRR_HINT:
      case NO_BKA_HINT:
      case NO_BNL_HINT:
      case NO_ICP_HINT:
      case NO_MRR_HINT:
      case NO_RANGE_OPTIMIZATION_HINT:
      case NO_SEMIJOIN_HINT:
      case QB_NAME_HINT:
      case SEMIJOIN_HINT:
      case SUBQUERY_HINT:
        break;
      default:
        DBUG_ASSERT(false);
      }
      add_digest(TOK_HINT_ADJUST(prev_token));
    }
  }
}
 