/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiSaveRestorePose.h,v $
  Language:  C++
  Date:      $Date: 2005-07-07 15:16:42 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGuiSaveRestorePose_H__
#define __mafGuiSaveRestorePose_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafGuiTransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgGui;
class mafMatrix;
//----------------------------------------------------------------------------
/** transform gui

  @sa
 
  @todo

*/
class mafGuiSaveRestorePose : public mafGuiTransformInterface
{
public:

  mafGuiSaveRestorePose(mafVME *input, mafObserver *listener = NULL);
	~mafGuiSaveRestorePose(); 

  void OnEvent(mafEventBase *maf_event);

  /** 
  Enable-Disable the GUI's widgets */
	void EnableWidgets(bool enable);

  //----------------------------------------------------------------------------
  //gui constants: 
  //----------------------------------------------------------------------------

  // this constants must be visible from the owner object  
  enum GUI_SAVE_RESTORE_ID
  {
    ID_SAVE = MINID,
    ID_REMOVE,
    ID_APPLY,
  };

protected:
  /** 
  Create the GUI*/
  void CreateGui();

  /**
  Find in input vme if there are stored poses and add these into the listbox.*/
  void ReadSavedPoses();

  /**
  Save the current vme pose and add the associated string to the listbox.*/
  void SavePose(mafMatrix *abs_pose);

  /**
  Remove the selected pose and remove the string from the listbox.*/
  void RemovePose(int sel_pose);

  /**
  Restore the selected pose.*/
  void RestorePose(int sel_pose);

  wxListBox				 *m_PositionsList;
};
#endif
