/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOp2DMeasure.h,v $
  Language:  C++
  Date:      $Date: 2009-11-04 16:07:49 $
  Version:   $Revision: 1.2.2.1 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOp2DMeasure_H__
#define __medOp2DMeasure_H__

#include "mafOp.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medInteractor2DDistance;
class medInteractor2DAngle;
class medInteractor2DIndicator;
class mafEvent;

/**
class name : medOp2DMeasure
*/
class medOp2DMeasure: public mafOp
{
public:
  /** constructor. */
	medOp2DMeasure(const wxString &label = "2DMeasure");
  /** destructor. */
	~medOp2DMeasure(); 

  /** Precess events coming from other objects */
	/*virtual*/ void OnEvent(mafEventBase *maf_event);
  
  /** RTTI macro */
  mafTypeMacro(medOp2DMeasure, mafOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
	/*virtual*/ mafOp* Copy();
	
	/** Return true for the acceptable vme type. */
  /*virtual*/ bool Accept(mafNode *node);

	/** Set the input vme for the operation. */
	/*virtual*/ void OpRun();

protected:
  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  /*virtual*/ void OpStop(int result);

  medInteractor2DDistance *m_2DDistanceInteractor;
  medInteractor2DAngle *m_2DAngleInteractor;
  medInteractor2DIndicator *m_2DIndicatorInteractor;

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
