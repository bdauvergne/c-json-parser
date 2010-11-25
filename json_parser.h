/* Copyright 2010 Benjamin Dauvergne <bdauvergne@entrouvert.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef json_parser_h
#define json_parser_h 1
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#  include <stddef.h>
#endif
#include <sys/types.h>

#ifndef JSON_PARSER_STRICT
# define JSON_PARSER_STRICT 1
#else
# define JSON_PARSER_STRICT 0
#endif

#ifndef JSON_PARSER_DEPTH
# define JSON_PARSER_DEPTH 100
#endif

typedef struct json_parser json_parser;

/* Callbacks should return non-zero to indicate an error. The parse will
 * then halt execution.
 *
 * json_data_cb does not return data chunks. It will be call arbitrarilly
 * many times for each string.
 */
typedef int (*json_data_cb) (json_parser*, const char *at, size_t length);
typedef int (*json_cb) (json_parser*);

struct json_parser {
  unsigned short state;
  unsigned short stack[JSON_PARSER_DEPTH];
  size_t stack_size;

  void *data;
  const char *data_mark;
  size_t data_size;

  json_cb on_array_begin;
  json_cb on_object_begin;
  json_data_cb on_string;
  json_cb on_end;
  /* numbers */
  json_cb on_separator;
  json_cb on_minus;
  json_data_cb on_int;
  json_data_cb on_frac;
  json_data_cb on_exp;
};

void json_parser_init(json_parser *);
size_t json_parser_execute(json_parser *, const char *data, size_t len);

#ifdef __cplusplus
}
#endif
#endif
