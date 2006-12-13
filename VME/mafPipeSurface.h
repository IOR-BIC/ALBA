/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurface.h,v $
  Language:  C++
  Date:      $Date: 2006-12-13 14:20:29 $
  Version:   $Revision: 1.19 $
  Authors:   Silvano Imboden - Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeSurface_H__
#define __mafPipeSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"
#include "mmgVMEChooserAccept.h"
#include "mafVMEImage.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkTexture;
class vtkPolyDataMapper;
class vtkPolyData;
class mafLODActor;
class vtkProperty;
class mafAxes;
class mmgMaterialButton;
class mmaMaterial;
//class vtkActor;

//----------------------------------------------------------------------------
// mafPipeSurface :
//----------------------------------------------------------------------------
class mafPipeSurface : public mafPipe
{
public:
  mafTypeMacro(mafPipeSurface,mafPipe);

               mafPipeSurface();
  virtual     ~mafPipeSurface();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

  void SetEnableActorLOD(bool value);

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_RENDERING_DISPLAY_LIST,
    ID_USE_VTK_PROPERTY,
    ID_USE_LOOKUP_TABLE,
    ID_LUT,
    ID_ENABLE_LOD,
    ID_LAST
  };

  /** Set the actor picking*/
  void SetActorPicking(int enable = true);

protected:
  vtkPolyDataMapper	      *m_Mapper;
  mafLODActor             *m_Actor;
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
  mafLODActor             *m_OutlineActor;
  mafAxes                 *m_Axes;

  int m_UseVTKProperty;
  int m_UseLookupTable;
  int m_EnableActorLOD;

  int m_ScalarVisibility;
  int m_RenderingDisplayListFlag;
  mmaMaterial *m_SurfaceMaterial;
  mmgMaterialButton *m_MaterialButton;

  void UpdateProperty(bool fromTag = false);

  virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeSurface_H__
