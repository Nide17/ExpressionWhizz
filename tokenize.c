/*
 * tokenize.c
 *
 * Functions to tokenize and manipulate lists of tokens
 *
 * Author: Howdy Pierce <howdy@sleepymoose.net>
 * Contributor: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "clist.h"
#include "tokenize.h"
#include "token.h"

// Documented in .h file
const char *TT_to_str(TokenType tt)
{
  switch (tt)
  {
  case TOK_VALUE:
    return "VALUE";
  case TOK_PLUS:
    return "PLUS";
  case TOK_MINUS:
    return "MINUS";
  case TOK_MULTIPLY:
    return "MULTIPLY";
  case TOK_DIVIDE:
    return "DIVIDE";
  case TOK_POWER:
    return "POWER";
  case TOK_OPEN_PAREN:
    return "OPEN_PAREN";
  case TOK_CLOSE_PAREN:
    return "CLOSE_PAREN";
  case TOK_END:
    return "(end)";
  }
  __builtin_unreachable();
}

// implement the functions in tokenize.c. Most of the functions here are convenience
// functions (though you will still need to implement them); the real work of tokenization is
// performed in TOK_tokenize_input. Spend some time to get this working correctly
// Documented in .h file
bool isValidMathSign(char sign)
{
  if (sign == '+' || sign == '-' || sign == '*' || sign == '/' || sign == '^')
    return true;
  return false;
}

bool isExponent(char exponent)
{
  if (exponent == 'e' || exponent == 'E' || exponent == 'p' || exponent == 'P')
    return true;

  return false;
}

bool isParenthesis(char parenthesis)
{
  if (parenthesis == '(' || parenthesis == ')')
    return true;

  return false;
}

bool validate_input(const char *input, char *errmsg, size_t errmsg_sz)
{
  // 3. Check if input is valid
  errmsg[0] = '\0';
  int i = 0;
  while (input[i++] != '\0')
  {
    // OPTIONAL PERIOD
    if (input[i] == '.')
    {
      if (!isdigit(input[i + 1]))
      {
        snprintf(errmsg, errmsg_sz, "Position %d: unexpected character %c", i + 1, input[i]);
        break;
      }
    }

    // EXPONENTS - e, E, p, P
    if (isExponent(input[i]))
    {
      if (!isdigit(input[i + 1]) && input[i + 1] != '+' && input[i + 1] != '-')
      {
        snprintf(errmsg, errmsg_sz, "Position %d: unexpected character %c", i + 1, input[i]);
        break;
      }
    }

    // ANY THING ELSE THAT IS NOT optional period, then a required decimal digit, then any sequence of letters,
    // digits, underscores, periods, and exponents: e+, e-, E+, E-, p+, p-, P+, and P-
    if (!isdigit(input[i]) && !isspace(input[i]) && input[i] != '.' && !isValidMathSign(input[i]) && !isParenthesis(input[i]) && !isExponent(input[i]))
    {
      snprintf(errmsg, errmsg_sz, "Position %d: unexpected character %c", i + 1, input[i]);
      break;
    }
  }

  return true;
}

CList TOK_tokenize_input(const char *input, char *errmsg, size_t errmsg_sz)
{
  errmsg[0] = '\0';
  CList tokens = CL_new();

  // 1. Validate input
  if (!validate_input(input, errmsg, errmsg_sz))
  {
    // Destroy the list of tokens
    CL_free(tokens);
    return NULL;
  }

  // 4. Tokenize input
  int i = 0;
  while (input[i] != '\0')
  {
    if (isspace(input[i]))
    {
      i++;
      continue;
    }
    else if (isdigit(input[i]))
    {
      char *end;
      double value = strtod(&input[i], &end);
      CL_append(tokens, (CListElementType){TOK_VALUE, value});
      i = end - input;
    }
    else if (input[i] == '+')
    {
      CL_append(tokens, (CListElementType){TOK_PLUS, 0.0});
      i++;
    }
    else if (input[i] == '-')
    {
      CL_append(tokens, (CListElementType){TOK_MINUS, 0.0});
      i++;
    }
    else if (input[i] == '*')
    {
      CL_append(tokens, (CListElementType){TOK_MULTIPLY, 0.0});
      i++;
    }
    else if (input[i] == '/')
    {
      CL_append(tokens, (CListElementType){TOK_DIVIDE, 0.0});
      i++;
    }
    else if (input[i] == '^')
    {
      CL_append(tokens, (CListElementType){TOK_POWER, 0.0});
      i++;
    }
    else if (input[i] == '(')
    {
      CL_append(tokens, (CListElementType){TOK_OPEN_PAREN, 0.0});
      i++;
    }
    else if (input[i] == ')')
    {
      CL_append(tokens, (CListElementType){TOK_CLOSE_PAREN, 0.0});
      i++;
    }
    else
    {
      // Destroy the list of tokens
      CL_free(tokens);
      snprintf(errmsg, errmsg_sz, "Position %d: unexpected character %c", i + 1, input[i]);
      return NULL;
    }
  }

  // 6. Return the list of tokens
  return tokens;
}

// Documented in .h file
TokenType TOK_next_type(CList tokens)
{
  if (tokens == NULL)
    return TOK_END;

  if (tokens->head != NULL)
    return tokens->head->element.type;

  return TOK_END;
}

// Documented in .h file
Token TOK_next(CList tokens)
{
  if (tokens == NULL)
    return (Token){TOK_END, 0.0};

  Token nextToken = {TOK_END, 0.0};

  if (tokens->head != NULL)
    nextToken = tokens->head->element;

  return nextToken;
}

// Documented in .h file
void TOK_consume(CList tokens)
{
  if (tokens == NULL)
    return;

  if (tokens->head != NULL)
    CL_remove(tokens, 0);
}

// Documented in .h file
void print_element(int pos, CListElementType element, void *cb_data)
{
  CListElementType *data = (CListElementType *)cb_data;

  // CListElementType
  printf("%s: %d %s\n", (char *)data, pos, TT_to_str(element.type));
}

void TOK_print(CList tokens)
{
  if (tokens == NULL)
    return;

  // For debugging: Prints the list of tokens, one per line
  CL_foreach(tokens, print_element, "DEBUG OUTPUT");
}
