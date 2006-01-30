/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLutSwatch.h,v $
  Language:  C++
  Date:      $Date: 2006-01-30 13:53:11 $
  Version:   $Revision: 1.3 $
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
  void     SetEditable(bool b) {m_Editable = b;};  

  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};
protected:
  mafObserver *m_Listener;

  wxBitmap m_Bmp;      
  void OnEraseBackground(wxEraseEvent& event) {};  // overrided to prevent flickering
  void OnPaint(wxPaintEvent &event);                
  void Update();                

  void OnLeftMouseButtonDown(wxMouseEvent &event);
  void OnLeftMouseButtonUp(wxMouseEvent &event);
  void OnMouseMotion(wxMouseEvent &event);
  
  // widgets
  vtkLookupTable *m_Lut;
  bool            m_MouseInWindow;
  wxString        m_Tip;
  int             m_MouseX;
  unsigned long   m_UpdateTime;
  bool            m_Editable;
  wxFont          m_Font;
  DECLARE_EVENT_TABLE()
};
#endif
