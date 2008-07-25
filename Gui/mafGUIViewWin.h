/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIViewWin.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
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

//----------------------------------------------------------------------------
// mafGUIViewWin - This class is used only to trap the OnSize event
//----------------------------------------------------------------------------
class mafGUIViewWin : public wxPanel
{
public:
  mafGUIViewWin(
        wxWindow *parent, 
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0
        );
  
	void OnSize(wxSizeEvent &event);
  mafView  *m_Owner;
DECLARE_EVENT_TABLE()
};
#endif
