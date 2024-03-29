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

/*
 * Helper function to check if a character is a valid math sign
 *
 * Parameters:
 *   sign   The character to check
 *
 * Returns: true if the character is a valid math sign, false otherwise
 */
bool isValidMathSign(char sign)
{
  if (sign == '+' || sign == '-' || sign == '*' || sign == '/' || sign == '^')
    return true;
  return false;
}

// Documented in .h file
CList TOK_tokenize_input(const char *input, char *errmsg, size_t errmsg_sz)
{
  int i = 0;
  CList tokens = CL_new();

  while (input[i] != '\0')
  {
    if (isspace(input[i]))
      i++;
      
    else if (isdigit(input[i]) || (input[i] == '.' && isdigit(input[i + 1])))
    {
      char *end;
      // convert string to double, starting at address of input[i]
      // and store the address of the first character after the number in end
      // if the number is 1.2e3, end will point to the 'e' & double value will be 1.2
      double value = strtod(&input[i], &end);

      // append the token to the list of tokens
      CL_append(tokens, (CListElementType){TOK_VALUE, value});

      // advance i to the first character after the number
      i = end - input;
    }
    else if (input[i] == '+')
    {
      if (input[i + 1] == '+' && CL_nth(tokens, CL_length(tokens) - 1).type == TOK_VALUE && isValidMathSign(input[i + 2]))
      {
        Token prev_token = CL_remove(tokens, CL_length(tokens) - 1);
        Token new_token = {TOK_VALUE, prev_token.value + 1};
        CL_append(tokens, new_token);
        i += 2;
      }
      else
      {
        CL_append(tokens, (CListElementType){TOK_PLUS, 0.0});
        i++;
      }
    }
    else if (input[i] == '-')
    {
      if (input[i + 1] == '-' && CL_nth(tokens, CL_length(tokens) - 1).type == TOK_VALUE && isValidMathSign(input[i + 2]))
      {
        Token prev_token = CL_remove(tokens, CL_length(tokens) - 1);
        Token new_token = {TOK_VALUE, prev_token.value - 1};
        CL_append(tokens, new_token);
        i += 2;
      }
      else
      {
        CL_append(tokens, (CListElementType){TOK_MINUS, 0.0});
        i++;
      }
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
      CL_append(tokens, (CListElementType){TOK_END, 0.0});
      snprintf(errmsg, errmsg_sz, "Position %d: unexpected character %c", i + 1, input[i]);
      CL_free(tokens);
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
