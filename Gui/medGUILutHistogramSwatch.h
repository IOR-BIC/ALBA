/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGUILutHistogramSwatch.h,v $
  Language:  C++
  Date:      $Date: 2011-07-13 12:38:21 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUILutSwatch_H__
#define __mafGUILutSwatch_H__

#include "mafEvent.h"
#include "mafDecl.h"
#include "vtkLookupTable.h"
//----------------------------------------------------------------------------
/** mafGUILutSwatch : widget representing a LUT, usually used to call the LutEditor.
@sa mafGUILutEditor
*/
class mafVME;

class MAF_EXPORT medGUILutHistogramSwatch: public wxPanel
{
public:
  /** constructor. */
  medGUILutHistogramSwatch(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(100,50), long style = wxTAB_TRAVERSAL );

  /** Set the Lock Up Table */
  void     SetLut(vtkLookupTable *lut); 
  /** Set the VME for the histogram */
  void     SetVME(mafVME *vme); 
  
  /** 
  -if b=true LeftMouseButtonDown will pop-up the LutEditor
  -the user is notified anyway
  -default = false
  */
  void     SetEditable(bool b) {m_Editable = b;};  

  /** Set the event listener */
  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};
protected:
  mafObserver *m_Listener;

  wxBitmap m_Bmp;      
  /** overrided to prevent flickering */
  void OnEraseBackground(wxEraseEvent& event) {};  
  /** OnPaint event handler */ 
  void OnPaint(wxPaintEvent &event);                
  /* Update Function */
  void Update();                

  /* If Editable run medGUILutHistogramEditor */
  void OnLeftMouseButtonDown(wxMouseEvent &event);
  /* Do nothing */
  void OnLeftMouseButtonUp(wxMouseEvent &event);
  /* Show/update the label */
  void OnMouseMotion(wxMouseEvent &event);
  
  // widgets
  vtkLookupTable *m_Lut;
  mafVME         *m_VME;
  bool            m_MouseInWindow;
  wxString        m_Tip;
  int             m_MouseX;
  unsigned long   m_UpdateTime;
  bool            m_Editable;
  wxFont          m_Font;
  DECLARE_EVENT_TABLE()
};
#endif
