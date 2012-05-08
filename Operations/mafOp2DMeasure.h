/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOp2DMeasure.h,v $
  Language:  C++
  Date:      $Date: 2012-04-16 12:35:56 $
  Version:   $Revision: 1.1.2.4 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOp2DMeasure_H__
#define __mafOp2DMeasure_H__

#include "mafOp.h"
#include "mafString.h"

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,int);
#endif

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
	mafOp2DMeasure(const wxString &label = "2DMeasure");
	~mafOp2DMeasure(); 
	virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(mafOp2DMeasure, mafOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
	mafOp* Copy();
	
	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Set the input vme for the operation. */
	virtual void OpRun();

protected:

  //----------------------------------------------------------------------------
  // Widgets ID's
  //----------------------------------------------------------------------------
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
