/*=========================================================================
=========================================================================*/
// .NAME vtkMAFImplicitPolyData - treat polygons in input as implicit planes
// .SECTION Description
// vtkMAFImplicitPolyData provides the basis for using arbritrary polygonal data
// as an implicit surface, points are evaluated against nearest polygons which
// are handled as implicit planes.
// vtkMAFImplicitPolyData is a concrete implementation of the abstract class
// vtkImplicitFunction.
// An internal instance of vtkTriangleFilter is used to filter vertices and
// lines out of the input PolyData, and a MYvtkPointLocator is used to find the
// nearest triangle to a candidate point.

#ifndef __vtkMAFImplicitPolyData_h
#define __vtkMAFImplicitPolyData_h

#include <math.h>
#include "vtkPolyData.h"
#include "vtkImplicitFunction.h"
#include "vtkTriangleFilter.h"
#include "vtkIdList.h"

#define PointLocator vtkPointLocator
#include "vtkPointLocator.h"

#include "vtkMAFConfigure.h"

class VTK_vtkMAF_EXPORT vtkMAFImplicitPolyData : public vtkImplicitFunction
{
public:
  static vtkMAFImplicitPolyData *New();
  vtkTypeRevisionMacro(vtkMAFImplicitPolyData, vtkImplicitFunction);

  void PrintSelf(ostream& os, vtkIndent indent);

  void SetEvaluateBounds( double eBounds[6] );
  
  /**
  Evaluate plane equation of nearest triangle to point x[3].*/
  double EvaluateFunction(double x[3]);

  /**
  Evaluate function gradient of nearest triangle to point x[3].*/
  void EvaluateGradient(double x[3], double g[3]);

  /**
  Set the input polydata used for the implicit function evaluation.
  Passes input through an internal instance of vtkTriangleFilter to remove
  vertices and lines, leaving only triangular polygons for evaluation as
  implicit planes.*/
  void SetInput(vtkPolyData *input);

  /**
  Set / get the function value to use if no input polydata specified.*/
  vtkSetMacro(NoValue,double);
  vtkGetMacro(NoValue,double);

  /**
  Set / get the function gradient to use if no input polydata specified.*/
  vtkSetVector3Macro(NoGradient,double);
  vtkGetVector3Macro(NoGradient,double);

protected:
  vtkMAFImplicitPolyData();
  ~vtkMAFImplicitPolyData();

  /**
  Return the MTime also considering the Input dependency.*/
  unsigned long GetMTime();

	double NoValue;
	double NoGradient[3];

	vtkTriangleFilter *tri;
	vtkPolyData *input;
	PointLocator *locator;
  vtkPolygon *poly;
  vtkIdList *cells;

  double EvaluateBounds[6];
  int   EvaluateBoundsSet;
};
#endif
