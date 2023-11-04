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

bool isValidInput(const char *input, char *errmsg, size_t errmsg_sz)
{
  // 3. Check if input is valid
  int i = 0;
  while (input[i] != '\0')
  {
    // OPTIONAL PERIOD - MUST BE FOLLOWED BY A DIGIT
    if (input[i] == '.')
    {
      if (!isdigit(input[i + 1]))
      {
        printf("Error: unexpected digit %c\n", input[i]);
        snprintf(errmsg, errmsg_sz, "Position %d: unexpected character %c", i + 1, input[i]);
        return false;
      }
    }

    // EXPONENTS - e, E, p, P
    if (isExponent(input[i]))
    {
      if (!isdigit(input[i - 1]) || (!isdigit(input[i + 1]) && input[i + 1] != '+' && input[i + 1] != '-'))
      {
        printf("Error: unexpected exponent %c\n", input[i]);
        snprintf(errmsg, errmsg_sz, "Position %d: unexpected character %c", i + 1, input[i]);
        return false;
      }

      // OPTIONAL SIGN - MUST BE FOLLOWED BY A DIGIT
      if (input[i + 1] == '+' || input[i + 1] == '-')
      {
        if (!isdigit(input[i + 2]))
        {
          printf("Error: unexpected sign %c\n", input[i]);
          snprintf(errmsg, errmsg_sz, "Position %d: unexpected character %c", i + 1, input[i]);
          return false;
        }
      }
    }

    // Any thing else that is not a required decimal digit, then any sequence of letters,
    // digits, underscores, periods, and exponents: e+, e-, E+, E-, p+, p-, P+, and P-
    if (!isdigit(input[i]) && input[i] != ' ' && input[i] != '.' && !isValidMathSign(input[i]) && !isParenthesis(input[i]) && !isExponent(input[i]) && input[i] != '_')
    {
      // Look ahead to check if it's part of a valid sequence
      int j = i + 1;
      while (isdigit(input[j]) || input[j] == '_' || input[j] == '.' ||
             (input[j] == 'e' && (input[j + 1] == '+' || input[j + 1] == '-') && isdigit(input[j + 2])) ||
             (input[j] == 'E' && (input[j + 1] == '+' || input[j + 1] == '-') && isdigit(input[j + 2])) ||
             (input[j] == 'p' && (input[j + 1] == '+' || input[j + 1] == '-') && isdigit(input[j + 2])) ||
             (input[j] == 'P' && (input[j + 1] == '+' || input[j + 1] == '-') && isdigit(input[j + 2])))
      {
        j++;
      }
      // If we found a valid sequence, continue from the last valid character
      if (j > i + 1)
      {
        i = j - 1;
      }
      else
      {
        printf("Error: unexpected character %c\n", input[i]);
        snprintf(errmsg, errmsg_sz, "Position %d: unexpected character %c", i + 1, input[i]);
        return false;
      }
    }

    i++;
  }

  return true;
}

// Documented in .h file
CList TOK_tokenize_input(const char *input, char *errmsg, size_t errmsg_sz)
{
  errmsg[0] = '\0';
  printf("Input: %s\n", input);
  CList tokens = CL_new();
  printf("Tokens: %p\n", tokens);

  // 1. Validate input
  if (!isValidInput(input, errmsg, errmsg_sz))
  {
    printf("Invalid input: %s\n", input);
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
    }
    else if (isdigit(input[i]))
    {
      char *end;

      // convert string to double, starting at address of input[i] 
      // and store the address of the first character after the number in end
      // if the number is 1.2e3, end will point to the 'e' & double value will be 1.2
      double value = strtod(&input[i], &end); 

      // append the token to the list of tokens
      printf("Value: %f\n", value);
      CL_append(tokens, (CListElementType){TOK_VALUE, value});
      printf("Tokens: %p\n", tokens);

      // advance i to the first character after the number
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
      printf("Error: unexpected last character %c\n", input[i]);
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
