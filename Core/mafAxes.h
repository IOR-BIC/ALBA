/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAxes.h,v $
  Language:  C++
  Date:      $Date: 2009-02-17 15:28:15 $
  Version:   $Revision: 1.1.22.1 $
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

@sa mafAxesTest for example usage

@todo API improvements needed, see mafAxesTest for api improvement use cases
*/
{
public:

  /** Constructor note: given renderer ren cannot be NULL */
           mafAxes(vtkRenderer *ren, mafVME* vme = NULL);
  virtual ~mafAxes();
	
	/** Set the axes visibility. */
  void     SetVisibility(bool show);

  /** Force the position of the Axes, if omitted the ABS vme matrix is used instead. 
  Works only if a vme was specified in the constructor*/
  void     SetPose( vtkMatrix4x4 *abs_pose_matrix = NULL);

protected:
  mafVME      *m_Vme;
  vtkRenderer *m_Renderer;
	
	vtkAxes                *m_Axes;
	vtkPolyDataMapper2D		 *m_AxesMapper;
	vtkCoordinate 				 *m_Coord;
	vtkActor2D						 *m_AxesActor;
	vtkLookupTable				 *m_AxesLUT;
  
  /** friend test class */
  friend class mafAxesTest;
};
#endif
