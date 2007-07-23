/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeMeshSlice.h,v $
  Language:  C++
  Date:      $Date: 2007-07-23 14:21:29 $
  Version:   $Revision: 1.2 $
  Authors:   Matteo Giacomoni - Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
	SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#ifndef __mafPipeMeshSlice_H__
#define __mafPipeMeshSlice_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class mafAxes;
class vtkMAFMeshCutter;
class vtkPlane;
class vtkDelaunay2D;

//----------------------------------------------------------------------------
// mafPipeMeshSlice :
//----------------------------------------------------------------------------
class mafPipeMeshSlice : public mafPipe
{
public:
  mafTypeMacro(mafPipeMeshSlice,mafPipe);

               mafPipeMeshSlice();
  virtual     ~mafPipeMeshSlice ();

  /**Return the thickness of the border*/	
  double GetThickness();

  /**Set the thickness value*/
  void SetThickness(double thickness); 

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

  /** Set the origin of the slice*/
  void SetSlice(double *Origin);

  /** Set the normal of the slice*/
	void SetNormal(double *Normal);

	void ShowBoxSelectionOn(){m_ShowSelection=true;};
	void ShowBoxSelectionOff(){m_ShowSelection=false;};

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {						
		ID_BORDER_CHANGE = Superclass::ID_LAST,
    ID_LAST,
  };

  virtual mmgGui  *CreateGui();
protected:
  vtkPolyDataMapper	      *m_Mapper;
  vtkActor                *m_Actor;
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
  vtkActor                *m_OutlineActor;
  mafAxes                 *m_Axes;
  vtkPlane				        *m_Plane;
  vtkMAFMeshCutter		    *m_Cutter;

  double				           m_Border;

	bool	m_ShowSelection;

  double	m_Origin[3];
  double	m_Normal[3];

  int m_ScalarVisibility;
  int m_RenderingDisplayListFlag;

	vtkDelaunay2D *m_Filter;

};  
#endif // __mafPipeMeshSlice_H__
