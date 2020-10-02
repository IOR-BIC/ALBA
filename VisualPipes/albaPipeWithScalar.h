/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeWithScalar
 Authors: Gianluigi Crimi, Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeWithScalar_H__
#define __albaPipeWithScalar_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkLookupTable;
class albaGUILutSwatch;
class albaGUILutSlider;
class vtkDataSetMapper;
class vtkScalarBarActor;
class vtkActor;
class mmaMaterial;
class vtkDataSet;

//----------------------------------------------------------------------------
// albaPipeWithScalar :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipeWithScalar : public albaPipe
{
public:
	//albaTypeMacro(albaPipeWithScalar,albaPipe);

	albaPipeWithScalar();
	virtual     ~albaPipeWithScalar();

	void ManageScalarOnExecutePipe(vtkDataSet * dataSet);

	/** process events coming from Gui */
	virtual void OnEvent(albaEventBase *alba_event);

	/**Set the scalar bar position */
	void SetScalarBarPos(int pos);

	/** IDs for the GUI */
	enum PIPE_WITH_SCALARS_WIDGET_ID
	{
		ID_SCALARS = albaPipe::ID_LAST,
		ID_LUT,
		ID_LUT_SLIDER,
		ID_SCALAR_MAP_ACTIVE,
		ID_ENABLE_SCALAR_BAR,
		ID_SCALAR_BAR_LAB_N,
		ID_SCALAR_BAR_POS,
		ID_LAST,
	};

	enum PIPE_TYPE_SCALARS
	{
		POINT_TYPE = 0,
		CELL_TYPE,
	};

	enum SCALAR_BAR_POSITIONS
	{
		SB_ON_BOTTOM = 0,
		SB_ON_TOP,
		SB_ON_LEFT,
		SB_ON_RIGHT
	};

  /** Set/Get Active Scalar */
  void SetActiveScalar(int index){m_ScalarIndex = index;};
  int GetScalarIndex(){return m_ScalarIndex;};

  /** Get Number of Scalars */
  int GetNumberOfArrays(){return m_NumberOfArrays;};

  /** Set scalar map active, so you can see scalar associated to points or cells*/
  void SetScalarMapActive(int value){m_ScalarMapActive = value;};
  
  /** Set the lookup table */
	void SetLookupTable(vtkLookupTable *table);

	/** Set the lookup table */
	void SetLookupTableToMapper();

  /** Gets the lookup table*/
	vtkLookupTable *GetLookupTable(){return m_Table;};

	/** Update the properties */
	virtual void UpdateProperty(bool fromTag = false);

	/** Create the ScalarBar Actor for Scalar show */
	void CreateScalarBarActor();

	/** Create the ScalarBar Actor for Scalar show */
	void DeleteScalarBarActor();

	/** Show/Hide Scalar Bar Actor */
	void ShowScalarBarActor(bool show = true);

protected:
	
  vtkLookupTable  *m_Table;
	
	albaGUILutSwatch *m_LutSwatch;
	albaGUILutSlider		*m_LutSlider;

  void CreateFieldDataControlArrays();
	
  /** Update data value to selected scalar */
  void UpdateActiveScalarsInVMEDataVectorItems();
  
  /** Update the visualization with changed scalar*/
  void UpdateVisualizationWithNewSelectedScalars();

	/**Enables or disables Gui Components */
	void EnableDisableGuiComponents();

  wxString                *m_ScalarsInComboBoxNames;
  wxString                *m_ScalarsVTKName;
	
	mmaMaterial             *m_ObjectMaterial;
	vtkDataSetMapper        *m_Mapper; 
	vtkActor                *m_Actor;
	vtkScalarBarActor				*m_ScalarBarActor;


  int                      m_PointCellArraySeparation;
  int                      m_ScalarIndex;
  int                      m_NumberOfArrays;
  int                      m_ActiveScalarType;
  int                      m_ScalarMapActive;
	int											 m_ShowScalarBar;
	int											 m_ScalarBarLabNum;
	int											 m_ScalarBarPos;


  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	void CreateScalarsGui(albaGUI *gui);
};  
#endif // __albaPipeWithScalar_H__
