/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkALBABridgeHoleFilter.h,v $
Language:  C++
Date:      $Date: 2009-03-20 16:39:48 $
Version:   $Revision: 1.1.2.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2012
University of Bedfordshire
=========================================================================*/


#ifndef __vtkALBAHalfTubeRemoval_h
#define __vtkALBAHalfTubeRemoval_h

#include "albaConfigure.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkALBAPolyDataNavigator.h"

#include <ostream>
#include <string>



//------------------------------------------------------------------------------
/// vtkALBAHalfTubeRemoval. \n
/// Remove half of a tube. \n
/// Inputs are polydata (assumed to be a tube and its center line) \n
/// and a viewing position. \n
/// Points which are nearer/further than the center line are removed. \n
//
// Version 5.12.13
//------------------------------------------------------------------------------
class ALBA_EXPORT vtkALBAHalfTubeRemoval : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkALBAHalfTubeRemoval,vtkPolyDataAlgorithm);
  static vtkALBAHalfTubeRemoval *New();
  void PrintSelf(ostream& os, vtkIndent indent) const {}

  /// Overload standard modified time function. 
	vtkMTimeType GetMTime();

  /// Set the center line which defines the boundary between front and back
  void SetCenterLine(vtkPolyData* centerline) ;

  /// Set position of camera
  void SetCameraPos(double camPos[3]) ;

  /// Set removal mode off (straight-through filter)
  void SetRemovalModeOff() ;

  /// Set mode to remove front
  void SetRemovalModeFront() ;

  /// Set mode to remove back
  void SetRemovalModeBack() ;

  /// Set automatic viewing position. \n
  /// Returns the configuration so it can be copied to the camera.
  void SetViewingPositionAuto(double pos[3], double focus[3], double upVector[3]) ;

protected:
  vtkALBAHalfTubeRemoval();
  ~vtkALBAHalfTubeRemoval();

	/** Execute method */
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  vtkPolyData *m_Input ;
  vtkPolyData *m_Output ;

private:
  /// Find nearest point on center line. \n
  /// No point locator used - assumes that no. of points is small.
  int FindNearestPoint(const double p[3], double pnearest[3], double& rsq) const ;

  /// Dist squared between points
  inline double DistSq(const double p[3], const double q[3]) const ;

  /// Find eigenvalues and eigenvectors of 3x3 matrix. \n
  /// eigenvalues are sorted in order of largest to smallest. \n
  /// eigenvectors are the columns of V[row][col]. \n
  /// Symmetric matrices only !
  void EigenVectors3x3(double A[3][3], double lambda[3], double V[3][3]) const ;

  /// Find principal axes of polydata
  void PrincipalAxes(vtkPolyData* polydata, double u[3], double v[3], double w[3]) const ;

  enum{
    REMOVE_NONE = 0,
    REMOVE_FRONT,
    REMOVE_BACK,
  };
  int m_RemovalMode ;

  vtkPolyData* m_CenterLine ;
  double m_CameraPos[3] ;
};


#endif