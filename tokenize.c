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
CList TOK_tokenize_input(const char *input, char *errmsg, size_t errmsg_sz)
{
  CList tokens = CL_new();
  // TODO: Add your code here
  // 1. Check if input is NULL\n
  if (input == NULL)
  {
    printf("Input is NULL\n");
    snprintf(errmsg, errmsg_sz, "Input is NULL\n");
    return NULL;
  }

  // 2. Check if input is empty
  if (strlen(input) == 0)
  {
    printf("Input is empty");
    snprintf(errmsg, errmsg_sz, "Input is empty");
    return NULL;
  }

  // 3. Check if input is valid
  int i = 0;
  while (input[i] != '\0')
  {
    if (input[i] != ' ' && input[i] != '+' && input[i] != '-' && input[i] != '*' && input[i] != '/' && input[i] != '^' && input[i] != '(' && input[i] != ')' && !isdigit(input[i]))
    {
      snprintf(errmsg, errmsg_sz, "Input is invalid");
      return NULL;
    }
    i++;
  }

  // 4. Tokenize input
  i = 0;
  while (input[i] != '\0')
  {
    if (input[i] == ' ')
    {
      i++;
      continue;
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
    else if (isdigit(input[i]))
    {
      char *end;
      double value = strtod(&input[i], &end);
      CL_append(tokens, (CListElementType){TOK_VALUE, value});
      i = end - input;
    }
  }

  // 5. Check if input is valid
  int open_paren = 0;
  int close_paren = 0;
  int plus = 0;
  int minus = 0;
  int multiply = 0;
  int divide = 0;
  int power = 0;
  int value = 0;
  int prev = 0;
  int next = 0;
  int count = 0;

  for (struct _cl_node *node = tokens->head; node != NULL; node = node->next)
  {
    if (node->element.type == TOK_OPEN_PAREN)
    {
      open_paren++;
      prev = 0;
      next = 1;
    }
    else if (node->element.type == TOK_CLOSE_PAREN)
    {
      close_paren++;
      prev = 1;
      next = 0;
    }
    else if (node->element.type == TOK_PLUS)
    {
      plus++;
      prev = 1;
      next = 1;
    }
    else if (node->element.type == TOK_MINUS)
    {
      minus++;
      prev = 1;
      next = 1;
    }
    else if (node->element.type == TOK_MULTIPLY)
    {
      multiply++;
      prev = 1;
      next = 1;
    }
    else if (node->element.type == TOK_DIVIDE)
    {
      divide++;
      prev = 1;
      next = 1;
    }
    else if (node->element.type == TOK_POWER)
    {
      power++;
      prev = 1;
      next = 1;
    }
    else if (node->element.type == TOK_VALUE)
    {
      value++;
      prev = 0;
      next = 0;
    }
    count++;
  }

  if (open_paren != close_paren)
  {
    snprintf(errmsg, errmsg_sz, "Input is invalid");
    return NULL;
  }

  if (prev == 1 && next == 1)
  {
    snprintf(errmsg, errmsg_sz, "Input is invalid");
    return NULL;
  }

  if (plus > 1 || minus > 1 || multiply > 1 || divide > 1 || power > 1)
  {
    snprintf(errmsg, errmsg_sz, "Input is invalid");
    return NULL;
  }

  if (value == 0)
  {
    snprintf(errmsg, errmsg_sz, "Input is invalid");
    return NULL;
  }
  return tokens;
}

// Documented in .h file
TokenType TOK_next_type(CList tokens)
{
  if (tokens == NULL)
  {
    printf("next_type - tokens is NULL\n");
    return TOK_END;
  }

  // TODO: Add your code here
  assert(tokens != NULL);
  if (tokens->head != NULL)
  {
    return tokens->head->element.type;
  }
  return TOK_END;
}

// Documented in .h file
Token TOK_next(CList tokens)
{
  if (tokens == NULL)
  {
    printf("next - tokens is NULL\n");
    return (Token){TOK_END, 0.0};
  }

  // TODO: Add your code here
  assert(tokens != NULL);
  Token nextToken = {TOK_END, 0.0}; // Initialize with a default value

  if (tokens->head != NULL)
  {
    nextToken = tokens->head->element;
    CL_remove(tokens, 0); // Remove the token from the list
  }

  return nextToken;
}

// Documented in .h file
void TOK_consume(CList tokens)
{
  if (tokens == NULL)
  {
    printf("consume - tokens is NULL\n");
    return;
  }
  // TODO: Add your code here
  assert(tokens != NULL);
  if (tokens->head != NULL)
  {
    CL_remove(tokens, 0); // Remove the token from the list
  }
}

// Documented in .h file
void TOK_print(CList tokens)
{
  if (tokens == NULL)
  {
    printf("print - tokens is NULL\n");
    return;
  }
  // TODO: Add your code here
  assert(tokens != NULL);
  for (struct _cl_node *node = tokens->head; node != NULL; node = node->next)
  {
    printf("%s ", TT_to_str(node->element.type));
  }
}
