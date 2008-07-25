/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIColorSwatch.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUIColorSwatch_H__
#define __mafGUIColorSwatch_H__

#include "mafColor.h"
//----------------------------------------------------------------------------

/** mafGUIColorSwatch : small widget representing an RGBA color.
Transparency is visually suggested using a b/w chessboard
which become more visible as long as the alpha increase.

*/
class mafGUIColorSwatch: public wxPanel
{
public:
  mafGUIColorSwatch(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(100,50), long style = wxTAB_TRAVERSAL);

  void     SetColor      (mafColor c); 
  mafColor GetColor   () {return m_Color;}; 

protected:

  wxBitmap m_Bmp;      
  void OnEraseBackground(wxEraseEvent& event) {};  // overrided to prevent flickering
  void OnPaint(wxPaintEvent &event);                

  mafColor      m_Color;
  DECLARE_EVENT_TABLE()
};
#endif
