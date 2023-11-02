/*
 * token_test.c
 *
 * Tests for tokenization functions
 *
 * Author: Howdy Pierce <howdy@sleepymoose.net>
 * Contributor: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h> // strlen
#include <ctype.h>  // isblank
#include <math.h>   // fabs
#include <stdbool.h>

#include "clist.h"
#include "token.h"
#include "tokenize.h"
#include "expr_tree.h"
#include "parse.h"

// Checks that value is true; if not, prints a failure message and
// returns 0 from this function
#define test_assert(value)                                         \
  {                                                                \
    if (!(value))                                                  \
    {                                                              \
      printf("FAIL %s[%d]: %s\n", __FUNCTION__, __LINE__, #value); \
      goto test_error;                                             \
    }                                                              \
  }

Token tokens[] = {{TOK_VALUE, 2}, {TOK_PLUS}, {TOK_MINUS}, {TOK_MULTIPLY}, {TOK_DIVIDE}, {TOK_POWER}, {TOK_OPEN_PAREN}, {TOK_CLOSE_PAREN}, {TOK_END}, {TOK_DIVIDE}, {TOK_POWER}};

const int num_tokens = sizeof(tokens) / sizeof(tokens[0]);

/*
 * Returns true if tok1 and tok2 compare equally, false otherwise
 */
static bool test_tok_eq(Token tok1, Token tok2)
{
  if (tok1.type != tok2.type)
    return false;

  if (tok1.type == TOK_VALUE && fabs(tok1.value - tok2.value) >= 0.0001)
    return false;

  return true;
}

struct inspect_data
{
  int num_calls;
  int num_correct;
};

void inspect_element(int pos, CListElementType element, void *data)
{
  struct inspect_data *id = (struct inspect_data *)data;

  id->num_calls++;
  if (test_tok_eq(element, tokens[pos]))
    id->num_correct++;
}

/*
 * Tests the CL_new, CL_free, CL_append, CL_length, CL_nth, CL_foreach
 * and CL_pop functions, using struct token type. This should give you
 * some assurance that your CList is working correctly.
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_cl_token()
{
  struct inspect_data id = {0, 0};
  CList list = CL_new();

  for (int i = 0; i < num_tokens; i++)
  {
    CL_append(list, tokens[i]);
    test_assert(CL_length(list) == i + 1);
    test_assert(test_tok_eq(CL_nth(list, i), tokens[i]));
  }

  // test foreach
  CL_foreach(list, inspect_element, &id);
  test_assert(id.num_calls == num_tokens);
  test_assert(id.num_correct == num_tokens);

  // pop everything off, make sure that works
  for (int i = 0; i < num_tokens; i++)
  {
    test_assert(test_tok_eq(CL_pop(list), tokens[i]));
  }

  test_assert(CL_length(list) == 0);

  CL_free(list);
  return 1;

test_error:
  CL_free(list);
  return 0;
}

/*
 * Exactly like strcmp, but ignores spaces.  Therefore the following
 * strings compare alike: "ab", " ab", "  a  b  ", "a b"
 *
 * Parameters:
 *   s1, s2    The strings to be compared
 *
 * Returns: -1, 0, or 1 depending on whether s1 sorts before, equal
 * to, or after s2.
 */
static int strcmp_sp(const char *s1, const char *s2)
{
  // advance past leading spaces
  while (isblank(*s1))
    s1++;
  while (isblank(*s2))
    s2++;

  while (*s1 != '\0')
  {
    if (isblank(*s1))
      s1++;
    else if (isblank(*s2))
      s2++;
    else if (*s1 != *s2)
      return (*s1 - *s2);
    else
    {
      s1++;
      s2++;
    }
  }

  while (isblank(*s2))
    s2++;

  return (*s1 - *s2);
}

/*
 * Tests the ET_node, ET_value, ET_tree2string, ET_depth, and
 * ET_evaluate functions. This should provide some assurance that your
 * ExprTree module is working correctly.
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_expr_tree()
{
  ExprTree tree = NULL;
  char buffer[1024];
  double result;
  size_t len;
  int depth;
  const double value = 0.125;

  // -(0.125) (using unary negation)
  tree = ET_node(UNARY_NEGATE, ET_value(value), NULL);

  len = ET_tree2string(tree, buffer, sizeof(buffer));
  result = ET_evaluate(tree);
  depth = ET_depth(tree);
  test_assert(result == -value);
  test_assert(strcmp_sp(buffer, "(-0.125)") == 0);
  test_assert(strlen(buffer) == len);
  test_assert(depth == 2);

  // double unary negation: (- (-0.125)))
  tree = ET_node(UNARY_NEGATE, tree, NULL);

  len = ET_tree2string(tree, buffer, sizeof(buffer));
  result = ET_evaluate(tree);
  depth = ET_depth(tree);
  test_assert(result == value);
  test_assert(strcmp_sp(buffer, "(-(-0.125))") == 0);
  test_assert(strlen(buffer) == len);
  test_assert(depth == 3);

  ET_free(tree);

  // 6.5 * (4 + 3)
  tree = ET_node(OP_ADD, ET_value(4), ET_value(3));
  tree = ET_node(OP_MUL, ET_value(6.5), tree);

  len = ET_tree2string(tree, buffer, sizeof(buffer));
  result = ET_evaluate(tree);
  depth = ET_depth(tree);

  test_assert(result == 45.5);
  test_assert(strcmp_sp(buffer, "(6.5 * (4 + 3))") == 0);
  test_assert(strlen(buffer) == len);
  test_assert(depth == 3);

  ET_free(tree);
  return 1;

test_error:
  ET_free(tree);
  return 0;
}

/*
 * Tests the TOK_next_type and TOK_consume functions
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_tok_next_consume()
{
  CList list = CL_new();

  for (int i = 0; i < num_tokens; i++)
  {
    CL_append(list, tokens[i]);
    test_assert(CL_length(list) == i + 1);
    test_assert(test_tok_eq(CL_nth(list, i), tokens[i]));
  }

  for (int i = 0; i < num_tokens; i++)
  {
    test_assert(TOK_next_type(list) == tokens[i].type);
    TOK_consume(list);
  }

  test_assert(CL_length(list) == 0);

  test_assert(TOK_next_type(list) == TOK_END);
  TOK_consume(list);
  test_assert(TOK_next_type(list) == TOK_END);
  TOK_consume(list);
  test_assert(TOK_next_type(list) == TOK_END);
  TOK_consume(list);

  CL_free(list);
  return 1;

test_error:
  CL_free(list);
  return 0;
}

/*
 * Tests the TOK_tokenize_input function
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_tokenize_input()
{
  typedef struct
  {
    const char *string;
    const Token exp_tokens[100];
  } test_matrix_t;

  test_matrix_t tests[] =
      {
          // from writeup examples
          {"3.5", {{TOK_VALUE, 3.5}, {TOK_END}}},
          {"     3", {{TOK_VALUE, 3}, {TOK_END}}},
          {"      10  ", {{TOK_VALUE, 10}, {TOK_END}}},
          {"      -125.25  ", {{TOK_MINUS, 0}, {TOK_VALUE, 125.25}, {TOK_END}}},
          {"3 + 5", {{TOK_VALUE, 3}, {TOK_PLUS, 0}, {TOK_VALUE, 5}, {TOK_END}}},
          {"3 + 5 * 2", {{TOK_VALUE, 3}, {TOK_PLUS, 0}, {TOK_VALUE, 5}, {TOK_MULTIPLY, 0}, {TOK_VALUE, 2}, {TOK_END}}},

          // additional test cases
          {"(3 + 5) * 2", {{TOK_OPEN_PAREN, 0}, {TOK_VALUE, 3}, {TOK_PLUS, 0}, {TOK_VALUE, 5}, {TOK_CLOSE_PAREN, 0}, {TOK_MULTIPLY, 0}, {TOK_VALUE, 2}, {TOK_END}}},
          {"3 + (5 * 2)", {{TOK_VALUE, 3}, {TOK_PLUS, 0}, {TOK_OPEN_PAREN, 0}, {TOK_VALUE, 5}, {TOK_MULTIPLY, 0}, {TOK_VALUE, 2}, {TOK_CLOSE_PAREN, 0}, {TOK_END}}},
          {"3 + 5 * (2 - 1)", {{TOK_VALUE, 3}, {TOK_PLUS, 0}, {TOK_VALUE, 5}, {TOK_MULTIPLY, 0}, {TOK_OPEN_PAREN, 0}, {TOK_VALUE, 2}, {TOK_MINUS, 0}, {TOK_VALUE, 1}, {TOK_CLOSE_PAREN, 0}, {TOK_END}}},
          {"3 + 5 * (2 - 1) / 4", {{TOK_VALUE, 3}, {TOK_PLUS, 0}, {TOK_VALUE, 5}, {TOK_MULTIPLY, 0}, {TOK_OPEN_PAREN, 0}, {TOK_VALUE, 2}, {TOK_MINUS, 0}, {TOK_VALUE, 1}, {TOK_CLOSE_PAREN, 0}, {TOK_DIVIDE, 0}, {TOK_VALUE, 4}, {TOK_END}}},
          {"3 + 5 * (2 - 1) / 4 ^ 2", {{TOK_VALUE, 3}, {TOK_PLUS, 0}, {TOK_VALUE, 5}, {TOK_MULTIPLY, 0}, {TOK_OPEN_PAREN, 0}, {TOK_VALUE, 2}, {TOK_MINUS, 0}, {TOK_VALUE, 1}, {TOK_CLOSE_PAREN, 0}, {TOK_DIVIDE, 0}, {TOK_VALUE, 4}, {TOK_POWER, 0}, {TOK_VALUE, 2}, {TOK_END}}},
          // {"3 + 5 * (2 - 1) / 4 ^ 2 - 1", {{TOK_VALUE, 3}, {TOK_PLUS, 0}, {TOK_VALUE, 5}, {TOK_MULTIPLY, 0}, {TOK_OPEN_PAREN, 0}, {TOK_VALUE, 2}, {TOK_MINUS, 0}, {TOK_VALUE, 1}, {TOK_CLOSE_PAREN, 0}, {TOK_DIVIDE, 0}, {TOK_VALUE, 4}, {TOK_POWER, 0}, {TOK_VALUE, 2}, {TOK_MINUS, 0}, {TOK_VALUE, 1}, {TOK_END}}},
          // {"3 + 5 * (2 - 1) / 4 ^ 2 - 1 * 2", {{TOK_VALUE, 3}, {TOK_PLUS, 0}, {TOK_VALUE, 5}, {TOK_MULTIPLY, 0}, {TOK_OPEN_PAREN, 0}, {TOK_VALUE, 2}, {TOK_MINUS, 0}, {TOK_VALUE, 1}, {TOK_CLOSE_PAREN, 0}, {TOK_DIVIDE, 0}, {TOK_VALUE, 4}, {TOK_POWER, 0}, {TOK_VALUE, 2}, {TOK_MINUS, 0}, {TOK_VALUE, 1}, {TOK_MULTIPLY, 0}, {TOK_VALUE, 2}, {TOK_END}}},

          // empty inputs
          {"", {{TOK_END}}},
          {" ", {{TOK_END}}},
          {"     \t\n\r", {{TOK_END}}}};

  const int num_tests = sizeof(tests) / sizeof(test_matrix_t);
  char errmsg[128];

  CList list;

  for (int i = 0; i < num_tests; i++)
  {
    list = TOK_tokenize_input(tests[i].string, errmsg, sizeof(errmsg));

    for (int t = 0; tests[i].exp_tokens[t].type != TOK_END; t++)
    {
      // printf("Type: %d\n", TOK_next_type(list));
      // printf("Value: %f\n", TOK_next(list).value);
      // printf("Expected Type: %d\n", tests[i].exp_tokens[t].type);
      test_assert(TOK_next_type(list) == tests[i].exp_tokens[t].type);
      if (TOK_next_type(list) == TOK_VALUE)
        test_assert(TOK_next(list).value == tests[i].exp_tokens[t].value);
      TOK_consume(list);
    }

    CL_free(list);
    list = NULL;
  }

  // // Test erroneous inputs
  // test_assert(TOK_tokenize_input("3pi", errmsg, sizeof(errmsg)) == NULL);
  // test_assert(strcasecmp(errmsg, "Position 2: unexpected character p") == 0);

  //
  // TODO: Add your code here
  //

  return 1;

test_error:
  CL_free(list);
  return 0;
}

/*
 * Runs the parser on one test case, and checks that the resultant
 * ExprTree matches the expected results for both depth and evaluated
 * value.
 *
 * Parameters:
 *   exp_value    The value we expect
 *   exp_depth    The expected depth of the ExprTree
 *   token_arr    An array of tokens, which must be terminated with TOK_END
 *
 * Returns: 1 if the test passes, 0 otherwise
 */
int test_parse_once(double exp_value, int exp_depth, const Token token_arr[])
{
  CList tokens = NULL;
  ExprTree tree = NULL;
  char errmsg[256];

  tokens = CL_new();

  for (int i = 0; token_arr[i].type != TOK_END; i++)
    CL_append(tokens, token_arr[i]);

  tree = Parse(tokens, errmsg, sizeof(errmsg));

  test_assert(ET_depth(tree) == exp_depth);
  test_assert(fabs(ET_evaluate(tree) - exp_value) < 0.0001);

  CL_free(tokens);
  ET_free(tree);

  return 1;

test_error:
  CL_free(tokens);
  ET_free(tree);
  return 0;
}

/*
 * Tests the recursive descent parser
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_parse()
{
  test_assert(test_parse_once(3.5, 1, (Token[]){{TOK_VALUE, 3.5}, {TOK_END}}));
  // test_assert(test_parse_once(3.5, 1, (Token[]){{TOK_VALUE, 3.5}, {TOK_PLUS}, {TOK_VALUE, 0}, {TOK_END}}));
  // test_assert(test_parse_once(3.5, 1, (Token[]){{TOK_VALUE, 3.5}, {TOK_PLUS}, {TOK_VALUE, 0}, {TOK_PLUS}, {TOK_VALUE, 0}, {TOK_END}}));
  // test_assert(test_parse_once(3.5, 1, (Token[]){{TOK_VALUE, 3.5}, {TOK_PLUS}, {TOK_VALUE, 0}, {TOK_PLUS}, {TOK_VALUE, 0}, {TOK_PLUS}, {TOK_VALUE, 0}, {TOK_END}}));
  // test_assert(test_parse_once(3.5, 1, (Token[]){{TOK_VALUE, 3.5}, {TOK_PLUS}, {TOK_VALUE, 0}, {TOK_PLUS}, {TOK_VALUE, 0}, {TOK_PLUS}, {TOK_VALUE, 0}, {TOK_PLUS}, {TOK_VALUE, 0}, {TOK_END}}));
  return 1;

test_error:
  return 0;
}

/*
 * Tests the recursive descent parser for proper associativity: + - *
 * / are left-associative, whereas ^ is right-associative
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_parse_associativity()
{
  test_assert(test_parse_once(5, 3,
                              (Token[]){{TOK_VALUE, 10}, {TOK_MINUS}, {TOK_VALUE, 2}, {TOK_MINUS}, {TOK_VALUE, 3}, {TOK_END}}));

  // test_assert(test_parse_once(5, 3, (Token[]){{TOK_VALUE, 10}, {TOK_MINUS}, {TOK_VALUE, 2}, {TOK_MINUS}, {TOK_VALUE, 3}, {TOK_MINUS}, {TOK_VALUE, 4}, {TOK_END}}));

  return 1;

test_error:
  return 0;
}

/*
 * Tests the recursive descent parser with erroneous inputs
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_parse_errors()
{
  char errmsg[128];
  CList tokens = NULL;

  tokens = TOK_tokenize_input("2++3", errmsg, sizeof(errmsg));
  test_assert(CL_length(tokens) == 4);
  test_assert(Parse(tokens, errmsg, sizeof(errmsg)) == NULL); // TODO: check error message
  test_assert(strcasecmp(errmsg, "Unexpected token PLUS") == 0);
  CL_free(tokens);

  tokens = TOK_tokenize_input("3 + (2*", errmsg, sizeof(errmsg));
  test_assert(CL_length(tokens) == 5);
  test_assert(Parse(tokens, errmsg, sizeof(errmsg)) == NULL);
  test_assert(strcasecmp(errmsg, "Unexpected token (end)") == 0);
  CL_free(tokens);

  tokens = TOK_tokenize_input("3 + ) 2", errmsg, sizeof(errmsg));
  test_assert(CL_length(tokens) == 4);
  test_assert(Parse(tokens, errmsg, sizeof(errmsg)) == NULL);
  test_assert(strcasecmp(errmsg, "Unexpected token CLOSE_PAREN") == 0);
  CL_free(tokens);

  tokens = TOK_tokenize_input("1 + 2 (", errmsg, sizeof(errmsg));
  test_assert(CL_length(tokens) == 4);
  test_assert(Parse(tokens, errmsg, sizeof(errmsg)) == NULL);
  test_assert(strcasecmp(errmsg, "Syntax error on token OPEN_PAREN") == 0);
  CL_free(tokens);

  // tokens = TOK_tokenize_input("sine", errmsg, sizeof(errmsg)); // unexpected char s - token (end)
  // test_assert(CL_length(tokens) == 0);
  // test_assert(Parse(tokens, errmsg, sizeof(errmsg)) == NULL);
  // printf("Error message: %s\n", errmsg);
  // test_assert(strcasecmp(errmsg, "Position 1: unexpected character s") == 0);
  // CL_free(tokens);

  return 1;

test_error:
  CL_free(tokens);
  return 0;
}

int main()
{
  int passed = 0;
  int num_tests = 0;

  num_tests++;
  passed += test_cl_token();
  num_tests++;
  passed += test_expr_tree();
  num_tests++;
  passed += test_tok_next_consume();
  num_tests++;
  passed += test_tokenize_input();
  num_tests++;
  passed += test_parse();
  num_tests++;
  passed += test_parse_associativity();
  num_tests++;
  passed += test_parse_errors();

  printf("Passed %d/%d test cases\n", passed, num_tests);
  fflush(stdout);
  return 0;
}
