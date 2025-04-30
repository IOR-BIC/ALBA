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
class vtkALBADistanceFilter;
class albaGUIHistogramWidget;
class albaGUIDialog;

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
		ID_DENSITY_MAPS,
		ID_SELECT_DENS_VME,
		ID_LUT,
		ID_LUT_SLIDER,
		ID_SCALAR_MAP_ACTIVE,
		ID_ENABLE_SCALAR_BAR,
		ID_SHOW_HISTOGRAM,
		ID_CLOSE_HISTOGRAM,
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
		SB_ON_TOP = 0,
		SB_ON_BOTTOM,
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

	int GetScalarBarLabNum() const { return m_ScalarBarLabNum+3; }
	void SetScalarBarLabNum(int val) { m_ScalarBarLabNum = val-3; }

	void SetDensityVolume(albaVME *vol);

	static bool VolumeAccept(albaVME *node);

	
	int IsDensisyMapActive() const { return m_DensisyMapActive; }
	void SetDensisyMapActive(int val);
protected:
	
  vtkLookupTable  *m_Table;
	albaVME *m_DensityVolume;
	
	albaGUILutSwatch *m_LutSwatch;
	albaGUILutSlider		*m_LutSlider;
	wxComboBox *m_ScalarComboBox;

  void CreateFieldDataControlArrays();
	
  /** Update data value to selected scalar */
  void UpdateActiveScalarsInVMEDataVectorItems();
  
  /** Update the visualization with changed scalar*/
  void UpdateVisualizationWithNewSelectedScalars();

	/**Enables or disables Gui Components */
	void EnableDisableGuiComponents();

	/** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	void CreateScalarsGui(albaGUI *gui);

	/** Creates the Stack for density maps visualization */
	void CreateDensityMapStack();

	/** Destroys the Density Map Stack */
	void DestroyDensityMapStack();

	/** Creates/Show the Histogram */
	void CreateHistogramDialog();

  wxString                *m_ScalarsInComboBoxNames;
  wxString                *m_ScalarsVTKName;
	
	mmaMaterial             *m_ObjectMaterial;
	vtkDataSetMapper        *m_Mapper; 
	vtkActor                *m_Actor;
	vtkScalarBarActor				*m_ScalarBarActor;
	vtkALBADistanceFilter		*m_DensityFilter;
	albaGUIDialog						*m_Dialog;
	albaGUIHistogramWidget  *m_Histogram;

  int                      m_PointCellArraySeparation;
  int                      m_ScalarIndex;
	int											 m_OldScalarIndex;
  int                      m_NumberOfArrays;
  int                      m_ActiveScalarType;
	int											 m_OldActiveScalarType;
	int                      m_ScalarMapActive;  //Gui option to enable scalar maps generation to be active a volume must be selected
	int                      m_DensisyMapActive;
	int											 m_ShowScalarBar;
	int											 m_ScalarBarLabNum;
	int											 m_ScalarBarPos;
	int											 m_MapsGenActive;  //True when the maps generation is active (not gui option)
	int											 m_OldMapsGenActive;  //True when the maps generation is active (not gui option)

	albaString							 m_DensVolName;


private:
	void DeleteHistogramDialog();
};
#endif // __albaPipeWithScalar_H__
