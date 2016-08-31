#include "http_lib.h"

bool split_on_test__correct() {
  string_vector test = split_on("Hello I'm test", " ", -1);
  bool result = (strcmp(get(&test, 0), "Hello") == 0 &&
                 strcmp(get(&test, 1), "I'm") == 0 &&
                 strcmp(get(&test, 2), "test") == 0);
  printf("Test split_on splits a sample text correctly\n");
  return result;
}

bool split_on_test__separator() {
  string_vector test = split_on("/", "/", -1);
  bool result = test.length == 1;
  if (result)
    result &= strlen(get(&test, 0)) == 0;
  printf("Test split_on splits a string containing just a separator\n");
  return result;
}

bool join_on__join_multiple() {
  string_vector test = new_string_vector(0);
  char* word1 = "word1";
  char* word2 = "word2";
  append(&test, word1);
  append(&test, word2);
  char* result_string = join_on(&test, "1234");
  bool result = strcmp(result_string, "word11234word2") == 0;
  printf("Test join_on correctly joins two strings\n");
  return result;
}

bool join_on__join_single() {
  string_vector test = new_string_vector(0);
  char* word = "word";
  append(&test, word);
  char* result_string = join_on(&test, "1234");
  bool result = strcmp(result_string, "word") == 0;
  printf("Test join_on with one string just returns it\n");
  return result;
}

bool join_on__join_empty_joined() {
  string_vector test = new_string_vector(0);
  char* empty = "";
  append(&test, empty);
  char* result_string = join_on(&test, "/");
  bool result = strcmp(result_string, "") == 0;
  printf("Test join_on behaves correctly with empty strings in the joined vector\n");
  return result;
}

bool join_on__join_empty_vector() {
  string_vector test = new_string_vector(0);
  char* result_string = join_on(&test, "/");
  bool result = strcmp(result_string, "") == 0;
  printf("Test join_on behaves correctly with an empty vector\n");
  return result;
}

bool (*tests[]) () = {
  split_on_test__correct,
  split_on_test__separator,
  join_on__join_multiple,
  join_on__join_single,
  join_on__join_empty_joined,
  join_on__join_empty_vector
};

int main() {
  int test_count = sizeof(tests) / sizeof(tests[0]);
  for (int i = 0; i < test_count; i++) {
    bool result = tests[i]();
    if (!result) {
      fprintf(stderr, "ERROR on test %d\n", i);
      fprintf(stderr, "RESULT: %d\n", result);
      return 1;
    }
  }
  return 0;
}
