/*=========================================================================
Program:   MAF2Medical
Module:    $RCSfile: medVMEStentDeploymentVisualPipe.h,v $
Language:  C++
Date:      $Date: 2012-10-23 10:15:31 $
Version:   $Revision: 1.1.2.7 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2013
University of Bedfordshire UK
=========================================================================*/

#ifndef __medVMEStentDeploymentVisualPipe_H__
#define __medVMEStentDeploymentVisualPipe_H__

#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkDepthSortPolyData.h"
#include "vtkTubeFilter.h"



//------------------------------------------------------------------------------
/// medVMEStentDeploymentVisualPipe. \n
/// Helper class for medVMEStent. \n
/// Visual pipe for stent, vessel, catheter and center line
//------------------------------------------------------------------------------
class medVMEStentDeploymentVisualPipe
{
public:
  /// constructor
  medVMEStentDeploymentVisualPipe(vtkRenderer *renderer) ;

  /// destructor
  ~medVMEStentDeploymentVisualPipe() ;

  /// Get visibility
  int GetVisibility() const {return m_Visibility ;}

  /// Set visibility
  void SetVisibility(int visibility) ;

  /// Set the catheter
  void SetCatheter(vtkPolyData *catheter) ;

  /// Show catheter
  void ShowCatheter(int visibility) ;

  /// Set the center line
  void SetCenterLine(vtkPolyData *centerline) ;

  /// Show center line
  void ShowCenterLine(int visibility) ;

  /// Set the vessel
  void SetVessel(vtkPolyData *vessel) ;

  /// Show vessel
  void ShowVessel(int visibility) ;

  /// Set the stent
  void SetStent(vtkPolyData *stent) ;

  /// Show stent
  void ShowStent(int visibility) ;

  /// Set stent tube Radius
  void SetStentTubeRadius(double rad) ;

  /// Set vessel wireframe on
  void SetWireframeOn() ;

  /// Set vessel wireframe off
  void SetWireframeOff() ;

  /// Reset the camera position. \n
  /// Don't forget to call renderer->ResetCamera() before the first call.
  void ResetCameraPosition() ;

  /// Reset the camera focal point
  void ResetCameraFocalPoint() ;

  /// Update and render
  void Render() ;

private:

  // Update the visibility of the components
  void UpdateVisibility() ;

  /// Get the highest index of the valid points, \n
  /// ie points which are members of cells. \n
  /// Needed because the stent contains unused and undefined points.
  int GetHighestValidPointIndex(vtkPolyData *pd) const ;

  //----------------------------------------------------------------------------
  // Member variables
  //----------------------------------------------------------------------------

  // renderer
  vtkRenderer *m_Renderer ;
  int m_Visibility ;         
  bool m_FirstCall ;

  // catheter pipeline
  int m_DefinedCatheter ;
  int m_VisibilityCatheter ;
  vtkPolyData *m_CatheterPolydata ;
  vtkDepthSortPolyData *m_CatheterDepthSort ;
  vtkPolyDataMapper *m_CatheterMapper ;
  vtkActor *m_CatheterActor ;

  // center line pipeline
  int m_DefinedCenterLine ;
  int m_VisibilityCenterLine ;
  vtkPolyData *m_CenterLinePolydata ;
  vtkPolyDataMapper *m_CenterLineMapper ;
  vtkActor *m_CenterLineActor ;

  // vessel pipeline
  int m_DefinedVessel ;
  int m_VisibilityVessel ;
  vtkPolyData *m_VesselPolydata ;
  vtkDepthSortPolyData *m_VesselDepthSort ;
  vtkPolyDataMapper *m_VesselMapper ;
  vtkActor *m_VesselActor ;
  double m_VesselOpacity ;

  // stent pipeline
  int m_DefinedStent ;
  int m_VisibilityStent ;
  double m_StentTubeRadius ;
  vtkPolyData *m_StentPolydata ;
  vtkTubeFilter *m_StentTubeFilter ;
  vtkPolyDataMapper *m_StentMapper ;
  vtkActor *m_StentActor ;

  // saved position of stent (or other principal item) in view
  double m_CenterOld[3] ;
} ;



#endif