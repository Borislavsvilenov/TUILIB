#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BASE_LEN 5

  // ============================================================================
  // Data Structures
  // ============================================================================
  typedef struct {
    size_t itemSize;
    size_t size;
    size_t cap;
    void* data;
  } DynamicArray;

  bool da_init(DynamicArray* DA, size_t IS);
  void da_free(DynamicArray* DA);

  void* da_get(const DynamicArray* DA, size_t idx);

  bool da_push(DynamicArray* DA, const void* item);
  bool da_replace(DynamicArray* DA, const void* item, size_t idx);
  bool da_insert(DynamicArray* DA, const void* item, size_t idx);

  void* da_pop(DynamicArray* DA);
  void* da_remove(DynamicArray* DA, size_t idx);

#ifdef __cplusplus
}
#endif

#endif
