#include <stdlib.h>
#include "mesh.h"


Mesh::Mesh() {
  this->nodes = NULL;
  this->elements = NULL;
  this->mats = NULL;
}

Mesh::~Mesh() {
  ID_TYPE id;
  

  if (this->nodes) delete [] this->nodes;
  for (id = 0; id < this->GetNumberOfElements(); id++) 
    if (this->elements[id] != NULL)
      elements[id]->Delete();

  if (this->elements) 
    delete [] this->elements;
}
  

int Mesh::SetNumberOfNodes(ID_TYPE numNodes) {
  if ( (this->nodes = new Node[numNodes]) == NULL)
    return 1;
  this->numNodes = numNodes;
  return 0;
}

int Mesh::SetNumberOfElements(ID_TYPE numElements) {
  if ( (this->elements = new Element*[numElements] ) == NULL)
    return 1;
  this->numElements = numElements;
  return 0;
}

int Mesh::SetNode(ID_TYPE id, ID_TYPE key, point x) {
  int i;
  
  if (id >= this->GetNumberOfNodes())
    return 1;

  this->nodes[id].key = key;
  for (i=0; i<3; i++)
    this->nodes[id].x[i] = x[i];
  
  return 0;
}


int  Mesh::SetElement(ID_TYPE id, Element *element) {
   if (id >= this->GetNumberOfElements())
     return 1;
   
   this->elements[id] = element;
   return 0;
}
   

