/*        _                
 *   __ _| | _____  ___ __ 
 *  / _` | |/ _ \ \/ / '__|
 * | (_| | |  __/>  <| |   
 *  \__,_|_|\___/_/\_\_|   
 *
 * The tiny lexer.
 * @author Arthur de Fluiter, 2017
 *
 * IMPLEMENTING THE INTERFACE IN LEXER.H
 */
#include <alexr.h>
#include <string.h>
#include <stdlib.h>

/* variables needed to print error messages */
static const char *error_msg    = LERR_NOERROR;
static int         error_line;
static int         error_off;

/**
 * print the lexer error here, may need expanding.
 */
void plerror(FILE *f)  {
    fprintf(f, "[-] Lexer error on line %d, character %d: '%s'\n", 
        error_line, error_off, error_msg);
}



/**
 * The actual tokenizer is in here,
 * starts by calculating where the previous token left off 
 */
static void read_next_tok(tokstream_t *ts) {
    char   symbol;
    tok_t *tok   = &ts->ts_buffer;
    char  *read  = ts->ts_copy + (ts->ts_tokoff += strlen(tok->t_str));
    char  *write = tok->t_str;

    /* skip whitespace */
    for (; (symbol = *read); read++, ts->ts_tokoff++) {
        if (!is_whitespace(symbol))
            break;
        
        if (symbol == '\n') {
            tok->t_line++;
            tok->t_off = 1;
        }
        else {
            tok->t_off++;
        }
    }
    
    symbol = *read;

    /* if theres nothing to read, set offset to -1 */
    if (!symbol) {
        ts->ts_tokoff = -1;
        return;
    }
    
    if (is_digit(symbol)) {
        for (;is_digit(*read); tok->t_off++)
            *write++ = *read++;
    }
    else if (is_letter(symbol) || symbol == '_') {
        for (;is_letter(*read) || is_digit(*read) || *read == '_'; tok->t_off++)
            *write++ = *read++;
    }
    else if (is_quote(symbol)) {
        *write++ = *read++;

        for (;*read && !is_quote(*read); tok->t_off++)
            *write++ = *read++;

        if (!*read)
            goto wrong_quotes;

        *write++ = *read++;
        tok->t_off++;
    }
    else if (is_dop(symbol)) {
        *write++ = *read++;

        if (*read != symbol)
            goto wrong_symbol;

        *write++ = *read++;
        tok->t_off    += 1;
    }
    else if (is_op(symbol) || is_parens(symbol)) {
        *write++ = *read++;
        tok->t_off++;
    }
    else {
        goto wrong_symbol;
    }

    *write++ = '\0';
    return;

    wrong_quotes:
    ts->ts_tokoff = -1;
    error_msg  = LERR_QUOTE;
    error_line = tok->t_line;
    error_off  = tok->t_off;

    wrong_symbol:
    ts->ts_tokoff = -1;
    error_msg  = LERR_WRONGSYM;
    error_line = tok->t_line;
    error_off  = tok->t_off;
    return; 
}

tokstream_t *str2ts(const char *string) {
    tok_t       *token;
    tokstream_t *result   = malloc(sizeof(tokstream_t));
    char        *tok_str  = malloc(strlen(string) + 1);
    char        *toks_str = malloc(strlen(string) + 1);

    if (!result || !tok_str || !toks_str)
        goto alloc_error;

    strcpy(toks_str, string);
    
    /* the 'first' token will be empty */
    tok_str[0] = 0;

    result->ts_copy          = toks_str;
    result->ts_tokoff        = 0; 

    token = &result->ts_buffer;
    token->t_str  = tok_str;
    token->t_line = 1;
    token->t_off  = 1;

    /* read first real token */
    read_next_tok(result);
    return result;

    alloc_error:
    if (result)   free(result);
    if (tok_str)  free(tok_str);
    if (toks_str) free(toks_str);

    error_msg = LERR_ALLOC;
    error_line = 0;
    error_off = 0;

    return NULL;
}

bool hasnexttok(tokstream_t *ts) {
    char  *read;
    
    if (ts->ts_tokoff == -1)
        return false;

    read = ts->ts_copy + ts->ts_tokoff + strlen(ts->ts_buffer.t_str) + 1;

    /* skip whitespace */
    while (is_whitespace(*read))
        read++;
    
    return (*read != '\0');
}

char *nexttok(tokstream_t *ts) {
    if (!hasnexttok(ts))
        return NULL;
    
    read_next_tok(ts);
    return curtok(ts);
}

char *curtok(tokstream_t *ts) {
    if (ts->ts_tokoff == -1)
        return NULL;

    return ts->ts_buffer.t_str;
}

void  clean_ts(tokstream_t *ts) {
    if (ts) {
        if (ts->ts_copy) free(ts->ts_copy);
        if (ts->ts_buffer.t_str) free(ts->ts_buffer.t_str);
        free(ts);
    }
}

void reset_ts(tokstream_t *ts) {
    ts->ts_buffer.t_str[0] = '\0';
    ts->ts_tokoff = 0;
    read_next_tok(ts);
}
