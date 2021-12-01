/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISaveRestorePose
 Authors: Paolo Quadrani , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUISaveRestorePose_H__
#define __albaGUISaveRestorePose_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaObserver.h"
#include "albaGUITransformInterface.h"
#include "albaTagArray.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUI;
class albaMatrix;

/** Input VME ABS Matrix Store/Restore GUI Component
  
  Component to store/restore input vme abs pose
  
  Example usage to plug this component into an operation:
  
  guiSaveRestorePose = new albaGUISaveRestorePose(opInputVME, this);
  clientOpGUI->AddGui(guiSaveRestorePose->GetGui());

  @sa albaOpTransformOld
  
*/
class ALBA_EXPORT albaGUISaveRestorePose : public albaGUITransformInterface
{
public:

  enum ID_TYPE_GUI
  {
    ID_POSE_GUI = 0,
    ID_SCALE_GUI, 
  };

  albaGUISaveRestorePose(albaVME *input, albaObserver *listener = NULL , int typeGui = ID_POSE_GUI, bool testMode = false);
	~albaGUISaveRestorePose(); 

  /** Events handling */
  void OnEvent(albaEventBase *alba_event);

  /** 
  Enable-Disable the GUI's widgets */
	void EnableWidgets(bool enable);

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
  Find in input vme if there are stored abs poses and fill the listbox with their names*/
  void FillListBoxWithABSPosesStoredInInputVME();

  /**
  Prompt the user for the new pose to be saved name , add the associated string to the listbox
  and add the corresponding matrix data to the input vme tagarray*/
  void StorePose();
  
  /**
  Restore the selected pose to the input vme from the listbox pose id.*/
  void RestorePose(int absPoseListBoxID);

  /**
  Remove the given pose from the input vme tag array and the associated string from the listbox*/
  void RemovePose(int absPoseListBoxID);
  
  /** Helper functions */
  void StorePoseHelper( albaString absPoseTagName);
  void RestorePoseHelper( albaString absPoseTagName );
  void RemovePoseHelper( albaString absPoseTagName );

  wxListBox				 *m_PositionsList;
  int               m_TypeGui;

  /** friend test */
  friend class albaGUISaveRestorePoseTest;

};
#endif
