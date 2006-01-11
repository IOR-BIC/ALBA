#include "hashtable.h"
#include <stdio.h>

void HashTableInit(HashTable *table) {
  int i;
  
  for (i=0; i < 10; i++) 
    (*table)[i] = NULL;
}

void HashTableDelete(HashTable *table) {
  int i;
  
  for (i=0; i < 10; i++) 
    if ((*table)[i] != NULL) {
      HashTableDelete(&((*table)[i])->nodes);
      free((*table)[i]);
    }
}

int HashTableSetValue(HashTable *table, ID_TYPE key, void *values) {
  int remainder;

  remainder = key % 10;
  key /= 10;
  if ((*table)[remainder] == NULL) {
    (*table)[remainder] = (HashNode *) malloc(sizeof(HashNode));
    (*table)[remainder]->values = NULL;
    HashTableInit (&((*table)[remainder]->nodes));
  }  
  if (key)
    return HashTableSetValue(&((*table)[remainder]->nodes), key, values);
  else {
     (*table)[remainder]->values = values;
     return 0;
  }
}


void *HashTableGetValue(HashTable *table, ID_TYPE key) {
  int remainder;

  remainder = key % 10;
  key /= 10;

  if ((*table)[remainder] == NULL) {
    return NULL;
  }
    
  if (key)
    return HashTableGetValue(&((*table)[remainder]->nodes), key);
  else 
    return  (*table)[remainder]->values;
  
}

void HashTablePrintRec(HashTable *table, void Print(void *values), unsigned int level, ID_TYPE key) {
  int i;
  ID_TYPE newkey;

  for (i=0; i < 10; i++) 

    if ((*table)[i]) {
      newkey = i * level + key;
      if ((*table)[i]->values) {		
	printf("%u : ", newkey);	
	Print((*table)[i]->values);
      }
      HashTablePrintRec(&((*table)[i]->nodes), Print , level * 10, newkey);
    }
	
}

void HashTablePrint(HashTable *table, void Print(void *values)) {
  HashTablePrintRec(table, Print , 1, 0);
}
  
