/*
 * token.h
 *
 * Enum containing all possible tokens
 *
 * Author: Howdy Pierce <howdy@sleepymoose.net>
 * Contributor: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */

#ifndef _TOKEN_H_
#define _TOKEN_H_

typedef enum {
  TOK_VALUE,
  TOK_PLUS,
  TOK_MINUS,
  TOK_MULTIPLY,
  TOK_DIVIDE,
  TOK_POWER,
  TOK_OPEN_PAREN,
  TOK_CLOSE_PAREN,
  TOK_END
} TokenType;

typedef struct {
  TokenType type;
  double value;
} Token;


#endif /* _TOKEN_H_ */
