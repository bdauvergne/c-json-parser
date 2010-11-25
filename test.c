#include "./json_parser.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

const char json1[] = "{ \"coucou\": 1.0 }";
const char json2[] = "{ \"coucou\": [ 1.0, -2.0e+10, 23423434324, 344354354] }";

void test_json_content (const char *data) {
  json_parser parser;
  size_t consumed;

  json_parser_init(&parser);
  consumed = json_parser_execute(&parser, data, strlen(data), JSON_PARSER_DEPTH);
  assert(strlen(data) == consumed);
  assert(parser.state == 0);
}

int main (int argc, char **argv) {
  test_json_content(json1);
  test_json_content(json2);
  printf ("Size of json_parser: %u\n", sizeof(json_parser));
}

