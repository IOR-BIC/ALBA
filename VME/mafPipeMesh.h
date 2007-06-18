/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeMesh.h,v $
Language:  C++
Date:      $Date: 2007-06-18 13:08:18 $
Version:   $Revision: 1.3 $
Authors:   Daniele Giunchi
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
class mmaMaterial;
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
	enum PIPE_MESH_WIDGET_ID
	{
		ID_LAST = Superclass::ID_LAST,
    ID_WIREFRAME,
    ID_SCALARS,
    ID_LUT,
	};

  enum PIPE_MESH_TYPE_SCALARS
  {
    POINT_TYPE = 0,
    CELL_TYPE,
  };

  
  
  /** Get assembly front/back */
  virtual vtkMAFAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  virtual vtkMAFAssembly *GetAssemblyBack(){return m_AssemblyBack;};

	
  /** Core of the pipe */
  virtual void ExecutePipe();
  
  /** Add/RemoveTo Assembly Front/back */
  virtual void AddActorsToAssembly(vtkMAFAssembly *assembly);
  virtual void RemoveActorsFromAssembly(vtkMAFAssembly *assembly);
  
  /** Set the actor picking*/
	void SetActorPicking(int enable = true);

  /** Set the actor wireframe*/
  void SetWireframeOn();
  void SetWireframeOff();

  /** Set/Get Active Scalar */
  void SetActiveScalar(int index){m_ScalarIndex = index;};
  int GetScalarIndex(){return m_ScalarIndex;};

  /** Get Number of Scalars */
  int GetNumberOfArrays(){return m_NumberOfArrays;};

protected:
	mmaMaterial             *m_MeshMaterial;
	vtkGeometryFilter                  *m_GeometryFilter;
  mafParabolicMeshToLinearMeshFilter *m_LinearizationFilter;
	vtkDataSetMapper        *m_Mapper;
  vtkDataSetMapper        *m_MapperWired;
	vtkActor                *m_Actor;
  vtkActor                *m_ActorWired;
	vtkOutlineCornerFilter  *m_OutlineBox;
	vtkPolyDataMapper       *m_OutlineMapper;
	vtkProperty             *m_OutlineProperty;
	vtkActor                *m_OutlineActor;
	mafAxes                 *m_Axes;
  vtkLookupTable          *m_Table;

  void CreateFieldDataControlArrays();
	void UpdateProperty(bool fromTag = false);
  void UpdateScalars();

  wxString                *m_ScalarsName;
  wxString                *m_ScalarsVTKName;

  int                      m_PointCellArraySeparation;
  int                      m_ScalarIndex;
  int                      m_NumberOfArrays;
  int                      m_ActiveScalarType;
  int                      m_Wireframe;

	virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeMesh_H__
