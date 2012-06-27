/*=========================================================================

 Program: MAF2
 Module: mafGUIColorSwatch
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIColorSwatch_H__
#define __mafGUIColorSwatch_H__

#include "mafColor.h"
//----------------------------------------------------------------------------

/** mafGUIColorSwatch : small widget representing an RGBA color.
Transparency is visually suggested using a b/w chessboard
which become more visible as long as the alpha increase.

*/
class MAF_EXPORT mafGUIColorSwatch: public wxPanel
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
