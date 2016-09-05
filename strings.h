#include "vector.h"
#include <stdbool.h>

/* typedef struct { */
/*   char_vector _data; */
/* } String; */

/* #define new_string(X) _Generic((X),                                     \ */
/*                                string: new_string_String,               \ */
/*                                char*: new_string_c_str,                 \ */
/*                                void*: new_string_raw_data,              \ */
/*                                char: new_string_char,                   \ */
/*                                char_vector: new_string_char_vector,     \ */
/*                                data_vector: new_string_data_vector,     \ */
/*                                byte_vector: new_string_bytes_vector,    \ */
/*                                default: new_string_c_str                \ */
/*                                )(X) */

/* String new_string_String(String string); */
/* String new_string_c_str(char* string); */
/* String new_string_raw_data(void* data); */
/* String new_string_char(char c); */
/* String new_string_char_vector(char_vector string); */
/* String new_string_data_vector(data_vector data); */
/* String new_string_bytes_vector(byte_vector bytes); */

/* void fill_c_string(String string, char* destination); */

/* char* c_string(String string); */

bool char_in_string(const char c, const char* string);

string_vector split_on(const char* to_split, const char* split,
                       int max_length);

char* join_on(string_vector *vector, const char* join_string);

char* string_slice_int(const char* string, int start, int end);
char* string_slice_char_ptr(const char* string, const char* start, const char* end);

#define string_slice(X, I, J) _Generic((I),                             \
                                       int: string_slice_int,           \
                                       char*: string_slice_char_ptr,    \
  default: string_slice_char_ptr)(X, I, J)

char* lstrip_whitespace(const char* original);

char* rstrip_whitespace(const char* original);

char* strip_whitespace(const char* original);
