/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoAddLandmark.h,v $
  Language:  C++
  Date:      $Date: 2005-10-21 10:05:32 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoAddLandmark_H__
#define __mmoAddLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafOp.h"
#include "mafString.h"

#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMELandmark;
class mafVMELandmarkCloud;
class mafInteractor;
class mmiPicker;

//----------------------------------------------------------------------------
// mmoAddLandmark :
//----------------------------------------------------------------------------
class mmoAddLandmark: public mafOp
{
public:
	mmoAddLandmark(wxString label);
	~mmoAddLandmark(); 
	virtual void OnEvent(mafEventBase *maf_event);
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

  /** Used to add a landmark to the cloud */
  void AddLandmark(double pos[3]);

protected:
	/** Create the AddLandmarks interface. */
	void CreateGui();

	/** Delete the AddLandmarks interface. */
	void DeleteGui();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  /** Used in redo to add the landmarks to the tree*/
  void AddLandmark();

  /** Used in undo to remove the added landmarks*/
  void RemoveLandmark();

	int m_AddToCurrentTime;

  mafVMELandmarkCloud	*m_Cloud;
	mafVME      				*m_PickedVme;
  std::vector<mafVMELandmark *> m_LandmarkAdded;
	 
	bool                 m_CloudCreatedFlag;
	mafString						 m_LandmarkName;
  mmiPicker           *m_LandmarkPicker;
  mafInteractor       *m_OldBehavior;

	double							 m_LandmarkPosition[3];
};
#endif
