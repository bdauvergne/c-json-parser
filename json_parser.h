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
 * many times for each string, same for on_int, on_frac and on on_exp. The and ofan in of a string
 * will be signaled by a call to on_separator, between a key and a value, or between two values of
 * an array or an object, or a call to on_end at the end of an array or an object.
 *
 */
typedef int (*json_data_cb) (json_parser*, const char *at, size_t length);
typedef int (*json_cb) (json_parser*);

struct json_parser {
  unsigned short state;
  size_t stack_size;
  unsigned short stop_on_callback : 1;

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
  unsigned short stack[JSON_PARSER_DEPTH];
};

void json_parser_init(json_parser *);
size_t json_parser_execute(json_parser *, const char *data, size_t len,
  size_t depth);

#ifdef __cplusplus
}
#endif
#endif
