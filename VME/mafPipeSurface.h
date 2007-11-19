/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurface.h,v $
  Language:  C++
  Date:      $Date: 2007-11-19 11:57:17 $
  Version:   $Revision: 1.24 $
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
class vtkActor;
class vtkGlyph3D;
class vtkPolyDataNormals;
class vtkLineSource;
class vtkCellCenters;
class vtkArrowSource;
class vtkFeatureEdges;

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
		ID_NORMAL_VISIBILITY,
		ID_EDGE_VISIBILITY,
    ID_LAST
  };

  /** Set the actor picking*/
  void SetActorPicking(int enable = true);

protected:
  vtkPolyDataMapper	      *m_Mapper;
	vtkLineSource						*m_Arrow;
	vtkPolyDataNormals			*m_Normal;
	vtkGlyph3D							*m_NormalGlyph;
	vtkPolyDataMapper				*m_NormalMapper;
	vtkActor								*m_NormalActor;
	vtkCellCenters					*m_CenterPointsFilter;
	vtkArrowSource					*m_NormalArrow;
  vtkActor             *m_Actor;
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
  mafLODActor             *m_OutlineActor;
  mafAxes                 *m_Axes;
	vtkFeatureEdges					*m_ExtractEdges;
	vtkPolyDataMapper				*m_EdgesMapper;
	vtkActor								*m_EdgesActor;

  int m_UseVTKProperty;
  int m_UseLookupTable;
  int m_EnableActorLOD;

  int m_ScalarVisibility;
	int m_NormalVisibility;
	int m_EdgeVisibility;
  int m_RenderingDisplayListFlag;
  mmaMaterial *m_SurfaceMaterial;
  mmgMaterialButton *m_MaterialButton;

  void UpdateProperty(bool fromTag = false);

	void CreateEdgesPipe();
	void CreateNormalsPipe();

  virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeSurface_H__
