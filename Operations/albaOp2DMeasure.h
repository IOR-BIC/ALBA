/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOp2DMeasure
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOp2DMeasure_H__
#define __albaOp2DMeasure_H__

#include "albaOp.h"
#include "albaString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaInteractor2DDistance;
class albaInteractor2DAngle;
class albaInteractor2DIndicator;
class albaEvent;

//----------------------------------------------------------------------------
// albaOp2DMeasure :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOp2DMeasure: public albaOp
{
public:
	enum MEASURE2D_ID
	{
		ID_MEASURE_TYPE = MINID,
		ID_PLOT_PROFILE,
		ID_MANUAL_DISTANCE,
		ID_MANUAL_ANGLE,
		ID_MANUAL_INDICATOR,
		ID_UNDO_MEASURE,

		ID_STORE_MEASURE,
		ID_REMOVE_MEASURE,
		ID_MEASURE_LIST,

		ID_DISTANCE_TYPE,
		ID_ANGLE_TYPE,
		ID_INDICATOR_TYPE,
		MINID,
	};

	albaOp2DMeasure(const wxString &label = "2DMeasure");
	~albaOp2DMeasure(); 
	virtual void OnEvent(albaEventBase *alba_event);
  
  albaTypeMacro(albaOp2DMeasure, albaOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
	albaOp* Copy();

	/** Set the input vme for the operation. */
	virtual void OpRun();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

	albaInteractor2DDistance *m_DistanceInteractor2D;
  albaInteractor2DAngle *m_AngleInteractor2D;
  albaInteractor2DIndicator *m_IndicatorInteractor2D;

  albaString m_DistanceMeasure;
  albaString m_AcuteAngle;
  albaString m_ObtuseAngle;
  albaString m_MeasureText;
  int m_MeasureType;
  std::vector<int> m_FirstPositionInteractor;
  wxListBox *m_MeasureList;

  int m_GenerateHistogramFlag;
  albaString m_ManualDistance;
  albaString m_ManualAngle;
  albaString m_ManualIndicator;

	double m_Unused[1]; //< particular variable to store unused value; it serves for ToDouble function of wxString

	std::vector<int> m_InteractorType;
};
#endif
