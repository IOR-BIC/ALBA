/*=========================================================================

 Program: MAF2
 Module: mafGUISaveRestorePose
 Authors: Paolo Quadrani , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUISaveRestorePose_H__
#define __mafGUISaveRestorePose_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafGUITransformInterface.h"
#include "mafTagArray.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafMatrix;

/** Input VME ABS Matrix Store/Restore GUI Component
  
  Component to store/restore input vme abs pose
  
  Example usage to plug this component into an operation:
  
  guiSaveRestorePose = new mafGUISaveRestorePose(mafVME::SafeDownCast(opInputVME), this);
  clientOpGUI->AddGui(guiSaveRestorePose->GetGui());

  @sa mafOpMAFTransform
  
*/
class MAF_EXPORT mafGUISaveRestorePose : public mafGUITransformInterface
{
public:

  enum ID_TYPE_GUI
  {
    ID_POSE_GUI = 0,
    ID_SCALE_GUI, 
  };

  mafGUISaveRestorePose(mafVME *input, mafObserver *listener = NULL , int typeGui = ID_POSE_GUI, bool testMode = false);
	~mafGUISaveRestorePose(); 

  /** Events handling */
  void OnEvent(mafEventBase *maf_event);

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
  void StorePoseHelper( mafString absPoseTagName);
  void RestorePoseHelper( mafString absPoseTagName );
  void RemovePoseHelper( mafString absPoseTagName );

  wxListBox				 *m_PositionsList;
  int               m_TypeGui;

  /** friend test */
  friend class mafGUISaveRestorePoseTest;

};
#endif
