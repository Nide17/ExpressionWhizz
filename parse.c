/*
 * parse.c
 *
 * Code that implements a recursive descent parser for arithmetic
 * expressions
 *
 * Author: Niyomwungeri Parmenide ISHIMWE<parmenin@andrew.cmu.edu>
 */

#include <stdio.h>

#include "parse.h"
#include "tokenize.h"

/*
 * Forward declarations for the functions (rules) to produce the
 * ExpressionWhizz grammar.  See the assignment writeup for the grammar.
 * Each function has the same signature, so we will document all of
 * them here.
 *
 * Parameters:
 *   tokens     List of tokens remaining to be parsed
 *   errmsg     Return space for an error message, filled in in case of error
 *   errmsg_sz  The size of errmsg
 *
 * Returns: The parsed ExprTree on success. If a parsing error is
 *   encountered, copies an error message into errmsg and returns
 *   NULL.
 */
static ExprTree additive(CList tokens, char *errmsg, size_t errmsg_sz);
static ExprTree multiplicative(CList tokens, char *errmsg, size_t errmsg_sz);
static ExprTree exponential(CList tokens, char *errmsg, size_t errmsg_sz);
static ExprTree primary(CList tokens, char *errmsg, size_t errmsg_sz);

static ExprTree additive(CList tokens, char *errmsg, size_t errmsg_sz)
{
  ExprTree ret = NULL;
  // TODO: Add your code here

  // Perform the addition or subtraction recursively
  if (TOK_next_type(tokens) == TOK_PLUS)
  {
    TOK_consume(tokens);

    ExprTree right = multiplicative(tokens, errmsg, errmsg_sz);
    if (right == NULL)
    {
      snprintf(errmsg, errmsg_sz, "Expected expression after '+'");
      return NULL;
    }

    ret = ET_node(OP_ADD, ret, right);
  }
  else if (TOK_next_type(tokens) == TOK_MINUS)
  {
    TOK_consume(tokens);

    ExprTree right = multiplicative(tokens, errmsg, errmsg_sz);
    if (right == NULL)
    {
      snprintf(errmsg, errmsg_sz, "Expected expression after '-'");
      return NULL;
    }

    ret = ET_node(OP_SUB, ret, right);
  }

  return ret;
}

static ExprTree multiplicative(CList tokens, char *errmsg, size_t errmsg_sz)
{
  ExprTree ret = NULL;
  // TODO: Add your code here

  // Perform the multiplication or division recursively
  if (TOK_next_type(tokens) == TOK_MULTIPLY)
  {
    TOK_consume(tokens);

    ExprTree right = exponential(tokens, errmsg, errmsg_sz);
    if (right == NULL)
    {
      snprintf(errmsg, errmsg_sz, "Expected expression after '*'");
      return NULL;
    }

    ret = ET_node(OP_MUL, ret, right);
  }
  else if (TOK_next_type(tokens) == TOK_DIVIDE)
  {
    TOK_consume(tokens);

    ExprTree right = exponential(tokens, errmsg, errmsg_sz);
    if (right == NULL)
    {
      snprintf(errmsg, errmsg_sz, "Expected expression after '/'");
      return NULL;
    }

    ret = ET_node(OP_DIV, ret, right);
  }

  return ret;
}

static ExprTree exponential(CList tokens, char *errmsg, size_t errmsg_sz)
{
  ExprTree ret = NULL;

  // TODO: Add your code here
  // Perform the exponentiation recursively
  if (TOK_next_type(tokens) == TOK_POWER)
  {
    TOK_consume(tokens);

    ExprTree right = primary(tokens, errmsg, errmsg_sz);
    if (right == NULL)
    {
      snprintf(errmsg, errmsg_sz, "Expected expression after '^'");
      return NULL;
    }

    ret = ET_node(OP_POWER, ret, right);
  }

  return ret;
}

static ExprTree primary(CList tokens, char *errmsg, size_t errmsg_sz)
{
  ExprTree ret = NULL;

  if (TOK_next_type(tokens) == TOK_VALUE)
  {
    ret = ET_value(TOK_next(tokens).value);
    TOK_consume(tokens);
  }
  else if (TOK_next_type(tokens) == TOK_OPEN_PAREN)
  {
    // TODO: Add your code here
    // Handle parenthesized expressions
    TOK_consume(tokens);

    ret = additive(tokens, errmsg, errmsg_sz);
    if (ret == NULL)
    {
      snprintf(errmsg, errmsg_sz, "Expected expression after '('");
      return NULL;
    }

    if (TOK_next_type(tokens) != TOK_CLOSE_PAREN)
    {
      snprintf(errmsg, errmsg_sz, "Expected ')' after expression");
      return NULL;
    }

    TOK_consume(tokens);
  }
  else if (TOK_next_type(tokens) == TOK_MINUS)
  {
    TOK_consume(tokens);

    ret = primary(tokens, errmsg, errmsg_sz);
    if (ret == NULL)
      return NULL;
    ret = ET_node(UNARY_NEGATE, ret, NULL);
  }
  else
  {
    // TODO: Add your code here
    // Handle other unary operators
    if (TOK_next_type(tokens) == TOK_PLUS)
    {
      TOK_consume(tokens);

      ret = primary(tokens, errmsg, errmsg_sz);
      if (ret == NULL)
        return NULL;
      ret = ET_node(OP_SUB, ret, NULL);
    }
    else
    {
      snprintf(errmsg, errmsg_sz, "Expected value or '('");
      return NULL;
    }
  }
  return ret;
}

ExprTree Parse(CList tokens, char *errmsg, size_t errmsg_sz)
{
  // TODO: Add your code here
  // Call the first rule of the grammar
  ExprTree ret = additive(tokens, errmsg, errmsg_sz);
  if (ret == NULL)
  {
    snprintf(errmsg, errmsg_sz, "Expected expression");
    return NULL;
  }

  if (TOK_next_type(tokens) != TOK_END)
  {
    snprintf(errmsg, errmsg_sz, "Expected end of expression");
    return NULL;
  }
  return NULL;
}
