/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeMeshSlice
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeMeshSlice_H__B
#define __albaPipeMeshSlice_H__B

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeSlice.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mmaMaterial;
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class albaAxes;
class vtkLookupTable;
class albaGUIMaterialButton;
class vtkALBAMeshCutter;
class vtkPlane;
class vtkPolyDataNormals;
class albaGUILutSwatch;

//----------------------------------------------------------------------------
// albaPipeMeshSlice :
//----------------------------------------------------------------------------

// TODO: REFACTOR THIS (Note by Stefano) 
// This class need some cleanup: there are really too many Update methods in cpp file...
/**
  class name : albaPipeMeshSlice
  Pipe for sliceing a mesh, modified by University of Bedfordshire.
*/
class ALBA_EXPORT albaPipeMeshSlice : public albaPipeSlice
{
public:
  /** RTTI macro*/
	albaTypeMacro(albaPipeMeshSlice,albaPipeSlice);

  /** constructor */
	albaPipeMeshSlice();
  /** destructor */
	virtual     ~albaPipeMeshSlice();

	/** process events coming from gui */
	virtual void OnEvent(albaEventBase *alba_event);

  /** creation of the pipe */
	virtual void Create(albaSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  /** called when a vme is selected by user */
	virtual void Select(bool select); 

	/** IDs for the GUI */
	enum PIPE_MESH_WIDGET_ID
	{
		ID_WIREFRAME = Superclass::ID_LAST,
    ID_WIRED_ACTOR_VISIBILITY,
    ID_BORDER_CHANGE,
    ID_SCALARS,
    ID_LUT,
    ID_SCALAR_MAP_ACTIVE,
    ID_USE_VTK_PROPERTY,
    ID_LAST,  
	};

  /** scalars type based on topology */
  enum PIPE_MESH_TYPE_SCALARS
  {
    POINT_TYPE = 0,
    CELL_TYPE,
  };

  
  
  /** Get assembly front */
  virtual vtkALBAAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  /** Get assembly back */
  virtual vtkALBAAssembly *GetAssemblyBack(){return m_AssemblyBack;};

	
  /** Core of the pipe */
  virtual void ExecutePipe();
  
  /** Add/RemoveTo Assembly Front */
  virtual void AddActorsToAssembly(vtkALBAAssembly *assembly);
  /** Add/RemoveTo Assembly back */
  virtual void RemoveActorsFromAssembly(vtkALBAAssembly *assembly);
  
  /** Set the actor picking*/
	void SetActorPicking(int enable = true);

  /** Set the actor wireframe to on*/
  void SetWireframeOn();
  /** Set the actor wireframe to off*/
  void SetWireframeOff();

  /** Set the actor border visible */
  void SetWiredActorVisibilityOn();
  /** Set the actor border not visible */
  void SetWiredActorVisibilityOff();

  /** Set the flip of normal filter to on*/
  void SetFlipNormalOn();
  /** Set the flip of normal filter to off*/
  void SetFlipNormalOff();

  /** Set Active Scalar */
  void SetActiveScalar(int index){m_ScalarIndex = index;};
  /** Get Active Scalar */
  int GetScalarIndex(){return m_ScalarIndex;};

  /** Get Number of Scalars */
  int GetNumberOfArrays(){return m_NumberOfArrays;};

  /** Set scalar map active, so you can see scalar associated to points or cells*/
  void SetScalarMapActive(int value){m_ScalarMapActive = value;};
  /** Set VTK Property to visualize the material of vme*/
  void SetUseVTKProperty(int value){m_UseVTKProperty = value;};
  
  /**Return the thickness of the border*/	
  double GetThickness();

  /**Set the thickness value*/
  void SetThickness(double thickness);
  
  /** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
  /*virtual*/ void SetSlice(double* Origin, double* Normal);  

 /** Set the lookup table */
	void SetLookupTable(vtkLookupTable *table);
  
  /** Gets the lookup table*/
	vtkLookupTable *GetLookupTable(){return m_Table;};

protected:
	mmaMaterial             *m_MeshMaterial;
	vtkPolyDataMapper        *m_Mapper;
  vtkPolyDataMapper        *m_MapperWired;
	vtkActor                *m_Actor;
	vtkOutlineCornerFilter  *m_OutlineBox;
	vtkPolyDataMapper       *m_OutlineMapper;
	vtkProperty             *m_OutlineProperty;
	vtkActor                *m_OutlineActor;
	albaAxes                 *m_Axes;
  vtkLookupTable          *m_Table;

  vtkActor                *m_ActorWired;
  
  vtkPlane				        *m_Plane;
  vtkALBAMeshCutter		    *m_Cutter;
  vtkPolyDataNormals *m_NormalFilter;

	albaGUILutSwatch *m_LutSwatch;
  
  /** create controls checking the field arrays of the data */
  void CreateFieldDataControlArrays();
  /** Update Properties of actor */
	void UpdateProperty(bool fromTag = false);
	/**Update data value to selected scalar */
  void UpdateScalars();

  /** Update the normal filter with changed scalar*/
  void UpdateVtkPolyDataNormalFilterActiveScalar();
  /** Update the visualization with changed scalar*/
  void UpdateLUTAndMapperFromNewActiveScalars();

  wxString                *m_ScalarsName;
  wxString                *m_ScalarsVTKName;

  albaGUIMaterialButton       *m_MaterialButton;

  int                      m_PointCellArraySeparation;
  int                      m_ScalarIndex;
  int                      m_NumberOfArrays;
  int                      m_ActiveScalarType;
  int                      m_Wireframe;
  int                      m_BorderElementsWiredActor;
  int                      m_ScalarMapActive;
  int                      m_UseVTKProperty;
  double				           m_Border;

  int m_RenderingDisplayListFlag; 


  /** allow the creation of the gui */
	virtual albaGUI  *CreateGui();
};  
#endif // __albaPipeMeshSlice_H__B
