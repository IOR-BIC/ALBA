#include <stdlib.h>
#include "types.h"

typedef struct HashNode {
  void *values;
  struct HashNode *nodes[10];
} HashNode;

typedef HashNode *HashTable[10];

void HashTableInit(HashTable *table);
void HashTableDelete(HashTable *table);
int HashTableSetValue(HashTable *table, ID_TYPE key, void *values);
void *HashTableGetValue(HashTable *table, ID_TYPE key);
void HashTablePrint(HashTable *table, void Print(void *values));
