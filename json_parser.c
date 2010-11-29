#include "./json_parser.h"
#include <stdint.h>
#include <assert.h>
#include <string.h> /* strncmp */

#ifndef NULL
# define NULL ((void*)0)
#endif

#define MARK \
do {                                                                 \
  parser->data_mark = p;                                            \
  parser->data_size = 0;                                            \
} while (0)

#define CALLBACK(FOR)                                                \
do {                                                                 \
  if (0 != FOR##_callback(parser, p)) return (p - data);             \
  parser->data_mark = NULL;                                         \
} while (0)

#define CALLBACK2(FOR)                                               \
do {                                                                 \
  if (parser->on_##FOR && 0 != parser->on_##FOR(parser)) return (p - data);                \
} while (0)

#define DEFINE_CALLBACK(FOR) \
static inline int FOR##_callback (json_parser *parser, const char *p) \
{ \
  if (!parser->data_mark) return 0; \
  assert(parser->data_mark); \
  const char *mark = parser->data_mark; \
  int r = 0; \
  if (parser->on_##FOR) r = parser->on_##FOR(parser, mark, p - mark); \
  return r; \
}

DEFINE_CALLBACK(string)
DEFINE_CALLBACK(int)
DEFINE_CALLBACK(frac)
DEFINE_CALLBACK(exp)

enum state
  { s_eof
  , s_start_value
  , s_separator_or_end
  , s_string
  , s_start_int
  , s_int
  , s_start_frac_or_exp
  , s_frac
  , s_start_exp
  , s_exp
  , s_start_key_string
  , s_key_string
  , s_colon
  , s_array
  , s_object
  };

#define WS(ch) (ch == 0x20 || ch == 0x09 || ch == 0x0A || ch == 0x0D)
#define CHECK_DEPTH if (stack_size == depth) goto error;

size_t json_parser_execute (json_parser *parser,
                            const char *data,
                            size_t len,
                            size_t depth)
{
  char c, ch;
  const char *p = data, *pe;
  enum state state = parser->state;
  size_t stack_size = parser->stack_size;
  unsigned short *stack = parser->stack;

  for (p=data, pe=data+len; p != pe; p++) {
    ch = *p;

    /* skip ws */
    switch (state) {
        case s_start_value:
        case s_separator_or_end:
        case s_start_key_string:
        case s_eof:
        case s_colon:
            if (WS(ch))
                continue;
        default:
            break;
    }

    switch (state) {
        case s_start_value:
          /* start of a strings */
          if (ch == '"') {
              state = s_string;
              break;
          }
          /* start of a number */
          if (ch == 0x2D) {
            state = s_start_int;
            CALLBACK2(minus);
            break;
          }
          if (ch == '0') {
            MARK;
            state = s_start_frac_or_exp;
            break;
          }
          if (ch >= '1' && ch <= '9') {
              MARK;
              state = s_int;
              break;
          }
          /* start of an array */
          if (ch == '[') {
            CHECK_DEPTH;
            stack[stack_size++] = s_array;
            state = s_start_value;
            CALLBACK2(array_begin);
            break;
          }
          /* start of an object */
          if (ch == '{') {
            CHECK_DEPTH;
            stack[stack_size++] = s_object;
            state = s_start_key_string;
            CALLBACK2(object_begin);
            break;
          }
          goto error;
        end_of_value:
          if (stack_size == 1) {
            state = s_eof;
            if (WS(ch)) {
              break;
            }
            goto error;
          } else {
            state = s_separator_or_end;
          }
          if (WS(ch)) {
            break;
          }
        case s_separator_or_end:
          if (stack[stack_size-1] == s_array && ch == ',') {
            CALLBACK2(separator);
            state = s_start_value;
            break;
          }
          if ((stack[stack_size-1] == s_array && ch == ']') 
           || (stack[stack_size-1] == s_object && ch == '}')) {
            CALLBACK2(end);
            --stack_size;
            if (stack_size == 1) {
              state = s_eof;
            }
            break;
          }
          if (stack[stack_size-1] == s_object && ch == ',') {
            CALLBACK2(separator);
            state = s_start_key_string;
            break;
          }
          if (stack_size == 1 && WS(ch)) {
            state = s_eof;
            break;
          }
          goto error;
        case s_string:
          if (ch == '"') {
            CALLBACK(string);
            state = s_separator_or_end;
            break;
          }
          break;
        case s_start_int:
          if (ch == '0') {
            MARK;
            state = s_start_frac_or_exp;
            break;
          }
          if (ch >= '1' && ch <= '9') {
              MARK;
              state = s_int;
              break;
          }
          goto error;
        case s_int:
          if (ch >= '0' && ch <= '9') {
            break;
          }
          CALLBACK(int);
          if (ch == '.') {
            state = s_frac;
            break;
          }
          if (ch == 'e') {
            state = s_exp;
            break;
          }
          goto end_of_value;
        case s_start_frac_or_exp:
          CALLBACK(int);
          if (ch == '.') {
            state = s_frac;
            break;
          }
          if (ch == 'e') {
            state = s_exp;
            break;
          }
          goto end_of_value;
        case s_frac:
          if (! parser->data_mark && ch <= '0' && ch >= '9') {
            goto error;
          }
          if (ch >= '0' && ch <= '9') {
            if (! parser->data_mark)
              MARK;
            break;
          }
          CALLBACK(frac);
          if (ch == 'e') {
            state = s_start_exp;
            break;
          }
          goto end_of_value;
        case s_start_exp:
            if (ch == '-' || ch == '+') {
              MARK;
              state = s_exp;
              break;
            }
            goto error;
        case s_exp:
          if (! parser->data_mark && ch <= '0' && ch >= '9') {
            goto error;
          }
          if (ch >= '0' && ch <= '9') {
            break;
          }
          CALLBACK(exp);
          goto end_of_value;
        case s_start_key_string:
          if (ch == '"') {
            state = s_key_string;
            break;
          }
          goto error;
        case s_key_string:
          if (! parser->data_mark) {
            MARK;
          }
          if (ch == '"') {
            CALLBACK(string);
            state = s_colon;
            break;
          }
          break;
        case s_colon:
          if (ch == ':') {
            CALLBACK2(separator);
            state = s_start_value;
            break;
          }
          goto error;
        case s_eof:
          goto error;
        default:
            goto error;
    }
  }

  switch (state) {
    case s_key_string:
    case s_string:
      CALLBACK(string);
      break;
    case s_int:
    case s_start_frac_or_exp:
      CALLBACK(int);
      break;
    case s_frac:
      CALLBACK(frac);
      break;
    case s_exp:
      CALLBACK(exp);
      break;
  }

  parser->state = state;
  parser->stack_size = stack_size;
  return len;

error:
  parser->state = state;
  return (p - data);
}


void
json_parser_init (json_parser *parser)
{
  parser->state = s_start_value;

  parser->stack[0] = s_eof;
  parser->stack_size = 1;
  parser->data_mark = NULL;
  parser->data_size = 0;
  parser->stop_on_callback = 0;
  parser->nread = 0;

  parser->on_array_begin = NULL;
  parser->on_object_begin = NULL;
  parser->on_string = NULL;
  parser->on_separator = NULL;
  parser->on_end = NULL;
  parser->on_minus = NULL;
  parser->on_int = NULL;
  parser->on_frac = NULL;
  parser->on_exp = NULL;
}

