/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOp2DMeasure.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOp2DMeasure_H__
#define __mafOp2DMeasure_H__

#include "mafOp.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmi2DDistance;
class mmi2DAngle;
class mmi2DIndicator;
class mafEvent;

//----------------------------------------------------------------------------
// mafOp2DMeasure :
//----------------------------------------------------------------------------
/** */
class mafOp2DMeasure: public mafOp
{
public:
	mafOp2DMeasure(const wxString &label = "2DMeasure");
	~mafOp2DMeasure(); 
	virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(mafOp2DMeasure, mafOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
	mafOp* Copy();
	
	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Set the input vme for the operation. */
	void OpRun();

protected:
  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

	mmi2DDistance *m_2DDistanceInteractor;
  mmi2DAngle *m_2DAngleInteractor;
  mmi2DIndicator *m_2DIndicatorInteractor;

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
