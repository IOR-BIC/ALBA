/*=========================================================================

  Progra:   Visualization Toolkit
  Module:    vtkMaskPolyDataFilter.h,v $
  Language:  C++
  Date:      2001/01
  Version:   1.1
  Author:    Roberto Gori(r.gori@cineca.it), patched by Marco Petrone (m.petroen@cineca.it), Gianluigi Crimi


=========================================================================*/
// .NAME vtkMaskPolyDataFilter - Mask vtkDataSet with user-specified polygonal dataset
// .SECTION Description
// vtkMaskPolyDataFilter is a filter to mask through data using any subclass of 
// vtkPolyData. That is, scalars are masked where  signed distance to polygonal dataset
// D(x,y,z) <= value, where you can specify a value to mask with.
// vtkMaskPolyDataFilter computes the distance from the geometry
// to the points of an input dataset. This distance
// function can then be "contoured" to generate new, offset surfaces from
// the original geometry. An important feature of this object is
// "capping": after the implicit model is created,
// the values on the boundary of output dataset are set to
// the maximum distance  value. This is used to force closure of the resulting contoured
// surface. Note, however, that large cap values can generate weird surface
// normals in those cells adjacent to the boundary of the dataset. Using
// smaller cap value will reduce this effect.

// .SECTION See Also
// vtkMaskFilter vtkImpicitModeller

#ifndef __vtkMaskPolyDataFilter_h
#define __vtkMaskPolyDataFilter_h

#include "vtkDataSetAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkPolygon.h"
#include "albaConfigure.h"
//#include "vtkSVTKWin32Header.h"

class ALBA_EXPORT vtkMaskPolyDataFilter : public vtkDataSetAlgorithm 
{
public:
  
  /** RTTI macro */
  vtkTypeMacro(vtkMaskPolyDataFilter,vtkDataSetAlgorithm );

  const char *GetClassName() {return "vtkMaskPolyDataFilter";};
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
  Construct with user-specified implicit Surface; initial value of 0.0; and
  generating Mask scalars turned off.*/
  static vtkMaskPolyDataFilter *New();

  /**
  Set/Get a signed distance value */
  vtkSetMacro(Distance,double);
  vtkGetMacro(Distance,double);

  /**
  Set/Get the InsideOut flag. 
  When off, a vertex is masked if its distance from the polygonal dataset 
  is greater than the Dinstace ivar. 
  When  on, a vertex is masked if its distance from the polygonal dataset 
  is less than or equal to the Dinstace ivar.
  InsideOut is off by default.*/
  vtkSetMacro(InsideOut,int);
  vtkGetMacro(InsideOut,int);
  vtkBooleanMacro(InsideOut,int);


	/**
	Set/Get the Binarize flag.
	When off, the inside our outside data is copied from the input volume
	When  on, the output volume is a binary one 
	Binarize is off by default.*/
	vtkSetMacro(Binarize, int);
	vtkGetMacro(Binarize, int);
	vtkBooleanMacro(Binarize, int);

	/**
	Set/Get the Binarize flag.
	When off, a single pass algorithm is executed 
	When  on, the algorithm is executed three times, one in each ortho direction, and then the the output is decided by a 2 of 3 rule.
	TriplePass is off by default.*/
	vtkSetMacro(TriplePass, int);
	vtkGetMacro(TriplePass, int);
	vtkBooleanMacro(TriplePass, int);


  /**
  Specify the polygonal dataset to perform the masking.*/
  void SetMask(vtkPolyData *mask) {this->SetInputData(1, mask);};
  vtkPolyData *GetMask();

	/**
	Set / get the Inside value*/
	vtkSetMacro(InsideValue, double);
	vtkGetMacro(InsideValue, double);

	/**
	Set / get the Outside value*/
	vtkSetMacro(OutsideValue, double);
	vtkGetMacro(OutsideValue, double);

	
	/** Creates the current slice mask, used for algorithm optimization */
	void InitCurrentSliceMask();

	/** Updates the current slice mask depending on value, after the call on this function
	    the current slice mask will contains only cells that intersect current plane */
	void UpdateCurrentSliceMask(double value, int plane);
  
protected:
  vtkMaskPolyDataFilter(vtkPolyData *cf=NULL);
  ~vtkMaskPolyDataFilter();
  
  vtkMaskPolyDataFilter(const vtkMaskPolyDataFilter&);
  void operator=(const vtkMaskPolyDataFilter&);

	/**  Mask through data generating surface. */
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

	/**Execute standard Algorithm*/
	int StandardAlgorithm(vtkDataSet *input, vtkDataSet *output);

	/**Execute Triple pass Algorithm to reduce glitches */
	int TriplePassAlgorithm(vtkDataSet *input, vtkDataSet *output);

	vtkPolyData *Mask;
	vtkPolyData *CurrentSliceMask;
	vtkIdType *IdConversionTable;
	double Distance;
	double Distance2;
	double InsideValue;
	double OutsideValue;
	int Binarize;
	int InsideOut;
	int TriplePass;
};

#endif


