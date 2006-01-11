#include  "elements.h"


typedef struct {
  int key;
  float ro;
  float E;
} mat;


class Mesh {
public:
  Mesh();
  ~Mesh();

  static Mesh *New() { return new Mesh; };
  int SetNumberOfNodes(ID_TYPE numNodes);
  ID_TYPE GetNumberOfNodes() { return numNodes; };
  int SetNumberOfElements(ID_TYPE numElements);
  ID_TYPE GetNumberOfElements() { return numElements;};
  int SetNode(ID_TYPE id, ID_TYPE key, point x);
  Node *GetNode(ID_TYPE id) { return &nodes[id];};
  int SetElement(ID_TYPE id, Element *element);
  Element *GetElement(ID_TYPE id) { return elements[id]; };
  
private:
  Node *nodes;
  Element **elements;
  mat *mats;

  ID_TYPE numNodes;
  ID_TYPE numElements;
  ID_TYPE numMats;
};

