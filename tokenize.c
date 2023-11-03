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
  int i = 0;
  while (input[i] != '\0')
  {
    // ANY THING ELSE THAT IS NOT optional period, then a required decimal digit, then any sequence of letters, digits, underscores, periods, and exponents: e+, e-, E+, E-, p+, p-, P+, and P-
    if (!isdigit(input[i]) && !isspace(input[i]) && input[i] != '.' && !isValidMathSign(input[i]) && !isParenthesis(input[i]) && !isExponent(input[i]))
    {
      snprintf(errmsg, errmsg_sz, "Position %d: unexpected character %c", i + 1, input[i]);
     break;
    }

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

    // OPTIONAL SIGN - +, - (if it is not followed by a digit): at most 2 for increment and decrement
    i++;
  }

  return true;
}

CList TOK_tokenize_input(const char *input, char *errmsg, size_t errmsg_sz)
{
  errmsg[0] = '\0';
  CList tokens = CL_new();

  // 1. Validate input
  if (!validate_input(input, errmsg, errmsg_sz))
    return NULL;

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
      // printf("Unexpected character %c\n", input[i]);
      snprintf(errmsg, errmsg_sz, "Position %d: unexpected character %c", i + 1, input[i]);
      return NULL;
    }
  }

  // // 5. Check if input is valid
  // int open_paren = 0;
  // int close_paren = 0;
  // int plus = 0;
  // int minus = 0;
  // int multiply = 0;
  // int divide = 0;
  // int power = 0;
  // int value = 0;
  // int prev = 0;
  // int next = 0;
  // int count = 0;

  // // POINT TO THE HEAD OF THE LIST AND TRAVERSE THE LIST
  // for (struct _cl_node *node = tokens->head; node != NULL; node = node->next)
  // {
  //   if (node->element.type == TOK_OPEN_PAREN)
  //   {
  //     open_paren++;
  //     prev = 0;
  //     next = 1;
  //   }
  //   else if (node->element.type == TOK_CLOSE_PAREN)
  //   {
  //     close_paren++;
  //     prev = 1;
  //     next = 0;
  //   }
  //   else if (node->element.type == TOK_PLUS)
  //   {
  //     plus++;
  //     prev = 1;
  //     next = 1;
  //   }
  //   else if (node->element.type == TOK_MINUS)
  //   {
  //     minus++;
  //     prev = 1;
  //     next = 1;
  //   }
  //   else if (node->element.type == TOK_MULTIPLY)
  //   {
  //     multiply++;
  //     prev = 1;
  //     next = 1;
  //   }
  //   else if (node->element.type == TOK_DIVIDE)
  //   {
  //     divide++;
  //     prev = 1;
  //     next = 1;
  //   }
  //   else if (node->element.type == TOK_POWER)
  //   {
  //     power++;
  //     prev = 1;
  //     next = 1;
  //   }
  //   else if (node->element.type == TOK_VALUE)
  //   {
  //     value++;
  //     prev = 0;
  //     next = 0;
  //   }
  //   else
  //   {
  //     snprintf(errmsg, errmsg_sz, "Input is invalid");
  //     return NULL;
  //   }
  //   count++;
  // }

  // if (open_paren != close_paren)
  // {
  //   snprintf(errmsg, errmsg_sz, "Parentheses are not balanced");
  //   return NULL;
  // }

  // if (prev == 1 && next == 1)
  // {
  //   snprintf(errmsg, errmsg_sz, "Wrong input to the left or right of the operator");
  //   return NULL;
  // }

  // if (plus > 1 || minus > 1 || multiply > 1 || divide > 1 || power > 1)
  // {
  //   snprintf(errmsg, errmsg_sz, "Operators are not balanced");
  //   return NULL;
  // }

  // if (value == 0)
  //   printf("No value\n");

  // 6. Return the list of tokens
  // printf("Number of tokens: %s\n", TOK_print(tokens));
  return tokens;
}

// Documented in .h file
TokenType TOK_next_type(CList tokens)
{
  if (tokens == NULL)
  {
    // printf("next_type - tokens is NULL\n");
    return TOK_END;
  }

  assert(tokens != NULL);
  if (tokens->head != NULL)
  {
    // printf("Next type: %s\n", TT_to_str(tokens->head->element.type));
    // printf("Next value: %f\n", tokens->head->element.value);
    return tokens->head->element.type;
  }

  return TOK_END;
}

// Documented in .h file
Token TOK_next(CList tokens)
{
  if (tokens == NULL)
    return (Token){TOK_END, 0.0};

  assert(tokens != NULL);
  Token nextToken = {TOK_END, 0.0};

  if (tokens->head != NULL)
  {
    nextToken = tokens->head->element;
    // CL_remove(tokens, 0); // Remove the token from the list
  }

  return nextToken;
}

// Documented in .h file
void TOK_consume(CList tokens)
{
  if (tokens == NULL)
    return;

  assert(tokens != NULL);
  if (tokens->head != NULL)
  {
    CL_remove(tokens, 0); // Remove the token from the list
  }
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
  {
    printf("No tokens\n");
    return;
  }

  assert(tokens != NULL);

  // For debugging: Prints the list of tokens, one per line
  CL_foreach(tokens, print_element, "DEBUG OUTPUT");
}
