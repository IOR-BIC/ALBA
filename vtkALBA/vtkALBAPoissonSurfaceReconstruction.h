/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPoissonSurfaceReconstruction
 Authors: Fuli Wu
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAPoissonSurfaceReconstruction_h
#define __vtkALBAPoissonSurfaceReconstruction_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaConfigure.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkPolyDataAlgorithm.h"

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sysinfoapi.h>
#include <memoryapi.h>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

using std::unordered_map;

/**
class name: vtkALBAPoissonSurfaceReconstruction

This class implement Poisson Surface Reconstruction method.
A paper can be viewed here: research.microsoft.com/en-us/um/people/hoppe/poissonrecon.pdf
*/
class ALBA_EXPORT vtkALBAPoissonSurfaceReconstruction : public vtkPolyDataAlgorithm
{
public:
	/** create instance of the object */
	static vtkALBAPoissonSurfaceReconstruction *New();
  /** RTTI macro */
  vtkTypeMacro(vtkALBAPoissonSurfaceReconstruction,vtkPolyDataAlgorithm);
  /** print object information */
  void PrintSelf(ostream& os, vtkIndent indent);

	/** Set Input port information to accept the right type */
	int FillInputPortInformation(int, vtkInformation *info);

  // Description:
  // This error function allows our ported code to report error messages neatly.
  // This is not for external use. 
  void Error(const char *message);

protected:
  /** constructor */
  vtkALBAPoissonSurfaceReconstruction();
  /** destructor */
  ~vtkALBAPoissonSurfaceReconstruction();

  // Description:
  // the main function that does the work
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  /** computation of extents and update values*/
	int RequestUpdateExtent( vtkInformation *request, vtkInformationVector **inputVector,	vtkInformationVector *outputVector);

  /** only check if input is not null */
  int RequestInformation(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector); 
  
private:
  /** copy constructor not implemented */
  vtkALBAPoissonSurfaceReconstruction(const vtkALBAPoissonSurfaceReconstruction&);
  /** operator= non implemented */
  void operator=(const vtkALBAPoissonSurfaceReconstruction&);
};





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


/**
class name: Polynomial
Template class that represents a polynomial with a specific degree.
*/
template<int Degree>
class Polynomial{
public:
	double coefficients[Degree+1];

  /** constructor */
	Polynomial(void){memset(coefficients,0,sizeof(double)*(Degree+1));};
  /** copy constructor */
	template<int Degree2>
	Polynomial(const Polynomial<Degree2>& P){
		memset(coefficients,0,sizeof(double)*(Degree+1));
		for(int i=0;i<=Degree && i<=Degree2;i++){coefficients[i]=P.coefficients[i];}
	};
  /** overload operator () which retrieves  the sum of the product of the coefficients*/
	double operator()(const double& t) const{
		double temp=1;
		double v=0;
		for(int i=0;i<=Degree;i++){
			v+=temp*coefficients[i];
			temp*=t;
		}
		return v;
	};
  /** calculate integral */
	double Integral(const double& tMin,const double& tMax) const{
		double v=0;
		double t1,t2;
		t1=tMin;
		t2=tMax;
		for(int i=0;i<=Degree;i++){
			v+=coefficients[i]*(t2-t1)/(i+1);
			if(t1!=-DBL_MAX && t1!=DBL_MAX){t1*=tMin;}
			if(t2!=-DBL_MAX && t2!=DBL_MAX){t2*=tMax;}
		}
		return v;
	}
;

  /** operator== overload , checking coefficients */
	int operator == (const Polynomial& p) const{
		for(int i=0;i<=Degree;i++){if(coefficients[i]!=p.coefficients[i]){return 0;}}
		return 1;
	};
  /** operator!= overload, checking coefficients */
	int operator != (const Polynomial& p) const{
		for(int i=0;i<=Degree;i++){if(coefficients[i]==p.coefficients[i]){return 0;}}
		return 1;
	};
  /** check if all coefficients are zero*/
	int IsZero(void) const{
		for(int i=0;i<=Degree;i++){if(coefficients[i]!=0){return 0;}}
		return 1;
	};
  /** set  all coefficients as zero*/
	void SetZero(void){memset(coefficients,0,sizeof(double)*(Degree+1));};

  /** overload operator, according to the operation over coefficients */
	template<int Degree2>
	Polynomial& operator  = (const Polynomial<Degree2> &p){
		int d=Degree<Degree2?Degree:Degree2;
		memset(coefficients,0,sizeof(double)*(Degree+1));
		memcpy(coefficients,p.coefficients,sizeof(double)*(d+1));
		return *this;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial& operator += (const Polynomial& p){
		for(int i=0;i<=Degree;i++){coefficients[i]+=p.coefficients[i];}
		return *this;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial& operator -= (const Polynomial& p){
		for(int i=0;i<=Degree;i++){coefficients[i]-=p.coefficients[i];}
		return *this;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator -  (void) const{
		Polynomial q=*this;
		for(int i=0;i<=Degree;i++){q.coefficients[i]=-q.coefficients[i];}
		return q;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator +  (const Polynomial& p) const{
		Polynomial q;
		for(int i=0;i<=Degree;i++){q.coefficients[i]=(coefficients[i]+p.coefficients[i]);}
		return q;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator -  (const Polynomial& p) const{
		Polynomial q;
		for(int i=0;i<=Degree;i++)	{q.coefficients[i]=coefficients[i]-p.coefficients[i];}
		return q;
	};
  /** overload operator, according to the operation over coefficients */
	template<int Degree2>
	Polynomial<Degree+Degree2>  operator *  (const Polynomial<Degree2>& p) const{
		Polynomial<Degree+Degree2> q;
		for(int i=0;i<=Degree;i++){for(int j=0;j<=Degree2;j++){q.coefficients[i+j]+=coefficients[i]*p.coefficients[j];}}
		return q;
	};

  /** overload operator, according to the operation over coefficients */
	Polynomial& operator += (const double& s){
		coefficients[0]+=s;
		return *this;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial& operator -= (const double& s){
		coefficients[0]-=s;
		return *this;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial& operator *= (const double& s){
		for(int i=0;i<=Degree;i++){coefficients[i]*=s;}
		return *this;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial& operator /= (const double& s){
		for(int i=0;i<=Degree;i++){coefficients[i]/=s;}
		return *this;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator +  (const double& s) const{
		Polynomial<Degree> q=*this;
		q.coefficients[0]+=s;
		return q;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator -  (const double& s) const{
		Polynomial q=*this;
		q.coefficients[0]-=s;
		return q;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator *  (const double& s) const{
		Polynomial q;
		for(int i=0;i<=Degree;i++){q.coefficients[i]=coefficients[i]*s;}
		return q;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial  operator /  (const double& s) const{
		Polynomial q(degree());
		for(int i=0;i<=Degree;i++){q.coefficients[i]=coefficients[i]/s;}
		return q;
	};

  /** overload operator, according to the operation over coefficients */
	Polynomial Scale(const double& s) const{
		Polynomial q=*this;
		double s2=1.0;
		for(int i=0;i<=Degree;i++){
			q.coefficients[i]*=s2;
			s2/=s;
		}
		return q;
	};
  /** overload operator, according to the operation over coefficients */
	Polynomial Shift(const double& t) const{
		Polynomial<Degree> q;
		for(int i=0;i<=Degree;i++){
			double temp=1;
			for(int j=i;j>=0;j--){
				q.coefficients[j]+=coefficients[i]*temp;
				temp*=-t*j;
				temp/=(i-j+1);
			}
		}
		return q;
	};

  /** calculate derivative */
	Polynomial<Degree-1> Derivative(void) const{
		Polynomial<Degree-1> p;
		for(int i=0;i<Degree;i++){p.coefficients[i]=coefficients[i+1]*(i+1);}
		return p;
	};
  /** calculate integral */
	Polynomial<Degree+1> Integral(void) const{
		Polynomial<Degree+1> p;
		p.coefficients[0]=0;
		for(int i=0;i<=Degree;i++){p.coefficients[i+1]=coefficients[i]/(i+1);}
		return p;
	};

  /** print representation of polynomial */
	void Printnl(void) const{
		for(int j=0;j<=Degree;j++){
			printf("%6.4f x^%d ",coefficients[j],j);
			if(j<Degree && coefficients[j+1]>=0){printf("+");}
		}
		printf("\n");
	};

  /** overload operator, according to the operation over coefficients */
	Polynomial& AddScaled(const Polynomial& p,const double& s){
		for(int i=0;i<=Degree;i++){coefficients[i]+=p.coefficients[i]*s;}
		return *this;
	};

  /** overload operator, according to the operation over coefficients */
	static void Negate(const Polynomial& in,Polynomial& out){
		out=in;
		for(int i=0;i<=Degree;i++){out.coefficients[i]=-out.coefficients[i];}
	};
  /** overload operator, according to the operation over coefficients */
	static void Subtract(const Polynomial& p1,const Polynomial& p2,Polynomial& q){
		for(int i=0;i<=Degree;i++){q.coefficients[i]=p1.coefficients[i]-p2.coefficients[i];}
	};
  /** overload operator, according to the operation over coefficients */
	static void Scale(const Polynomial& p,const double& w,Polynomial& q){
		for(int i=0;i<=Degree;i++){q.coefficients[i]=p.coefficients[i]*w;}
	};
  /** overload operator, according to the operation over coefficients */
	static void AddScaled(const Polynomial& p1,const double& w1,const Polynomial& p2,const double& w2,Polynomial& q){
		for(int i=0;i<=Degree;i++){q.coefficients[i]=p1.coefficients[i]*w1+p2.coefficients[i]*w2;}
	};
  /** overload operator, according to the operation over coefficients */
	static void AddScaled(const Polynomial& p1,const Polynomial& p2,const double& w2,Polynomial& q){
		for(int i=0;i<=Degree;i++){q.coefficients[i]=p1.coefficients[i]+p2.coefficients[i]*w2;}
	};
  /** overload operator, according to the operation over coefficients */
	static void AddScaled(const Polynomial& p1,const double& w1,const Polynomial& p2,Polynomial& q){
		for(int i=0;i<=Degree;i++){q.coefficients[i]=p1.coefficients[i]*w1+p2.coefficients[i];}
	};

  /** calculate roots */
void GetSolutions(const double& c,std::vector<double>& roots,const double& EPS) const{
	double r[4][2];
	int rCount=0;
	roots.clear();
	switch(Degree){
	case 1:
		rCount=Factor(coefficients[1],coefficients[0]-c,r,EPS);
		break;
	case 2:
		rCount=Factor(coefficients[2],coefficients[1],coefficients[0]-c,r,EPS);
		break;
	case 3:
		rCount=Factor(coefficients[3],coefficients[2],coefficients[1],coefficients[0]-c,r,EPS);
		break;
		//	case 4:
		//		rCount=Factor(coefficients[4],coefficients[3],coefficients[2],coefficients[1],coefficients[0]-c,r,EPS);
		//		break;
	default:
		printf("Can't solve polynomial of degree: %d\n",Degree);
	}
	for(int i=0;i<rCount;i++){
		if(fabs(r[i][1])<=EPS){
			roots.push_back(r[i][0]);
			//printf("%d] %f\t%f\n",i,r[i][0],(*this)(r[i][0])-c);
		}
	}
};
};


/**
class name: StartingPolynomial
*/
template<int Degree>
class ALBA_EXPORT StartingPolynomial{
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
	PPolynomial(void){
		polyCount=0;
		polys=NULL;
	};
  /** Constructor. */
	PPolynomial(const PPolynomial<Degree>& p){
		polyCount=0;
		polys=NULL;
		set(p.polyCount);
		memcpy(polys,p.polys,sizeof(StartingPolynomial<Degree>)*p.polyCount);
	};
  /** Destructor. */
	~PPolynomial(void){
		if(polyCount){free(polys);}
		polyCount=0;
		polys=NULL;
	};

  /** Overload operator, according to the operation over coefficients. */
	PPolynomial& operator = (const PPolynomial& p){
		set(p.polyCount);
		memcpy(polys,p.polys,sizeof(StartingPolynomial<Degree>)*p.polyCount);
		return *this;
	};

  /** Return the size of polynomial object. */
	int size(void) const{return int(sizeof(StartingPolynomial<Degree>)*polyCount);};

  /** Free polynomials, allocate memory and set size starting polynomial.*/ 
	void set(const size_t& size){
		if(polyCount){free(polys);}
		polyCount=0;
		polys=NULL;
		polyCount=size;
		if(size){
			polys=(StartingPolynomial<Degree>*)malloc(sizeof(StartingPolynomial<Degree>)*size);
			memset(polys,0,sizeof(StartingPolynomial<Degree>)*size);
		}
	};
	/** Set sps as polynomials: this method will sort the elements in sps. */
	void set(StartingPolynomial<Degree>* sps,const int& count){
		int i,c=0;
		set(count);
		qsort(sps,count,sizeof(StartingPolynomial<Degree>),StartingPolynomial<Degree>::Compare);
		for(i=0;i<count;i++){
			if(!c || sps[i].start!=polys[c-1].start){polys[c++]=sps[i];}
			else{polys[c-1].p+=sps[i].p;}
		}
		reset(c);
	};
  /** Reallocate memory for newsize polys. */
	void reset(const size_t& newSize){
		polyCount=newSize;
		polys=(StartingPolynomial<Degree>*)realloc(polys,sizeof(StartingPolynomial<Degree>)*newSize);
	};


  /** Overload operator, return the sum of polys coefficients with index t.*/
	double operator()(const double& t) const{
		double v=0;
		for(int i=0;i<int(polyCount) && t>polys[i].start;i++){v+=polys[i].p(t);}
		return v;
	};
  /** Compute integral within integration range tMin and tMax, return the sum of integrals of polynomials. */
	double integral(const double& tMin,const double& tMax) const{
		int m=1;
		double start,end,s,v=0;
		start=tMin;
		end=tMax;
		if(tMin>tMax){
			m=-1;
			start=tMax;
			end=tMin;
		}
		for(int i=0;i<int(polyCount) && polys[i].start<end;i++){
			if(start<polys[i].start){s=polys[i].start;}
			else{s=start;}
			v+=polys[i].p.Integral(s,end);
		}
		return v*m;
	};
  /** Compute integral */
	double Integral(void) const{return integral(polys[0].start,polys[polyCount-1].start);};

  /** Overload operator, according to the operation over polys. */
	template<int Degree2>
	PPolynomial<Degree>& operator = (const PPolynomial<Degree2>& p){
		set(p.polyCount);
		for(int i=0;i<int(polyCount);i++){
			polys[i].start=p.polys[i].start;
			polys[i].p=p.polys[i].p;
		}
		return *this;
	};

  /** Combinate polynomials (sorting according to start value).*/
	PPolynomial  operator + (const PPolynomial& p) const{
		PPolynomial q;
		int i,j;
		size_t idx=0;
		q.set(polyCount+p.polyCount);
		i=j=-1;

		while(idx<q.polyCount){
			if		(j>=int(p.polyCount)-1)				{q.polys[idx]=  polys[++i];}
			else if	(i>=int(  polyCount)-1)				{q.polys[idx]=p.polys[++j];}
			else if(polys[i+1].start<p.polys[j+1].start){q.polys[idx]=  polys[++i];}
			else										{q.polys[idx]=p.polys[++j];}
			//		if(idx && polys[idx].start==polys[idx-1].start)	{polys[idx-1].p+=polys[idx].p;}
			//		else{idx++;}
			idx++;
		}
		return q;
	};
  /** Combinate polynomials (sorting according to start value). p polys with negative sign.*/
	PPolynomial  operator - (const PPolynomial& p) const{
		PPolynomial q;
		int i,j;
		size_t idx=0;
		q.set(polyCount+p.polyCount);
		i=j=-1;

		while(idx<q.polyCount){
			if		(j>=int(p.polyCount)-1)				{q.polys[idx]=  polys[++i];}
			else if	(i>=int(  polyCount)-1)				{q.polys[idx].start=p.polys[++j].start;q.polys[idx].p=p.polys[j].p*(-1.0);}
			else if(polys[i+1].start<p.polys[j+1].start){q.polys[idx]=  polys[++i];}
			else										{q.polys[idx].start=p.polys[++j].start;q.polys[idx].p=p.polys[j].p*(-1.0);}
			//		if(idx && polys[idx].start==polys[idx-1].start)	{polys[idx-1].p+=polys[idx].p;}
			//		else{idx++;}
			idx++;
		}
		return q;
	};

  /** Combinate polys according to multiplicative operation. */
	template<int Degree2>
	PPolynomial<Degree+Degree2> operator * (const Polynomial<Degree2>& p) const{
		PPolynomial<Degree+Degree2> q;
		q.set(polyCount);
		for(int i=0;i<int(polyCount);i++){
			q.polys[i].start=polys[i].start;
			q.polys[i].p=polys[i].p*p;
		}
		return q;
	};

  /** Combinate polys according to multiplicative operation. 
  A new polynomial is computed with the multiplication beetwen original polys member 
  and each member of the poly parameter./ */
	template<int Degree2>
	PPolynomial<Degree+Degree2> operator * (const PPolynomial<Degree2>& p) const{
		PPolynomial<Degree+Degree2> q;
		StartingPolynomial<Degree+Degree2> *sp;
		int i,j,spCount=int(polyCount*p.polyCount);

		sp=(StartingPolynomial<Degree+Degree2>*)malloc(sizeof(StartingPolynomial<Degree+Degree2>)*spCount);
		for(i=0;i<int(polyCount);i++){
			for(j=0;j<int(p.polyCount);j++){
				sp[i*p.polyCount+j]=polys[i]*p.polys[j];
			}
		}
		q.set(sp,spCount);
		free(sp);
		return q;
	};


  /** Overload operator, according to the operation over polys. */
	PPolynomial& operator += (const double& s){polys[0].p+=s;};
  /** Overload operator, according to the operation over polys. */
	PPolynomial& operator -= (const double& s){polys[0].p-=s;};
  /** Overload operator, according to the operation over polys. */
	PPolynomial& operator *= (const double& s){
		for(int i=0;i<int(polyCount);i++){polys[i].p*=s;}
		return *this;
	};
  /** Overload operator, according to the operation over polys. */
	PPolynomial& operator /= (const double& s){
		for(size_t i=0;i<polyCount;i++){polys[i].p/=s;}
		return *this;
	};
  /** Overload operator, according to the operation over polys. */
	PPolynomial  operator +  (const double& s) const{
		PPolynomial q=*this;
		q+=s;
		return q;
	};
  /** Overload operator, according to the operation over polys. */
	PPolynomial  operator -  (const double& s) const{
		PPolynomial q=*this;
		q-=s;
		return q;
	};
  /** Overload operator, according to the operation over polys. */
	PPolynomial  operator *  (const double& s) const{
		PPolynomial q=*this;
		q*=s;
		return q;
	};
  /** Overload operator, according to the operation over polys. */
	PPolynomial  operator /  (const double& s) const{
		PPolynomial q=*this;
		q/=s;
		return q;
	};

  /** Add operation with scaled polynomials. */
	PPolynomial& addScaled(const PPolynomial& poly,const double& scale){
		int i,j;
		StartingPolynomial<Degree>* oldPolys=polys;
		size_t idx=0,cnt=0,oldPolyCount=polyCount;
		polyCount=0;
		polys=NULL;
		set(oldPolyCount+p.polyCount);
		i=j=-1;
		while(cnt<polyCount){
			if		(j>=int( p.polyCount)-1)				{polys[idx]=oldPolys[++i];}
			else if	(i>=int(oldPolyCount)-1)				{polys[idx].start= p.polys[++j].start;polys[idx].p=p.polys[j].p*scale;}
			else if	(oldPolys[i+1].start<p.polys[j+1].start){polys[idx]=oldPolys[++i];}
			else											{polys[idx].start= p.polys[++j].start;polys[idx].p=p.polys[j].p*scale;}
			if(idx && polys[idx].start==polys[idx-1].start)	{polys[idx-1].p+=polys[idx].p;}
			else{idx++;}
			cnt++;
		}
		free(oldPolys);
		reset(idx);
		return *this;
	};

  /** Scale operation on every polynomial. */
	PPolynomial scale(const double& s) const{
		PPolynomial q;
		q.set(polyCount);
		for(size_t i=0;i<polyCount;i++){q.polys[i]=polys[i].scale(s);}
		return q;
	};
  /** Shift operation on every polynomial. */
	PPolynomial shift(const double& s) const{
		PPolynomial q;
		q.set(polyCount);
		for(size_t i=0;i<polyCount;i++){q.polys[i]=polys[i].shift(s);}
		return q;
	};

   /** calculate derivative on polys. */
	PPolynomial<Degree-1> derivative(void) const{
		PPolynomial<Degree-1> q;
		q.set(polyCount);
		for(size_t i=0;i<polyCount;i++){
			q.polys[i].start=polys[i].start;
			q.polys[i].p=polys[i].p.Derivative();
		}
		return q;
	};
   /** calculate integral on polys.  */
	PPolynomial<Degree+1> integral(void) const{
		int i;
		PPolynomial<Degree+1> q;
		q.set(polyCount);
		for(i=0;i<int(polyCount);i++){
			q.polys[i].start=polys[i].start;
			q.polys[i].p=polys[i].p.Integral();
			q.polys[i].p-=q.polys[i].p(q.polys[i].start);
		}
		return q;
	};

  /** solve polynomials. */
	void getSolutions(const double& c,std::vector<double>& roots,const double& EPS,const double& min=-DBL_MAX,const double& max=DBL_MAX) const{
		Polynomial<Degree> p;
		std::vector<double> tempRoots;

		p.SetZero();
		for(size_t i=0;i<polyCount;i++){
			p+=polys[i].p;
			if(polys[i].start>max){break;}
			if(i<polyCount-1 && polys[i+1].start<min){continue;}
			p.GetSolutions(c,tempRoots,EPS);
			for(size_t j=0;j<tempRoots.size();j++){
				if(tempRoots[j]>polys[i].start && (i+1==polyCount || tempRoots[j]<=polys[i+1].start)){
					if(tempRoots[j]>min && tempRoots[j]<max){roots.push_back(tempRoots[j]);}
				}
			}
		}
	};

  /** print method. */
	void printnl(void) const{
		Polynomial<Degree> p;

		if(!polyCount){
			Polynomial<Degree> p;
			printf("[-Infinity,Infinity]\n");
		}
		else{
			for(size_t i=0;i<polyCount;i++){
				printf("[");
				if		(polys[i  ].start== DBL_MAX){printf("Infinity,");}
				else if	(polys[i  ].start==-DBL_MAX){printf("-Infinity,");}
				else								{printf("%f,",polys[i].start);}
				if(i+1==polyCount)					{printf("Infinity]\t");}
				else if (polys[i+1].start== DBL_MAX){printf("Infinity]\t");}
				else if	(polys[i+1].start==-DBL_MAX){printf("-Infinity]\t");}
				else								{printf("%f]\t",polys[i+1].start);}
				p=p+polys[i].p;
				p.Printnl();
			}
		}
		printf("\n");
	};

  /** Compute moving average. */
	PPolynomial<Degree+1> MovingAverage(const double& radius){
		PPolynomial<Degree+1> A;
		Polynomial<Degree+1> p;
		StartingPolynomial<Degree+1>* sps;

		sps=(StartingPolynomial<Degree+1>*)malloc(sizeof(StartingPolynomial<Degree+1>)*polyCount*2);

		for(int i=0;i<int(polyCount);i++){
			sps[2*i  ].start=polys[i].start-radius;
			sps[2*i+1].start=polys[i].start+radius;
			p=polys[i].p.Integral()-polys[i].p.Integral()(polys[i].start);
			sps[2*i  ].p=p.Shift(-radius);
			sps[2*i+1].p=p.Shift( radius)*-1;
		}
		A.set(sps,int(polyCount*2));
		free(sps);
		return A*1.0/(2*radius);
	}
;

  /** Return two polynomials forming a constant function with width as constanct value. */
	static PPolynomial ConstantFunction(const double& radius=0.5){
		if(Degree<0){
			fprintf(stderr,"Could not set degree %d polynomial as constant\n",Degree);
			exit(0);
		}
		PPolynomial q;
		q.set(2);

		q.polys[0].start=-radius;
		q.polys[1].start= radius;

		q.polys[0].p.coefficients[0]= 1.0;
		q.polys[1].p.coefficients[0]=-1.0;
		return q;
	};
  /** Compute gaussian approximation. */
	static PPolynomial GaussianApproximation(const double& width=0.5){return PPolynomial<Degree-1>::GaussianApproximation().MovingAverage(width);};
//	void write(FILE* fp,const int& samples,const double& min,const double& max) const;
};

template<>
PPolynomial<0> PPolynomial<0>::GaussianApproximation(const double& width)
{
	return ConstantFunction(width);
}

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
double Length(const Point3D<Real>& p){return sqrt(SquareLength(p));};

/** Return square distance beetwen origin and a 3d point. */
template<class Real>
double SquareLength(const Point3D<Real>& p){return p.coords[0]*p.coords[0]+p.coords[1]*p.coords[1]+p.coords[2]*p.coords[2];};

/** Return distance beetwen two 3d points. */
template<class Real>
double Distance(const Point3D<Real>& p1,const Point3D<Real>& p2);

/** Return square distance beetwen two 3d points. */
template<class Real>
double SquareDistance(const Point3D<Real>& p1,const Point3D<Real>& p2);

/** Cross product beetwen two 3d points: thrid input parameter will save resulting point. */
template <class Real>
void CrossProduct(const Point3D<Real>& p1,const Point3D<Real>& p2,Point3D<Real>& p){
	p.coords[0]= p1.coords[1]*p2.coords[2]-p1.coords[2]*p2.coords[1];
	p.coords[1]=-p1.coords[0]*p2.coords[2]+p1.coords[2]*p2.coords[0];
	p.coords[2]= p1.coords[0]*p2.coords[1]-p1.coords[1]*p2.coords[0];
};

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
class ALBA_EXPORT TriangulationEdge
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
class ALBA_EXPORT TriangulationTriangle
{
public:
  /** Constructor. */
	TriangulationTriangle(void);
	int EIndex[3];
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
	int Factor(const int& tIndex,int& p1,int& p2,int& p3){
		if(triangles[tIndex].EIndex[0]<0 || triangles[tIndex].EIndex[1]<0 || triangles[tIndex].EIndex[2]<0){return 0;}
		if(edges[triangles[tIndex].EIndex[0]].tIndex[0]==tIndex){p1=edges[triangles[tIndex].EIndex[0]].pIndex[0];}
		else													{p1=edges[triangles[tIndex].EIndex[0]].pIndex[1];}
		if(edges[triangles[tIndex].EIndex[1]].tIndex[0]==tIndex){p2=edges[triangles[tIndex].EIndex[1]].pIndex[0];}
		else													{p2=edges[triangles[tIndex].EIndex[1]].pIndex[1];}
		if(edges[triangles[tIndex].EIndex[2]].tIndex[0]==tIndex){p3=edges[triangles[tIndex].EIndex[2]].pIndex[0];}
		else													{p3=edges[triangles[tIndex].EIndex[2]].pIndex[1];}
		return 1;
	};
  
  /** calculate total area of triangles*/
	double Area(void){
		double a=0;
		for(int i=0;i<int(triangles.size());i++){a+=Area(i);}
		return a;
	};
  /** calculate area  of the triangle*/
	double Area(const int& tIndex){
		int p1,p2,p3;
		Factor(tIndex,p1,p2,p3);
		return Area(p1,p2,p3);
	};
  /** calculate area with cross product */
	double Area(const int& p1,const int& p2,const int& p3){
		Point3D<Real> q1,q2,q;
		for(int i=0;i<3;i++){
			q1.coords[i]=points[p2].coords[i]-points[p1].coords[i];
			q2.coords[i]=points[p3].coords[i]-points[p1].coords[i];
		}
		CrossProduct(q1,q2,q);
		return Length(q);
	};
  /** flip edges in order to minimize the area of the triangle */
	int FlipMinimize(const int& eIndex){
		double oldArea,newArea;
		int oldP[3],oldQ[3],newP[3],newQ[3];
		TriangulationEdge newEdge;

		if(edges[eIndex].tIndex[0]<0 || edges[eIndex].tIndex[1]<0){return 0;}

		if(!this->Factor(edges[eIndex].tIndex[0],oldP[0],oldP[1],oldP[2])){return 0;}
		if(!this->Factor(edges[eIndex].tIndex[1],oldQ[0],oldQ[1],oldQ[2])){return 0;}

		oldArea=this->Area(oldP[0],oldP[1],oldP[2])+this->Area(oldQ[0],oldQ[1],oldQ[2]);
		int idxP,idxQ;
		for(idxP=0;idxP<3;idxP++){
			int i;
			for(i=0;i<3;i++){if(oldP[idxP]==oldQ[i]){break;}}
			if(i==3){break;}
		}
		for(idxQ=0;idxQ<3;idxQ++){
			int i;
			for(i=0;i<3;i++){if(oldP[i]==oldQ[idxQ]){break;}}
			if(i==3){break;}
		}
		if(idxP==3 || idxQ==3){return 0;}
		newP[0]=oldP[idxP];
		newP[1]=oldP[(idxP+1)%3];
		newP[2]=oldQ[idxQ];
		newQ[0]=oldQ[idxQ];
		newQ[1]=oldP[(idxP+2)%3];
		newQ[2]=oldP[idxP];

		newArea=this->Area(newP[0],newP[1],newP[2])+this->Area(newQ[0],newQ[1],newQ[2]);
		if(oldArea<=newArea){return 0;}

		// Remove the entry in the hash_table for the old edge
		edgeMap.erase(EdgeIndex(edges[eIndex].pIndex[0],edges[eIndex].pIndex[1]));
		// Set the new edge so that the zero-side is newQ
		edges[eIndex].pIndex[0]=newP[0];
		edges[eIndex].pIndex[1]=newQ[0];
		// Insert the entry into the hash_table for the new edge
		edgeMap[EdgeIndex(newP[0],newQ[0])]=eIndex;
		// Update the triangle information
		for(int i=0;i<3;i++){
			int idx;
			idx=edgeMap[EdgeIndex(newQ[i],newQ[(i+1)%3])];
			triangles[edges[eIndex].tIndex[0]].EIndex[i]=idx;
			if(idx!=eIndex){
				if(edges[idx].tIndex[0]==edges[eIndex].tIndex[1]){edges[idx].tIndex[0]=edges[eIndex].tIndex[0];}
				if(edges[idx].tIndex[1]==edges[eIndex].tIndex[1]){edges[idx].tIndex[1]=edges[eIndex].tIndex[0];}
			}

			idx=edgeMap[EdgeIndex(newP[i],newP[(i+1)%3])];
			triangles[edges[eIndex].tIndex[1]].EIndex[i]=idx;
			if(idx!=eIndex){
				if(edges[idx].tIndex[0]==edges[eIndex].tIndex[0]){edges[idx].tIndex[0]=edges[eIndex].tIndex[1];}
				if(edges[idx].tIndex[1]==edges[eIndex].tIndex[0]){edges[idx].tIndex[1]=edges[eIndex].tIndex[1];}
			}
		}
		return 1;
	};

  /** insert new triangle in the structure */
	int AddTriangle(const int& p1,const int& p2,const int& p3){
		unordered_map<long long,int>::iterator iter;
		int tIdx,eIdx,p[3];
		p[0]=p1;
		p[1]=p2;
		p[2]=p3;
		triangles.push_back(TriangulationTriangle());
		tIdx=int(triangles.size())-1;

		for(int i=0;i<3;i++)
		{
			long long e = EdgeIndex(p[i],p[(i+1)%3]);
			iter=edgeMap.find(e);
			if(iter==edgeMap.end())
			{
				TriangulationEdge edge;
				edge.pIndex[0]=p[i];
				edge.pIndex[1]=p[(i+1)%3];
				edges.push_back(edge);
				eIdx=int(edges.size())-1;
				edgeMap[e]=eIdx;
				edges[eIdx].tIndex[0]=tIdx;
			}
			else{
				eIdx=edgeMap[e];
				if(edges[eIdx].pIndex[0]==p[i]){
					if(edges[eIdx].tIndex[0]<0){edges[eIdx].tIndex[0]=tIdx;}
					else{printf("Edge Triangle in use 1\n");return 0;}
				}
				else{
					if(edges[eIdx].tIndex[1]<0){edges[eIdx].tIndex[1]=tIdx;}
					else{printf("Edge Triangle in use 2\n");return 0;}
				}

			}
			triangles[tIdx].EIndex[i]=eIdx;
		}
		return tIdx;
	};

protected:
	unordered_map<long long,int> edgeMap;
	static long long EdgeIndex(const int& p1,const int& p2){
		if(p1>p2)	{return ((long long)(p1)<<32) | ((long long)(p2));}
		else		{return ((long long)(p2)<<32) | ((long long)(p1));}
	};
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



/**
class name: Square
Represent a square in a marching square algorithms
*/
class ALBA_EXPORT Square{
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
class ALBA_EXPORT Cube{
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



/**
className: MemoryInfo
Used for keeping trace of the physical memory (total and free), swap space (total and free), virtual address space (total and free)
and page size.
*/
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


/**
class name: Vector
Define mathematical entity vector, overloading operators.
*/
template<class T>
class Vector
{
public:
  /** constructor */
	Vector(){
		m_N = 0;
		m_pV = 0;
	};
  /** overloaded constructor */
	Vector( const Vector<T>& V ){
		m_N = 0;
		m_pV = 0;
		Resize(V.m_N);
		memcpy( m_pV, V.m_pV, m_N*sizeof(T) );
	};
  /** overloaded constructor */
	Vector( size_t N ){
		m_N=0;
		m_pV=0;
		Resize(N);
	};
  /** overloaded constructor */
	Vector( size_t N, T* pV ){
		Resize(N);
		memcpy( m_pV, pV, N*sizeof(T) );
	};
  /** destructor */
	~Vector(){Resize(0);};

  /** overload operator ()*/
	const T& operator () (size_t i) const{
		assert( i < m_N );
		return m_pV[i];
	};
  /** overload operator ()*/
	T& operator () (size_t i){
		return m_pV[i];
	};
  /** overload operator []*/
	const T& operator [] (size_t i) const{
		return m_pV[i];
	};
  /** overload operator []*/
	T& operator [] (size_t i){
		return m_pV[i];
	};

  /** set elements to zero */
	void SetZero(){for (size_t i=0; i<m_N; i++){m_pV[i] = T(0);}};

  /** return dimension of vector */
	size_t Dimensions() const{return m_N;};
  /** resize vector */
	void Resize( size_t N ){
		if(m_N!=N){
			if(m_N){delete[] m_pV;}
			m_pV=NULL;
			m_N = N;
			if(N){m_pV = new T[N];}
		}
		memset( m_pV, 0, N*sizeof(T) );
	};

  /** operload operator for math operations */
	Vector operator * (const T& A) const{
		Vector V(*this);
		for (size_t i=0; i<m_N; i++)
			V.m_pV[i] *= A;
		return V;
	};
  /** operload operator for math operations */
	Vector operator / (const T& A) const{
		Vector V(*this);
		for (size_t i=0; i<m_N; i++)
			V.m_pV[i] /= A;
		return V;
	};
  /** operload operator for math operations */
	Vector operator - (const Vector& V0) const{
		Vector<T> V(m_N);
		for (size_t i=0; i<m_N; i++)
			V.m_pV[i] = m_pV[i] - V0.m_pV[i];

		return V;
	};
  /** operload operator for math operations */
	Vector operator + (const Vector& V0) const{
		Vector<T> V(m_N);
		for (size_t i=0; i<m_N; i++)
			V.m_pV[i] = m_pV[i] + V0.m_pV[i];

		return V;
	};

  /** operload operator for math operations */
	Vector& operator *= (const T& A){
		for (size_t i=0; i<m_N; i++)
			m_pV[i] *= A;
		return *this;
	};
  /** operload operator for math operations */
	Vector& operator /= (const T& A){
		for (size_t i=0; i<m_N; i++)
			m_pV[i] /= A;
		return *this;
	};
  /** operload operator for math operations */
	Vector& operator += (const Vector& V){
		for (size_t i=0; i<m_N; i++)
			m_pV[i] += V.m_pV[i];

		return *this;
	};
  /** operload operator for math operations */
	Vector& operator -= (const Vector& V){
		for (size_t i=0; i<m_N; i++)
			m_pV[i] -= V.m_pV[i];

		return *this;
	};

  /** add a scaled vector */
	Vector& AddScaled(const Vector& V,const T& scale){
		for (size_t i=0; i<m_N; i++)
			m_pV[i] += V.m_pV[i]*scale;

		return *this;
	};
  /** subtract a scaled vector*/
	Vector& SubtractScaled(const Vector& V,const T& scale){
		for (size_t i=0; i<m_N; i++)
			m_pV[i] -= V.m_pV[i]*scale;

		return *this;
	};
  /** return the result of a sum of scaled vectors */
	static void Add(const Vector& V1,const T& scale1,const Vector& V2,const T& scale2,Vector& Out){
		for (size_t i=0; i<V1.m_N; i++)
			Out.m_pV[i]=V1.m_pV[i]*scale1+V2.m_pV[i]*scale2;
	};
  /** return the result of a sum of a non-scaled vector  and a scaled one*/
	static void Add(const Vector& V1,const T& scale1,const Vector& V2,Vector& Out){
		for (size_t i=0; i<V1.m_N; i++)
			Out.m_pV[i]=V1.m_pV[i]*scale1+V2.m_pV[i];
	};

  /** operload operator for math operations */
	Vector operator - () const{
		Vector<T> V(m_N);

		for (size_t i=0; i<m_N; i++)
			V.m_pV[i] = -m_pV[i];

		return V;
	};

  /** operload operator for assignment */
	Vector& operator = (const Vector& V){
		Resize(V.m_N);
		memcpy( m_pV, V.m_pV, m_N*sizeof(T) );
		return *this;
	};

  /** define dot product */
	T Dot( const Vector& V ) const{
		T V0 = T();
		for (size_t i=0; i<m_N; i++)
			V0 += m_pV[i]*V.m_pV[i];

		return V0;
	};
 
  /** define lenght of a vector */
	T Length() const{
		T N = T();
		for (size_t i = 0; i<m_N; i++)
			N += m_pV[i]*m_pV[i];
		return sqrt(N);	
	};

  /** calculate norm */
	T Norm( size_t Ln ) const{
		T N = T();
		for (size_t i = 0; i<m_N; i++)
			N += pow(m_pV[i], (T)Ln);
		return pow(N, (T)1.0/Ln);	
	};
  /** normalize the vector*/
	void Normalize(){
		T N = 1.0f/Norm(2);
		for (size_t i = 0; i<m_N; i++)
			m_pV[i] *= N;
	};

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
	NVector(){
		m_N = 0;
		m_pV = 0;
	};
  /** overloaded constructor */
	NVector( const NVector& V ){
		m_N = 0;
		m_pV = 0;
		Resize(V.m_N);
		memcpy( m_pV, V.m_pV, m_N*sizeof(T)*Dim );
	};
  /** overloaded constructor */
	NVector( size_t N ){
		m_N=0;
		m_pV=0;
		Resize(N);
	};
  /** overloaded constructor */
	NVector( size_t N, T* pV ){
		Resize(N);
		memcpy( m_pV, pV, N*sizeof(T)*Dim );
	};
	
  /** destructor */
	~NVector(){Resize(0);};

  /** overload operator ()*/
	const T* operator () (size_t i) const{
		assert( i < m_N );
		return &m_pV[i*Dim];
	};
  /** overload operator ()*/
	T* operator () (size_t i){
		return &m_pV[i*Dim];
	};
  /** overload operator []*/
	const T* operator [] (size_t i) const{
		return &m_pV[i*Dim];
	};
  /** overload operator []*/
	T* operator [] (size_t i){
		return &m_pV[i*Dim];
	};

  /** set elements to zero */
	void SetZero(){for (size_t i=0; i<m_N*Dim; i++){m_pV[i] = T(0);}};
  
  /** retrieve dimensions*/
	size_t Dimensions() const{return m_N;};
  /** resize vector */
	void Resize( size_t N ){
		if(m_N!=N){
			if(m_N){delete[] m_pV;}
			m_pV=NULL;
			m_N = N;
			if(N){m_pV = new T[Dim*N];}
		}
		memset( m_pV, 0, N*sizeof(T)*Dim );
	};

  /** operload operator for math operations */
	NVector operator * (const T& A) const{
		NVector<T,Dim> V(*this);
		for (size_t i=0; i<m_N*Dim; i++)
			V.m_pV[i] *= A;
		return V;
	};
  /** operload operator for math operations */
	NVector operator / (const T& A) const{
		NVector<T,Dim> V(*this);
		for (size_t i=0; i<m_N*Dim; i++)
			V.m_pV[i] /= A;
		return V;
	};
  /** operload operator for math operations */
	NVector operator - (const NVector& V0) const{
		NVector<T,Dim> V(m_N);
		for (size_t i=0; i<m_N*Dim; i++)
			V.m_pV[i] = m_pV[i] - V0.m_pV[i];

		return V;
	};
  /** operload operator for math operations */
	NVector operator + (const NVector& V0) const{
		NVector<T,Dim> V(m_N);
		for (size_t i=0; i<m_N*Dim; i++)
			V.m_pV[i] = m_pV[i] + V0.m_pV[i];

		return V;
	};

  /** operload operator for math operations */
	NVector& operator *= (const T& A){
		for (size_t i=0; i<m_N*Dim; i++)
			m_pV[i] *= A;
		return *this;
	};
  /** operload operator for math operations */
	NVector& operator /= (const T& A){
		for (size_t i=0; i<m_N*Dim; i++)
			m_pV[i] /= A;
		return *this;
	};
  /** operload operator for math operations */
	NVector& operator += (const NVector& V){
		for (size_t i=0; i<m_N*Dim; i++)
			m_pV[i] += V.m_pV[i];

		return *this;
	};
  /** operload operator for math operations */
	NVector& operator -= (const NVector& V){
		for (size_t i=0; i<m_N*Dim; i++)
			m_pV[i] -= V.m_pV[i];

		return *this;
	};

  /** add a scaled vector */
	NVector& AddScaled(const NVector& V,const T& scale){
		for (size_t i=0; i<m_N*Dim; i++)
			m_pV[i] += V.m_pV[i]*scale;

		return *this;
	};
  /** subtract a scaled vector*/
	NVector& SubtractScaled(const NVector& V,const T& scale){
		for (size_t i=0; i<m_N*Dim; i++)
			m_pV[i] -= V.m_pV[i]*scale;

		return *this;
	};
  /** return the result of a sum of scaled vectors */
	static void Add(const NVector& V1,const T& scale1,const NVector& V2,const T& scale2,NVector& Out){
		for (size_t i=0; i<V1.m_N*Dim; i++)
			Out.m_pV[i]=V1.m_pV[i]*scale1+V2.m_pV[i]*scale2;
	};
  /** return the result of a sum of a non-scaled vector  and a scaled one*/
	static void Add(const NVector& V1,const T& scale1,const NVector& V2,				NVector& Out){
		for (size_t i=0; i<V1.m_N*Dim; i++)
			Out.m_pV[i]=V1.m_pV[i]*scale1+V2.m_pV[i];
	};

  /** operload operator for math operations */
	NVector operator - () const{
		NVector<T,Dim> V(m_N);

		for (size_t i=0; i<m_N*Dim; i++)
			V.m_pV[i] = -m_pV[i];

		return V;
	};

  /** operload operator for assignment */
	NVector& operator = (const NVector& V)
	{
		Resize(V.m_N);
		memcpy( m_pV, V.m_pV, m_N*sizeof(T)*Dim );
		return *this;
	};

  /** define dot product */
	T Dot( const NVector& V ) const{
		T V0 = T();
		for (size_t i=0; i<m_N*Dim; i++)
			V0 += m_pV[i]*V.m_pV[i];

		return V0;
	};

  /** define lenght of a vector */
	T Length() const{
		T N = T();
		for (size_t i = 0; i<m_N*Dim; i++)
			N += m_pV[i]*m_pV[i];
		return sqrt(N);	
	};

  /** calculate norm */
	T Norm( size_t Ln ) const{
		T N = T();
		for (size_t i = 0; i<m_N*Dim; i++)
			N += pow(m_pV[i], (T)Ln);
		return pow(N, (T)1.0/Ln);	
	};
  /** normalize the n-vector*/
	void Normalize(){
		T N = 1.0f/Norm(2);
		for (size_t i = 0; i<m_N*3; i++)
			m_pV[i] *= N;
	};

	T* m_pV;
protected:
	size_t m_N;

};



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
	static int UseAllocator(void){return UseAlloc;};
  /**  use allocator , allocating block size */
	static void SetAllocator(const int& blockSize){
		if(blockSize>0){
			UseAlloc=1;
			Allocator.Set(blockSize);
		}
		else{UseAlloc=0;}
	};

	int rows;
	int* rowSizes;
	MatrixEntry<T>** m_ppElements;

  /** constructor */
	SparseMatrix(){
		rows=0;
		rowSizes=NULL;
		m_ppElements=NULL;
	};
  /** overloaded constructor */
	SparseMatrix(int nrows) {
		rows = 0;
		rowSizes = NULL;
		m_ppElements = NULL;
		Resize(nrows);
	};
  /** resize the matrix*/
	void Resize( int r ){
		int i;
		if(rows>0){
			if(!UseAlloc){for(i=0;i<rows;i++){if(rowSizes[i]){free(m_ppElements[i]);}}}
			free(m_ppElements);
			free(rowSizes);
		}
		rows=r;
		if(r){
			rowSizes=(int*)malloc(sizeof(int)*r);
			memset(rowSizes,0,sizeof(int)*r);
			m_ppElements=(MatrixEntry<T>**)malloc(sizeof(MatrixEntry<T>*)*r);
		}
	}
;
  /** change row size */
	void SetRowSize( int row , int count ){
		if(row>=0 && row<rows){
			if(UseAlloc){m_ppElements[row]=Allocator.NewElements(count);}
			else{
				if(rowSizes[row]){free(m_ppElements[row]);}
				if(count>0){m_ppElements[row]=(MatrixEntry<T>*)malloc(sizeof(MatrixEntry<T>)*count);}
			}
			rowSizes[row]=count;
		}
	};
  /** retrieve total number of entries summing each rowsize*/
	int Entries(void){
		int e=0;
		for(int i=0;i<rows;i++){e+=int(rowSizes[i]);}
		return e;
	};

  /** overloaded constructor */
	SparseMatrix( const SparseMatrix& M ){
		rows = 0;
		rowSizes = NULL;
		m_ppElements = NULL;
		Resize(M.rows);
		for (int i=0; i<rows; i++){
			SetRowSize(i,M.rowSizes[i]);
			for(int j=0;j<rowSizes[i];j++){m_ppElements[i][j]=M.m_ppElements[i][j];}
		}
	};
  /** destructor */
	~SparseMatrix(){Resize(0);};

  /** set elements to zero */
	void SetZero(){
		Resize(this->m_N, this->m_M);
	};
  /** set matrix as identity */
	void SetIdentity(){
		SetZero();
		for(int ij=0; ij < Min( this->Rows(), this->Columns() ); ij++)
			(*this)(ij,ij) = T(1);
	};

  /** operload operator for math operations */
	SparseMatrix<T>& operator = (const SparseMatrix<T>& M){
		Resize(M.rows);
		for (int i=0; i<rows; i++){
			SetRowSize(i,M.rowSizes[i]);
			for (int j=0; j<rowSizes[i]; j++){m_ppElements[i][j]=M.m_ppElements[i][j];}
		}
		return *this;
	};

  /** operload operator for math operations */
	SparseMatrix<T> operator * (const T& V) const{
		SparseMatrix<T> M(*this);
		M *= V;
		return M;
	};
  /** operload operator for math operations */
	SparseMatrix<T>& operator *= (const T& V){
		for (int i=0; i<this->rows; i++)
		{
			for(int ii=0;ii<rowSizes[i];ii++)
			{
				m_ppElements[i][ii].Value*=V;
			}
		}
		return *this;
	};

  /** operload operator for math operations */
	SparseMatrix<T> operator * (const SparseMatrix<T>& M) const{
		return Multiply(M);
	};
  /** multiplication with another sparse matrix */
	SparseMatrix<T> Multiply( const SparseMatrix<T>& M ) const{
		SparseMatrix<T> R( this->Rows(), M.Columns() );
		for(int i=0; i<R.Rows(); i++){
			for(int ii=0;ii<m_ppElements[i].size();ii++){
				int N=m_ppElements[i][ii].N;
				T Value=m_ppElements[i][ii].Value;
				for(int jj=0;jj<M.m_ppElements[N].size();jj++){
					R(i,M.m_ppElements[N][jj].N) += Value * M.m_ppElements[N][jj].Value;
				}
			}
		}
		return R;		
	};
  /** multiplication with another transpose sparse matrix */
	SparseMatrix<T> MultiplyTranspose( const SparseMatrix<T>& Mt ) const;

  /** operload operator for math operations */
	template<class T2>
	Vector<T2> operator * (const Vector<T2>& V) const{
		return Multiply(V);
	};
  /** Multiply with a vector and return the result */
	template<class T2>
	Vector<T2> Multiply( const Vector<T2>& V ) const{
		Vector<T2> R( rows );

		for (int i=0; i<rows; i++)
		{
			T2 temp=T2();
			for(int ii=0;ii<rowSizes[i];ii++){
				temp+=m_ppElements[i][ii].Value * V.m_pV[m_ppElements[i][ii].N];
			}
			R(i)=temp;
		}
		return R;
	}
;
	template<class T2>
  /** Multiply sparse matrix and vector and retrieve result */
	void Multiply( const Vector<T2>& In, Vector<T2>& Out ) const{
		for (int i=0; i<rows; i++){
			T2 temp=T2();
			for(int j=0;j<rowSizes[i];j++){temp+=m_ppElements[i][j].Value * In.m_pV[m_ppElements[i][j].N];}
			Out.m_pV[i]=temp;
		}
	};

  /** transpose sparse matrix*/
	SparseMatrix<T> Transpose() const{
		SparseMatrix<T> M( Columns(), Rows() );

		for (int i=0; i<Rows(); i++)
		{
			for(int ii=0;ii<m_ppElements[i].size();ii++){
				M(m_ppElements[i][ii].N,i) = m_ppElements[i][ii].Value;
			}
		}
		return M;
	};
  /** Solve for x s.t. M(x)=b by solving for x s.t. M^tM(x)=M^t(b) */
	static int Solve			(const SparseMatrix<T>& M,const Vector<T>& b,const int& iters,Vector<T>& solution,const T eps=1e-8){
		SparseMatrix mTranspose=M.Transpose();
		Vector<T> bb=mTranspose*b;
		Vector<T> d,r,Md;
		T alpha,beta,rDotR;
		int i;

		solution.Resize(M.Columns());
		solution.SetZero();

		d=r=bb;
		rDotR=r.Dot(r);

		for(i=0;i<iters && rDotR>eps;i++){
			T temp;
			Md=mTranspose*(M*d);
			alpha=rDotR/d.Dot(Md);
			solution+=d*alpha;
			r-=Md*alpha;
			temp=r.Dot(r);
			beta=temp/rDotR;
			rDotR=temp;
			d=r+d*beta;
		}
		return i;
	};

  /** reduced as symmetric sparse matrix and solve*/
	template<class T2>
	static int SolveSymmetric	(const SparseMatrix<T>& M,const Vector<T2>& b,const int& iters,Vector<T2>& solution,const T2 eps=1e-8,const int& reset=1){
		Vector<T2> d,r,Md;
		T2 alpha,beta,rDotR;
		Md.Resize(b.Dimensions());
		if(reset){
			solution.Resize(b.Dimensions());
			solution.SetZero();
		}
		d=r=b-M.Multiply(solution);
		rDotR=r.Dot(r);
		if(b.Dot(b)<=eps){
			solution.SetZero();
			return 0;
		}

		int i;
		for(i=0;i<iters;i++){
			T2 temp;
			M.Multiply(d,Md);
			temp=d.Dot(Md);
			if(temp<=eps){break;}
			alpha=rDotR/temp;
			r.SubtractScaled(Md,alpha);
			temp=r.Dot(r);
			if(temp/b.Dot(b)<=eps){break;}
			beta=temp/rDotR;
			solution.AddScaled(d,alpha);
			if(beta<=eps){break;}
			rDotR=temp;
			Vector<T2>::Add(d,beta,r,d);
		}
		return i;
	};

};

template<class T> int SparseMatrix<T>::UseAlloc=0;
template<class T> Allocator<MatrixEntry<T> > SparseMatrix<T>::Allocator;

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



/**
class name: OctNode
It represents a node of an octree. An octree is a tree data structure in which each internal node has exactly eight children. 
Octrees are most often used to partition a three dimensional space by recursively subdividing it into eight octants. 
Octrees are the three-dimensional analog of quadtrees.
*/
template<class NodeData,class Real=float>
class ALBA_EXPORT OctNode
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
	static const int DepthShift;
  static const int OffsetShift;
  static const int OffsetShift1;
  static const int OffsetShift2;
  static const int OffsetShift3;
	static const int DepthMask;
  static const int OffsetMask;

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



typedef float Real;
typedef float FunctionDataReal;
typedef OctNode<class TreeNodeData,Real> TreeOctNode;

/** class name: RootInfo*/
class RootInfo{
public:
	const TreeOctNode* Node;
	int EdgeIndex;
	long long Key;
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
class ALBA_EXPORT SortedTreeNodes{
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
class ALBA_EXPORT TreeNodeData{
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
class ALBA_EXPORT Octree{
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
		unordered_map<long long,std::pair<RootInfo,int> >* vertexCount;
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
		unordered_map<long long,int>& boundaryRoots,unordered_map<long long,std::pair<Real,Point3D<Real> > >& boundaryNormalHash,CoredMeshData* mesh,const int& nonLinearFit);
	/** set marching cube root positions */
  int SetMCRootPositions(TreeOctNode* node,const int& sDepth,const Real& isoValue,
		unordered_map<long long,int>& boundaryRoots,unordered_map<long long,int>* interiorRoots,
		unordered_map<long long,std::pair<Real,Point3D<Real> > >& boundaryNormalHash,unordered_map<long long,std::pair<Real,Point3D<Real> > >* interiorNormalHash,
		std::vector<Point3D<float> >* interiorPositions,
		CoredMeshData* mesh,const int& nonLinearFit);

  /** get marching cubes triangles */
	int GetMCIsoTriangles(TreeOctNode* node,CoredMeshData* mesh,unordered_map<long long,int>& boundaryRoots,
		unordered_map<long long,int>* interiorRoots,std::vector<Point3D<float> >* interiorPositions,const int& offSet,const int& sDepth);

  /** add triangles to the mesh */
	static int AddTriangles(CoredMeshData* mesh,std::vector<CoredPointIndex> edges[3],std::vector<Point3D<float> >* interiorPositions,const int& offSet);
  /** add triangles to the mesh */
	static int AddTriangles(CoredMeshData* mesh,std::vector<CoredPointIndex>& edges,std::vector<Point3D<float> >* interiorPositions,const int& offSet);
  /** get marching cubes edges */
	void GetMCIsoEdges(TreeOctNode* node,unordered_map<long long,int>& boundaryRoots,unordered_map<long long,int>* interiorRoots,const int& sDepth,
		std::vector<std::pair<long long,long long> >& edges);
    /***/ 
	static int GetEdgeLoops(std::vector<std::pair<long long,long long> >& edges,std::vector<std::vector<std::pair<long long,long long> > >& loops);
	/** */
	static int InteriorFaceRootCount(const TreeOctNode* node,const int &faceIndex,const int& maxDepth);
	/** edge root count */
	static int EdgeRootCount(const TreeOctNode* node,const int& edgeIndex,const int& maxDepth);
	/** get octree root*/ 
	int GetRoot(const RootInfo& ri,const Real& isoValue,const int& maxDepth,Point3D<Real> & position,unordered_map<long long,std::pair<Real,Point3D<Real> > >& normalHash,
		Point3D<Real>* normal,const int& nonLinearFit);
	/** get octree root*/
	int GetRoot(const RootInfo& ri,const Real& isoValue,Point3D<Real> & position,unordered_map<long long,std::pair<Real,Point3D<Real> > >& normalHash,const int& nonLinearFit);
	/** return root index*/
	static int GetRootIndex(const TreeOctNode* node,const int& edgeIndex,const int& maxDepth,RootInfo& ri);
	/** return root index*/
	static int GetRootIndex(const TreeOctNode* node,const int& edgeIndex,const int& maxDepth,const int& sDepth,RootInfo& ri);
	/** return root index*/
	static int GetRootIndex(const long long& key,unordered_map<long long,int>& boundaryRoots,unordered_map<long long,int>* interiorRoots,CoredPointIndex& index);
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
