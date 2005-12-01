/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLutSwatch.h,v $
  Language:  C++
  Date:      $Date: 2005-12-01 15:22:06 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgLutSwatch_H__
#define __mmgLutSwatch_H__

#include "mafEvent.h"
#include "mafDecl.h"
#include "mmgLutSwatch.h"
#include "vtkLookupTable.h"
//----------------------------------------------------------------------------
/** mmgLutSwatch : widget representing a LUT, usually used to call the LutEditor.
@sa mmgLutEditor
*/
class mmgLutSwatch: public wxPanel
{
public:
  mmgLutSwatch(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(100,50), long style = wxTAB_TRAVERSAL );

  void     SetLut(vtkLookupTable *lut); 
  /** 
  -if b=true LeftMouseButtonDown will pop-up the LutEditor
  -the user is notified anyway
  -default = false
  */
  void     SetEditable(bool b) {m_editable = b;};  

  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};
protected:
  mafObserver *m_Listener;

  wxBitmap m_bmp;      
  void OnEraseBackground(wxEraseEvent& event) {};  // overrided to prevent flickering
  void OnPaint(wxPaintEvent &event);                
  void Update();                

  void OnLeftMouseButtonDown(wxMouseEvent &event);
  //void OnLeftMouseButtonUp(wxMouseEvent &event);
  void OnMouseMotion(wxMouseEvent &event);
  
  // widgets
  vtkLookupTable *m_lut;
  bool            m_mouse_in_window;
  wxString        m_tip;
  int             m_mouse_x;
  unsigned long   m_updatetime;
  bool            m_editable;
  wxFont m_font;
  DECLARE_EVENT_TABLE()
};
#endif

