/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIColorWidget
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIColorWidget_H__
#define __albaGUIColorWidget_H__

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaColor.h"
#include "albaGUIColorSwatch.h"
/** albaGUIColorWidget : helper class for albaGUILutEditor and albaGUIColorEditor.
    Usage: Create it, set the initial color, set the Listener.
    The listener will be informed when the user has changed the color.
*/
class ALBA_EXPORT albaGUIColorWidget: public wxPanel, public albaObserver
{
public:
  albaGUIColorWidget(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(350,135), long style = wxTAB_TRAVERSAL);

  virtual void SetListener(albaObserver *Listener) {m_Listener = Listener;};

  void      SetColor   (albaColor c);
  albaColor  GetColor   (); 

  void OnEvent( albaEventBase *event );

protected:
  albaObserver *m_Listener;

  wxBitmap m_HsvBmp; //the bitmap with the hsv chooser
  wxPoint  m_HsvPosition;   //position of the hsv chooser  
  int      m_HsvSize;    //size     of the hsv chooser  
  int      m_HsvInnerRadius;    //inner radius of the h cirle 
  int      m_HsvOuterRadius;    //outer radius of the h cirle 
  wxPoint  m_SvPosition;  //position of the sv chooser within the hsv bmp 
  int      m_SvSize;   //size     of the sv chooser  

  wxBitmap m_AlphaBmp; //the bitmap with the alpha chooser
  wxPoint  m_AlphaBmpPosition;     //position of the alpha chooser
  wxSize   m_AlphaBmpSize;      //size of the alpha chooser

  int      m_CursorSize;  //cursor size
  wxPoint  m_AlphaCursorPosition;      //position of the a cursor
  wxPoint  m_HsvCursorPosition;      //position of the h cursor
  wxPoint  m_SvCursorPosition;     //position of the sv cursor
  int      m_Dragging;     //= DRAG_NONE | DRAG_H | DRAG_SV

  void InitBitmap();         // create the bitmaps, draw the h circle
  void UpdateHSVBitmap();    // update the sv square (H changed)
  void UpdateAlphaBitmap();  // update the alpha chooser (A changed)
  void PaintHandles(wxPaintDC &DC);
  void OnPaint(wxPaintEvent &event);                

  void OnLeftMouseButtonDown(wxMouseEvent &event);
  void OnLeftMouseButtonUp(wxMouseEvent &event);
  void OnMouseMotion(wxMouseEvent &event);
  void OnColorChanged(bool notify = true);
  
  // widgets
  albaColor         m_Color;
  albaColor         m_Clipboard;

  albaGUIColorSwatch  *m_ColorSwatch;
  albaGUIColorSwatch  *m_PrevColorSwatch;

  DECLARE_EVENT_TABLE()
};
#endif
