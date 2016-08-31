#include "http_lib.h"

bool split_on_test__correct() {
  string_vector test = split_on("Hello I'm test", " ", -1);
  bool result = (strcmp(get(&test, 0), "Hello") == 0 &&
                 strcmp(get(&test, 1), "I'm") == 0 &&
                 strcmp(get(&test, 2), "test") == 0);
  printf("Test split_on splits a sample text correctly\n");
  return result;
}

int main() {
  if (!split_on_test__correct()) {
    fprintf(stderr, "ERROR\n");
    return 1;
  }
  return 0;
}
