/*=========================================================================

 Program: MAF2
 Module: mafPipeWithScalar
 Authors: Gianluigi Crimi, Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeWithScalar_H__
#define __mafPipeWithScalar_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkLookupTable;
class mafGUILutSwatch;
class vtkDataSetMapper;
class vtkActor;
class mmaMaterial;
class vtkDataSet;

//----------------------------------------------------------------------------
// mafPipeWithScalar :
//----------------------------------------------------------------------------
class MAF_EXPORT mafPipeWithScalar : public mafPipe
{
public:
	//mafTypeMacro(mafPipeWithScalar,mafPipe);

	mafPipeWithScalar();
	virtual     ~mafPipeWithScalar();

	void ManageScalarOnExecutePipe(vtkDataSet * dataSet);

	/** process events coming from Gui */
	virtual void OnEvent(mafEventBase *maf_event);

	/** IDs for the GUI */
	enum PIPE_WITH_SCALARS_WIDGET_ID
	{
		ID_SCALARS = mafPipe::ID_LAST,
		ID_LUT,
		ID_SCALAR_MAP_ACTIVE,
		ID_LAST,
	};

  enum PIPE_TYPE_SCALARS
  {
    POINT_TYPE = 0,
    CELL_TYPE,
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

protected:
	
  vtkLookupTable  *m_Table;
	
	mafGUILutSwatch *m_LutSwatch;

  void CreateFieldDataControlArrays();
	
  /** Update data value to selected scalar */
  void UpdateActiveScalarsInVMEDataVectorItems();
  
  /** Update the visualization with changed scalar*/
  void UpdateVisualizationWithNewSelectedScalars();

  wxString                *m_ScalarsInComboBoxNames;
  wxString                *m_ScalarsVTKName;
	
	mmaMaterial             *m_ObjectMaterial;
	vtkDataSetMapper        *m_Mapper; 
	vtkActor                *m_Actor;

  int                      m_PointCellArraySeparation;
  int                      m_ScalarIndex;
  int                      m_NumberOfArrays;
  int                      m_ActiveScalarType;
  int                      m_ScalarMapActive;


  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	void CreateScalarsGui(mafGUI *gui);
};  
#endif // __mafPipeWithScalar_H__
