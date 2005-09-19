/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAxes.h,v $
  Language:  C++
  Date:      $Date: 2005-09-19 13:40:05 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafAxes_h
#define __mafAxes_h

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class vtkRenderer;
class vtkAxes;
class vtkPolyDataMapper2D;
class vtkCoordinate;
class vtkActor2D;
class vtkLookupTable;
class vtkMatrix4x4;
//----------------------------------------------------------------------------
class mafAxes
//----------------------------------------------------------------------------
/**
create a vtkAxes and insert it in the passed renderer
as a vtkActor2D

If a vme is provided, the axes will represent 
the local vme-reference system.

If vme is NULL, the axes will represent the global
reference system
*/
{
public:
           mafAxes(vtkRenderer *ren, mafVME* vme = NULL);
  virtual ~mafAxes();
	
	/** Set the axes visibility. */
  void     SetVisibility(bool show);

  /** force the position of the Axes, if omitted the ABS vme matrix is used instead. Work only if vme was specified */
  void     SetPose( vtkMatrix4x4 *abs_pose_matrix = NULL);

protected:
  mafVME      *m_Vme;
  vtkRenderer *m_Renderer;
	
	vtkAxes                *m_Axes;
	vtkPolyDataMapper2D		 *m_AxesMapper;
	vtkCoordinate 				 *m_Coord;
	vtkActor2D						 *m_AxesActor;
	vtkLookupTable				 *m_AxesLUT;
};
#endif
