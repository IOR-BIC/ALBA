/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIColorSwatch
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIColorSwatch_H__
#define __albaGUIColorSwatch_H__

#include "albaColor.h"
//----------------------------------------------------------------------------

/** albaGUIColorSwatch : small widget representing an RGBA color.
Transparency is visually suggested using a b/w chessboard
which become more visible as long as the alpha increase.

*/
class ALBA_EXPORT albaGUIColorSwatch: public wxPanel
{
public:
  albaGUIColorSwatch(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(100,50), long style = wxTAB_TRAVERSAL);

  void     SetColor      (albaColor c); 
  albaColor GetColor   () {return m_Color;}; 

protected:

  wxBitmap m_Bmp;      
  void OnEraseBackground(wxEraseEvent& event) {};  // overrided to prevent flickering
  void OnPaint(wxPaintEvent &event);                

  albaColor      m_Color;
  DECLARE_EVENT_TABLE()
};
#endif
