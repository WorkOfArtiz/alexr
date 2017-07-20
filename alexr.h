/*        _                
 *   __ _| | _____  ___ __ 
 *  / _` | |/ _ \ \/ / '__|
 * | (_| | |  __/>  <| |   
 *  \__,_|_|\___/_/\_\_|   
 *
 * The tiny lexer.
 * @author Arthur de Fluiter, 2017
 */
#ifndef ALEXR_H
#define ALEXR_H
#include <stdio.h>
#include <stdbool.h>

#define LERR_NOERROR  "everything's fine :)"
#define LERR_WRONGSYM "wrong symbol"
#define LERR_QUOTE    "quotes weren't closed properly"
#define LERR_ALLOC    "couldn't allocate enough memory"


typedef struct token
{
    char *t_str;   /* the actual token */
    int   t_line;  /* determines the line on which the token starts */
    int   t_off;   /* determines the offset within the line */
} tok_t;

typedef struct tokstream
{
    tok_t ts_buffer;    /* this object is returned on next_token */
    char *ts_copy;      /* copy of the text to parse */
    int   ts_tokoff;    /* keeps track of at what byte offset the token starts */
} tokstream_t;

/*
 * ALEXR API
 *
 * tokenstreams (ts) are used to generate a sequence of tokens which is then
 * handed to the parser/evaluator
 */

/* nicely defined for-construct */
#define FOR_TOK(ts, tok) for (tok = curtok(ts); tok != NULL; tok = nexttok(ts))

/**
 * Prints ALEXR errors 
 *
 * @param f, file to print to.
 */
void plerror(FILE *f);

/**
 * Generates a tokenstream from a string
 *
 * @param s, containing the information to parse.
 * @param ts, the to be used tokenstream.
 * @return
 *      NULL - on failure, call plerror for details
 */
tokstream_t *str2ts(const char *s);

/**
 * Queries whether there still is a token 
 * 
 * @param ts, the stream to query
 */
bool hasnexttok(tokstream_t *ts);

/**
 * returns current token in the stream
 *
 * @param ts, tokenstream to use
 * @return
 *      NULL on end of input
 *      token on success
 */
char *curtok(tokstream_t *ts);

/**
 * Moves to next token and returns it
 *
 * @param ts, tokenstream to use
 * @return
 *      NULL on end of input
 *      token on success
 */
char *nexttok(tokstream_t *ts);

/**
 * Frees tokenstream, also frees token
 *
 * @param ts
 */
void  clean_ts(tokstream_t *ts);

/**
 * Resets the tokenstream
 *
 * @param ts
 */
void reset_ts(tokstream_t *ts);

/*
 * INTERNALS
 * small functions to identify certain tokens.
 */

inline static int is_whitespace(char c) {
    return c == '\t' || c == '\n' || c == ' ';
}

inline static int is_quote(char c) {
    return c == '"';
}

/* an operator that can be followed by another operator, e.g. >> */
inline static int is_dop(char c) {
    return c == '<' || c == '>';
}

inline static int is_op(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || is_dop(c);
}

inline static int is_parens(char c) {
    return c == '(' || c == ')';
}

inline static int is_upper(char c) {
    return (c >= 'A' && c <= 'Z');
}

inline static int is_lower(char c) {
    return (c >= 'a' && c <= 'z');
}

inline static int is_letter(char c) {
    return is_upper(c) || is_lower(c);
}

inline static int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

#endif // ALEXR_H
