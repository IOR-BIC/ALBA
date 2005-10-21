/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurface.h,v $
  Language:  C++
  Date:      $Date: 2005-10-21 13:36:33 $
  Version:   $Revision: 1.9 $
  Authors:   Silvano Imboden - Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeSurface_H_
#define _mafPipeSurface_H_

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkTexture;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class mafAxes;
class mmgMaterialButton;
class vtkCleanPolyData;
class vtkPolyDataNormals;
class vtkTriangleFilter;
class vtkStripper;

//----------------------------------------------------------------------------
// mafPipeSurface :
//----------------------------------------------------------------------------
class mafPipeSurface : public mafPipe
{
public:
  mafTypeMacro(mafPipeSurface,mafPipe);

               mafPipeSurface();
  virtual     ~mafPipeSurface ();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_OPTIMIZE_SURFACE,
    ID_RENDERING_DISPLAY_LIST,
    ID_LAST
  };

protected:
  vtkCleanPolyData        *m_CleanPolydata;
  vtkPolyDataNormals      *m_NormalFilter;
  vtkTriangleFilter       *m_TriangleFilter;
  vtkStripper             *m_Stripper;
  vtkTexture              *m_Texture;
  vtkPolyDataMapper	      *m_Mapper;
  vtkActor                *m_Actor;
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
  vtkActor                *m_OutlineActor;
  mafAxes                 *m_Axes;

  int m_ScalarVisibility;
  int m_OptimizedSurfaceFlag;
  int m_RenderingDisplayListFlag;
  mmgMaterialButton *m_MaterialButton;

//@@@  bool                    m_use_axes; //SIL. 22-5-2003 added line - 
  void UpdateProperty(bool fromTag = false);

  /** Optimize polydata for rendering. If vme is animated on data, 
  will be asked to optimize for all time stamps. */
  void OptimizeSurface(bool optimize = true);

  virtual mmgGui  *CreateGui();
};  
#endif // _mafPipeSurface_H_
