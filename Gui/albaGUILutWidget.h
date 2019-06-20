/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILutWidget
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUILutWidget_H__
#define __albaGUILutWidget_H__

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaColor.h"
#include "vtkLookupTable.h"
//----------------------------------------------------------------------------
/** albaGUILutWidget : Helper class for albaLutEditor
@sa albaLutEditor
*/
class albaGUILutWidget: public wxPanel, public albaObserver
{
public:
  albaGUILutWidget(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(300,290), long style = wxTAB_TRAVERSAL /*| wxSUNKEN_BORDER */);

  virtual void SetListener(albaObserver *Listener) {m_Listener = Listener;};
  void OnEvent( albaEventBase *event );

  void     SetSelectionColor(albaColor col);
  albaColor GetSelectionColor(); // return the color of the first selected entry
  void     ShadeSelectionInRGB();    
  void     ShadeSelectionInHSV();    
  void     GetSelection(int *min, int *max, int *num=NULL); 
  
  struct LutEntry {
    albaColor      m_Color;
    bool          m_Selected;
  } m_LutEntry[257];

  void SetNumEntry( int num );

  //void ReadLut(vtkLookupTable *lut);
  //void WriteLut(vtkLookupTable *lut);
  void SetLut(vtkLookupTable *lut);

protected:
  albaObserver     *m_Listener;
  vtkLookupTable  *m_Lut;
  
  bool     m_Dragging;
  int      m_DragBeginIdx;

  int m_SelectionRowMin;
  int m_SelectionColMin;
  int m_SelectionRowMax;
  int m_SelectionColMax; //selection rectangle expressed using row/cols coord -- used during drag operation

  wxBitmap m_Bmp;      //the bitmap with the LUT
  wxBitmap m_Bmp2;     //the bitmap with the LUT and the Selection Indicator
  wxPoint  m_BmpPosition;       //position of the bitmap
  wxSize   m_BmpSize;       //size of the bitmap

  int m_NumEntry;
  int m_EntryW;
  int m_EntryH;
  int m_EntryM; // margin
  int m_EntryPerRow;
  
  void InitBitmaps();        
  void DrawEntries();      
  void DrawEntry(int idx); 
  void DrawSelection(); 
  void OnEraseBackground(wxEraseEvent& event) {};  // overriden to prevent flickering
  void OnPaint(wxPaintEvent &event);                

  int  MouseToIndex(wxMouseEvent &event);
  void OnLeftMouseButtonDown(wxMouseEvent &event);
  void OnLeftMouseButtonUp(wxMouseEvent &event);
  void OnMouseMotion(wxMouseEvent &event);
  
  DECLARE_EVENT_TABLE()
};
#endif
