/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILutSwatch
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUILutSwatch_H__
#define __albaGUILutSwatch_H__

#include "albaEvent.h"
#include "albaDecl.h"
#include "albaGUILutSwatch.h"
#include "vtkLookupTable.h"
//----------------------------------------------------------------------------
/** albaGUILutSwatch : widget representing a LUT, usually used to call the LutEditor.
@sa albaGUILutEditor
*/
class ALBA_EXPORT albaGUILutSwatch: public wxPanel
{
public:
  albaGUILutSwatch(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(100,50), long style = wxTAB_TRAVERSAL );

  void     SetLut(vtkLookupTable *lut); 
  /** 
  -if b=true LeftMouseButtonDown will pop-up the LutEditor
  -the user is notified anyway
  -default = false
  */
  void     SetEditable(bool b); 

	virtual bool Enable(bool enable = true);

  virtual void SetListener(albaObserver *Listener) {m_Listener = Listener;};
protected:
  albaObserver *m_Listener;

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
	int							m_oldW;
	int							m_oldH;
	bool						m_Enabled;
  DECLARE_EVENT_TABLE()
};
#endif
