/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeMesh.h,v $
Language:  C++
Date:      $Date: 2008-02-01 13:32:03 $
Version:   $Revision: 1.6 $
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
class vtkDataSetMapper;
class vtkActor;
class vtkGeometryFilter;
class mafAxes;
class mafParabolicMeshToLinearMeshFilter;
class vtkLookupTable;
class mmgMaterialButton;

//----------------------------------------------------------------------------
// mafPipeMesh :
//----------------------------------------------------------------------------
class mafPipeMesh : public mafPipe
{
public:
	mafTypeMacro(mafPipeMesh,mafPipe);

	mafPipeMesh();
	virtual     ~mafPipeMesh();

	/** process events coming from Gui */
	virtual void OnEvent(mafEventBase *maf_event);

  /** Create the VTK rendering pipeline*/
	virtual void Create(mafSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
	virtual void Select(bool select); 

	/** IDs for the GUI */
	enum PIPE_MESH_WIDGET_ID
	{
		ID_LAST = Superclass::ID_LAST,
    ID_WIREFRAME,
    ID_WIRED_ACTOR_VISIBILITY,
    ID_SCALARS,
    ID_LUT,
    ID_SCALAR_MAP_ACTIVE,
    ID_USE_VTK_PROPERTY,
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

  /** Set the actor wire frame*/
  void SetWireframeOn();
  void SetWireframeOff();
  
  /** Set the actor border visible or not*/
  void SetWiredActorVisibilityOn();
  void SetWiredActorVisibilityOff();

  /** Set/Get Active Scalar */
  void SetActiveScalar(int index){m_ScalarIndex = index;};
  int GetScalarIndex(){return m_ScalarIndex;};

  /** Get Number of Scalars */
  int GetNumberOfArrays(){return m_NumberOfArrays;};

  /** Set scalar map active, so you can see scalar associated to points or cells*/
  void SetScalarMapActive(int value){m_ScalarMapActive = value;};
  /** Set VTK Property to visualize the material of vme*/
  void SetUseVTKProperty(int value){m_UseVTKProperty = value;};

protected:
	mmaMaterial             *m_MeshMaterial;
	vtkGeometryFilter                  *m_GeometryFilter;
  mafParabolicMeshToLinearMeshFilter *m_LinearizationFilter;
	vtkDataSetMapper        *m_Mapper;
  vtkDataSetMapper        *m_MapperWired;
	vtkActor                *m_Actor;
  vtkActor                *m_ActorWired;
	vtkActor                *m_OutlineActor;
	mafAxes                 *m_Axes;
  vtkLookupTable          *m_Table;

  void CreateFieldDataControlArrays();
	void UpdateProperty(bool fromTag = false);
	/**Update data value to selected scalar */
  void UpdateScalars();
  /** Update the visualization with changed scalar*/
  void UpdatePipeFromScalars();

  wxString                *m_ScalarsName;
  wxString                *m_ScalarsVTKName;

  mmgMaterialButton       *m_MaterialButton;

  int                      m_PointCellArraySeparation;
  int                      m_ScalarIndex;
  int                      m_NumberOfArrays;
  int                      m_ActiveScalarType;
  int                      m_Wireframe;
  int                      m_BorderElementsWiredActor;
  int                      m_ScalarMapActive;
  int                      m_UseVTKProperty;

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeMesh_H__
