/*=========================================================================

 Program: MAF2
 Module: mafOp2DMeasure
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOp2DMeasure_H__
#define __mafOp2DMeasure_H__

#include "mafOp.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafInteractor2DDistance;
class mafInteractor2DAngle;
class mafInteractor2DIndicator;
class mafEvent;

//----------------------------------------------------------------------------
// mafOp2DMeasure :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOp2DMeasure: public mafOp
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

	mafOp2DMeasure(const wxString &label = "2DMeasure");
	~mafOp2DMeasure(); 
	virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(mafOp2DMeasure, mafOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
	mafOp* Copy();
	
	/** Return true for the acceptable vme type. */
  bool Accept(mafVME*node);

	/** Set the input vme for the operation. */
	virtual void OpRun();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:
  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

	mafInteractor2DDistance *m_DistanceInteractor2D;
  mafInteractor2DAngle *m_AngleInteractor2D;
  mafInteractor2DIndicator *m_IndicatorInteractor2D;

  mafString m_DistanceMeasure;
  mafString m_AcuteAngle;
  mafString m_ObtuseAngle;
  mafString m_MeasureText;
  int m_MeasureType;
  std::vector<int> m_FirstPositionInteractor;
  wxListBox *m_MeasureList;

  int m_GenerateHistogramFlag;
  mafString m_ManualDistance;
  mafString m_ManualAngle;
  mafString m_ManualIndicator;

	double m_Unused[1]; //< particular variable to store unused value; it serves for ToDouble function of wxString

	std::vector<int> m_InteractorType;
};
#endif
