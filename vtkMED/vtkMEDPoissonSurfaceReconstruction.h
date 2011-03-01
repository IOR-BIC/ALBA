/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDPoissonSurfaceReconstruction.h,v $
Language:  C++
Date:      $Date: 2011-03-01 11:05:33 $
Version:   $Revision: 1.1.2.10 $
Authors:   Fuli Wu
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __vtkMEDPoissonSurfaceReconstruction_h
#define __vtkMEDPoissonSurfaceReconstruction_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkDataSetToPolyDataFilter.h"

#include <vector>
#include <hash_map>
#include <algorithm>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

using stdext::hash_map;

/**
class name: vtkMEDPoissonSurfaceReconstruction

This class implement Poisson Surface Reconstruction method.
A paper can be viewed here: research.microsoft.com/en-us/um/people/hoppe/poissonrecon.pdf
*/
class VTK_GRAPHICS_EXPORT vtkMEDPoissonSurfaceReconstruction : public vtkDataSetToPolyDataFilter
{
public:
  /** create instance of the object */
  static vtkMEDPoissonSurfaceReconstruction *New();
  /** RTTI macro */
  vtkTypeRevisionMacro(vtkMEDPoissonSurfaceReconstruction,vtkDataSetToPolyDataFilter);
  /** print object information */
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This error function allows our ported code to report error messages neatly.
  // This is not for external use. 
  void Error(const char *message);

protected:
  /** constructor */
  vtkMEDPoissonSurfaceReconstruction();
  /** destructor */
  ~vtkMEDPoissonSurfaceReconstruction();

  // Description:
  // the main function that does the work
  void Execute();

  /** computation of extents and update values*/
  void ComputeInputUpdateExtents(vtkDataObject *output);
  /** only check if input is not null */
  void ExecuteInformation(); 
  
private:
  /** copy constructor not implemented */
  vtkMEDPoissonSurfaceReconstruction(const vtkMEDPoissonSurfaceReconstruction&);
  /** operator= non implemented */
  void operator=(const vtkMEDPoissonSurfaceReconstruction&);
};


/*=========================================================================
Copyright (c) 2006, Michael Kazhdan and Matthew Bolitho
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
=========================================================================*/


#define PI 3.1415926535897932384
#define SQRT_3 1.7320508075688772935
#define DIMENSION 3

#define ITERATION_POWER   ((double)(1.0/3))
#define MEMORY_ALLOCATOR_BLOCK_SIZE 1<<12

#define READ_SIZE 1024

#define PAD_SIZE (Real(1.0))


#define SCALE 1.25

double ArcTan2(const double& y,const double& x);
double Angle(const double in[2]);
void Sqrt(const double in[2],double out[2]);
void Add(const double in1[2],const double in2[2],double out[2]);
void Subtract(const double in1[2],const double in2[2],double out[2]);
void Multiply(const double in1[2],const double in2[2],double out[2]);
void Divide(const double in1[2],const double in2[2],double out[2]);

int Factor(double a1,double a0,double roots[1][2],const double& EPS);
int Factor(double a2,double a1,double a0,double roots[2][2],const double& EPS);
int Factor(double a3,double a2,double a1,double a0,double roots[3][2],const double& EPS);
int Factor(double a4,double a3,double a2,double a1,double a0,double roots[4][2],const double& EPS);

int Solve(const double* eqns,const double* values,double* solutions,const int& dim);

void PSR_main();

/*=========================================================================
Here, for header file definition
=========================================================================*/


/*=========================================================================
Allocate.h
=========================================================================*/

/** 
      class name: AllocatorState
     This templated class assists in memory allocation and is well suited for instances
      when it is known that the sequence of memory allocations is performed in a stack-based
      manner, so that memory allocated last is released first. It also preallocates memory
      in chunks so that multiple requests for small chunks of memory do not require separate
      system calls to the memory manager.
      The allocator is templated off of the class of objects that we would like it to allocate,
      ensuring that appropriate constructors and destructors are called as necessary.
  */

class AllocatorState{
public:
	int index,remains;
};

template<class T>
/**
class name: Allocator
memory handler
*/
class Allocator{
	int blockSize;
	int index,remains;
	std::vector<T*> memory;
public:
  /** constructor */
	Allocator(void){
		blockSize=index=remains=0;
	}
  /** destructor */
	~Allocator(void){
		Reset();
	}

	/** This method is the allocators destructor. It frees up any of the memory that
	  * it has allocated. */
	void Reset(void){
		for(size_t i=0;i<memory.size();i++){delete[] memory[i];}
		memory.clear();
		blockSize=index=remains=0;
	}
	/** This method returns the memory state of the allocator. */
	AllocatorState getState(void) const{
		AllocatorState s;
		s.index=index;
		s.remains=remains;
		return s;
	}


	/** This method rolls back the allocator so that it makes all of the memory previously
	  * allocated available for re-allocation. Note that it does it not call the constructor
	  * again, so after this method has been called, assumptions about the state of the values
	  * in memory are no longer valid. */
	void RollBack(void){
		if(memory.size()){
			for(size_t i=0;i<memory.size();i++){
				for(int j=0;j<blockSize;j++){
					memory[i][j].~T();
					new(&memory[i][j]) T();
				}
			}
			index=0;
			remains=blockSize;
		}
	}
	/** This method rolls back the allocator to the previous memory state and makes all of the memory previously
	  * allocated available for re-allocation. Note that it does it not call the constructor
	  * again, so after this method has been called, assumptions about the state of the values
	  * in memory are no longer valid. */
    /// IT DOESN'T WORK! bugged unused code :(
	/*void RollBack(const AllocatorState& state){
		if(state.index<index || (state.index==index && state.remains<remains)){
			if(state.index<index){
				for(int j=state.remains;j<blockSize;j++){
					memory[state.index][j].~T();
					new(&memory[state.index][j]) T();
				}
				for(int i=state.index+1;i<index-1;i++){
					for(int j=0;j<blockSize;j++){
						memory[i][j].~T();
						new(&memory[i][j]) T();
					}
				}
				for(int j=0;j<remains;j++){
					memory[index][j].~T();
					new(&memory[index][j]) T();
				}
				index=state.index;
				remains=state.remains;
			}
			else{
				for(int j=0;j<state.remains;j<remains){
					memory[index][j].~T();
					new(&memory[index][j]) T();
				}
				remains=state.remains;
			}
		}
        }*/ /// IT DOESN'T WORK! bugged unused code :(

	/** This method initializes the constructor and the blockSize variable specifies the
	  * the number of objects that should be pre-allocated at a time. */
	void Set(const int& blockSize){
		Reset();
		this->blockSize=blockSize;
		index=-1;
		remains=0;
	}

	/** This method returns a pointer to an array of elements objects. If there is left over pre-allocated
	  * memory, this method simply returns a pointer to the next free piece of memory, otherwise it pre-allocates
	  * more memory. Note that if the number of objects requested is larger than the value blockSize with which
	  * the allocator was initialized, the request for memory will fail.
	  */
	T* NewElements(const int& elements=1){
		T* mem;
		if(!elements){return NULL;}
		if(elements>blockSize){
			fprintf(stderr,"Allocator Error, elements bigger than block-size: %d>%d\n",elements,blockSize);
			return NULL;
		}
		if(remains<elements){
			if(index==memory.size()-1){
				mem=new T[blockSize];
				if(!mem){fprintf(stderr,"Failed to allocate memory\n");exit(0);}
				memory.push_back(mem);
			}
			index++;
			remains=blockSize;
		}
		mem=&(memory[index][blockSize-remains]);
		remains-=elements;
		return mem;
	}
};

/*=========================================================================
BindaryNode.h
=========================================================================*/
/**
class name: BinaryNode
template class which represent a binary node of a binary tree, infact  the topology of the octree (used inside poisson surface reconstruction) 
defines a set of binary trees.
*/
template<class Real>
class BinaryNode{
public:
  /**  return double depth, not used in the filter */
	static inline int CenterCount(int depth){return 1<<depth;}
  /** return  maxdepth after increment, double and decrement, used for calculate number of base functions of polyomial*/
	static inline int CumulativeCenterCount(int maxDepth){return (1<<(maxDepth+1))-1;}
  /** retrieve index giving tree depth and an offset*/
	static inline int Index(int depth, int offSet){return (1<<depth)+offSet-1;}
  /** retrieve index  of the node in the corner*/
	static inline int CornerIndex(int maxDepth,int depth,int offSet,int forwardCorner)
	  {return (offSet+forwardCorner)<<(maxDepth-depth);}
  /** retrieve position  of the node in the corner*/
	static inline Real CornerIndexPosition(int index,int maxDepth)
	  {return Real(index)/(1<<maxDepth);}
  /**retrieve the width of the node */
	static inline Real Width(int depth)
	  {return Real(1.0/(1<<depth));}
  /**retrieve the width and the center  of the node */
	static inline void CenterAndWidth(int depth,int offset,Real& center,Real& width)
	  {
	    width=Real(1.0/(1<<depth));
	    center=Real((0.5+offset)*width);
	  }
  /**retrieve the width and the center  of the node */
	static inline void CenterAndWidth(int idx,Real& center,Real& width)
	  {
	    int depth,offset;
	    DepthAndOffset(idx,depth,offset);
	    CenterAndWidth(depth,offset,center,width);
	  }
  /**retrieve the depth and the offset  of the node */
	static inline void DepthAndOffset(int idx, int& depth,int& offset)
	  {
	    int i=idx+1;
	    depth=-1;
	    while(i){
	      i>>=1;
	      depth++;
	    }
	    offset=(idx+1)-(1<<depth);
	  }
};

/*=========================================================================
Polynomial.h
=========================================================================*/
/**
class name: Polynomial
Template class that represents a polynomial with a specific degree.
*/
template<int Degree>
class Polynomial{
public:
	double coefficients[Degree+1];

  /** constructor */
	Polynomial(void);
  /** copy constructor */
	template<int Degree2>
	Polynomial(const Polynomial<Degree2>& P);
  /** overload operator () which retrieves  the sum of the product of the coefficients*/
	double operator()(const double& t) const;
  /** calculate integral */
	double integral(const double& tMin,const double& tMax) const;

  /** operator== overload , checking coefficients */
	int operator == (const Polynomial& p) const;
  /** operator!= overload, checking coefficients */
	int operator != (const Polynomial& p) const;
  /** check if all coefficients are zero*/
	int isZero(void) const;
  /** set  all coefficients as zero*/
	void setZero(void);

  /** overload operator, according to the operation over coefficients */
	template<int Degree2>
	Polynomial& operator  = (const Polynomial<Degree2> &p);
  /** overload operator, according to the operation over coefficients */
	Polynomial& operator += (const Polynomial& p);
  /** overload operator, according to the operation over coefficients */
	Polynomial& operator -= (const Polynomial& p);
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator -  (void) const;
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator +  (const Polynomial& p) const;
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator -  (const Polynomial& p) const;
  /** overload operator, according to the operation over coefficients */
	template<int Degree2>
	Polynomial<Degree+Degree2>  operator *  (const Polynomial<Degree2>& p) const;

  /** overload operator, according to the operation over coefficients */
	Polynomial& operator += (const double& s);
  /** overload operator, according to the operation over coefficients */
	Polynomial& operator -= (const double& s);
  /** overload operator, according to the operation over coefficients */
	Polynomial& operator *= (const double& s);
  /** overload operator, according to the operation over coefficients */
	Polynomial& operator /= (const double& s);
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator +  (const double& s) const;
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator -  (const double& s) const;
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator *  (const double& s) const;
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator /  (const double& s) const;

  /** overload operator, according to the operation over coefficients */
	Polynomial scale(const double& s) const;
  /** overload operator, according to the operation over coefficients */
	Polynomial shift(const double& t) const;

  /** calculate derivative */
	Polynomial<Degree-1> derivative(void) const;
  /** calculate integral */
	Polynomial<Degree+1> integral(void) const;

  /** print representation of polynomial */
	void printnl(void) const;

  /** overload operator, according to the operation over coefficients */
	Polynomial& addScaled(const Polynomial& p,const double& scale);

  /** overload operator, according to the operation over coefficients */
	static void Negate(const Polynomial& in,Polynomial& out);
  /** overload operator, according to the operation over coefficients */
	static void Subtract(const Polynomial& p1,const Polynomial& p2,Polynomial& q);
  /** overload operator, according to the operation over coefficients */
	static void Scale(const Polynomial& p,const double& w,Polynomial& q);
  /** overload operator, according to the operation over coefficients */
	static void AddScaled(const Polynomial& p1,const double& w1,const Polynomial& p2,const double& w2,Polynomial& q);
  /** overload operator, according to the operation over coefficients */
	static void AddScaled(const Polynomial& p1,const Polynomial& p2,const double& w2,Polynomial& q);
  /** overload operator, according to the operation over coefficients */
	static void AddScaled(const Polynomial& p1,const double& w1,const Polynomial& p2,Polynomial& q);

  /** calculate roots */
	void getSolutions(const double& c,std::vector<double>& roots,const double& EPS) const;
};

/*=========================================================================
PPolynomial.h
=========================================================================*/
/**
class name: StartingPolynomial
*/
template<int Degree>
class StartingPolynomial{
public:
	Polynomial<Degree> p;
	double start;

	template<int Degree2>
  /** overload *, multiplying polynomial member variable with parameter one*/
	StartingPolynomial<Degree+Degree2>  operator * (const StartingPolynomial<Degree2>& p) const;
  /** Scale polynomial coefficients according to dividing factor s.*/
	StartingPolynomial scale(const double& s) const;
  /** Right shifting polynomial coefficients, usign multiplicative factor t. */
	StartingPolynomial shift(const double& t) const;
  /** Overload operator, according to the operation over coefficients. */
	int operator < (const StartingPolynomial& sp) const;
  /** Method to compare polynomials, according to the operation over coefficients. */
	static int Compare(const void* v1,const void* v2);
};

template<int Degree>
/**
class name: PPolynomial
*/
class PPolynomial{
public:
	size_t polyCount;
	StartingPolynomial<Degree>* polys;

  /** Constructor. */
	PPolynomial(void);
  /** Constructor. */
	PPolynomial(const PPolynomial<Degree>& p);
  /** Destructor. */
	~PPolynomial(void);

  /** Overload operator, according to the operation over coefficients. */
	PPolynomial& operator = (const PPolynomial& p);

  /** Return the size of polynomial object. */
	int size(void) const;

  /** Free polynomials, allocate memory and set size starting polynomial.*/ 
	void set(const size_t& size);
	/** Set sps as polynomials: this method will sort the elements in sps. */
	void set(StartingPolynomial<Degree>* sps,const int& count);
  /** Reallocate memory for newsize polys. */
	void reset(const size_t& newSize);


  /** Overload operator, return the sum of polys coefficients with index t.*/
	double operator()(const double& t) const;
  /** Compute integral within integration range tMin and tMax, return the sum of integrals of polynomials. */
	double integral(const double& tMin,const double& tMax) const;
  /** Compute integral */
	double Integral(void) const;

  /** Overload operator, according to the operation over polys. */
	template<int Degree2>
	PPolynomial<Degree>& operator = (const PPolynomial<Degree2>& p);

  /** Combinate polynomials (sorting according to start value).*/
	PPolynomial  operator + (const PPolynomial& p) const;
  /** Combinate polynomials (sorting according to start value). p polys with negative sign.*/
	PPolynomial  operator - (const PPolynomial& p) const;

  /** Combinate polys according to multiplicative operation. */
	template<int Degree2>
	PPolynomial<Degree+Degree2> operator * (const Polynomial<Degree2>& p) const;

  /** Combinate polys according to multiplicative operation. 
  A new polynomial is computed with the multiplication beetwen original polys member 
  and each member of the poly parameter./ */
	template<int Degree2>
	PPolynomial<Degree+Degree2> operator * (const PPolynomial<Degree2>& p) const;


  /** Overload operator, according to the operation over polys. */
	PPolynomial& operator += (const double& s);
  /** Overload operator, according to the operation over polys. */
	PPolynomial& operator -= (const double& s);
  /** Overload operator, according to the operation over polys. */
	PPolynomial& operator *= (const double& s);
  /** Overload operator, according to the operation over polys. */
	PPolynomial& operator /= (const double& s);
  /** Overload operator, according to the operation over polys. */
	PPolynomial  operator +  (const double& s) const;
  /** Overload operator, according to the operation over polys. */
	PPolynomial  operator -  (const double& s) const;
  /** Overload operator, according to the operation over polys. */
	PPolynomial  operator *  (const double& s) const
  /** Overload operator, according to the operation over polys. */;
	PPolynomial  operator /  (const double& s) const;

  /** Add operation with scaled polynomials. */
	PPolynomial& addScaled(const PPolynomial& poly,const double& scale);

  /** Scale operation on every polynomial. */
	PPolynomial scale(const double& s) const;
  /** Shift operation on every polynomial. */
	PPolynomial shift(const double& t) const;

   /** calculate derivative on polys. */
	PPolynomial<Degree-1> derivative(void) const;
   /** calculate integral on polys.  */
	PPolynomial<Degree+1> integral(void) const;

  /** solve polynomials. */
	void getSolutions(const double& c,std::vector<double>& roots,const double& EPS,const double& min=-DBL_MAX,const double& max=DBL_MAX) const;

  /** print method. */
	void printnl(void) const;

  /** Compute moving average. */
	PPolynomial<Degree+1> MovingAverage(const double& radius);

  /** Return two polynomials forming a constant function with width as constanct value. */
	static PPolynomial ConstantFunction(const double& width=0.5);
  /** Compute gaussian approximation. */
	static PPolynomial GaussianApproximation(const double& width=0.5);
//	void write(FILE* fp,const int& samples,const double& min,const double& max) const;
};


/*=========================================================================
FunctionData.h
=========================================================================*/
template<int Degree,class Real>
/**
class name: FunctionData
*/
class FunctionData{
	int useDotRatios;
	int normalize;
public:
	const static int     DOT_FLAG;
	const static int   D_DOT_FLAG;
	const static int  D2_DOT_FLAG;
	const static int   VALUE_FLAG;
	const static int D_VALUE_FLAG;

	int depth,res,res2;
	Real *dotTable,*dDotTable,*d2DotTable;
	Real *valueTables,*dValueTables;
	PPolynomial<Degree> baseFunction;
	PPolynomial<Degree-1> dBaseFunction;
	PPolynomial<Degree+1>* baseFunctions;

  /** Constructor. */
	FunctionData(void);
  /** Destructor. */
	~FunctionData(void);

  /** Set dot tables. */
	virtual void   SetDotTables(const int& flags);
  /** Clear dot tables.*/
	virtual void ClearDotTables(const int& flags);

  /** Set value tables. */
	virtual void   SetValueTables(const int& flags,const double& smooth=0);
  /** Set value tables. valueSmooth parameter used for value tables, normalSmoot for dot tables.*/
	virtual void   SetValueTables(const int& flags,const double& valueSmooth,const double& normalSmooth);
  /** Clear value tables. */
	virtual void ClearValueTables(void);

  /** create base functions from parameters*/
	void Set(const int& maxDepth,const PPolynomial<Degree>& F,const int& normalize,const int& useDotRatios=1);

  /** compute dot product */
	Real   dotProduct(const double& center1,const double& width1,const double& center2,const double& width2) const;

  /** compute dot product with derivative base functions  */
	Real  dDotProduct(const double& center1,const double& width1,const double& center2,const double& width2) const;

  /** compute dot product with derivative base functions  like previous except for width2 division*/
	Real d2DotProduct(const double& center1,const double& width1,const double& center2,const double& width2) const;

  /** retrieve symmetric index */
	static inline int SymmetricIndex(const int& i1,const int& i2);
  /** retrieve symmetric index */
	static inline int SymmetricIndex(const int& i1,const int& i2,int& index);
};

/*=========================================================================
Geometry.h
=========================================================================*/
/** Return a Real random value. */
template<class Real>
Real Random(void);

template<class Real>
/** 
struct name: Point3D
Represents a Point with 3d coordinates in the space.
*/
struct Point3D{Real coords[3];};

/** Return a 3d point with random real coordinates. */
template<class Real>
Point3D<Real> RandomBallPoint(void);

/**Return a 3d point with random real coordinates as a center of a sphere.*/
template<class Real>
Point3D<Real> RandomSpherePoint(void);

/** Return distance beetwen origin and a 3d point. */
template<class Real>
double Length(const Point3D<Real>& p);

/** Return square distance beetwen origin and a 3d point. */
template<class Real>
double SquareLength(const Point3D<Real>& p);

/** Return distance beetwen two 3d points. */
template<class Real>
double Distance(const Point3D<Real>& p1,const Point3D<Real>& p2);

/** Return square distance beetwen two 3d points. */
template<class Real>
double SquareDistance(const Point3D<Real>& p1,const Point3D<Real>& p2);

/** Cross product beetwen two 3d points: thrid input parameter will save resulting point. */
template <class Real>
void CrossProduct(const Point3D<Real>& p1,const Point3D<Real>& p2,Point3D<Real>& p);

/**
class name: Edge
Represents an edge of a triangle.
*/
class Edge{
public:
	double p[2][2];
  /** return lenght of the edge. */
	double Length(void) const{
		double d[2];
		d[0]=p[0][0]-p[1][0];
		d[1]=p[0][1]-p[1][1];

		return sqrt(d[0]*d[0]+d[1]*d[1]);
	}
};
/**
class name: Triangle
Represents a triangle.
*/
class Triangle{
public:
	double p[3][3];
  /** return the area value of triangle. */
	double Area(void) const{
		double v1[3],v2[3],v[3];
		for(int d=0;d<3;d++){
			v1[d]=p[1][d]-p[0][d];
			v2[d]=p[2][d]-p[0][d];
		}
		v[0]= v1[1]*v2[2]-v1[2]*v2[1];
		v[1]=-v1[0]*v2[2]+v1[2]*v2[0];
		v[2]= v1[0]*v2[1]-v1[1]*v2[0];
		return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])/2;
	}
  /** return the width/height value */
	double AspectRatio(void) const{
		double d=0;
		int i,j;
		for(i=0;i<3;i++){
	  for(i=0;i<3;i++)
			for(j=0;j<3;j++){d+=(p[(i+1)%3][j]-p[i][j])*(p[(i+1)%3][j]-p[i][j]);}
		}
		return Area()/d;
	}
	
};
/**
class name: CoredPointIndex
Index of a point located in the core
*/
class CoredPointIndex{
public:
	int index;
	char inCore;

  /** overload operator */
	int operator == (const CoredPointIndex& cpi) const {return (index==cpi.index) && (inCore==cpi.inCore);};
  /** overload operator */
	int operator != (const CoredPointIndex& cpi) const {return (index!=cpi.index) || (inCore!=cpi.inCore);};
};
/**
class name: EdgeIndex
Indexes of the points inside the edge
*/
class EdgeIndex{
public:
	int idx[2];
};
/**
class name: EdgeIndex
Indexes of the cored points inside the edge
*/
class CoredEdgeIndex{
public:
	CoredPointIndex idx[2];
};
/**
class name: TriangleIndex
Indexes of the points inside the triangle
*/
class TriangleIndex{
public:
	int idx[3];
};

/**
class name: TriangulationEdge
Edge created by triangulation
*/
class TriangulationEdge
{
public:
  /** Constructor. */
	TriangulationEdge(void);
	int pIndex[2];
	int tIndex[2];
};

/**
class name: TriangulationTriangle
Triangle created by triangulation
*/
class TriangulationTriangle
{
public:
  /** Constructor. */
	TriangulationTriangle(void);
	int eIndex[3];
};

/**
  class name: Triangulation
Handle the triangulation process
*/
template<class Real>
class Triangulation
{
public:

	std::vector<Point3D<Real> >		points;
	std::vector<TriangulationEdge>				edges;
	std::vector<TriangulationTriangle>			triangles;

  /** return 0 or 1 , modifying triangle points coordinate */
	int Factor(const int& tIndex,int& p1,int& p2,int& p3);
  
  /** calculate total area of triangles*/
	double Area(void);
  /** calculate area  of the triangle*/
	double Area(const int& tIndex);
  /** calculate area with cross product */
	double Area(const int& p1,const int& p2,const int& p3);
  /** flip edges in order to minimize the area of the triangle */
	int FlipMinimize(const int& eIndex);
  /** insert new triangle in the structure */
	int AddTriangle(const int& p1,const int& p2,const int& p3);

protected:
	hash_map<long long,int> edgeMap;
	static long long EdgeIndex(const int& p1,const int& p2);
	double Area(const Triangle& t);
};


/** simplify mesh with edge collapse. */
template<class Real>
void EdgeCollapse(const Real& edgeRatio,std::vector<TriangleIndex>& triangles,std::vector< Point3D<Real> >& positions,std::vector<Point3D<Real> >* normals);
/** simplify mesh with triangle collapse. */
template<class Real>
void TriangleCollapse(const Real& edgeRatio,std::vector<TriangleIndex>& triangles,std::vector<Point3D<Real> >& positions,std::vector<Point3D<Real> >* normals);

/**
class name: CoredMeshData
interface class that handle mesh in the core
*/
class CoredMeshData{
public:
	std::vector<Point3D<float> > inCorePoints;
	const static int IN_CORE_FLAG[3];
  /** reset Iterator */
	virtual void ResetIterator(void)=0;

  /** add  a point out of core */
	virtual int AddOutOfCorePoint(const Point3D<float>& p)=0;
  /** insert new triangle */
	virtual int AddTriangle(const TriangleIndex& t,const int& icFlag=(IN_CORE_FLAG[0] | IN_CORE_FLAG[1] | IN_CORE_FLAG[2]))=0;

  /** iterate on  next out of core point  */
	virtual int NextOutOfCorePoint(Point3D<float>& p)=0;
  /** iterate on next triangle */
	virtual int NextTriangle(TriangleIndex& t,int& inCoreFlag)=0;

  /** number of out of core points */
	virtual int OutOfCorePointCount(void)=0;
  /** number of triangles */
	virtual int TriangleCount(void)=0;
};
/**
class name: CoredVectorMeshData
  represents data meshes inside the core as vector
*/
class CoredVectorMeshData : public CoredMeshData{
	std::vector<Point3D<float> > oocPoints;
	std::vector<TriangleIndex> triangles;
	int oocPointIndex,triangleIndex;
public:
  /** constructor */
	CoredVectorMeshData::CoredVectorMeshData(void);

  /** reset point and triangle index. */
	void ResetIterator(void);

  /** Add a 3d point which is out of core*/
	int AddOutOfCorePoint(const Point3D<float>& p);
  /** add a triangle*/
	int AddTriangle(const TriangleIndex& t,const int& inCoreFlag=(CoredMeshData::IN_CORE_FLAG[0] | CoredMeshData::IN_CORE_FLAG[1] | CoredMeshData::IN_CORE_FLAG[2]));

  /** retrieve next point, return 0 if current is the last point. */
	int NextOutOfCorePoint(Point3D<float>& p);
  /** retrieve next triangle, return 0 if current is the last triangle. */
	int NextTriangle(TriangleIndex& t,int& inCoreFlag);

  /** return size of oocPoints vector. */
	int OutOfCorePointCount(void);
  /** return size of triangles vector. */
	int TriangleCount(void);
};
/**
class name: CoredVectorMeshData
  represents data meshes inside the core as file descriptors
*/
class CoredFileMeshData : public CoredMeshData{
	FILE *oocPointFile,*triangleFile;
	int oocPoints,triangles;
public:
  /** object constructor */
	CoredFileMeshData(void);
  /** object destructor */
	~CoredFileMeshData(void);

  /** reset iterator */
	void ResetIterator(void);

  /** Add a 3d point which is out of core*/
	int AddOutOfCorePoint(const Point3D<float>& p);
  /** add a triangle*/
	int AddTriangle(const TriangleIndex& t,const int& inCoreFlag=(CoredMeshData::IN_CORE_FLAG[0] | CoredMeshData::IN_CORE_FLAG[1] | CoredMeshData::IN_CORE_FLAG[2]));

  /** retrieve next point, return 0 if current is the last point. */
	int NextOutOfCorePoint(Point3D<float>& p);
  /** retrieve next triangle, return 0 if current is the last triangle. */
	int NextTriangle(TriangleIndex& t,int& inCoreFlag);

  /** return size of oocPoints vector. */
	int OutOfCorePointCount(void);
  /** return size of triangles vector. */
	int TriangleCount(void);
};


/*=========================================================================
MarchingCubes.h
=========================================================================*/
/**
class name: Square
Represent a square in a marching square algorithms
*/
class Square{
public:
	const static int CORNERS=4,EDGES=4,NEIGHBORS=4;
  /** return corner index */
	static int  CornerIndex			(const int& x,const int& y);
  /**  calculate corner with 0,1 notation */
	static void FactorCornerIndex	(const int& idx,int& x,int& y);
  /** return edge index, depending by orientation */
	static int  EdgeIndex			(const int& orientation,const int& i);
  /** retrieve in i  new index, depending by orientation */
	static void FactorEdgeIndex		(const int& idx,int& orientation,int& i);

  /** calculating orientation, retrieve index of the corner which can be reflected*/
	static int  ReflectCornerIndex	(const int& idx,const int& edgeIndex);
  /** calculating orientation, retrieve index of the edge which can be reflected*/
	static int  ReflectEdgeIndex	(const int& idx,const int& edgeIndex);

  /** calculate corner indexes */
	static void EdgeCorners(const int& idx,int& c1,int &c2);
};
/**
class name: Cube
Represent a cube in a marching cube algorithms
*/
class Cube{
public:
	const static int CORNERS=8,EDGES=12,NEIGHBORS=6;

  /** retrieve corner index */
	static int  CornerIndex			(const int& x,const int& y,const int& z);
  /**  calculate corner with 0,1 notation */
	static void FactorCornerIndex	(const int& idx,int& x,int& y,int& z);
  /** retrieve edge indexes */
	static int  EdgeIndex			(const int& orientation,const int& i,const int& j);
  /**  calculate edge index */
	static void FactorEdgeIndex		(const int& idx,int& orientation,int& i,int &j);
  /** retrieve face index */
	static int  FaceIndex			(const int& dir,const int& offSet);
  /** retrieve face index */
	static int  FaceIndex			(const int& x,const int& y,const int& z);
  /**  calculate face index */
	static void FactorFaceIndex		(const int& idx,int& x,int &y,int& z);
  /**  calculate face index */
	static void FactorFaceIndex		(const int& idx,int& dir,int& offSet);

  /** retrieve antipodal corner index */
	static int  AntipodalCornerIndex	(const int& idx);
  /** retrieve face corner index (can be face-reflected)  depending by orientation */
	static int  FaceReflectCornerIndex	(const int& idx,const int& faceIndex);
  /** retrieve face edge  index (can be face-reflected )  depending by orientation */
	static int  FaceReflectEdgeIndex	(const int& idx,const int& faceIndex);
  /** retrieve face face index (can be face-reflected)  depending by orientation */
	static int	FaceReflectFaceIndex	(const int& idx,const int& faceIndex);
  /** retrieve face corner index (can be edge-reflected)  depending by orientation */
	static int	EdgeReflectCornerIndex	(const int& idx,const int& edgeIndex);
  /** retrieve edge corner index (can be edge-reflected)  depending by orientation */
	static int	EdgeReflectEdgeIndex	(const int& edgeIndex);

  /** check face adjacent to given edge */
	static int  FaceAdjacentToEdges	(const int& eIndex1,const int& eIndex2);
  /** check faces adjacent to given edge */
	static void FacesAdjacentToEdge	(const int& eIndex,int& f1Index,int& f2Index);

  /** retrieve edge corners */
	static void EdgeCorners(const int& idx,int& c1,int &c2);
  /** retrieve face corners */
	static void FaceCorners(const int& idx,int& c1,int &c2,int& c3,int& c4);
};

/**
class name: MarchingSquares
Computer graphics algorithm  that generates contour lines for a two-dimensional scalar field
*/
class MarchingSquares{
  /**  return a simple interpolation  v1/(v1-v2)*/
	static double Interpolate(const double& v1,const double& v2);
  /**  calculate vertex using also interpolation */
	static void SetVertex(const int& e,const double values[Square::CORNERS],const double& iso);
public:
	const static int MAX_EDGES=2;
	static const int edgeMask[1<<Square::CORNERS];
	static const int edges[1<<Square::CORNERS][2*MAX_EDGES+1];
	static double vertexList[Square::EDGES][2];

  /** return index of  points translating from 0,1 notation*/
	static int GetIndex(const double values[Square::CORNERS],const double& iso);
  /** check if there is ambiguity  checking if index is 5 or 10*/
	static int IsAmbiguous(const double v[Square::CORNERS],const double& isoValue);
  /** add new edges */
	static int AddEdges(const double v[Square::CORNERS],const double& isoValue,Edge* edges);
  /** add edges indexes without calculate positions*/
	static int AddEdgeIndices(const double v[Square::CORNERS],const double& isoValue,int* edges);
};

/**
class name: MarchingSquares
Computer graphics algorithm  that extracts a polygonal mesh of an isosurface  from a three-dimensional scalar field
*/
class MarchingCubes{
   /**  return a simple interpolation  v1/(v1-v2)*/
	static double Interpolate(const double& v1,const double& v2);
  /**  calculate vertex using also interpolation */
	static void SetVertex(const int& e,const double values[Cube::CORNERS],const double& iso);
	/** Return face index */
  static int GetFaceIndex(const double values[Cube::CORNERS],const double& iso,const int& faceIndex);

  /**  return a simple interpolation  v1/(v1-v2) (float version)*/
	static float Interpolate(const float& v1,const float& v2);
  /**  calculate vertex using also interpolation (float version)*/
	static void SetVertex(const int& e,const float values[Cube::CORNERS],const float& iso);
	/** Return face index (float version) */
  static int GetFaceIndex(const float values[Cube::CORNERS],const float& iso,const int& faceIndex);

  /** Return face index (using marching cube index) */
	static int GetFaceIndex(const int& mcIndex,const int& faceIndex);
public:
	const static int MAX_TRIANGLES=5;
	static const int edgeMask[1<<Cube::CORNERS];
	static const int triangles[1<<Cube::CORNERS][3*MAX_TRIANGLES+1];
	static const int cornerMap[Cube::CORNERS];
	static double vertexList[Cube::EDGES][3];

  /** Add triangle indexes without calculating position */
	static int AddTriangleIndices(const int& mcIndex,int* triangles);
  /** return index of  points translating from 0,1 notation*/
	static int GetIndex(const double values[Cube::CORNERS],const double& iso);
  /** check if there is ambiguity  checking if index is 5 or 10*/
	static int IsAmbiguous(const double v[Cube::CORNERS],const double& isoValue,const int& faceIndex);
  /** check if there are roots controlling if retrieving index is different from 0 and 15  for the face */
	static int HasRoots(const double v[Cube::CORNERS],const double& isoValue);
  /** check if there are roots controlling if retrieving index is different from 0 and 255 */
	static int HasRoots(const double v[Cube::CORNERS],const double& isoValue,const int& faceIndex);
  /** Add triangle */
	static int AddTriangles(const double v[Cube::CORNERS],const double& isoValue,Triangle* triangles);
  /** Add triangle indexes without calculating position */
	static int AddTriangleIndices(const double v[Cube::CORNERS],const double& isoValue,int* triangles);

  /** return index of  points translating from 0,1 notation (float version)*/
	static int GetIndex(const float values[Cube::CORNERS],const float& iso);
  /** check if there is ambiguity  checking if index is 5 or 10 (float version)*/
	static int IsAmbiguous(const float v[Cube::CORNERS],const float& isoValue,const int& faceIndex);
  /** check if there are roots controlling if retrieving index is different from 0 and 15  for the face (float version)*/
	static int HasRoots(const float v[Cube::CORNERS],const float& isoValue);
  /** check if there are roots controlling if retrieving index is different from 0 and 255  (float version)*/
	static int HasRoots(const float v[Cube::CORNERS],const float& isoValue,const int& faceIndex);
  /** Add triangle (float version)*/
	static int AddTriangles(const float v[Cube::CORNERS],const float& isoValue,Triangle* triangles);
  /** Add triangle indexes without calculating position (float version)*/
	static int AddTriangleIndices(const float v[Cube::CORNERS],const float& isoValue,int* triangles);

  /** check if there is ambiguity  checking if index is 5 or 10 (using marching cube index)*/
	static int IsAmbiguous(const int& mcIndex,const int& faceIndex);
  /** check if there are roots controlling if retrieving index is different from 0 and 255 (using marching cube index)*/
	static int HasRoots(const int& mcIndex);
  /** check if there are roots in selected face */
	static int HasFaceRoots(const int& mcIndex,const int& faceIndex);
  /** check if there are roots in selected edge */
	static int HasEdgeRoots(const int& mcIndex,const int& edgeIndex);
};


/*=========================================================================
MemoryUseage.h
=========================================================================*/
class MemoryInfo{
public:
	size_t TotalPhysicalMemory;
	size_t FreePhysicalMemory;
	size_t TotalSwapSpace;
	size_t FreeSwapSpace;
	size_t TotalVirtualAddressSpace;
	size_t FreeVirtualAddressSpace;
	size_t PageSize;

  /** set memory variables, retrieving information from the system*/
	void set(void){
		MEMORYSTATUSEX Mem;
		SYSTEM_INFO Info;
		ZeroMemory( &Mem, sizeof(Mem));
		ZeroMemory( &Info, sizeof(Info)); 
		Mem.dwLength = sizeof(Mem);
		::GlobalMemoryStatusEx( &Mem );
		::GetSystemInfo( &Info );

		TotalPhysicalMemory = (size_t)Mem.ullTotalPhys;
		FreePhysicalMemory = (size_t)Mem.ullAvailPhys;
		TotalSwapSpace = (size_t)Mem.ullTotalPageFile;
		FreeSwapSpace = (size_t)Mem.ullAvailPageFile;
		TotalVirtualAddressSpace = (size_t)Mem.ullTotalVirtual;
		FreeVirtualAddressSpace = (size_t)Mem.ullAvailVirtual;
		PageSize = (size_t)Info.dwPageSize;
	}
  /** return virtual memory used calculating throung base addresses and regions size*/
	size_t usage(void) const {return TotalVirtualAddressSpace-FreeVirtualAddressSpace;}

  /** retrieve used memory*/
	static size_t Usage(void){
		MEMORY_BASIC_INFORMATION mbi; 
		size_t      dwMemUsed = 0; 
		PVOID      pvAddress = 0; 


		memset(&mbi, 0, sizeof(MEMORY_BASIC_INFORMATION)); 
		while(VirtualQuery(pvAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION)){ 
			if(mbi.State == MEM_COMMIT && mbi.Type == MEM_PRIVATE){dwMemUsed += mbi.RegionSize;}
			pvAddress = ((BYTE*)mbi.BaseAddress) + mbi.RegionSize; 
		} 
		return dwMemUsed; 
	} 
};

/*=========================================================================
Vector.h
=========================================================================*/
/**
class name: Vector
Define mathematical entity vector, overloading operators.
*/
template<class T>
class Vector
{
public:
  /** constructor */
	Vector();
  /** overloaded constructor */
	Vector( const Vector<T>& V );
  /** overloaded constructor */
	Vector( size_t N );
  /** overloaded constructor */
	Vector( size_t N, T* pV );
  /** destructor */
	~Vector();

  /** overload operator ()*/
	const T& operator () (size_t i) const;
  /** overload operator ()*/
	T& operator () (size_t i);
  /** overload operator []*/
	const T& operator [] (size_t i) const;
  /** overload operator []*/
	T& operator [] (size_t i);

  /** set elements to zero */
	void SetZero();

  /** return dimension of vector */
	size_t Dimensions() const;
  /** resize vector */
	void Resize( size_t N );

  /** operload operator for math operations */
	Vector operator * (const T& A) const;
  /** operload operator for math operations */
	Vector operator / (const T& A) const;
  /** operload operator for math operations */
	Vector operator - (const Vector& V) const;
  /** operload operator for math operations */
	Vector operator + (const Vector& V) const;

  /** operload operator for math operations */
	Vector& operator *= (const T& A);
  /** operload operator for math operations */
	Vector& operator /= (const T& A);
  /** operload operator for math operations */
	Vector& operator += (const Vector& V);
  /** operload operator for math operations */
	Vector& operator -= (const Vector& V);

  /** add a scaled vector */
	Vector& AddScaled(const Vector& V,const T& scale);
  /** subtract a scaled vector*/
	Vector& SubtractScaled(const Vector& V,const T& scale);
  /** return the result of a sum of scaled vectors */
	static void Add(const Vector& V1,const T& scale1,const Vector& V2,const T& scale2,Vector& Out);
  /** return the result of a sum of a non-scaled vector  and a scaled one*/
	static void Add(const Vector& V1,const T& scale1,const Vector& V2,Vector& Out);

  /** operload operator for math operations */
	Vector operator - () const;

  /** operload operator for assignment */
	Vector& operator = (const Vector& V);

  /** define dot product */
	T Dot( const Vector& V ) const;
 
  /** define lenght of a vector */
	T Length() const;

  /** calculate norm */
	T Norm( size_t Ln ) const;
  /** normalize the vector*/
	void Normalize();

	T* m_pV;
protected:
	size_t m_N;

};
/**
class Name: NVector
template class that introduce Dimensionality
*/
template<class T,int Dim>
class NVector
{
public:
  /** constructor */
	NVector();
  /** overloaded constructor */
	NVector( const NVector& V );
  /** overloaded constructor */
	NVector( size_t N );
  /** overloaded constructor */
	NVector( size_t N, T* pV );
  /** destructor */
	~NVector();

  /** overload operator ()*/
	const T* operator () (size_t i) const;
  /** overload operator ()*/
	T* operator () (size_t i);
  /** overload operator []*/
	const T* operator [] (size_t i) const;
  /** overload operator []*/
	T* operator [] (size_t i);

  /** set elements to zero */
	void SetZero();
  
  /** retrieve dimensions*/
	size_t Dimensions() const;
  /** resize vector */
	void Resize( size_t N );

  /** operload operator for math operations */
	NVector operator * (const T& A) const;
  /** operload operator for math operations */
	NVector operator / (const T& A) const;
  /** operload operator for math operations */
	NVector operator - (const NVector& V) const;
  /** operload operator for math operations */
	NVector operator + (const NVector& V) const;

  /** operload operator for math operations */
	NVector& operator *= (const T& A);
  /** operload operator for math operations */
	NVector& operator /= (const T& A);
  /** operload operator for math operations */
	NVector& operator += (const NVector& V);
  /** operload operator for math operations */
	NVector& operator -= (const NVector& V);

  /** add a scaled vector */
	NVector& AddScaled(const NVector& V,const T& scale);
  /** subtract a scaled vector*/
	NVector& SubtractScaled(const NVector& V,const T& scale);
  /** return the result of a sum of scaled vectors */
	static void Add(const NVector& V1,const T& scale1,const NVector& V2,const T& scale2,NVector& Out);
  /** return the result of a sum of a non-scaled vector  and a scaled one*/
	static void Add(const NVector& V1,const T& scale1,const NVector& V2,				NVector& Out);

  /** operload operator for math operations */
	NVector operator - () const;

  /** operload operator for assignment */
	NVector& operator = (const NVector& V);

  /** define dot product */
	T Dot( const NVector& V ) const;

  /** define lenght of a vector */
	T Length() const;

  /** calculate norm */
	T Norm( size_t Ln ) const;
  /** normalize the n-vector*/
	void Normalize();

	T* m_pV;
protected:
	size_t m_N;

};


/*=========================================================================
SparseMatrix.h
=========================================================================*/
/**
class name:  MatrixEntry
*/
template <class T>
struct MatrixEntry
{
  /** constructor */
	MatrixEntry( void )		{ N =-1; Value = 0; }
  /** overloaded  constructor */
	MatrixEntry( int i )	{ N = i; Value = 0; }
	int N;
	T Value;
};
/**
class name:  NMatrixEntry
*/
template <class T,int Dim>
struct NMatrixEntry
{
  /** constructor */
	NMatrixEntry( void )		{ N =-1; memset(Value,0,sizeof(T)*Dim); }
  /** overloaded  constructor */
	NMatrixEntry( int i )	{ N = i; memset(Value,0,sizeof(T)*Dim); }
	int N;
	T Value[Dim];
};
/**
class name:  SparseMatrix
sparse matrix is a matrix populated primarily with zeros
*/
template<class T> class SparseMatrix
{
private:
	static int UseAlloc;
public:
	static Allocator<MatrixEntry<T> > Allocator;
  /** return  allocator for sparse matrix  */
	static int UseAllocator(void);
  /**  use allocator , allocating block size */
	static void SetAllocator(const int& blockSize);

	int rows;
	int* rowSizes;
	MatrixEntry<T>** m_ppElements;

  /** constructor */
	SparseMatrix();
  /** overloaded constructor */
	SparseMatrix( int rows );
  /** resize the matrix*/
	void Resize( int rows );
  /** change row size */
	void SetRowSize( int row , int count );
  /** retrieve total number of entries summing each rowsize*/
	int Entries(void);

  /** overloaded constructor */
	SparseMatrix( const SparseMatrix& M );
  /** destructor */
	~SparseMatrix();

  /** set elements to zero */
	void SetZero();
  /** set matrix as identity */
	void SetIdentity();

  /** operload operator for math operations */
	SparseMatrix<T>& operator = (const SparseMatrix<T>& M);

  /** operload operator for math operations */
	SparseMatrix<T> operator * (const T& V) const;
  /** operload operator for math operations */
	SparseMatrix<T>& operator *= (const T& V);

  /** operload operator for math operations */
	SparseMatrix<T> operator * (const SparseMatrix<T>& M) const;
  /** multiplication with another sparse matrix */
	SparseMatrix<T> Multiply( const SparseMatrix<T>& M ) const;
  /** multiplication with another transpose sparse matrix */
	SparseMatrix<T> MultiplyTranspose( const SparseMatrix<T>& Mt ) const;

  /** operload operator for math operations */
	template<class T2>
	Vector<T2> operator * (const Vector<T2>& V) const;
  /** Multiply with a vector and return the result */
	template<class T2>
	Vector<T2> Multiply( const Vector<T2>& V ) const;
	template<class T2>
  /** Multiply sparse matrix and vector and retrieve result */
	void Multiply( const Vector<T2>& In, Vector<T2>& Out ) const;

  /** transpose sparse matrix*/
	SparseMatrix<T> Transpose() const;
  /** Solve for x s.t. M(x)=b by solving for x s.t. M^tM(x)=M^t(b) */
	static int Solve			(const SparseMatrix<T>& M,const Vector<T>& b,const int& iters,Vector<T>& solution,const T eps=1e-8);

  /** reduced as symmetric sparse matrix and solve*/
	template<class T2>
	static int SolveSymmetric	(const SparseMatrix<T>& M,const Vector<T2>& b,const int& iters,Vector<T2>& solution,const T2 eps=1e-8,const int& reset=1);

};
/**
class name:  SparseNMatrix
template class that introduce (from SparseMAtrix) dimensionality
*/
template<class T,int Dim> class SparseNMatrix
{
private:
	static int UseAlloc;
public:
	static Allocator<NMatrixEntry<T,Dim> > Allocator;
  /** return  allocator for sparse matrix  */
	static int UseAllocator(void);
  /**  use allocator , allocating block size */
	static void SetAllocator(const int& blockSize);

	int rows;
	int* rowSizes;
	NMatrixEntry<T,Dim>** m_ppElements;

  /** constructor */
	SparseNMatrix();
  /** overloaded constructor */
	SparseNMatrix( int rows );
  /** resize the matrix*/
	void Resize( int rows );
  /** change row size */
	void SetRowSize( int row , int count );
  /** retrieve total number of entries summing each rowsize*/
	int Entries(void);

  /** overloaded constructor */
	SparseNMatrix( const SparseNMatrix& M );
  /** destructor */
	~SparseNMatrix();

  /** operload operator for math operations */
	SparseNMatrix& operator = (const SparseNMatrix& M);
  
  /** operload operator for math operations */
	SparseNMatrix  operator *  (const T& V) const;
  /** operload operator for math operations */
	SparseNMatrix& operator *= (const T& V);

  /** operload operator for math operations */
	template<class T2>
	NVector<T2,Dim> operator * (const Vector<T2>& V) const;
  /** operload operator for math operations */
	template<class T2>
	Vector<T2> operator * (const NVector<T2,Dim>& V) const;
};


/**
class name: SparseSymmetricMatrix
Symmetric version of SparseMatrix (simpler the solutionof the system)
*/
template <class T>
class SparseSymmetricMatrix : public SparseMatrix<T>{
public:

  /** operload operator for math operations */
  template<class T2>
	Vector<T2> operator * (const Vector<T2>& V) const;
  /** Multiply with a vector and return the result */
	template<class T2>
	Vector<T2> Multiply( const Vector<T2>& V ) const;
  /** Multiply with a vector and return the result in the last parameter*/
	template<class T2>
	void Multiply( const Vector<T2>& In, Vector<T2>& Out ) const;

  /** solve the sytem with symmetric sparse matrix */
	template<class T2>
	static int Solve(const SparseSymmetricMatrix<T>& M,const Vector<T2>& b,const int& iters,Vector<T2>& solution,const T2 eps=1e-8,const int& reset=1);

  /** solve the sytem with symmetric sparse matrix */
	template<class T2>
	static int Solve(const SparseSymmetricMatrix<T>& M,const Vector<T>& diagonal,const Vector<T2>& b,const int& iters,Vector<T2>& solution,const T2 eps=1e-8,const int& reset=1);
};


/*=========================================================================
Octree.h
=========================================================================*/
/**
class name: OctNode
It represents a node of an octree. An octree is a tree data structure in which each internal node has exactly eight children. 
Octrees are most often used to partition a three dimensional space by recursively subdividing it into eight octants. 
Octrees are the three-dimensional analog of quadtrees.
*/
template<class NodeData,class Real=float>
class OctNode
{
private:
	static int UseAlloc;

  /**
      class name: AdjacencyCountFunction
     This is like a functor which count the adjacencies giving 2 nodes.
      */
	class AdjacencyCountFunction{
	public:
		int count;
    /** calculate number of adjacencies */
		void Function(const OctNode<NodeData,Real>* node1,const OctNode<NodeData,Real>* node2);
	};
  /**  process Node faces */
	template<class NodeAdjacencyFunction>
	void __processNodeFaces(OctNode* node,NodeAdjacencyFunction* F,const int& cIndex1,const int& cIndex2,const int& cIndex3,const int& cIndex4);
  /**  process Node edges */
	template<class NodeAdjacencyFunction>
	void __processNodeEdges(OctNode* node,NodeAdjacencyFunction* F,const int& cIndex1,const int& cIndex2);
  /**  process nodes */
	template<class NodeAdjacencyFunction>
	void __processNodeNodes(OctNode* node,NodeAdjacencyFunction* F);
  /** process node adjacencencies of the  node */
	template<class NodeAdjacencyFunction>
	static void __ProcessNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,OctNode* node1,const int& radius1,OctNode* node2,const int& radius2,const int& cWidth2,NodeAdjacencyFunction* F);
	/** process terminating node of the adjacentNode node */
  template<class TerminatingNodeAdjacencyFunction>
	static void __ProcessTerminatingNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,OctNode* node1,const int& radius1,OctNode* node2,const int& radius2,const int& cWidth2,TerminatingNodeAdjacencyFunction* F);
	/** process point  of adjacencent  nodes */
  template<class PointAdjacencyFunction>
	static void __ProcessPointAdjacentNodes(const int& dx,const int& dy,const int& dz,OctNode* node2,const int& radius2,const int& cWidth2,PointAdjacencyFunction* F);
	/** process fixed depth  of adjacencent  nodes */
  template<class NodeAdjacencyFunction>
	static void __ProcessFixedDepthNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,OctNode* node1,const int& radius1,OctNode* node2,const int& radius2,const int& cWidth2,const int& depth,NodeAdjacencyFunction* F);
	/** process max depth  of adjacencent  nodes */
  template<class NodeAdjacencyFunction>
	static void __ProcessMaxDepthNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,OctNode* node1,const int& radius1,OctNode* node2,const int& radius2,const int& cWidth2,const int& depth,NodeAdjacencyFunction* F);

	// This is made private because the division by two has been pulled out.
  /** check if nodes overlaps  */
	static inline int Overlap(const int& c1,const int& c2,const int& c3,const int& dWidth);
  /** check if node child overlaps  */
	inline static int ChildOverlap(const int& dx,const int& dy,const int& dz,const int& d,const int& cRadius2);

  /** return neighbor with the same face*/
	const OctNode* __faceNeighbor(const int& dir,const int& off) const;
  /** return neighbor with the same edge*/
	const OctNode* __edgeNeighbor(const int& o,const int i[2],const int idx[2]) const;
  /** return neighbor with the same face*/
	OctNode* __faceNeighbor(const int& dir,const int& off,const int& forceChildren);
  /** return neighbor with the same edge*/
	OctNode* __edgeNeighbor(const int& o,const int i[2],const int idx[2],const int& forceChildren);
public:
	static const int DepthShift,OffsetShift,OffsetShift1,OffsetShift2,OffsetShift3;
	static const int DepthMask,OffsetMask;

	static Allocator<OctNode> Allocator;
  /** return allocator */
	static int UseAllocator(void);
  /** allocate blocksize with allocator */
	static void SetAllocator(int blockSize);

	OctNode* parent;
	OctNode* children;
	short d,off[3];
	NodeData nodeData;

  /** constructor */
	OctNode(void);
  /** destructor*/
	~OctNode(void);
  /** initialize children list */
	int initChildren(void);

  /** calculate depth and offset */
	void depthAndOffset(int& depth,int offset[3]) const; 
  /** return depth */
	int depth(void) const;
  /** calculate depth and offset */
	static inline void DepthAndOffset(const long long& index,int& depth,int offset[3]);
  /** calculate center and width*/
	static inline void CenterAndWidth(const long long& index,Point3D<Real>& center,Real& width);
  /** return depth using index and depthmask*/
	static inline int Depth(const long long& index);
  /** calculate depth and return offset*/
	static inline void Index(const int& depth,const int offset[3],short& d,short off[3]);
	/** calculate center and width*/
  void centerAndWidth(Point3D<Real>& center,Real& width) const;

  /** return number of leaves*/
	int leaves(void) const;
 	/** return max depth of leaves */
  int maxDepthLeaves(const int& maxDepth) const;
  /** return number of nodes*/
	int nodes(void) const;
  /** return max depth */
	int maxDepth(void) const;

  /** return root node */
	const OctNode* root(void) const;

  /** return next leaf*/
	const OctNode* nextLeaf(const OctNode* currentLeaf=NULL) const;
  /** return next leaf*/
	OctNode* nextLeaf(OctNode* currentLeaf=NULL);
  /** return next node*/
	const OctNode* nextNode(const OctNode* currentNode=NULL) const;
	/** return next node*/
  OctNode* nextNode(OctNode* currentNode=NULL);
  /**return next Branch*/
	const OctNode* nextBranch(const OctNode* current) const;
  /**return next Branch*/
	OctNode* nextBranch(OctNode* current);

  /**create tree recursively until its max depth is equal to the parameter*/
	void setFullDepth(const int& maxDepth);

  /** print leaves */
	void printLeaves(void) const;
  /** print range */
	void printRange(void) const;
  /** process Node Faces */
	template<class NodeAdjacencyFunction>
	void processNodeFaces(OctNode* node,NodeAdjacencyFunction* F,const int& fIndex,const int& processCurrent=1);
	/** process Node Edges */
  template<class NodeAdjacencyFunction>
	void processNodeEdges(OctNode* node,NodeAdjacencyFunction* F,const int& eIndex,const int& processCurrent=1);
	/** process Node Corners */
  template<class NodeAdjacencyFunction>
	void processNodeCorners(OctNode* node,NodeAdjacencyFunction* F,const int& cIndex,const int& processCurrent=1);
	/** process nodes */
  template<class NodeAdjacencyFunction>
	void processNodeNodes(OctNode* node,NodeAdjacencyFunction* F,const int& processCurrent=1);
	
  /** Process Node Adjacent Nodes */
	template<class NodeAdjacencyFunction>
	static void ProcessNodeAdjacentNodes(const int& maxDepth,OctNode* node1,const int& width1,OctNode* node2,const int& width2,NodeAdjacencyFunction* F,const int& processCurrent=1);
	/** process node adjacencencies of the  node */
  template<class NodeAdjacencyFunction>
	static void ProcessNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,OctNode* node1,const int& radius1,OctNode* node2,const int& radius2,const int& width2,NodeAdjacencyFunction* F,const int& processCurrent=1);
	/** process terminating node of the adjacentNode node */
  template<class TerminatingNodeAdjacencyFunction>
	static void ProcessTerminatingNodeAdjacentNodes(const int& maxDepth,OctNode* node1,const int& width1,OctNode* node2,const int& width2,TerminatingNodeAdjacencyFunction* F,const int& processCurrent=1);
	/** process terminating node of the adjacentNode node */
  template<class TerminatingNodeAdjacencyFunction>
	static void ProcessTerminatingNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,OctNode* node1,const int& radius1,OctNode* node2,const int& radius2,const int& width2,TerminatingNodeAdjacencyFunction* F,const int& processCurrent=1);
	/** process point  of adjacencent  nodes */
  template<class PointAdjacencyFunction>
	static void ProcessPointAdjacentNodes(const int& maxDepth,const int center1[3],OctNode* node2,const int& width2,PointAdjacencyFunction* F,const int& processCurrent=1);
	/** process point  of adjacencent  nodes */
  template<class PointAdjacencyFunction>
	static void ProcessPointAdjacentNodes(const int& dx,const int& dy,const int& dz,OctNode* node2,const int& radius2,const int& width2,PointAdjacencyFunction* F,const int& processCurrent=1);
	/** process fixed depth  of adjacencent  nodes */
  template<class NodeAdjacencyFunction>
	static void ProcessFixedDepthNodeAdjacentNodes(const int& maxDepth,OctNode* node1,const int& width1,OctNode* node2,const int& width2,const int& depth,NodeAdjacencyFunction* F,const int& processCurrent=1);
	/** process fixed depth  of adjacencent  nodes */
  template<class NodeAdjacencyFunction>
	static void ProcessFixedDepthNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,OctNode* node1,const int& radius1,OctNode* node2,const int& radius2,const int& width2,const int& depth,NodeAdjacencyFunction* F,const int& processCurrent=1);
	/** process max depth  of adjacencent  nodes */
  template<class NodeAdjacencyFunction>
	static void ProcessMaxDepthNodeAdjacentNodes(const int& maxDepth,OctNode* node1,const int& width1,OctNode* node2,const int& width2,const int& depth,NodeAdjacencyFunction* F,const int& processCurrent=1);
	/** process max depth  of adjacencent  nodes */
  template<class NodeAdjacencyFunction>
	static void ProcessMaxDepthNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,OctNode* node1,const int& radius1,OctNode* node2,const int& radius2,const int& width2,const int& depth,NodeAdjacencyFunction* F,const int& processCurrent=1);
  /** return corner index */
	static int CornerIndex(const Point3D<Real>& center,const Point3D<Real> &p);

  /** return neighbor with the same face*/
	OctNode* faceNeighbor(const int& faceIndex,const int& forceChildren=0);
	/** return neighbor with the same face*/
  const OctNode* faceNeighbor(const int& faceIndex) const;
	/** return neighbor with the same edge*/
  OctNode* edgeNeighbor(const int& edgeIndex,const int& forceChildren=0);
  /** return neighbor with the same edge*/
	const OctNode* edgeNeighbor(const int& edgeIndex) const;
  /** return neighbor with the same corner*/
	OctNode* cornerNeighbor(const int& cornerIndex,const int& forceChildren=0);
  /** return neighbor with the same corner*/
	const OctNode* cornerNeighbor(const int& cornerIndex) const;

  /** retrieve the nearest leaf to Point3D argument*/
	OctNode* getNearestLeaf(const Point3D<Real>& p);
  /** retrieve the nearest leaf to Point3D argument*/
	const OctNode* getNearestLeaf(const Point3D<Real>& p) const;

  /** find the common edge betwee the nodes v1-v2*/
	static int CommonEdge(const OctNode* node1,const int& eIndex1,const OctNode* node2,const int& eIndex2);
  /** compare depths of octree nodes  v1-v2*/
	static int CompareForwardDepths(const void* v1,const void* v2);
  /** compare depths of octree nodes, using pointers */
	static int CompareForwardPointerDepths(const void* v1,const void* v2);
  /** compare depths of octree nodes v2-v1 */
	static int CompareBackwardDepths(const void* v1,const void* v2);
   /** compare depths of octree nodes, using pointers v2-v1 */
	static int CompareBackwardPointerDepths(const void* v1,const void* v2);


  /** overload operator assignment */
	template<class NodeData2>
	OctNode& operator = (const OctNode<NodeData2,Real>& node);

  /** function which calculate if nodes overlap*/
	static inline int Overlap2(const int &depth1,const int offSet1[DIMENSION],const Real& multiplier1,const int &depth2,const int offSet2[DIMENSION],const Real& multiplier2);

/*
	int write(const char* fileName) const;
	int write(FILE* fp) const;
	int read(const char* fileName);
	int read(FILE* fp);
*/
  /**
     class name: Neighbors
     contains a 3x3x3 structure in which store pointers of the neighbor
     */
	class Neighbors{
	public:
		OctNode* neighbors[3][3][3];
		Neighbors(void);
		void clear(void);
	};

  /**
      class name:NeighborKey
      Handler of octnode neighbors.
     */
	class NeighborKey{
	public:
		Neighbors* neighbors;
    /** constructor */
		NeighborKey(void);
    /** destructor */
		~NeighborKey(void);
    /** initialize variables */
		void set(const int& depth);
    /** insert neighbors */
		Neighbors& setNeighbors(OctNode* node);
    /** retrieve neighbors */
		Neighbors& getNeighbors(OctNode* node);
	};
  /**
      class name:Neighbors2
      Handler of octnode neighbors. Implement clear method.
     */
	class Neighbors2{
	public:
		const OctNode* neighbors[3][3][3];
    /** constructor */
		Neighbors2(void);
    /** clean the structure */
		void clear(void);
	};
  /**
      class name:NeighborKey2
      Handler of octnode neighbors2.
     */
	class NeighborKey2{
	public:
		Neighbors2* neighbors;
    /** constructor */
		NeighborKey2(void);
    /** destructor */
		~NeighborKey2(void);
    /** initialize variables */
		void set(const int& depth);
    /** retrieve neighbors */
		Neighbors2& getNeighbors(const OctNode* node);
	};
  /** index variable is fill with center indexes*/
	void centerIndex(const int& maxDepth,int index[DIMENSION]) const;
  /** return width of the node */
	int width(const int& maxDepth) const;
};


/*=========================================================================
MultiGridOctreeData.h
=========================================================================*/
typedef float Real;
typedef float FunctionDataReal;
typedef OctNode<class TreeNodeData,Real> TreeOctNode;

/** class name: RootInfo*/
class RootInfo{
public:
	const TreeOctNode* node;
	int edgeIndex;
	long long key;
};

/** class name: VertexData 
*/
class VertexData{
public:
	/** return edge index */
	static long long EdgeIndex(const TreeOctNode* node,const int& eIndex,const int& maxDepth,int index[DIMENSION]);
	/** return edge index */
	static long long EdgeIndex(const TreeOctNode* node,const int& eIndex,const int& maxDepth);
	/** return face index */
	static long long FaceIndex(const TreeOctNode* node,const int& fIndex,const int& maxDepth,int index[DIMENSION]);
	/** return face index */
	static long long FaceIndex(const TreeOctNode* node,const int& fIndex,const int& maxDepth);
	/** return corner index */
	static long long CornerIndex(const int& depth,const int offSet[DIMENSION],const int& cIndex,const int& maxDepth,int index[DIMENSION]);
	/** return corner index */
	static long long CornerIndex(const TreeOctNode* node,const int& cIndex,const int& maxDepth,int index[DIMENSION]);
	/** return corner index */
	static long long CornerIndex(const TreeOctNode* node,const int& cIndex,const int& maxDepth);
	/** return center index */
	static long long CenterIndex(const int& depth,const int offSet[DIMENSION],const int& maxDepth,int index[DIMENSION]);
	/** return center index */
	static long long CenterIndex(const TreeOctNode* node,const int& maxDepth,int index[DIMENSION]);
	/** return center index */
	static long long CenterIndex(const TreeOctNode* node,const int& maxDepth);
};
/** class name: SortedTreeNodes */
class SortedTreeNodes{
public:
	TreeOctNode** treeNodes;
	int *nodeCount;
	int maxDepth;
	/** Constructor */
	SortedTreeNodes(void);
	/** Destructor */
	~SortedTreeNodes(void);
	/** initialize and sort tree nodes, if set index > 0 assign index to nodes. */ 
	void Set(TreeOctNode& root,const int& setIndex);
};
/** class name: TreeNodeData
    index, weight and values associated with every tree node
*/
class TreeNodeData{
public:
	static int UseIndex;
	union{
		int mcIndex;
		struct{
			int nodeIndex;
			Real centerWeightContribution;
		};
	};
	Real value;
	/** Constructor */ 
	TreeNodeData(void);
	/** Destructor */
	~TreeNodeData(void);
};

/** class name: Octree
Tree with eight child nodes. */
template<int Degree>
class Octree{
	TreeOctNode::NeighborKey neighborKey;	
	TreeOctNode::NeighborKey2 neighborKey2;

	Real radius;
	int width;
  /** set idx as node index, update idx. */
	void setNodeIndices(TreeOctNode& tree,int& idx);
  /** compute product of dot values according to index array. */
	Real GetDotProduct(const int index[DIMENSION]) const;
  /* return laplacian of dot values according to index array. */
	Real GetLaplacian(const int index[DIMENSION]) const;
  /** return divergence according to index array. */
	Real GetDivergence(const int index[DIMENSION],const Point3D<Real>& normal) const;

  /** class name: DivergenceFunction*/
	class DivergenceFunction{
	public:
		Point3D<Real> normal;
		Octree<Degree>* ot;
		int index[DIMENSION],scratch[DIMENSION];
    /** compute divergence between two nodes. */
		void Function(TreeOctNode* node1,const TreeOctNode* node2);
	};

  /** class name: LaplacianProjectionFunction*/
	class LaplacianProjectionFunction{
	public:
		double value;
		Octree<Degree>* ot;
		int index[DIMENSION],scratch[DIMENSION];
    /** compute laplacian projection */
		void Function(TreeOctNode* node1,const TreeOctNode* node2);
	};
  /** class name: LaplacianMatrixFunction */
	class LaplacianMatrixFunction{
	public:
		int x2,y2,z2,d2;
		Octree<Degree>* ot;
		int index[DIMENSION],scratch[DIMENSION];
		int elementCount,offset;
		MatrixEntry<float>* rowElements;
    /** compute laplacian matrix; return 0 if OK*/
		int Function(const TreeOctNode* node1,const TreeOctNode* node2);
	};
  /**class name: RestrictedLaplacianMatrixFunction */
	class RestrictedLaplacianMatrixFunction{
	public:
		int depth,offset[3];
		Octree<Degree>* ot;
		Real radius;
		int index[DIMENSION],scratch[DIMENSION];
		int elementCount;
		MatrixEntry<float>* rowElements;
    /**compute restricted laplacian matrix */
		int Function(const TreeOctNode* node1,const TreeOctNode* node2);
	};

	///////////////////////////
	// Evaluation Functions  //
	///////////////////////////

  /**
  class name: PointIndexValueFunction
  product of values at node index.
  */
	class PointIndexValueFunction{
	public:
		int res2;
		FunctionDataReal* valueTables;
		int index[DIMENSION];
		Real value;
    /** update function*/
		void Function(const TreeOctNode* node);
	};
  /**
  class name: PointIndexValueAndNormalFunction
  product of values at node index
  compute normal coordinates
  */
	class PointIndexValueAndNormalFunction{
	public:
		int res2;
		FunctionDataReal* valueTables;
		FunctionDataReal* dValueTables;
		Real value;
		Point3D<Real> normal;
		int index[DIMENSION];
    /** update function*/
		void Function(const TreeOctNode* node);
	};

  /**
  class name: AdjacencyCountFunction
  update adjacency counter.
  */
	class AdjacencyCountFunction{
	public:
		int adjacencyCount;
   /** update function*/
		void Function(const TreeOctNode* node1,const TreeOctNode* node2);
	};
  /**
  class name: AdjacencySetFunction
  update adjacencies list.
  */
	class AdjacencySetFunction{
	public:
		int *adjacencies,adjacencyCount;
   /** update function*/
		void Function(const TreeOctNode* node1,const TreeOctNode* node2);
	};

  /**
  class name: RefineFunction
  Init children of a node if it has no children and its depth is smaller than a fixed value.
  */
	class RefineFunction{
	public:
		int depth;
    /** update function*/
		void Function(TreeOctNode* node1,const TreeOctNode* node2);
	};
  /**
  class name: FaceEdgesFunction
  */
	class FaceEdgesFunction{
	public:
		int fIndex,maxDepth;
		std::vector<std::pair<long long,long long> >* edges;
		hash_map<long long,std::pair<RootInfo,int> >* vertexCount;
     /** update function*/
		void Function(const TreeOctNode* node1,const TreeOctNode* node2);
	};

  /** solve matrix, depth value fixed */
	int SolveFixedDepthMatrix(const int& depth,const SortedTreeNodes& sNodes);
  /** solve matrix, depth value and starting depth fixed */
	int SolveFixedDepthMatrix(const int& depth,const int& startingDepth,const SortedTreeNodes& sNodes);

  /** compute laplacian, fixed depth */
	int GetFixedDepthLaplacian(SparseSymmetricMatrix<float>& matrix,const int& depth,const SortedTreeNodes& sNodes);
  /** compute restricted laplacian, fixed depth */
	int GetRestrictedFixedDepthLaplacian(SparseSymmetricMatrix<float>& matrix,const int& depth,const int* entries,const int& entryCount,const TreeOctNode* rNode,const Real& radius,const SortedTreeNodes& sNodes);

  /** set isosurface corners according to isovalue */
	void SetIsoSurfaceCorners(const Real& isoValue,const int& subdivisionDepth,const int& fullDepthIso);
  /** return 1 if is boundary face */
	static int IsBoundaryFace(const TreeOctNode* node,const int& faceIndex,const int& subdivideDepth);
  /** return 1 if is boundary edge */
	static int IsBoundaryEdge(const TreeOctNode* node,const int& edgeIndex,const int& subdivideDepth);
  /** return 1 if is boundary edge */
	static int IsBoundaryEdge(const TreeOctNode* node,const int& dir,const int& x,const int& y,const int& subidivideDepth);
  /** pre-validation */
	void PreValidate(const Real& isoValue,const int& maxDepth,const int& subdivideDepth);
  /** pre-validation */
	void PreValidate(TreeOctNode* node,const Real& isoValue,const int& maxDepth,const int& subdivideDepth);
  /** validation */
  void Validate(TreeOctNode* node,const Real& isoValue,const int& maxDepth,const int& fullDepthIso,const int& subdivideDepth);
	/** validation */
  void Validate(TreeOctNode* node,const Real& isoValue,const int& maxDepth,const int& fullDepthIso);
  /** sudivide according to isovalue */
	void Subdivide(TreeOctNode* node,const Real& isoValue,const int& maxDepth);

  /** set boundary marching cubes position */
	int SetBoundaryMCRootPositions(const int& sDepth,const Real& isoValue,
		hash_map<long long,int>& boundaryRoots,hash_map<long long,std::pair<Real,Point3D<Real> > >& boundaryNormalHash,CoredMeshData* mesh,const int& nonLinearFit);
	/** set marching cube root positions */
  int SetMCRootPositions(TreeOctNode* node,const int& sDepth,const Real& isoValue,
		hash_map<long long,int>& boundaryRoots,hash_map<long long,int>* interiorRoots,
		hash_map<long long,std::pair<Real,Point3D<Real> > >& boundaryNormalHash,hash_map<long long,std::pair<Real,Point3D<Real> > >* interiorNormalHash,
		std::vector<Point3D<float> >* interiorPositions,
		CoredMeshData* mesh,const int& nonLinearFit);

  /** get marching cubes triangles */
	int GetMCIsoTriangles(TreeOctNode* node,CoredMeshData* mesh,hash_map<long long,int>& boundaryRoots,
		hash_map<long long,int>* interiorRoots,std::vector<Point3D<float> >* interiorPositions,const int& offSet,const int& sDepth);

  /** add triangles to the mesh */
	static int AddTriangles(CoredMeshData* mesh,std::vector<CoredPointIndex> edges[3],std::vector<Point3D<float> >* interiorPositions,const int& offSet);
  /** add triangles to the mesh */
	static int AddTriangles(CoredMeshData* mesh,std::vector<CoredPointIndex>& edges,std::vector<Point3D<float> >* interiorPositions,const int& offSet);
  /** get marching cubes edges */
	void GetMCIsoEdges(TreeOctNode* node,hash_map<long long,int>& boundaryRoots,hash_map<long long,int>* interiorRoots,const int& sDepth,
		std::vector<std::pair<long long,long long> >& edges);
    /***/ 
	static int GetEdgeLoops(std::vector<std::pair<long long,long long> >& edges,std::vector<std::vector<std::pair<long long,long long> > >& loops);
	/** */
	static int InteriorFaceRootCount(const TreeOctNode* node,const int &faceIndex,const int& maxDepth);
	/** edge root count */
	static int EdgeRootCount(const TreeOctNode* node,const int& edgeIndex,const int& maxDepth);
	/** get octree root*/ 
	int GetRoot(const RootInfo& ri,const Real& isoValue,const int& maxDepth,Point3D<Real> & position,hash_map<long long,std::pair<Real,Point3D<Real> > >& normalHash,
		Point3D<Real>* normal,const int& nonLinearFit);
	/** get octree root*/
	int GetRoot(const RootInfo& ri,const Real& isoValue,Point3D<Real> & position,hash_map<long long,std::pair<Real,Point3D<Real> > >& normalHash,const int& nonLinearFit);
	/** return root index*/
	static int GetRootIndex(const TreeOctNode* node,const int& edgeIndex,const int& maxDepth,RootInfo& ri);
	/** return root index*/
	static int GetRootIndex(const TreeOctNode* node,const int& edgeIndex,const int& maxDepth,const int& sDepth,RootInfo& ri);
	/** return root index*/
	static int GetRootIndex(const long long& key,hash_map<long long,int>& boundaryRoots,hash_map<long long,int>* interiorRoots,CoredPointIndex& index);
	/** return root pair*/
	static int GetRootPair(const RootInfo& root,const int& maxDepth,RootInfo& pair);

	int NonLinearUpdateWeightContribution(TreeOctNode* node,const Point3D<Real>& position,const Real& weight=Real(1.0));
	Real NonLinearGetSampleWeight(TreeOctNode* node,const Point3D<Real>& position);
	void NonLinearGetSampleDepthAndWeight(TreeOctNode* node,const Point3D<Real>& position,const Real& samplesPerNode,Real& depth,Real& weight);
	int NonLinearSplatOrientedPoint(TreeOctNode* node,const Point3D<Real>& point,const Point3D<Real>& normal);
	void NonLinearSplatOrientedPoint(const Point3D<Real>& point,const Point3D<Real>& normal,const int& kernelDepth,const Real& samplesPerNode,const int& minDepth,const int& maxDepth);

	/** return 1 if node has normals */
	int HasNormals(TreeOctNode* node,const Real& epsilon);

  /** return center value of octree */
	Real getCenterValue(const TreeOctNode* node);
  /**return value of input corner */  
	Real getCornerValue(const TreeOctNode* node,const int& corner);
  /** compute value and normals of input corner*/
	void getCornerValueAndNormal(const TreeOctNode* node,const int& corner,Real& value,Point3D<Real>& normal);
public:
	static double maxMemoryUsage;
  /** return memory usage. update max memory usage attribute. */
	static double MemoryUsage(void);
	std::vector< Point3D<Real> >* normals;
	Real postNormalSmooth;
	TreeOctNode tree;
	FunctionData<Degree,FunctionDataReal> fData;
	/** Constructor */
	Octree(void);

	/** set polynomials as function data */
	void setFunctionData(const PPolynomial<Degree>& ReconstructionFunction,const int& maxDepth,const int& normalize,const Real& normalSmooth=-1);
	/** finalize using RefineFunction */
	void finalize1(const int& refineNeighbors=-1);
	/** finalize using RefineFunction */ 
	void finalize2(const int& refineNeighbors=-1);
	/** set tree according to input parameters */
	int setTree(const int& maxDepth,const int& kernelDepth,const Real& samplesPerNode,
		const Real& scaleFactor,Point3D<Real>& center,Real& scale,const int& resetSampleDepths,const int& useConfidence);

	/** set laplacian weights */
	void SetLaplacianWeights(void);
	/** clip tree */
	void ClipTree(void);
	/** return number of iterations. */
	int LaplacianMatrixIteration(const int& subdivideDepth);

	/** return isovalue */
	Real GetIsoValue(void);
	/** get marching cubes triangles */
	void GetMCIsoTriangles(const Real& isoValue,CoredMeshData* mesh,const int& fullDepthIso=0,const int& nonLinearFit=1);
	/** get marching cubes triangles */
	void GetMCIsoTriangles(const Real& isoValue,const int& subdivideDepth,CoredMeshData* mesh,const int& fullDepthIso=0,const int& nonLinearFit=1);
};


#endif