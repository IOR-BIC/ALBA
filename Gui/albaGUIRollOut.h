/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIRollOut
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIRollOut_H__
#define __albaGUIRollOut_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaString.h"
#include "albaObserver.h"

//----------------------------------------------------------------------------
// class forward :
//----------------------------------------------------------------------------
class albaGUI;
class albaGUIPicButton;

//----------------------------------------------------------------------------
// albaGUIRollOut :
//----------------------------------------------------------------------------
/** This class plugs and manage a roll-out gui.
The main gui and the rolled out gui are passed through the constructor and the class
plug the title bar with the picture button on the main gui and then plug the sub-gui
that can be shown/hidden by pressing on the picture button on the left of the title.
*/
class ALBA_EXPORT albaGUIRollOut: public wxPanel
{
public:
           albaGUIRollOut(albaGUI *parent, albaString title, albaGUI *roll_gui, int id = -1, bool rollOutOpen = true);
  virtual ~albaGUIRollOut();

  /** Open/Close the Roll-out.*/
  void RollOut(bool open = true);


  void SetListener(albaObserver *listener) {m_Listener = listener;};

protected:
  /** Answer the click on the Picture button to open/close the roll out gui.*/
  void OnRollOut(wxCommandEvent &event);

  albaGUI *m_MainGui; ///< Gui on which will be plugged the RollOut gui.
  albaGUI *m_RollGui; ///< Child gui that will be rolled out.
  albaGUIPicButton *m_RollOutButton; ///< Picture button representing the icon to open/close the roll out gui.
  albaObserver *m_Listener;
  DECLARE_EVENT_TABLE()
};
#endif
