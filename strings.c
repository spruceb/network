#include "strings.h"
#include <string.h>

bool char_in_string(const char c, const char* string) {
  for (const char* p = string; *p != '\0'; p++) {
    if (*p == c)
      return true;
  }
  return false;
}

char* join_on(string_vector *vector, const char* join_string) {
  if (vector->length < 1) {
    char* result = malloc(sizeof(char));
    *result = '\0';
    return result;
  }
  size_t num_joins = vector->length - 1;
  size_t join_length = strlen(join_string);
  size_t joins_size = join_length * num_joins;
  size_t joined_size = 0;
  size_t_vector lengths = new_size_t_vector(0);
  for (int i = 0; i < vector->length; i++) {
    size_t length = strlen(get(vector, i));
    joined_size += length;
    append(&lengths, length);
  }
  size_t total_size = joined_size + joins_size;
  char* result_string = malloc(total_size + 1);
  result_string[total_size] = '\0';
  char* input_pointer = result_string;
  for (int i = 0; i < vector->length; i++) {
    size_t string_length = get(&lengths, i);
    memcpy(input_pointer, get(vector, i), string_length);
    input_pointer += string_length;
    if (i < vector->length - 1) {
      memcpy(input_pointer, join_string, join_length);
      input_pointer += join_length;
    }
  }
  free(lengths.data);
  return result_string;
}

string_vector split_on(const char* to_split, const char* split,
                       int max_length) {
  string_vector vec = new_string_vector(0);
  size_t length;
  if (max_length < 0) {
    length = strlen(to_split);
  } else {
    length = max_length;
  }
  int last_index = 0;
  for (int i = 0; i < length; i++) {
    if (char_in_string(to_split[i], split)) {
      char* section = malloc(sizeof(char) * (i - last_index + 1));
      section[i - last_index] = '\0';
      strncpy(section, to_split + last_index, i - last_index);
      append(&vec, section);
      i++;
      last_index = i;
    }
  }
  if (last_index < length - 1) {
    char* section = malloc(sizeof(char) * (length - last_index + 1));
    strncpy(section, to_split + last_index, length - last_index);
    section[length - last_index] = '\0';
    append(&vec, section);
  }
  return vec;
}

char* string_slice_char_ptr(const char* string, const char* start, const char* end) {
  if (!start)
    start = string;
  if (!end)
    while (*end++);
  int length = end - start;
  char* result = malloc((length + 1) * sizeof(*result));
  result[length] = '\0';
  strncpy(result, start, length);
  return result;
}

char* string_slice_ints(const char* string, int start, int end) {
  if (start < 0)
    start = 0;
  if (end < 0)
    end = strlen(string);
  return string_slice_char_ptr(string, string + start, string + end);
}
