/*
 * expr_tree.c
 *
 * A dynamically allocated tree to handle arbitrary arithmetic
 * expressions
 *
 * Author: Howdy Pierce <howdy@sleepymoose.net>
 * Contributor: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "expr_tree.h"

#define LEFT 0
#define RIGHT 1

struct _expr_tree_node
{
  ExprNodeType type;
  union
  {
    struct _expr_tree_node *child[2];
    double value;
  } n;
};

/*
 * Convert an ExprNodeType into a printable character
 *
 * Parameters:
 *   ent    The ExprNodeType to convert
 *
 * Returns: A single character representing the ent
 */
static char ExprNodeType_to_char(ExprNodeType ent)
{
  switch (ent)
  {
  case OP_SUB:
  case UNARY_NEGATE:
    return '-';
  case OP_ADD:
    return '+';
  case OP_MUL:
    return '*';
  case OP_DIV:
    return '/';
  case OP_POWER:
    return '^';
  default:
    assert(0);
  }
}

// Documented in .h file
ExprTree ET_value(double value)
{
  ExprTree tree = malloc(sizeof(struct _expr_tree_node));
  assert(tree != NULL);
  tree->type = VALUE;
  tree->n.value = value;
  return tree;
}

// Documented in .h file
ExprTree ET_node(ExprNodeType op, ExprTree left, ExprTree right)
{
  ExprTree tree = malloc(sizeof(struct _expr_tree_node));
  assert(tree != NULL);
  tree->type = op;
  tree->n.child[LEFT] = left;
  tree->n.child[RIGHT] = right;
  return tree;
}

// Documented in .h file
void ET_free(ExprTree tree)
{
  if (tree == NULL)
    return;

  if (tree->type != VALUE)
  {
    ET_free(tree->n.child[LEFT]);
    ET_free(tree->n.child[RIGHT]);
  }

  free(tree);
}

// Documented in .h file
int ET_count(ExprTree tree)
{
  if (tree == NULL)
    return 0;

  if (tree->type == VALUE)
    return 1;

  return 1 + ET_count(tree->n.child[LEFT]) + ET_count(tree->n.child[RIGHT]);
}

// Documented in .h file
int ET_depth(ExprTree tree)
{
  if (tree == NULL)
    return 0;

  if (tree->type == VALUE)
    return 1;

  int left = ET_depth(tree->n.child[LEFT]);
  int right = ET_depth(tree->n.child[RIGHT]);

  return 1 + (left > right ? left : right);
}

// Documented in .h file
double ET_evaluate(ExprTree tree)
{
  if (tree == NULL)
    return 0;

  if (tree->type == VALUE)
    return tree->n.value;

  double left = ET_evaluate(tree->n.child[LEFT]);
  double right = ET_evaluate(tree->n.child[RIGHT]);

  switch (tree->type)
  {
  case OP_ADD:
    return left + right;
  case OP_SUB:
    return left - right;
  case OP_MUL:
    return left * right;
  case OP_DIV:
    return left / right;
  case OP_POWER:
    return pow(left, right);
  case UNARY_NEGATE:
    return -left;
  default:
    assert(0);
  }
}

// Documented in .h file
size_t ET_tree2string(ExprTree tree, char *buf, size_t buf_sz)
{
  if (tree == NULL || buf == NULL || buf_sz == 0)
    return 0;

  size_t length = 0;
  char leftBuffer[buf_sz];
  char rightBuffer[buf_sz];

  // write to buffer if it is a value
  if (tree->type == VALUE)
    length = snprintf(buf, buf_sz, "%g", tree->n.value);
  else
  {
    // process the left child
    size_t leftLength = ET_tree2string(tree->n.child[LEFT], leftBuffer, buf_sz);

    // print to the buffer if unary negate
    if (tree->type == UNARY_NEGATE)
      length = snprintf(buf, buf_sz, "(-%s)", leftBuffer);

    else
    {
      // process the right child
      size_t rightLength = ET_tree2string(tree->n.child[RIGHT], rightBuffer, buf_sz);

      // print to the buffer both children
      if (tree->n.child[LEFT]->type != VALUE)
      {
        char tempBuffer[buf_sz];
        snprintf(tempBuffer, buf_sz, "%s", leftBuffer);
        strcpy(leftBuffer, tempBuffer);
        leftLength += 2;
      }

      if (tree->n.child[RIGHT]->type != VALUE)
      {
        char tempBuffer[buf_sz];
        snprintf(tempBuffer, buf_sz, "%s", rightBuffer);
        strcpy(rightBuffer, tempBuffer);
        rightLength += 2;
      }

      // finally print to the buffer the whole expression
      length = snprintf(buf, buf_sz, "(%s %c %s)", leftBuffer, ExprNodeType_to_char(tree->type), rightBuffer);
    }
  }

  // truncate the string if it is too long for the buffer
  if (length >= buf_sz)
  {
    buf[buf_sz - 2] = '$';
    buf[buf_sz - 1] = '\0';
    return buf_sz - 1;
  }

  buf[length] = '\0';
  return length;
}