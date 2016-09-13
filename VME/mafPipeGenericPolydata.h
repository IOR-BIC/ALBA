/*=========================================================================

 Program: MAF2
 Module: mafPipeGenericPolydata
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeGenericPolydata_H__
#define __mafPipeGenericPolydata_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipeWithScalar.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

class mafAxes;
class mafGUIMaterialButton;
class vtkMAFPolyDataNormals;
class vtkPolyData;
class vtkDataSet;

//----------------------------------------------------------------------------
// mafPipeGenericPolydata :
//----------------------------------------------------------------------------
class MAF_EXPORT mafPipeGenericPolydata : public mafPipeWithScalar
{
public:
	//mafTypeMacro(mafPipeGenericPolydata,mafPipe);

	mafPipeGenericPolydata();
	virtual     ~mafPipeGenericPolydata();

	/** process events coming from Gui */
	virtual void OnEvent(mafEventBase *maf_event);

  /** Create the VTK rendering pipeline*/
	virtual void Create(mafSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
	virtual void Select(bool select); 

	/** IDs for the GUI */
	enum PIPE_SURFACE_WIDGET_ID
	{
		ID_WIREFRAME = mafPipeWithScalar::ID_LAST,
    ID_NORMALS_TYPE,
    ID_EDGE_VISIBILITY,
		ID_BORDER_CHANGE,
    ID_SCALARS,
    ID_LUT,
    ID_USE_VTK_PROPERTY,
		ID_LAST,
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

	/** Set Normal type generation */
	void SetNormalsTypeToPoints();
	void SetNormalsTypeToCells();
	int GetNormalsType() {return m_ShowCellsNormals;};
  
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
	vtkMAFPolyDataNormals   *m_NormalsFilter;
	mafAxes                 *m_Axes;
	vtkPolyData							*m_InputAsPolydata;

	mafGUILutSwatch *m_LutSwatch;

	virtual vtkPolyData* GetInputAsPolyData() = 0;
	
  void UpdateProperty(bool fromTag = false);

  wxString                *m_ScalarsInComboBoxNames;
  wxString                *m_ScalarsVTKName;

  mafGUIMaterialButton       *m_MaterialButton;

  int                      m_Wireframe;
	int											 m_ShowCellsNormals;
  int                      m_BorderElementsWiredActor;
  int                      m_UseVTKProperty;
	double				           m_Border;

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	virtual mafGUI  *CreateGui();
};  
#endif // __mafPipeGenericPolydata_H__
