/*=========================================================================

 Program: MAF2
 Module: mafPipeSurfaceTextured
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class vtkActor;

//----------------------------------------------------------------------------
// mafPipeSurfaceTextured :
//----------------------------------------------------------------------------
/** Visual pipe used to render VTK polydata and allowing to manage scalar visibility,
lookup table and textures applied to the polydata.*/
class MAF_EXPORT mafPipeSurfaceTextured : public mafPipe
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
  static bool ImageAccept(mafVME*node) {return(node != NULL && node->IsMAFType(mafVMEImage));};

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

	/** Get assembly front/back */
	virtual vtkMAFAssembly *GetAssemblyFront(){return m_AssemblyFront;};
	virtual vtkMAFAssembly *GetAssemblyBack(){return m_AssemblyBack;};

	void SetScalarVisibilityOn(){m_ScalarVisibility = TRUE;};
	void SetScalarVisibilityOff(){m_ScalarVisibility = FALSE;};

	void SetUseVtkPropertyOn(){m_UseVTKProperty = TRUE;};
	void SetUseVtkPropertyOff(){m_UseVTKProperty = FALSE;};

protected:
  vtkTexture              *m_Texture;
  vtkPolyDataMapper	      *m_Mapper;
  mafLODActor             *m_Actor;
  mafLODActor             *m_OutlineActor;
  mafAxes                 *m_Axes;
  vtkActor               *m_GhostActor;

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
