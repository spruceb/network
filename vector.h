#ifndef VECTOR_H_
#define VECTOR_H_
#include <stdlib.h>

typedef uint8_t* bytes;
#define DEFINE_VECTOR_HEADER(T, N)                                      \
                                                                        \
  typedef struct {                                                      \
    T *data;                                                            \
    size_t length;                                                      \
    size_t capacity;                                                    \
  } N##_vector;                                                         \
                                                                        \
  N##_vector new_##N##_vector(size_t length);                           \
                                                                        \
  T get_##N(N##_vector *vector, int index);                             \
                                                                        \
  void set_##N(N##_vector *vector, int index, T value);                 \
                                                                        \
  int append_##N(N##_vector *vector, T value);

#define DEFINE_VECTOR(T, N)                                             \
                                                                        \
  N##_vector new_##N##_vector(size_t length) {                          \
    N##_vector new_vec;                                                 \
    new_vec.data = malloc(sizeof(T) * length);                          \
    new_vec.length = length;                                            \
    new_vec.capacity = length;                                          \
    return new_vec;                                                     \
  }                                                                     \
                                                                        \
  T get_##N(N##_vector *vector, int index) {                            \
    if (index >= vector->length || (index < 0) &&                       \
        (-index) > vector->length) {                                    \
      fprintf(stderr,                                                   \
              "Index %d is out of array bounds (length %zd)\n",         \
              index, vector->length);                                   \
      return *(vector->data);                                           \
    }                                                                   \
    if (index >= 0) {                                                   \
      return vector->data[index];                                       \
    } else {                                                            \
      return vector->data[vector->length + index];                      \
    }                                                                   \
  }                                                                     \
                                                                        \
  void set_##N(N##_vector *vector, int index, T value) {                \
    if (index >= vector->length || (index < 0) &&                       \
        (-index) > vector->length) {                                    \
      fprintf(stderr,                                                   \
              "Index %d is out of array bounds (length %zd)\n",         \
              index, vector->length);                                   \
      return;                                                           \
    }                                                                   \
    if (index >= 0) {                                                   \
      vector->data[index] = value;                                      \
    } else {                                                            \
      vector->data[vector->length - index] = value;                     \
    }                                                                   \
  }                                                                     \
                                                                        \
  int append_##N(N##_vector *vector, T value) {                         \
    if (vector->length >= vector->capacity) {                           \
      T *realloced = realloc(vector->data,                              \
                             (vector->capacity + 1) * 2 * sizeof(T));   \
      if (!realloced) {                                                 \
        free(vector->data);                                             \
        return -1;                                                      \
      }                                                                 \
      vector->data = realloced;                                         \
      vector->capacity = (vector->capacity + 1) * 2;                    \
    }                                                                   \
    vector->data[vector->length] = value;                               \
    vector->length++;                                                   \
    return vector->length;                                              \
  }


DEFINE_VECTOR_HEADER(int, int)
DEFINE_VECTOR_HEADER(char *, string)
DEFINE_VECTOR_HEADER(double, double)
DEFINE_VECTOR_HEADER(float, float)
DEFINE_VECTOR_HEADER(void *, data)
DEFINE_VECTOR_HEADER(bytes, byte)
DEFINE_VECTOR_HEADER(char, char)
DEFINE_VECTOR_HEADER(size_t, size_t)

#define get(X, I) _Generic((X),                                         \
                             int_vector *: get_int,                     \
                           string_vector *: get_string,                 \
                             double_vector *: get_double,               \
                           float_vector *: get_float,                   \
                             data_vector *: get_data,                   \
                           char_vector *: get_char,                     \
                             size_t_vector *: get_size_t,               \
                           byte_vector *: get_byte,                     \
 default: get_int                                                       \
                             )(X, I)


#define set(X, I, V) _Generic((X),                                      \
                              int_vector *: set_int,                    \
                              string_vector *: set_string,              \
                              double_vector *: set_double,              \
                              float_vector *: set_float,                \
                              data_vector *: set_data,                  \
                              char_vector *: set_char,                  \
                              size_t_vector *: set_size_t,              \
                              byte_vector *: set_byte,                  \
  default: set_int                                                      \
                                )(X, I, V)


#define append(X, V) _Generic((X),                                      \
                              int_vector *: append_int,                 \
                              string_vector *: append_string,           \
                              double_vector *: append_double,           \
                              float_vector *: append_float,             \
                              data_vector *: append_data,               \
                              char_vector *: append_char,               \
                              size_t_vector *: append_size_t,           \
                              byte_vector *: append_byte,               \
  default: append_int                                                   \
                              )(X, V)

#endif
