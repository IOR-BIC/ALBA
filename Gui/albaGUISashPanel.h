/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISashPanel
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUISashPanel_H__
#define __albaGUISashPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include "albaGUIPanel.h"
//----------------------------------------------------------------------------
//  albaGUISashPanel :
/** albaGUISashPanel is a wxSashLayoutWindow, that is
    a user-resizable panel docked on one side of the frame window.
    In the constructor you specify the docking-side and the initial width/height.
    albaGUISashPanel answer to the HIDE message (and hide itself) that may be sent
    from a albaGUINamedPanel with a CloseButton.

    To use sashes you have to place an OnSashDrag event-handler in the frame-window;
    I can't figure out why I can't handle that message in this class itself.

    In the future may be possible to make this window dockable. (convertible to 
    a floating window and viceversa )

    albaGUISashPanel add a menu_item to the "View" menu, allowing the user to show/hide the sash;
    menu_string is the text showed in the menu item - if you specify an empty string 
    the menu item will not be created.

  \tech
  albaGUISash create a menu item called "View" (it must be present)
  albaGUISash must be put only on one frame - it find the menu calling frame->GetMenuBar()
  albaGUISash manage by itself the Show - so it can set in sync mode the checkbutton on men - do not matter to manage the UpdateUI
  albaGUISash::Show needs to call frame::Layout into the frame, and do it by firing a message with id = ID_LAYOUT 
  If albaGUISash receive the message HIDE_SASH, it call directly Show, a menu event can be send anyway.

  From Logic point of view:
  
  1) To use the sash must be used an ID that remain in MENU_START MENU_END - sash create the menu-item with the own id
  2) Logic must take the Sash's pointer, to call sash->Show responding to men event
  3) Create the menu before the sash
  4) Create the "View" menu
*/
//----------------------------------------------------------------------------
class albaGUISashPanel: public wxSashLayoutWindow
{
public:
  albaGUISashPanel (wxWindow* parent,
                wxWindowID id =-1, 
                wxDirection side = wxRIGHT, 
                int defaultsize = 100,
                wxString menu_string = "",
                bool sizable = true);
  virtual ~albaGUISashPanel();

  /** Put the panel on the panel stack removing the previous panel. */
	bool Put(wxWindow* win);

	/** Show/Hide the Sash panel. */
	virtual bool Show(bool show);
  wxWindow *GetCurrGui() {return m_CurrentGui;};

protected:
  wxWindow         *m_CurrentGui;
  wxMenuBar        *m_MenuBar;
  wxFrame          *m_Frame;

  virtual void OnHide( wxCommandEvent &event );
DECLARE_EVENT_TABLE()
};
#endif
