/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgRollOut.h,v $
  Language:  C++
  Date:      $Date: 2008-01-08 15:13:16 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgRollOut_H__
#define __mmgRollOut_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafString.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// class forward :
//----------------------------------------------------------------------------
class mmgGui;
class mmgPicButton;

//----------------------------------------------------------------------------
// mmgRollOut :
//----------------------------------------------------------------------------
/** This class plugs and manage a roll-out gui.
The main gui and the rolled out gui are passed through the constructor and the class
plug the title bar with the picture button on the main gui and then plug the sub-gui
that can be shown/hidden by pressing on the picture button on the left of the title.
*/
class mmgRollOut: public wxPanel
{
public:
           mmgRollOut(mmgGui *parent, mafString title, mmgGui *roll_gui, int id = -1, bool rollOutOpen = true);
  virtual ~mmgRollOut();

  /** Open/Close the Roll-out.*/
  void RollOut(bool open = true);


  void SetListener(mafObserver *listener) {m_Listener = listener;};

protected:
  /** Answer the click on the Picture button to open/close the roll out gui.*/
  void OnRollOut(wxCommandEvent &event);

  mmgGui *m_MainGui; ///< Gui on which will be plugged the RollOut gui.
  mmgGui *m_RollGui; ///< Child gui that will be rolled out.
  mmgPicButton *m_RollOutButton; ///< Picture button representing the icon to open/close the roll out gui.
  mafObserver *m_Listener;
  DECLARE_EVENT_TABLE()
};
#endif
