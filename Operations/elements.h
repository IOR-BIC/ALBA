#include "dataset.h"

typedef float point[3];

typedef struct {
  ID_TYPE key;
  point x;
} Node;


class Element {
public:
  void Delete() { delete [] nodes; }
  void SetKey(ID_TYPE key) { this->key = key; };
  void GetKey(ID_TYPE &key) { key = this->key; };
  ID_TYPE GetKey() { return  this->key; };
  int SetNode(ID_TYPE id, Node *node);  
  int GetNode(ID_TYPE id, Node *node);
  Node *GetNode(ID_TYPE id) { return nodes[id];};
  void SetNodes(Node **nodes) { this->nodes = nodes;};
  virtual ID_TYPE GetNumberOfNodes() = 0;
  virtual float ComputeScalar(DataSet *dataset, int numSteps) = 0;
  //virtual float ComputeJacobian(point x) {return 0; };
  virtual int GetType() = 0;
  void SetMatKey(ID_TYPE key) { this->matKey = key; };
  ID_TYPE GetMatKey() { return  this->matKey; };
    
protected:
  ID_TYPE key;  
  ID_TYPE matKey;
  Node **nodes;
};

class Tetra : public Element {
public:
  Tetra();
  static Tetra *New() { return new Tetra; };
  ID_TYPE GetNumberOfNodes() { return 4; };
  float ComputeScalar(DataSet *dataset, int numSteps);
  //float ComputeJacobian(point x);
  int GetType() { return 5;};
};

class Tetra10: public Element {
public:
  Tetra10();
  static Tetra10 *New() { return new Tetra10; };
  ID_TYPE GetNumberOfNodes() { return 10; };
  float ComputeScalar(DataSet *dataset, int numSteps);
  //float ComputeJacobian(point x);
  int GetType() { return 5;};
};

class Wedge: public Element {
public:
  Wedge();
  static Wedge *New() { return new Wedge; };
  ID_TYPE GetNumberOfNodes() { return 6; };
  float ComputeScalar(DataSet *dataset, int numSteps);
  //float ComputeJacobian(point x);
  int GetType() { return 7;};
};

class Hexa: public Element {
public:
  Hexa();
  static Hexa *New() { return new Hexa; };
  ID_TYPE GetNumberOfNodes() { return 8; };
  float ComputeScalar(DataSet *dataset, int numSteps);
  //float ComputeJacobian(point x);
  int GetType() { return 8;};
};


