/*=========================================================================
Program:	 ALBA (Agile Library for Biomedical Applications)
Module:    albaOpMeasure2D.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpMeasure2D_H__
#define __albaOpMeasure2D_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------

#include "albaOp.h"
#include <string>
#include <vector>
#include "albaInteractor2DMeasure.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaEvent;
class albaVMEImage;
class albaVMEVolumeRGB;
class albaInteractor2DMeasure_Point;
class albaInteractor2DMeasure_Distance;

// Class Name: albaOpMeasure2D
class ALBA_EXPORT albaOpMeasure2D: public albaOp
{
public:

	enum INTERACTION_TYPE
	{
		POINT,
		DISTANCE,
		MID_POINT,
		CENTER_POINT,
		LINE,
		LINE_DISTANCE,
		ANGLE,
		ANGLE_LINE,
	};

	//Widgets ID's	
	enum OP_INTERACTION_SAMPLE_ID
	{
		ID_EDIT_LINE = MINID,
		ID_SELECT_INTERACTOR,
		ID_MEASURE_LIST,
		ID_MEASURE_MAX,
		ID_SELECT_MEASURE,
		ID_REMOVE_MEASURE,
		ID_NUM_LINES,
		ID_SHOW_TEXT,
		ID_SHOW_ARROW,
		ID_LOAD_MEASURES,
		ID_SAVE_MEASURES,
		ID_MEASURE,
		ID_MEASURE_LAB,
		ID_MEASURE_ENABLE,
		ID_MEASURE_EDIT,
		ID_MEASURE_MOVE,
	};

  albaOpMeasure2D(const wxString &label = "Measure2D");
 ~albaOpMeasure2D(); 
  
  albaTypeMacro(albaOpMeasure2D, albaOp);

  albaOp* Copy();

	/** Builds operation's interface. */
  void OpRun();

	/** Precess events coming from other objects */
  void OnEvent(albaEventBase *alba_event);

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME *node);

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

  /** Create the GUI */
  virtual void CreateGui();
	
	albaView* OpenImageView(int viewId);

	/**/
	virtual void InitInteractors();

	/** Select a Measure Interactor */
	void SetMeasureInteractor(int index);

	/* Update Measure ListBox */
	void UpdateMeasureList();

	/** Remove Measures */
	void RemoveMeasure(bool clearAll = false);

	/** Load Measures from Tags */
	void Load();
	/** Save Measures to Tags */
	void Save();

	int m_SelectedMeasure;
	int m_MaxMeasures;

	int m_Enable;
	int m_Edit;
	int m_Move;
	int m_ShowText;

	albaString m_Measure;
	albaString m_MeasureLabel;
	albaString m_MeasureType;

	wxListBox *m_MeasureListBox;

	std::vector<albaInteractor2DMeasure *> m_InteractorVector;

	int m_CurrentInteractor;
	int m_SelectedInteractor;
};
#endif
