/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgColorSwatch.h,v $
  Language:  C++
  Date:      $Date: 2007-09-04 16:22:15 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgColorSwatch_H__
#define __mmgColorSwatch_H__

#include "mafColor.h"
//----------------------------------------------------------------------------

/** mmgColorSwatch : small widget representing an RGBA color.
Transparency is visually suggested using a b/w chessboard
which become more visible as long as the alpha increase.

*/
class mmgColorSwatch: public wxPanel
{
public:
  mmgColorSwatch(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
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
