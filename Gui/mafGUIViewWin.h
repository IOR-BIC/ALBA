/*=========================================================================

 Program: MAF2
 Module: mafGUIViewWin
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIViewWin_H__
#define __mafGUIViewWin_H__

//----------------------------------------------------------------------------
// forward references
//----------------------------------------------------------------------------
class mafView;

/**
  class name: mafGUIViewWin 
  This class is used only to trap the OnSize event.
*/
class mafGUIViewWin : public wxPanel
{
public:
  /** constructor */
  mafGUIViewWin(
        wxWindow *parent, 
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0
        );
  
  /** called when resizing the object */
	void OnSize(wxSizeEvent &event);
  mafView  *m_Owner;
/** Event table declaration macro  */
DECLARE_EVENT_TABLE()
};
#endif
