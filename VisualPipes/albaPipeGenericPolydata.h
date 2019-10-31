/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeGenericPolydata
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeGenericPolydata_H__
#define __albaPipeGenericPolydata_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaPipeWithScalar.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

class albaAxes;
class albaGUIMaterialButton;
class vtkALBAPolyDataNormals;
class vtkPolyData;
class vtkDataSet;

//----------------------------------------------------------------------------
// albaPipeGenericPolydata :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipeGenericPolydata : public albaPipeWithScalar
{
public:
	//albaTypeMacro(albaPipeGenericPolydata,albaPipe);

	albaPipeGenericPolydata();
	virtual     ~albaPipeGenericPolydata();

	/** process events coming from Gui */
	virtual void OnEvent(albaEventBase *alba_event);

  /** Create the VTK rendering pipeline*/
	virtual void Create(albaSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
	virtual void Select(bool select); 

	/** IDs for the GUI */
	enum PIPE_SURFACE_WIDGET_ID
	{
		ID_WIREFRAME = albaPipeWithScalar::ID_LAST,
    ID_NORMALS_TYPE,
    ID_EDGE_VISIBILITY,
		ID_BORDER_CHANGE,
    ID_USE_VTK_PROPERTY,
		ID_LAST,
	};

  /** Get assembly front/back */
  virtual vtkALBAAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  virtual vtkALBAAssembly *GetAssemblyBack(){return m_AssemblyBack;};
	
  /** Core of the pipe */
  virtual void ExecutePipe();

  /** Add/RemoveTo Assembly Front/back */
  virtual void AddActorsToAssembly(vtkALBAAssembly *assembly);
  virtual void RemoveActorsFromAssembly(vtkALBAAssembly *assembly);
  
  /** Set the actor picking*/
	void SetActorPicking(int enable = true);

  /** Set the actor wire frame*/
  void SetWireframeOn();
  void SetWireframeOff();

	/** Set Normal type generation */
	void SetNormalsTypeToPoints();
	void SetNormalsTypeToCells();
	int GetNormalsType() {return m_ShowCellsNormals;};
	
	/** Set the flip of normal filter to on*/
	void SetFlipNormalOn();
	/** Set the flip of normal filter to off*/
	void SetFlipNormalOff();
  
  /** Set the actor border visible or not*/
  void SetEdgesVisibilityOn();
  void SetEdgesVisibilityOff();
  
  /** Set VTK Property to visualize the material of vme*/
  void SetUseVTKProperty(int value){m_UseVTKProperty = value;};

	/**Return the thickness of the border*/	
	double GetThickness();

	/**Set the thickness value*/
	void SetThickness(double thickness);

protected:

  vtkDataSetMapper        *m_MapperWired;
  vtkActor                *m_ActorWired;
	vtkActor                *m_OutlineActor;
	vtkALBAPolyDataNormals  *m_NormalsFilter;
	albaAxes                *m_Axes;
	vtkPolyData							*m_InputAsPolydata;

	albaGUILutSwatch *m_LutSwatch;

	virtual vtkPolyData* GetInputAsPolyData() = 0;
	

  albaGUIMaterialButton       *m_MaterialButton;

  int                      m_Wireframe;
	int											 m_ShowCellsNormals;
  int                      m_BorderElementsWiredActor;
  int                      m_UseVTKProperty;
	int											 m_FlipNormals;
	double				           m_Border;

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	virtual albaGUI  *CreateGui();
};  
#endif // __albaPipeGenericPolydata_H__
