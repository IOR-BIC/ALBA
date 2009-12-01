/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIViewWin.h,v $
  Language:  C++
  Date:      $Date: 2009-12-01 14:36:33 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
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
