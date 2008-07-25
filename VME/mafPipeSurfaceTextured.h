/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurfaceTextured.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:05:59 $
  Version:   $Revision: 1.7 $
  Authors:   Silvano Imboden - Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeSurfaceTextured_H__
#define __mafPipeSurfaceTextured_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"
#include "mafVMEImage.h"
#include "mafAxes.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkTexture;
class vtkPolyDataMapper;
class vtkPolyData;
class mafLODActor;
class mafGUIMaterialButton;
class mmaMaterial;

//----------------------------------------------------------------------------
// mafPipeSurfaceTextured :
//----------------------------------------------------------------------------
/** Visual pipe used to render VTK polydata and allowing to manage scalar visibility,
lookup table and textures applied to the polydata.*/
class mafPipeSurfaceTextured : public mafPipe
{
public:
  mafTypeMacro(mafPipeSurfaceTextured,mafPipe);

               mafPipeSurfaceTextured();
  virtual     ~mafPipeSurfaceTextured();

  /** process events coming from Gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  /** Let to enable/disable the Level Of Detail behavior.*/
  void SetEnableActorLOD(bool value);

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_RENDERING_DISPLAY_LIST,
    ID_CHOOSE_TEXTURE,
    ID_TEXTURE_MAPPING_MODE,
    ID_USE_VTK_PROPERTY,
    ID_USE_TEXTURE,
    ID_USE_LOOKUP_TABLE,
    ID_LUT,
    ID_ENABLE_LOD,
    ID_LAST
  };

  /** Callback used to choose a node type mafVMEImage to be used as a texture to apply on the polydata.*/
  static bool ImageAccept(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMEImage));};

  /** Set the actor picking*/
  void SetActorPicking(int enable = true);

  /** Turn On the axes visibility.*/
	void ShowAxisOn(){m_ShowAxis = 1;m_Axes->SetVisibility(m_Selected&&m_ShowAxis);};
  
  /** Turn Off the axes visibility.*/
	void ShowAxisOff(){m_ShowAxis = 0;m_Axes->SetVisibility(m_Selected&&m_ShowAxis);};

  /** Turn Off the selection visibility.*/
  void SelectionActorOff(){m_SelectionVisibility = FALSE;};

  /** Turn Off the selection visibility.*/
  void SelectionActorOn(){m_SelectionVisibility = TRUE;};

protected:
  vtkTexture              *m_Texture;
  vtkPolyDataMapper	      *m_Mapper;
  mafLODActor             *m_Actor;
  mafLODActor             *m_OutlineActor;
  mafAxes                 *m_Axes;

  int m_UseVTKProperty;
  int m_UseTexture;
  int m_UseLookupTable;
  int m_EnableActorLOD;

	int m_ShowAxis;

  int m_ScalarVisibility;
  int m_SelectionVisibility;
  int m_RenderingDisplayListFlag;
  mmaMaterial *m_SurfaceMaterial;
  mafGUIMaterialButton *m_MaterialButton;

  void UpdateProperty(bool fromTag = false);

  /** Generate texture coordinate for polydata according to the mapping mode*/
  void GenerateTextureMapCoordinate();

  /** Create the Gui of the visual pipe used to change visual parameters.*/
  virtual mafGUI  *CreateGui();
};  
#endif // __mafPipeSurfaceTextured_H__
