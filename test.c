#include "./json_parser.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

const char json1[] = "{ \"coucou\": 1.0 }";
const char json2[] = "{ \"coucou\": [ 1.0, -2.0e+10, 23423434324, 344354354] }";
const char json3[] = "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[ 1 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]";
const char json4[] = "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[ 1 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]";

void test_json_content (const char *data, int len_consumed, int state) {
  json_parser parser;
  int consumed = 0;

  json_parser_init(&parser);
  consumed = json_parser_execute(&parser, data, strlen(data), JSON_PARSER_DEPTH);
  assert(len_consumed == consumed);
  assert(parser.state == state);
}

int main (int argc, char **argv) {
  json_parser p;
  test_json_content(json1, strlen(json1), 0);
  test_json_content(json2, strlen(json2), 0);
  test_json_content(json3, strlen(json3), 0);
  /* this test should fail to parse the 100th open-[ */
  test_json_content(json4, 99, 1);
  printf ("Size of json_parser: %u\n", sizeof(json_parser));
}

