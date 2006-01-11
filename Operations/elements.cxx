#include "elements.h"
#include <iostream>

#define P(i) this->nodes[i]->x

double Determinant3x3(double c1[3], double c2[3], double c3[3])
{
  return c1[0]*c2[1]*c3[2] + c2[0]*c3[1]*c1[2] + c3[0]*c1[1]*c2[2] -
         c1[0]*c3[1]*c2[2] - c2[0]*c1[1]*c3[2] - c3[0]*c2[1]*c1[2];
}

int Element::SetNode(ID_TYPE id, Node *node) {
  if (id < this->GetNumberOfNodes()) {    
    this->nodes[id] = node;
    return 0;
  }
  return 1;  
}

int Element::GetNode(ID_TYPE id,Node *node ) {
  if (id < this->GetNumberOfNodes()) {    
    node = this->nodes[id];
    return 0;
  }
  return 1;  
}

// Tetra class
Tetra::Tetra() {
  this->nodes = new Node*[4];
}

float Tetra::ComputeScalar(DataSet *dataset, int numSteps) {
  double step;
  double Jr[3],  Js[3], Jt[3];  
  double detJ, volume=0, integral=0;
  double w[4];
  int i, nodeId;
  point x;
  double r,s,t,cr,cs,ct;

  step = 1.0 / (double) numSteps;

  // Compute Jacobian
  for (i=0; i<3; i++) {
    Jr[i] = P(1)[i] - P(0)[i];
    Js[i] = P(2)[i] - P(0)[i]; 
    Jt[i] = P(3)[i] - P(0)[i];
  }  

  // Computes determinant of Jacobian
  detJ = Determinant3x3(Jr, Js, Jt);  
  
  for (t = step / 2.0;  t < 1;  t += step) {
    ct  = 1 - t;
    for (s = step / 2.0; s < ct; s += step) {
      cs = 1 - s;	  
      for (r = step / 2.0; r < cs - t; r += step) {

	cr = 1 - r;
	volume += detJ;
	
	// Compute weights

	w[0] = cr - s - t;
	w[1] = r;
	w[2] = s;
	w[3] = t;	     	  
	  	
	// Compute global  coordinates
	for(i=0; i<3; i++) {
	  x[i] = 0;
	  for(nodeId=0; nodeId < 4; nodeId++) 
	    x[i] += (float) (w[nodeId] * P(nodeId)[i]);	  
	}		

	integral += dataset->ComputeScalar(x) * detJ;
	
      } // end r loop
    } // end s loop
  } // end t loop 

  return (float) (integral / volume);
}

// Tetra10 class
Tetra10::Tetra10() {
  this->nodes = new Node*[10];
}


float Tetra10::ComputeScalar(DataSet *dataset, int numSteps) {
  double step;
  double Jr[3],  Js[3], Jt[3];  
  double detJ, volume=0, integral=0;
  double w[10];
  int i, nodeId;
  point x;
  double r, s, t, cs, ct, l, lr, ls, lt;

  step = 1.0 / (float) numSteps;
    
  for (t = step / 2.0;  t < 1;  t += step) {
    ct = 1 - t;
    lt = 4 * t;
    for (s = step / 2.0; s < ct; s += step) {
      cs = 1 - s;	  
      ls = 4 * s;
      for (r = step / 2.0; r < cs - t; r += step) {
	l = 4 * (1 - r - s - t);
	lr = 4*r ;
	    
	for (i=0; i<3; i++) {
	  Jr[i] = P(0)[i] * (1 - l) + P(1)[i] * (lr - 1) + P(4)[i] * (l - lr) + (P(5)[i] - P(6)[i]) * ls + (P(8)[i] - P(7)[i]) * lt;
	  Js[i] = P(0)[i] * (1 - l) + P(2)[i] * (ls - 1) + P(6)[i] * (l - ls) + (P(3)[i] - P(4)[i]) * lr + (P(9)[i] - P(7)[i]) * lt;
	  Jt[i] = P(0)[i] * (1 - l) + P(3)[i] * (lt - 1) + P(7)[i] * (l - lt) + (P(8)[i] - P(4)[i]) * lr + (P(9)[i] - P(6)[i]) * ls;
	}
 
	
	detJ = Determinant3x3(Jr, Js, Jt);

	volume += detJ;
	
	// Compute weights

	l = l / 4 ;
		  
	w[0] = (2 * l - 1) * l;
	w[1] = (2 * r - 1) * r;
	w[2] = (2 * s - 1) * s;
	w[3] = (2 * t - 1) * t;
	w[4] = 4 * l * r;
	w[5] = 4 * r * s;
	w[6] = 4 * l * s;
	w[7] = 4 * l * t;
	w[8] = 4 * r * t;
	w[9] = 4 * s * t;
	  	
	// Compute global  coordinates
	for(i=0; i<3; i++) {
	  x[i] = 0;
	  for(nodeId=0; nodeId < 10; nodeId++) 
	    x[i] += (float) (w[nodeId] * P(nodeId)[i]);	  
	}		

	integral += dataset->ComputeScalar(x) * detJ;
	
      } // end r loop
    } // end s loop
  } // end t loop 

  return (float) (integral / volume);
}


// Wedge class
Wedge::Wedge() {
  this->nodes = new Node*[6];
}

float Wedge::ComputeScalar(DataSet *dataset, int numSteps) {
  double step;
  double Jr[3],  Js[3], Jt[3];  
  double detJ, volume=0, integral=0;
  double w[6];
  int i, nodeId;
  point x;
  double r,s,t,cr,cs,ct;

  step = 1.0 / (float) numSteps;
    
  for (t = step / 2.0;  t < 1;  t += step) {
    ct  = 1 - t;
    for (i=0; i<3; i++) {
      Jr[i] = (P(1)[i] - P(0)[i]) * ct + (P(4)[i] - P(3)[i]) * t;
      Js[i] = (P(2)[i] - P(0)[i]) * ct + (P(5)[i] - P(3)[i]) * t;
    }
    for (s = step / 2.0; s < 1; s += step) {
      cs = 1 - s;	  	  
      for (r = step / 2.0; r < cs; r += step) {
	cr = 1 - r;
	for (i=0; i<3; i++) {
	  Jt[i] = (P(3)[i] - P(0)[i]) * (cs - r) + (P(4)[i] - P(1)[i]) * r  + (P(5)[i] - P(2)[i]) * s;
	}
 
	
	detJ = Determinant3x3(Jr, Js, Jt);

	volume += detJ;
	
	// Compute weights

	w[0] = (cr - s) * ct;
	w[1] = s * ct;
	w[2] = r * ct;
	w[3] = (cr - s) * t;
	w[4] = s * t;
	w[5] = r * t;
	
	  	
	// Compute global  coordinates
	for(i=0; i<3; i++) {
	  x[i] = 0;
	  for(nodeId=0; nodeId < 6; nodeId++) 
	    x[i] += (float) (w[nodeId] * P(nodeId)[i]);	  
	}		

	integral += dataset->ComputeScalar(x) * detJ;
	
      } // end r loop
    } // end s loop
  } // end t loop 

  return (float) (integral / volume);
}


// Hexa class
Hexa::Hexa() {
  this->nodes = new Node*[8];
}


float Hexa::ComputeScalar(DataSet *dataset, int numSteps) {
  double step;
  double Jr[3],  Js[3], Jt[3];  
  double detJ, volume=0, integral=0;
  double w[8];
  int i, nodeId;
  point x;
  double r,s,t,cr,cs,ct;

  step = 1.0 / (float) numSteps;
    
  
  for (t = step / 2.0;  t < 1;  t += step) {
    ct  = 1 - t;
    for (s = step / 2.0; s < 1; s += step) {
      cs = 1 - s;
      for (i=0; i<3; i++)  
	Jr[i] = (P(1)[i] - P(0)[i]) * cs * ct + (P(2)[i] - P(3)[i]) * s * ct + (P(5)[i] - P(4)[i]) * cs * t + (P(6)[i] - P(7)[i]) * s * t;	  
      for (r = step / 2.0; r < 1; r += step) {
	cr = 1 - r;
	for (i=0; i<3; i++) {
	  Js[i] = (P(3)[i] - P(0)[i]) * cr * ct + (P(2)[i] - P(1)[i]) * r * ct + (P(7)[i] - P(4)[i]) * cr * t + (P(6)[i] - P(5)[i]) * r * t;
	  Jt[i] = (P(4)[i] - P(0)[i]) * cr * cs + (P(5)[i] - P(1)[i]) * r * cs + (P(7)[i] - P(3)[i]) * cr * s + (P(6)[i] - P(2)[i]) * r * s;
	}
	
	detJ = Determinant3x3(Jr, Js, Jt);

	volume += detJ;
	
	// Compute weights

	w[0] = cr * cs * ct;
	w[1] = r * cs * ct;
	w[2] = r * s * ct;
	w[3] = cr * s * ct;
	w[4] = cr * cs * t;
	w[5] = r * cs * t;
	w[6] = r * s * t;
	w[7] = cr * s * t;
	  	
	// Compute global  coordinates
	for(i=0; i<3; i++) {
	  x[i] = 0;
	  for(nodeId=0; nodeId < 8; nodeId++) 
	    x[i] += (float) (w[nodeId] * P(nodeId)[i]);	  
	    
	}		
	
	integral += dataset->ComputeScalar(x) * detJ;
		
      } // end r loop
    } // end s loop
  } // end t loop 


  return (float) (integral / volume);
}
