/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurface.h,v $
  Language:  C++
  Date:      $Date: 2008-02-01 13:32:03 $
  Version:   $Revision: 1.27 $
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

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkTexture;
class vtkPolyDataMapper;
class vtkPolyData;
class mafLODActor;
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
/** Visual pipe used to render VTK polydata and allowing to manage scalar visibility,
lookup table and some polygonal features like edges and normals.*/
class mafPipeSurface : public mafPipe
{
public:
  mafTypeMacro(mafPipeSurface,mafPipe);

               mafPipeSurface();
  virtual     ~mafPipeSurface();

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
  vtkActor             *m_Actor; ///< Actor representing the polygonal surface

  mafLODActor             *m_OutlineActor;
  mafAxes                 *m_Axes;
	vtkFeatureEdges					*m_ExtractEdges;
	vtkPolyDataMapper				*m_EdgesMapper;
	vtkActor								*m_EdgesActor;

  int m_UseVTKProperty; ///< Flag to switch On/Off the VTK property usage to color the surface
  int m_UseLookupTable; ///< Flag to switch On/Off the lookup table usage to color the surface
  int m_EnableActorLOD; ///< Flag to switch On/Off the usage of the Level Of Detail

  int m_ScalarVisibility; ///< Flag to switch On/Off the scalar visibility
	int m_NormalVisibility; ///< Flag to switch On/Off the visibility of normals on the surface
	int m_EdgeVisibility; ///< Flag to switch On/Off the visibility of edge feature on the surface
  int m_RenderingDisplayListFlag; ///< Flag to switch On/Off the 
  mmaMaterial *m_SurfaceMaterial;
  mmgMaterialButton *m_MaterialButton;

  void UpdateProperty(bool fromTag = false);

	void CreateEdgesPipe();
	void CreateNormalsPipe();

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeSurface_H__
