/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPointCloudCutter
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#ifndef __vtkALBAPointCloudCutter_h
#define __vtkALBAPointCloudCutter_h

#define _WINSOCKAPI_ 
//---------------------------------------------
// includes:
//---------------------------------------------
#include "vtkPolyDataAlgorithm.h"
#include "albaConfigure.h"
#include "vtkPlane.h"
#include <vector>

//---------------------------------------------
// class forward:
//---------------------------------------------
class vtkIdList;
class vtkCell;
class vtkMatrix4x4;

/** 

 class name: vtkALBAPointCloudCutter
VTK filter which cuts a point cloud with a plane.
This is very similar to vtkCutter().

Differences from vtkCutter():
1) vtkALBAPointCloudCutter() returns points close to the cutting plane
2) since the points are dimensionless, by cut we mean all the points that are less than a specific distance from the plane
3) input and outputs does not have cells
*/
class ALBA_EXPORT vtkALBAPointCloudCutter : public vtkPolyDataAlgorithm
{
public:
  /** RTTI macro*/
  vtkTypeMacro(vtkALBAPointCloudCutter, vtkPolyDataAlgorithm);

  /** return object instance */
  static vtkALBAPointCloudCutter *New() ;
  
  /** print object information */
  void PrintSelf(ostream& os, vtkIndent indent);                                ///< print self

   /** Overload standard modified time function. If cut function is modified,
  then this object is modified as well. */
  vtkMTimeType GetMTime();

  /** Set the cutting plane (but does not register the object) */
  void SetCutFunction(vtkPlane *P) ;

  /** Get the cutting plane */
  vtkPlane* GetCutFunction();
    
  /** initialize the cutter */
  void Initialize();

	// Description
	// Set Tolerance for cutting plane (distance from plane to consider a point as cut)
	vtkSetMacro(PlaneTolerance, double);
	vtkGetMacro(PlaneTolerance, double);

  
protected:
  /** constructor */
  vtkALBAPointCloudCutter() ;   
  /** destructor */                                                           
  ~vtkALBAPointCloudCutter() ;                                                             

  /** Request Data method */
	int RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);
  		
  /** transfer the scalars by interpolation from input to output */
  void TransferScalars() ;  

  /** do the whole thing */
  void CreateSlice() ;

	/** Uses transform matrix to calculate the local cutting coord*/
	void CalculateLocalCutCoord();
	  
  /** computes the intersection of the given plane with the mesh bounding box
  returns coordinates of the first intersection in pts, returns false, if the bounding box is not intersected*/
  bool GetIntersectionOfBoundsWithPlane(const double *origin, const double *norm);

	/** Modify input matrix to kept only rotational parts*/
	void ToRotationMatrix(vtkMatrix4x4 *matrix);

  // cutting function
  vtkPlane *CutFunction ;

	//Cutting Transformed Normal
	double CutTranformedNormal[4];

	//Cutting Transformed Origin
	double CutTranformedOrigin[4];

  // input and output
  vtkPolyData *InPolydata;
  vtkPolyData *OutPolydata ;

    
	std::vector<vtkIdType> PointsMapping;  //<mapping from output points to input points

	double PlaneTolerance;          //<tolerance for cutting plane (distance from plane to consider a point as cut)
private:
  void SlicePoints();
  
  friend class vtkALBAPointCloudCutterTest;
};
#endif
