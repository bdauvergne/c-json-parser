A simple JSON parser
====================

This parser is inpired by ry's http parser. As JSON can represent recursive
data structure, this parser could consume an unlimited amount of memory for its
state stack. The default depth supported is 100. For this depth structure size
is 256 bytes. The parser does not need memory allocation.

TODO
----

- optimize the state stack by not using a full byte for each level, as there is
  only two states, instead use a bit array.
- add a parameter for limiting string length
- correctly handle the unicode encoding as prescribed in
  http://www.ietf.org/rfc/rfc4627.txt
- check UTF-8 encoding of characters in strings
- add a higher level parser built on the lower level one which report only fully
  parsed string and numbers.
