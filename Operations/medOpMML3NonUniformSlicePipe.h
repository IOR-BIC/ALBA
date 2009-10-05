/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMML3NonUniformSlicePipe.h,v $
Language:  C++
Date:      $Date: 2009-10-05 16:41:58 $
Version:   $Revision: 1.1.2.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpMML3NonUniformSlicePipe_H__
#define __medOpMML3NonUniformSlicePipe_H__

#include "mafDefines.h"

#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkPolyData.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"

#include <vector>




//------------------------------------------------------------------------------
/// medOpMML3NonUniformSlicePipe. \n
/// Helper class for medOpMML3. \n
/// Visual pipe for selecting distribution of non uniform slices
//------------------------------------------------------------------------------
class medOpMML3NonUniformSlicePipe
{
public:
  /// constructor
  medOpMML3NonUniformSlicePipe(vtkPolyData *surface, vtkRenderer *renderer, int numberOfSections) ;

  ~medOpMML3NonUniformSlicePipe() ; ///< destructor

  /// set coords of ends of axis
  void SetEndsOfAxis(double *x0, double *x1) ;

  /// set positions of slices
  void SetSlicePositions(int numberOfSlices, double *alpha) ;

  /// update the pipeline
  void Update() ;

private:
  /// calculate axis unit vector
  void CalculateAxisVector(double *u) ;

  /// calculate axis mid-point
  void CalculateAxisMidPoint(double *midPoint) ;

  /// calculate axis length
  double CalculateAxisLength() ;

  /// calculate unit vectors normal to axis
  void CalculateAxisNormals(double *v,  double *w) ;

  /// update transform
  void UpdateTransform() ;

  /// update spheres
  void UpdateSpheres() ;

  /// update slice lines
  void UpdateSliceLines() ;

  double m_AxisStart[3], m_AxisEnd[3] ;

  vtkRenderer *m_Renderer ;
  vtkTransform *m_Transform ;
  vtkActor2D *m_ContourActor ;

  vtkSphereSource *m_Sphere[2] ;
  vtkActor2D *m_SphereActor[2] ;

  int m_NumberOfSections ;  // number of sections
  vtkActor2D *m_SectionLinesActor ; // fixed section lines

  int m_NumberOfSlices ;  // number of slices
  std::vector<double> m_Alpha ;       // alpha positions of slices
  vtkPolyData *m_SliceLinesPolydata ;
  vtkActor2D *m_SliceLinesActor ;   // lines showing slice positions


} ;



#endif