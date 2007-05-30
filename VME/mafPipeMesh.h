/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeMesh.h,v $
Language:  C++
Date:      $Date: 2007-05-30 11:58:42 $
Version:   $Revision: 1.2 $
Authors:   Daniele Giunchi - Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeMesh_H__
#define __mafPipeMesh_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkDataSetMapper;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class vtkGeometryFilter;
class mafAxes;
class mafParabolicMeshToLinearMeshFilter;
class vtkLookupTable;

//----------------------------------------------------------------------------
// mafPipeMesh :
//----------------------------------------------------------------------------
class mafPipeMesh : public mafPipe
{
public:
	mafTypeMacro(mafPipeMesh,mafPipe);

	mafPipeMesh();
	virtual     ~mafPipeMesh();

	/** process events coming from gui */
	virtual void OnEvent(mafEventBase *maf_event);

	virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
	virtual void Select(bool select); 

	/** IDs for the GUI */
	enum PIPE_SURFACE_WIDGET_ID
	{
		ID_LAST = Superclass::ID_LAST,
    ID_WIREFRAME,
    ID_SCALARS,
    ID_LUT,
	};

  enum PIPE_TYPE_SCALARS
  {
    POINT_TYPE = 0,
    CELL_TYPE,
  };

	/** Set the actor picking*/
	void SetActorPicking(int enable = true);

  /** Set the actor wireframe*/
  void SetWireframeOn();
  void SetWireframeOff();

protected:
	vtkGeometryFilter                  *m_GeometryFilter;
  mafParabolicMeshToLinearMeshFilter *m_LinearizationFilter;
	vtkPolyDataMapper        *m_Mapper;
  vtkPolyDataMapper	      *m_MapperWired;
	vtkActor                *m_Actor;
  vtkActor                *m_ActorWired;
	vtkOutlineCornerFilter  *m_OutlineBox;
	vtkPolyDataMapper       *m_OutlineMapper;
	vtkProperty             *m_OutlineProperty;
	vtkActor                *m_OutlineActor;
	mafAxes                 *m_Axes;
  vtkLookupTable          *m_Table;

	void UpdateProperty(bool fromTag = false);
  void UpdateScalarsPoints();

  wxString                *m_ScalarsPointsName;
  int                      m_ScalarPoints;
  int                      m_ActiveScalarType;
  int                      m_Wireframe;

	virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeMesh_H__
