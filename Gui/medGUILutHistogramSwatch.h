/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGUILutHistogramSwatch.h,v $
  Language:  C++
  Date:      $Date: 2011-07-14 08:23:37 $
  Version:   $Revision: 1.1.2.3 $
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
  void SetLut(vtkLookupTable *lut);

  /** Assign the external lookup table to the widget.*/
  vtkLookupTable *GetLut(){return m_Lut;};

  /** Set the VME for the histogram */
  void SetVME(mafVME *vme); 
  
  /** 
  -if b=true LeftMouseButtonDown will pop-up the LutEditor
  -the user is notified anyway
  -default = false
  */
  void SetEditable(bool b) {m_Editable = b;};  

  /* return true if the pop-up menu is enabled */
  bool IsEditable(){return m_Editable;};

  /** Set the event listener */
  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Get the event listener */
  virtual mafObserver *GetListener() {return m_Listener;};


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
