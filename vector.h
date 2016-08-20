#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdio.h>
#include <stdlib.h>

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

DEFINE_VECTOR_HEADER(int, int)
DEFINE_VECTOR_HEADER(char *, string)
DEFINE_VECTOR_HEADER(double, double)
DEFINE_VECTOR_HEADER(float, float)
DEFINE_VECTOR_HEADER(void *, data)
DEFINE_VECTOR_HEADER(char, char)
DEFINE_VECTOR_HEADER(size_t, size_t)

#define get(X, I) _Generic((X),                                         \
                           int_vector *: get_int,                       \
                           string_vector *: get_string,                 \
                           double_vector *: get_double,                 \
                           float_vector *: get_float,                   \
                           data_vector *: get_data,                     \
                           char_vector *: get_char,                     \
                           size_t_vector *: get_size_t,                 \
                           default: get_int                             \
                           )(X, I)                                      


#define set(X, I, V) _Generic((X),                                      \
                                int_vector *: set_int,                  \
                                string_vector *: set_string,            \
                                double_vector *: set_double,            \
                                float_vector *: set_float,              \
                                data_vector *: set_data,                \
                                char_vector *: set_char,                \
                                size_t_vector *: set_size_t             \
                                default: set_int                        \
                                )(X, I, V)


#define append(X, V) _Generic((X),                                      \
                              int_vector *: append_int,                 \
                              string_vector *: append_string,           \
                              double_vector *: append_double,           \
                              float_vector *: append_float,             \
                              data_vector *: append_data,               \
                              char_vector *: append_char,               \
                              size_t_vector *: append_size_t            \
                              default: append_int                       \
                              )(X, V)

#endif
