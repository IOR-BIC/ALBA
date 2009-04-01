/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpAddLandmark.h,v $
  Language:  C++
  Date:      $Date: 2009-04-01 13:05:40 $
  Version:   $Revision: 1.2.2.1 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpAddLandmark_H__
#define __mafOpAddLandmark_H__

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
class mafGUINamedPanel;
class mafGUIDictionaryWidget;
//----------------------------------------------------------------------------
// mafOpAddLandmark :
//----------------------------------------------------------------------------
class mafOpAddLandmark: public mafOp
{
public:
	mafOpAddLandmark(const wxString &label = "AddLandmark");
	~mafOpAddLandmark(); 
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafOpAddLandmark, mafOp);

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

  void SetPickingActiveFlag(bool picking){m_PickingActiveFlag = picking;}
  bool GetPickingActiveFlag(){return m_PickingActiveFlag;}

  mafVME* GetPickedVme(){return m_PickedVme;};
  mafVMELandmarkCloud* GetCloud(){return m_Cloud;};

  void SetLandmarkName(mafString name){m_LandmarkName = name;};

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

  /** Show a message box to inform the user that the added landmark already exists in the tree.*/
  void ExistingLandmarkMessage();

	int m_AddToCurrentTime;

  mafVMELandmarkCloud	*m_Cloud;
	mafVME      				*m_PickedVme;
  std::vector<mafVMELandmark *> m_LandmarkAdded;
	 
	bool                 m_CloudCreatedFlag;
	bool                 m_PickingActiveFlag;
	mafString						 m_LandmarkName;
  mmiPicker           *m_LandmarkPicker;
  mafInteractor       *m_OldBehavior;

	double							 m_LandmarkPosition[3];

  mafGUINamedPanel			  *m_GuiPanel;
  mafGUIDictionaryWidget *m_Dict;
};
#endif
