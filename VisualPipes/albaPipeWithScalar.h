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
class vtkDataArray;

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

	void SetScalarBarLabelsNum(int num);

	void SetScalarRange(double * sr);

	/**Set the scalar bar position */
	void SetScalarBarPos(int pos);

	/** IDs for the GUI */
	enum PIPE_WITH_SCALARS_WIDGET_ID
	{
		ID_SCALARS = albaPipe::ID_LAST,
		ID_COMPONENTS,
		ID_PROBE_MAPS,
		ID_SELECT_PROBE_VME,
		ID_PROBE_MODE,
		ID_PROBE_TARGET,
		ID_PROBE_MAX_DISTANCE,
		ID_LUT,
		ID_LUT_SLIDER,
		ID_SCALAR_MAP_ACTIVE,
		ID_ENABLE_SCALAR_BAR,
		ID_SHOW_HISTOGRAM,
		ID_SCALAR_BAR_LAB_N,
		ID_SCALAR_BAR_POS,
		ID_LAST,
	};

	enum PIPE_TYPE_SCALARS
	{
		POINT_TYPE = 0,
		CELL_TYPE,
	};

	enum PROBE_MODES
	{
		PROBE_DENSITY = 0,
		PROBE_DISTANCE,
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

	/** Create the ScalarBar Actor for Scalar show */
	void CreateScalarBarActor();

	/** Create the ScalarBar Actor for Scalar show */
	void DeleteScalarBarActor();

	/** Show/Hide Scalar Bar Actor */
	void ShowScalarBarActor(bool show = true);

	int GetScalarBarLabNum() const { return m_ScalarBarLabNum+3; }
	void SetScalarBarLabNum(int val) { m_ScalarBarLabNum = val-3; }

	void SetProbeVolume(albaVME *vol);

	static bool VolumeAccept(albaVME *node);

	
	int IsProbeMapActive() const { return m_ProbeMapActive; }
	void SetProbeMapActive(int val);

	/** Set/Get the probe mode: PROBE_DENSITY samples the volume scalars at the points, PROBE_DISTANCE computes for each point
	the distance along its normal to the isosurface of value ProbeTarget (requires point normals) */
	int GetProbeMode() const { return m_ProbeMode; }
	void SetProbeMode(int mode);

	/** Set/Get the isosurface value used in PROBE_DISTANCE mode */
	double GetProbeTarget() const { return m_ProbeTarget; }
	void SetProbeTarget(double target);

	/** Set/Get the maximum traversed distance used in PROBE_DISTANCE mode */
	double GetProbeMaxDistance() const { return m_ProbeMaxDistance; }
	void SetProbeMaxDistance(double maxDistance);

	/** Creates/Show the Histogram */
	void CreateHistogramDialog();


	wxString GetScalarName(int id) const;

	int GetComponentIndex() { return m_ComponentIndex; }
	void SetComponentIndex(int val) { m_ComponentIndex = val; }
protected:
	
  vtkLookupTable  *m_Table;
	vtkLookupTable  *m_BackupTable; //lut used before the probe map activation, restored on deactivation
	albaVME *m_ProbeVolume;
	
	albaGUILutSwatch *m_LutSwatch;
	albaGUILutSlider		*m_LutSlider;
	wxComboBox *m_ScalarComboBox;
	wxComboBox *m_ComponentsComboBox;

  void CreateFieldDataControlArrays();
	
  /** Update data value to selected scalar */
  void UpdateActiveScalarsInVMEDataVectorItems();
  
  /** Update the visualization with changed scalar*/
  void UpdateVisualizationWithNewSelectedScalars();

	/**Enables or disables Gui Components */
	void EnableDisableGuiComponents();

	/** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	void CreateScalarsGui(albaGUI *gui);

	/** Creates the Stack for probe maps visualization */
	void CreateProbeMapStack();

	/** Destroys the Density Map Stack */
	void DestroyProbeMapStack();

	/** Applies probe mode, target and max distance to the probe filter */
	void UpdateProbeFilterParameters();

	/** Returns true if the probed dataset has point normals, needed by PROBE_DISTANCE mode */
	bool HasPointNormals();

	/** Sets the lut preset related to the probe mode: red to green for density, green to red for distance */
	void ApplyProbeLut();

	/** Copies the current lut to the material and updates the lut swatch */
	void UpdateLutGui();
	
	/** Deletes the Histogram Dialog */
	void DeleteHistogramDialog();
	
	/** Updates the Component Management in GUI and set current component */
	void UpdateComponentsMangaement();

protected:
	vtkDataArray *GetCurrentScalars();

	/** Get the Component Name*/
	albaString GetComponentName(vtkDataArray *scalars, int compNum);

	/** Updates the scalar Bar Title */
	void UpdateScalarBarTitle();



  wxString                *m_ScalarsInComboBoxNames;
  wxString                *m_ScalarsVTKName;

	mmaMaterial             *m_ObjectMaterial;
	vtkDataSetMapper        *m_Mapper; 
	vtkActor                *m_Actor;
	vtkScalarBarActor				*m_ScalarBarActor;
	vtkALBADistanceFilter		*m_ProbeFilter;
	albaGUIDialog						*m_Dialog;
	albaGUIHistogramWidget  *m_Histogram;

  int                      m_PointCellArraySeparation;
  int                      m_ScalarIndex;
	int											 m_OldScalarIndex;
  int                      m_NumberOfArrays;
	int 									   m_ComponentIndex;
	int											 m_NumberOfComponents;
  int                      m_ActiveScalarType;
	int											 m_OldActiveScalarType;
	int                      m_ScalarMapActive;  //Gui option to enable scalar maps generation to be active a volume must be selected
	int                      m_ProbeMapActive;
	int                      m_ProbeMode;
	double                   m_ProbeTarget;
	double                   m_ProbeMaxDistance;
	int											 m_ShowScalarBar;
	int											 m_ScalarBarLabNum;
	int											 m_ScalarBarPos;
	int											 m_MapsStackActive;  //True when the maps generation is active (not gui option)
	int											 m_OldMapsGenActive;  //True when the maps generation is active (not gui option)

	albaString							 m_ProbeVolName;
};
#endif // __albaPipeWithScalar_H__
