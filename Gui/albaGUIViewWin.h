/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIViewWin
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIViewWin_H__
#define __albaGUIViewWin_H__

//----------------------------------------------------------------------------
// forward references
//----------------------------------------------------------------------------
class albaView;

/**
  class name: albaGUIViewWin 
  This class is used only to trap the OnSize event.
*/
class albaGUIViewWin : public wxPanel
{
public:
  /** constructor */
  albaGUIViewWin(
        wxWindow *parent, 
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0
        );
  
  /** called when resizing the object */
	void OnSize(wxSizeEvent &event);
  albaView  *m_Owner;
/** Event table declaration macro  */
DECLARE_EVENT_TABLE()
};
#endif
