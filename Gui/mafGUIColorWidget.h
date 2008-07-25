/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIColorWidget.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUIColorWidget_H__
#define __mafGUIColorWidget_H__

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafColor.h"
#include "mafGUIColorSwatch.h"
/** mafGUIColorWidget : helper class for mafGUILutEditor and mafGUIColorEditor.
    Usage: Create it, set the initial color, set the Listener.
    The listener will be informed when the user has changed the color.
*/
class mafGUIColorWidget: public wxPanel, public mafObserver
{
public:
  mafGUIColorWidget(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(350,135), long style = wxTAB_TRAVERSAL);

  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  void      SetColor   (mafColor c);
  mafColor  GetColor   (); 

  void OnEvent( mafEventBase *event );

protected:
  mafObserver *m_Listener;

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
  mafColor         m_Color;
  mafColor         m_Clipboard;

  mafGUIColorSwatch  *m_ColorSwatch;
  mafGUIColorSwatch  *m_PrevColorSwatch;

  DECLARE_EVENT_TABLE()
};
#endif
