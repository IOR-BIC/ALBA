/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgViewWin.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:30 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgViewWin_H__
#define __mmgViewWin_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// forward references
//----------------------------------------------------------------------------
class mafView;
//----------------------------------------------------------------------------
// mmgViewWin - This class is used only to trap the OnSize event
//----------------------------------------------------------------------------
class mmgViewWin : public wxPanel
{
public:
  mmgViewWin(
        wxWindow *parent, 
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0
        );
  
	void OnSize(wxSizeEvent &event);
  mafView  *m_owner;     
DECLARE_EVENT_TABLE()
};
#endif
