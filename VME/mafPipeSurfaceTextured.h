/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurfaceTextured.h,v $
  Language:  C++
  Date:      $Date: 2007-06-15 14:17:29 $
  Version:   $Revision: 1.3 $
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
class vtkOutlineCornerFilter;
class vtkTexture;
class vtkPolyDataMapper;
class vtkPolyData;
class mafLODActor;
class vtkProperty;
class mmgMaterialButton;
class mmaMaterial;
//class vtkActor;

//----------------------------------------------------------------------------
// mafPipeSurfaceTextured :
//----------------------------------------------------------------------------
class mafPipeSurfaceTextured : public mafPipe
{
public:
  mafTypeMacro(mafPipeSurfaceTextured,mafPipe);

               mafPipeSurfaceTextured();
  virtual     ~mafPipeSurfaceTextured ();

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
    ID_CHOOSE_TEXTURE,
    ID_TEXTURE_MAPPING_MODE,
    ID_USE_VTK_PROPERTY,
    ID_USE_TEXTURE,
    ID_USE_LOOKUP_TABLE,
    ID_LUT,
    ID_ENABLE_LOD,
    ID_LAST
  };

  static bool ImageAccept(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMEImage));};

  /** Set the actor picking*/
  void SetActorPicking(int enable = true);

	void ShowAxisOn(){m_ShowAxis = 1;m_Axes->SetVisibility(m_Selected&&m_ShowAxis);};
	void ShowAxisOff(){m_ShowAxis = 0;m_Axes->SetVisibility(m_Selected&&m_ShowAxis);};

protected:
  vtkTexture              *m_Texture;
  vtkPolyDataMapper	      *m_Mapper;
  mafLODActor             *m_Actor;
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
  mafLODActor             *m_OutlineActor;
  mafAxes                 *m_Axes;

  int m_UseVTKProperty;
  int m_UseTexture;
  int m_UseLookupTable;
  int m_EnableActorLOD;

	int m_ShowAxis;

  int m_ScalarVisibility;
  int m_RenderingDisplayListFlag;
  mmaMaterial *m_SurfaceMaterial;
  mmgMaterialButton *m_MaterialButton;

//@@@  bool                    m_use_axes; //SIL. 22-5-2003 added line - 
  void UpdateProperty(bool fromTag = false);

  /** 
  Generate texture coordinate for polydata according to the mapping mode*/
  void GenerateTextureMapCoordinate();

  virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeSurfaceTextured_H__

