/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIRollOut.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGUIRollOut_H__
#define __mafGUIRollOut_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafString.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// class forward :
//----------------------------------------------------------------------------
class mafGUI;
class mafGUIPicButton;

//----------------------------------------------------------------------------
// mafGUIRollOut :
//----------------------------------------------------------------------------
/** This class plugs and manage a roll-out gui.
The main gui and the rolled out gui are passed through the constructor and the class
plug the title bar with the picture button on the main gui and then plug the sub-gui
that can be shown/hidden by pressing on the picture button on the left of the title.
*/
class mafGUIRollOut: public wxPanel
{
public:
           mafGUIRollOut(mafGUI *parent, mafString title, mafGUI *roll_gui, int id = -1, bool rollOutOpen = true);
  virtual ~mafGUIRollOut();

  /** Open/Close the Roll-out.*/
  void RollOut(bool open = true);


  void SetListener(mafObserver *listener) {m_Listener = listener;};

protected:
  /** Answer the click on the Picture button to open/close the roll out gui.*/
  void OnRollOut(wxCommandEvent &event);

  mafGUI *m_MainGui; ///< Gui on which will be plugged the RollOut gui.
  mafGUI *m_RollGui; ///< Child gui that will be rolled out.
  mafGUIPicButton *m_RollOutButton; ///< Picture button representing the icon to open/close the roll out gui.
  mafObserver *m_Listener;
  DECLARE_EVENT_TABLE()
};
#endif
