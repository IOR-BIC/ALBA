/*=========================================================================

 Program: MAF2Medical
 Module: medGUILutHistogramSwatch
 Authors: Crimi Gianluigi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUILutSwatch_H__
#define __mafGUILutSwatch_H__

#include "mafDefines.h"
#include "mafEvent.h"
#include "vtkLookupTable.h"
//----------------------------------------------------------------------------
/** mafGUILutSwatch : widget representing a LUT, usually used to call the LutEditor.
@sa mafGUILutEditor
*/
class vtkDataSet;
class mmaVolumeMaterial;
class mafGUI;

class MAF_EXPORT medGUILutHistogramSwatch: public wxPanel
{
public:
  /** constructor. */
  medGUILutHistogramSwatch(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(184,18), long style = wxTAB_TRAVERSAL );

  /** constructor. */
  medGUILutHistogramSwatch::medGUILutHistogramSwatch(mafGUI *parent, wxWindowID id, wxString name, vtkDataSet *dataSet, mmaVolumeMaterial *material, wxSize size = wxSize(184,18), bool showText=true);

  /** Set the Lock Up Table */
  void SetMaterial(mmaVolumeMaterial *material);

  /** Set the VME for the histogram */
  void SetDataSet(vtkDataSet *dataSet); 
  
  /** 
  -if b=true LeftMouseButtonDown will pop-up the LutEditor
  -the user is notified anyway
  -default = false
  */
  void SetEditable(bool b) {m_Editable = b;};  

  /** Activate/Deactivate threshold view on LUT */
  void ShowThreshold(bool b);

  /** Activate/Deactivate threshold view on LUT */
  void EnableOverHighlight(bool b) {m_OverHighlight=b;};

  /** return true if the pop-up menu is enabled */
  bool IsEditable(){return m_Editable;};

  /* method for updating GUI on LUT update */
  void Modified();

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

  /** Update the panel when resized */
  void OnSize(wxSizeEvent &event);

  /* Get the lut index by the x position over the widget */
  int GetLutIndexByPos(float fullWidth, float x);
  
  int m_ShowThreshold;
  int m_OverHighlight;
  int m_Highlighted;
  int m_LastHighlighted;
  mmaVolumeMaterial *m_Material;
  vtkLookupTable *m_Lut;
  vtkDataSet *m_DataSet;

  // widgets
  bool            m_MouseInWindow;
  wxString        m_Tip;
  int             m_MouseX;
  unsigned long   m_UpdateTime;
  bool            m_Editable;
  wxFont          m_Font;
  DECLARE_EVENT_TABLE()
};
#endif
