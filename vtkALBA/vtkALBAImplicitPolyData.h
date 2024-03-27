/*=========================================================================
=========================================================================*/
// .NAME vtkALBAImplicitPolyData - treat polygons in input as implicit planes
// .SECTION Description
// vtkALBAImplicitPolyData provides the basis for using arbritrary polygonal data
// as an implicit surface, points are evaluated against nearest polygons which
// are handled as implicit planes.
// vtkALBAImplicitPolyData is a concrete implementation of the abstract class
// vtkImplicitFunction.
// An internal instance of vtkTriangleFilter is used to filter vertices and
// lines out of the input PolyData, and a MYvtkPointLocator is used to find the
// nearest triangle to a candidate point.

#ifndef __vtkALBAImplicitPolyData_h
#define __vtkALBAImplicitPolyData_h

#include <math.h>
#include "vtkPolyData.h"
#include "vtkImplicitFunction.h"
#include "vtkTriangleFilter.h"
#include "vtkIdList.h"

#define PointLocator vtkPointLocator
#include "vtkPointLocator.h"

#include "albaConfigure.h"

class ALBA_EXPORT vtkALBAImplicitPolyData : public vtkImplicitFunction
{
public:
  static vtkALBAImplicitPolyData *New();
  vtkTypeMacro(vtkALBAImplicitPolyData, vtkImplicitFunction);

  void PrintSelf(ostream& os, vtkIndent indent);
  
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

	/**
	Set / get the Concave mode, by default the filter uses the convex modality*/
	vtkSetMacro(ConcaveMode, bool);
	vtkGetMacro(ConcaveMode, bool);

protected:
  vtkALBAImplicitPolyData();
  ~vtkALBAImplicitPolyData();

  /**
  Return the MTime also considering the Input dependency.*/
	vtkMTimeType GetMTime();

	double NoValue;
	double NoGradient[3];

	bool ConcaveMode;

	vtkTriangleFilter *Tri;
	vtkPolyData *Input;
	PointLocator *Locator;
  vtkPolygon *Poly;
  vtkIdList *Cells;

  /** friend test class */
  friend class vtkALBAImplicitPolyDataTest;
};
#endif
