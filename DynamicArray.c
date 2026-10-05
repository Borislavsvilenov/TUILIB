#include "DynamicArray.h"

bool da_init(DynamicArray* DA, size_t IS) {
  DA->data = malloc(IS * BASE_LEN);

  if(DA->data == NULL) return false;

  DA->itemSize = IS;
  DA->cap = BASE_LEN;
  DA->size = 0;

  return true;
}

void da_free(DynamicArray* DA) {
  if(DA->data == NULL) return;

  free(DA->data);
  DA->data = NULL;
  DA->size = 0;
  DA->cap = 0;
}

void* da_get(const DynamicArray* DA, size_t idx) {
  if(idx >= DA->size || idx < 0) return NULL;

  return (char*)DA->data + (DA->itemSize * idx);
}

bool da_push(DynamicArray* DA, const void* item) {
  if(DA->size == DA->cap) {
    void* temp = realloc(DA->data, DA->itemSize * DA->cap * 2);

    if (temp == NULL) return false;

    DA->data = temp;
    DA->cap *= 2;
  }

  void* target = (char*)DA->data + (DA->itemSize * DA->size);
  memcpy(target, item, DA->itemSize);

  DA->size++;

  return true;
}

bool da_replace(DynamicArray* DA, const void* item, size_t idx) {
  void* target = da_get(DA, idx);
  if(target == NULL) return false;

  memcpy(target, item, DA->itemSize);

  return true;
}

bool da_insert(DynamicArray* DA, const void* item, size_t idx) {
  if(idx >= DA->size || idx < 0) return false;
  if(DA->size == DA->cap) {
    void* temp = realloc(DA->data, DA->itemSize * DA->cap * 2);

    if (temp == NULL) return false;

    DA->data = temp;
    DA->cap *= 2;
  }

  DA->size++;

  void* src;
  void* dest;

  for(size_t i = DA->size; i > idx; i--) {
    src = da_get(DA, i-1);
    dest = da_get(DA, i);

    memcpy(dest, src, DA->itemSize);
  }

  dest = da_get(DA, idx);
  memcpy(dest, item, DA->itemSize);

  return true;
}

void* da_pop(DynamicArray* DA) {
  void* ret = malloc(DA->itemSize);
  void* target = da_get(DA, DA->size - 1);

  memcpy(ret, target, DA->itemSize);

  target = NULL;
  DA->size--;
  return ret;
}

void* da_remove(DynamicArray* DA, size_t idx) {
  if(idx >= DA->size || idx < 0) return NULL;

  void* ret = malloc(DA->itemSize);
  void* src = da_get(DA, idx);

  memcpy(ret, src, DA->itemSize);
  src = NULL;
  
  void* dest;
  for(size_t i = idx+1; i > DA->size; i--) {
    src = da_get(DA, i);
    dest = da_get(DA, i-1);

    memcpy(dest, src, DA->itemSize);
    src = NULL;
  }

  DA->size--;
  return ret;
}
