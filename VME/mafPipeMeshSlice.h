/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeMeshSlice.h,v $
Language:  C++
Date:      $Date: 2008-07-25 07:05:59 $
Version:   $Revision: 1.9 $
Authors:   Daniele Giunchi , Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeMeshSlice_H__
#define __mafPipeMeshSlice_H__

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
class vtkPolyData;
class vtkActor;
class vtkProperty;
class mafAxes;
class vtkLookupTable;
class mafGUIMaterialButton;
class vtkMAFMeshCutter;
class vtkPlane;
class vtkPolyDataNormals;

//----------------------------------------------------------------------------
// mafPipeMeshSlice :
//----------------------------------------------------------------------------

// TODO: REFACTOR THIS (Note by Stefano) 
// This class need some cleanup: there are really too many Update methods in cpp file...

class mafPipeMeshSlice : public mafPipe
{
public:
	mafTypeMacro(mafPipeMeshSlice,mafPipe);

	mafPipeMeshSlice();
	virtual     ~mafPipeMeshSlice();

	/** process events coming from gui */
	virtual void OnEvent(mafEventBase *maf_event);

	virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
	virtual void Select(bool select); 

	/** IDs for the GUI */
	enum PIPE_MESH_WIDGET_ID
	{
		ID_LAST = Superclass::ID_LAST,
    ID_WIREFRAME,
    ID_WIRED_ACTOR_VISIBILITY,
    ID_BORDER_CHANGE,
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

  /** Set the actor wireframe*/
  void SetWireframeOn();
  void SetWireframeOff();

  /** Set the actor border visible or not*/
  void SetWiredActorVisibilityOn();
  void SetWiredActorVisibilityOff();

  /** Set the flip of normal filter*/
  void SetFlipNormalOn();
  void SetFlipNormalOff();

  /** Set/Get Active Scalar */
  void SetActiveScalar(int index){m_ScalarIndex = index;};
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
  
   /** Set the origin of the slice*/
  void SetSlice(double *Origin);

  /** Set the normal of the slice*/
	void SetNormal(double *Normal);

protected:
	mmaMaterial             *m_MeshMaterial;
	vtkPolyDataMapper        *m_Mapper;
  vtkPolyDataMapper        *m_MapperWired;
	vtkActor                *m_Actor;
	vtkOutlineCornerFilter  *m_OutlineBox;
	vtkPolyDataMapper       *m_OutlineMapper;
	vtkProperty             *m_OutlineProperty;
	vtkActor                *m_OutlineActor;
	mafAxes                 *m_Axes;
  vtkLookupTable          *m_Table;

  vtkActor                *m_ActorWired;
  
  vtkPlane				        *m_Plane;
  vtkMAFMeshCutter		    *m_Cutter;
  vtkPolyDataNormals *m_NormalFilter;
  

  void CreateFieldDataControlArrays();
	void UpdateProperty(bool fromTag = false);
	/**Update data value to selected scalar */
  void UpdateScalars();

  void UpdateVtkPolyDataNormalFilterActiveScalar();
  /** Update the visualization with changed scalar*/
  void UpdateLUTAndMapperFromNewActiveScalars();

  wxString                *m_ScalarsName;
  wxString                *m_ScalarsVTKName;

  mafGUIMaterialButton       *m_MaterialButton;

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
  
  double	m_Origin[3];
  double	m_Normal[3];

	virtual mafGUI  *CreateGui();
};  
#endif // __mafPipeMeshSlice_H__
