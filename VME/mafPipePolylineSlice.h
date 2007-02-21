/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePolylineSlice.h,v $
  Language:  C++
  Date:      $Date: 2007-02-21 17:22:47 $
  Version:   $Revision: 1.4 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipePolylineSlice_H__
#define __mafPipePolylineSlice_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class mafAxes;
class vtkFixedCutter;
class vtkPlane;
class vtkTubeFilter;

//----------------------------------------------------------------------------
// mafPipePolylineSlice :
//----------------------------------------------------------------------------
class mafPipePolylineSlice : public mafPipe
{
public:
  mafTypeMacro(mafPipePolylineSlice,mafPipe);

               mafPipePolylineSlice();
  virtual     ~mafPipePolylineSlice ();

  /**Return the thickness of the border*/	
  double GetThickness();

  /**Set the thickness value*/
  void SetThickness(double thickness); 

  /**Return the radius of the tube*/	
  double GetRadius();

  /**Set the radius value of the tube*/
  void SetRadius(double radius); 

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

  void SetSlice(double *Origin);

	void SetNormal(double *Normal);

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_RENDERING_DISPLAY_LIST,
    ID_CHOOSE_TEXTURE,
    ID_TEXTURE_MAPPING_MODE,
    ID_BORDER_CHANGE,
    ID_LAST,
  };

  virtual mmgGui  *CreateGui();
protected:
  vtkPolyDataMapper	      *m_Mapper;
  vtkActor                *m_Actor;
	vtkTubeFilter           *m_Tube;
	vtkTubeFilter           *m_TubeRadial;
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
  vtkActor                *m_OutlineActor;
  mafAxes                 *m_Axes;
  vtkPlane				        *m_Plane;
  vtkFixedCutter		      *m_Cutter;
  double				           m_Border;
  double                   m_Radius;

  double	m_Origin[3];
  double	m_Normal[3];

  int m_ScalarVisibility;
  int m_RenderingDisplayListFlag;
};  
#endif // __mafPipePolylineSlice_H__
