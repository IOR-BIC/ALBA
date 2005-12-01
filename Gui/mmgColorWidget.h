/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgColorWidget.h,v $
  Language:  C++
  Date:      $Date: 2005-12-01 15:22:05 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgColorWidget_H__
#define __mmgColorWidget_H__

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafColor.h"
#include "mmgColorSwatch.h"
/** mmgColorWidget : helper class for mmgLutEditor and mmgColorEditor.
    Usage: Create it, set the initial color, set the Listener.
    The listener will be informed when the user has changed the color.
*/
class mmgColorWidget: public wxPanel, public mafObserver
{
public:
  mmgColorWidget(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(350,135), long style = wxTAB_TRAVERSAL);

  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  void      SetColor   (mafColor c);
  mafColor  GetColor   (); 

  void OnEvent( mafEventBase *event );

protected:
  mafObserver *m_Listener;

  wxBitmap m_hsv_bmp; //the bitmap with the hsv chooser
  wxPoint  m_h_pos;   //position of the hsv chooser  
  int      m_h_sz;    //size     of the hsv chooser  
  int      m_h_r1;    //inner radius of the h cirle 
  int      m_h_r2;    //outer radius of the h cirle 
  wxPoint  m_sv_pos;  //position of the sv chooser within the hsv bmp 
  int      m_sv_sz;   //size     of the sv chooser  

  wxBitmap m_alpha_bmp; //the bitmap with the alpha chooser
  wxPoint  m_a_pos;     //position of the alpha chooser
  wxSize   m_a_sz;      //size of the alpha chooser

  int      m_cursor_size;  //cursor size
  wxPoint  m_acursor;      //position of the a cursor
  wxPoint  m_hcursor;      //position of the h cursor
  wxPoint  m_svcursor;     //position of the sv cursor
  int      m_dragging;     //= DRAG_NONE | DRAG_H | DRAG_SV

  void InitBitmap();         // create the bitmaps, draw the h circle
  void UpdateHSVBitmap();    // update the sv square (H changed)
  void UpdateAlphaBitmap();  // update the alpha chooser (A changed)
  void PaintHandles(wxPaintDC &DC);
  void OnEraseBackground(wxEraseEvent& event) {};  // overrided to prevent flickering
  void OnPaint(wxPaintEvent &event);                

  void OnLeftMouseButtonDown(wxMouseEvent &event);
  void OnLeftMouseButtonUp(wxMouseEvent &event);
  void OnMouseMotion(wxMouseEvent &event);
  void OnColorChanged(bool notify = true);
  
  // widgets
  mafColor         m_color;
  mafColor         m_clipboard;

  mmgColorSwatch  *m_col_swatch;
  mmgColorSwatch  *m_prevcol_swatch;

  DECLARE_EVENT_TABLE()
};
#endif

