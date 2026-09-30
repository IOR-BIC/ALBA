/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkALBALandmarkCloudPolydataFilter.h,v $
Language:  C++
Date:      $Date: 2009-03-20 16:39:48 $
Version:   $Revision: 1.1.2.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2014
University of Bedfordshire
=========================================================================*/


#ifndef __vtkALBALandmarkCloudPolydataFilter_h
#define __vtkALBALandmarkCloudPolydataFilter_h

#include "vtkPolyData.h"
#include "vtkPolyDataAlgorithm.h"


#include "vtkSphereSource.h"
#include "vtkALBAAddScalarsFilter.h"
#include <vector>


//------------------------------------------------------------------------------
/// vtkALBALandmarkCloudPolydataFilter. \n
/// Class which reads polydata and returns landmarks spheres. \n
/// The landmarks are associated with points on the polydata. \n
/// Useful for marking polydata points.
//
// Version: Nigel McFarlane 6.3.14
//------------------------------------------------------------------------------
class ALBA_EXPORT vtkALBALandmarkCloudPolydataFilter : public vtkPolyDataAlgorithm 
{
public:
  void PrintSelf(ostream& os, vtkIndent indent) const {}
  vtkTypeMacro(vtkALBALandmarkCloudPolydataFilter,vtkPolyDataAlgorithm);

  /// Constructor
  static vtkALBALandmarkCloudPolydataFilter *New();

  /// Clear list of landmarks
  void Clear() ;

  // Get number of landmarks
  int GetNumberOfLandmarks() const {return (int)m_SphereList.size();}

  /// Add single point from polydata.  Returns index of landmark.
  int AddLandmark(int ptId) ;

  /// Add single point from polydata.  Returns index of landmark.
  int AddLandmark(int ptId, double col[3]) ;

  /// Add single point from polydata.  Returns index of landmark.
  int AddLandmark(int ptId, double r, double g, double b) ;

  /// Add list of points from polydata
  void AddLandmarks(vtkIdList* ptIds) ;

  /// Add list of points from polydata
  void AddLandmarks(vtkIdList* ptIds, double col[3]) ;

  /// Add list of points from polydata
  void AddLandmarks(vtkIdList* ptIds, double r, double g, double b) ;

  /// Delete last landmark. \n
  /// No effect if list is already empty.
  void DeleteLastLandmark() ;

  /// Set theta and phi resolution
  void SetThetaPhiResolution(int thetaRes, int phiRes) ;

  /// Set radius
  void SetRadius(double r) ;

  /// Multiply radius by f
  void MultiplyRadius(double f) ;

  /// Set estimated radius based on polydata bounds
  void SetRadiusAuto(vtkPolyData* polydata) ;

  /// Get radius
  double GetRadius() const ;

  /// Get name of color scalars
  const char* GetScalarName() {return m_ScalarName ;}

  /// Set color of all landmarks
  void SetColor(double col[3]) ;

  /// Set color of all landmarks
  void SetColor(double r, double g, double b) ;

  /// Set color of landmark i
  void SetColor(int i, double col[3]) ;

  /// Set color of landmark i
  void SetColor(int i, double r, double g, double b) ;

  /// Set position of landmark i
  void SetPosition(int i, double x[3]) ;

  /// Set position of landmark i
  void SetPosition(int i, double x, double y, double z) ;

  /// Get position of landmark i
  void GetPosition(int i, double x[3]) const ;

  /// Get position of landmark i
  const double* GetPosition(int i) const ;

  /// Get id of landmark on polydata
  int GetPolydataId(int i) const {return m_PolydataIdsList[i];}

  /// Get bounds of landmark cloud. \n
  /// Returns false if no data.
  bool GetBounds(double b[6]) const ;

  /// Get center of landmark cloud. \n
  /// Returns false if no data.
  bool GetCenter(double x[3]) const ;

  /// Get centroid of landmark cloud. \n
  /// Returns false if no data.
  bool GetCentroid(double x[3]) const ;


protected:
  vtkALBALandmarkCloudPolydataFilter();
  ~vtkALBALandmarkCloudPolydataFilter();

  char m_ScalarName[256] ;

	/** Execute method */
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  // visual pipes
  std::vector<vtkSphereSource*> m_SphereList ;          // sphere sources
  std::vector<vtkALBAAddScalarsFilter*> m_AddScalarsList ; // filters which add color
  std::vector<int> m_PolydataIdsList ;                  // id's on polydata

  // current property settings
  int m_ThetaRes ;
  int m_PhiRes ;
  double m_Radius ;

  vtkPolyData *m_Input ;
  vtkPolyData *m_Output ;


private:
  vtkALBALandmarkCloudPolydataFilter( vtkALBALandmarkCloudPolydataFilter&);  // Not implemented.
  void operator=( vtkALBALandmarkCloudPolydataFilter&);  // Not implemented.
};

#endif
