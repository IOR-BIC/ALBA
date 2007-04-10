/*=========================================================================

  Progra:   Visualization Toolkit
  Module:    vtkMaskPolyDataFilter.h,v $
  Language:  C++
  Date:      2001/01
  Version:   1.1
  Author:    Roberto Gori(r.gori@cineca.it), patched by Marco Petrone (m.petroen@cineca.it)


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

#include "vtkDataSetToDataSetFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkPolygon.h"
#include "vtkMAFConfigure.h"
//#include "vtkSVTKWin32Header.h"

class VTK_vtkMAF_EXPORT vtkMaskPolyDataFilter : public vtkDataSetToDataSetFilter
{
public:
  
  const char *GetClassName() {return "vtkMaskPolyDataFilter";};
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
  Construct with user-specified implicit Surface; initial value of 0.0; and
  generating Mask scalars turned off.*/
  static vtkMaskPolyDataFilter *New() {return new vtkMaskPolyDataFilter;};

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
  Specify the polygonal dataset to perform the masking.*/
  void SetMask(vtkPolyData *mask) {this->SetNthInput(1, mask);};
  vtkPolyData *GetMask() { return (vtkPolyData *)(this->Inputs[1]);};
  
  /**
  If this flag is enabled, then the output scalar values will be
  generated from distances, and not the input scalar data.*/
/*  vtkSetMacro(GenerateMaskScalars,int);
  vtkGetMacro(GenerateMaskScalars,int);
  vtkBooleanMacro(GenerateMaskScalars,int);
 */
  
  /**
  Set / get the distance away from input geometry to
  sample. */
  vtkSetMacro(MaximumDistance,double);
  vtkGetMacro(MaximumDistance,double);

  /**
  Set / get the fill value*/
  vtkSetMacro(FillValue,double);
  vtkGetMacro(FillValue,double);
 
  
protected:
  vtkMaskPolyDataFilter(vtkPolyData *cf=NULL);
  ~vtkMaskPolyDataFilter();
  
  vtkMaskPolyDataFilter(const vtkMaskPolyDataFilter&);
  void operator=(const vtkMaskPolyDataFilter&);

  void Execute();
  vtkPolyData *Mask;
  double Distance; 
  double FillValue;
  int InsideOut;
//  int GenerateMaskScalars;
  double MaximumDistance;   
};

#endif


