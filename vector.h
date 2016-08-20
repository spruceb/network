#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdio.h>

#define DEFINE_VECTOR(T)                                                \
  typedef struct {                                                      \
    T *data;                                                            \
    size_t length;                                                      \
    size_t capacity;                                                    \
  } ##T_vector;                                                         \
                                                                        \
  ##T_vector new_##T_vector(size_t length) {                            \
    ##T_vector new_vec;                                                 \
    new_vec.data = malloc(sizeof(T) * length);                          \
    new_vec.length = length;                                            \
    new_vec.capacity = length;                                          \
  }                                                                     \
                                                                        \
  T get_##T(##T_vector *vector, int index) {                            \
    if (index >= vector->length || (-index) > vector->length) {         \
      fprintf(stderr,                                                   \
            "Index %d is out of array bounds (length %d)",              \
            index, vector->length);                                     \
      return *(vector->data);                                           \
    }                                                                   \
    if (index >= 0) {                                                   \
      return vector->data[index];                                       \
    } else {                                                            \
      return vector->data[vector->length + index];                      \
    }                                                                   \
  }                                                                     \
                                                                        \
  void set_##T(##T_vector *vector, int index, T value) {                \
    if (index >= vector->length || (-index) > vector->length) {         \
      fprintf(stderr,                                                   \
            "Index %d is out of array bounds (length %d)",              \
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












#endif
