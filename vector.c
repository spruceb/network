#include "vector.h"

#define DEFINE_VECTOR(T, N)                                             \
                                                                        \
N##_vector new_##N##_vector(size_t length) {                            \
  N##_vector new_vec;                                                   \
  new_vec.data = malloc(sizeof(T) * length);                            \
  new_vec.length = length;                                              \
  new_vec.capacity = length;                                            \
  return new_vec;                                                       \
}                                                                       \
                                                                        \
T get_##N(N##_vector *vector, int index) {                              \
  if (index >= vector->length || (index < 0) &&                         \
      (-index) > vector->length) {                                      \
    fprintf(stderr,                                                     \
            "Index %d is out of array bounds (length %zd)\n",           \
            index, vector->length);                                     \
    return *(vector->data);                                             \
  }                                                                     \
  if (index >= 0) {                                                     \
    return vector->data[index];                                         \
  } else {                                                              \
    return vector->data[vector->length + index];                        \
  }                                                                     \
}                                                                       \
                                                                        \
void set_##N(N##_vector *vector, int index, T value) {                  \
  if (index >= vector->length || (index < 0) &&                         \
      (-index) > vector->length) {                                      \
    fprintf(stderr,                                                     \
            "Index %d is out of array bounds (length %zd)\n",           \
            index, vector->length);                                     \
    return;                                                             \
  }                                                                     \
  if (index >= 0) {                                                     \
    vector->data[index] = value;                                        \
  } else {                                                              \
    vector->data[vector->length - index] = value;                       \
  }                                                                     \
}                                                                       \
                                                                        \
int append_##N(N##_vector *vector, T value) {                           \
  if (vector->length >= vector->capacity) {                             \
    T *realloced = realloc(vector->data,                                \
                           (vector->capacity + 1) * 2 * sizeof(T));     \
    if (!realloced) {                                                   \
      free(vector->data);                                               \
      return -1;                                                        \
    }                                                                   \
    vector->data = realloced;                                           \
    vector->capacity = (vector->capacity + 1) * 2;                      \
  }                                                                     \
  vector->data[vector->length] = value;                                 \
  vector->length++;                                                     \
  return vector->length;                                                \
}

DEFINE_VECTOR(int, int)
DEFINE_VECTOR(char *, string)
DEFINE_VECTOR(double, double)
DEFINE_VECTOR(float, float)
DEFINE_VECTOR(void *, data)
DEFINE_VECTOR(bytes, byte)
DEFINE_VECTOR(char, char)
DEFINE_VECTOR(size_t, size_t)
