/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurfaceTextured
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeSurfaceTextured_H__
#define __albaPipeSurfaceTextured_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaPipe.h"
#include "albaVMEImage.h"
#include "albaAxes.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkTexture;
class vtkPolyDataMapper;
class vtkPolyData;
class albaLODActor;
class albaGUIMaterialButton;
class mmaMaterial;
class vtkActor;

//----------------------------------------------------------------------------
// albaPipeSurfaceTextured :
//----------------------------------------------------------------------------
/** Visual pipe used to render VTK polydata and allowing to manage scalar visibility,
lookup table and textures applied to the polydata.*/
class ALBA_EXPORT albaPipeSurfaceTextured : public albaPipe
{
public:
  albaTypeMacro(albaPipeSurfaceTextured,albaPipe);

               albaPipeSurfaceTextured();
  virtual     ~albaPipeSurfaceTextured();

  /** process events coming from Gui */
  virtual void OnEvent(albaEventBase *alba_event);

  /** Create the VTK rendering pipeline*/
  virtual void Create(albaSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  /** Let to enable/disable the Level Of Detail behavior.*/
  void SetEnableActorLOD(bool value);

	/** Return Bounds */
	void GetBounds(double bounds[6]);

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_CHOOSE_TEXTURE,
    ID_TEXTURE_MAPPING_MODE,
    ID_USE_VTK_PROPERTY,
    ID_USE_TEXTURE,
    ID_USE_LOOKUP_TABLE,
    ID_LUT,
    ID_ENABLE_LOD,
    ID_LAST
  };

  /** Callback used to choose a node type albaVMEImage to be used as a texture to apply on the polydata.*/
  static bool ImageAccept(albaVME*node) {return(node != NULL && node->IsALBAType(albaVMEImage));};

  /** Set the actor picking*/
  void SetActorPicking(int enable = true);

  /** Turn On the axes visibility.*/
	void ShowAxisOn(){m_ShowAxis = 1;m_Axes->SetVisibility(m_Selected&&m_ShowAxis);};
  
  /** Turn Off the axes visibility.*/
	void ShowAxisOff(){m_ShowAxis = 0;m_Axes->SetVisibility(m_Selected&&m_ShowAxis);};

  /** Turn Off the selection visibility.*/
  void SelectionActorOff(){m_SelectionVisibility = false;};

  /** Turn Off the selection visibility.*/
  void SelectionActorOn(){m_SelectionVisibility = true;};

	/** Get assembly front/back */
	virtual vtkALBAAssembly *GetAssemblyFront(){return m_AssemblyFront;};
	virtual vtkALBAAssembly *GetAssemblyBack(){return m_AssemblyBack;};

	void SetScalarVisibilityOn(){m_ScalarVisibility = true;};
	void SetScalarVisibilityOff(){m_ScalarVisibility = false;};

	void SetUseVtkPropertyOn(){m_UseVTKProperty = true;};
	void SetUseVtkPropertyOff(){m_UseVTKProperty = false;};

protected:
  vtkTexture              *m_Texture;
  vtkPolyDataMapper	      *m_Mapper;
  albaLODActor             *m_Actor;
  albaLODActor             *m_OutlineActor;
  albaAxes                 *m_Axes;
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
  albaGUIMaterialButton *m_MaterialButton;

  void UpdateProperty(bool fromTag = false);

  /** Generate texture coordinate for polydata according to the mapping mode*/
  void GenerateTextureMapCoordinate();

  /** Create the Gui of the visual pipe used to change visual parameters.*/
  virtual albaGUI  *CreateGui();
};  
#endif // __albaPipeSurfaceTextured_H__
