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


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <wx/image.h>
#include <math.h>

#include "albaGUILutWidget.h"
#include "albaGUIColorWidget.h" 
#include "albaGUIValidator.h"

//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------
  
//----------------------------------------------------------------------------
// albaGUILutWidget
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUILutWidget,wxPanel)
  EVT_PAINT(albaGUILutWidget::OnPaint)
  EVT_ERASE_BACKGROUND(albaGUILutWidget::OnEraseBackground) 

  EVT_LEFT_DOWN(albaGUILutWidget::OnLeftMouseButtonDown)
  EVT_LEFT_UP(albaGUILutWidget::OnLeftMouseButtonUp)
  EVT_MOTION(albaGUILutWidget::OnMouseMotion)

END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUILutWidget::albaGUILutWidget(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)         
//----------------------------------------------------------------------------
{
	m_Listener = NULL;
  m_Lut = NULL;

  m_Dragging = false;
  m_SelectionRowMin=m_SelectionColMin=m_SelectionRowMax=m_SelectionColMax=0;
  
  m_NumEntry    = 256;
  m_EntryW      = 15;
  m_EntryH      = 15;
  m_EntryM      = 3;
  m_EntryPerRow = 16;

  m_BmpPosition = wxPoint(0,0);
  m_BmpSize = wxSize((m_EntryW + m_EntryM)*m_EntryPerRow + m_EntryM, (m_EntryH + m_EntryM)*16 + m_EntryM);      
  
  float s = 255.0/m_NumEntry;
  for(int i=0; i<m_NumEntry; i++)
  {
    m_LutEntry[i].m_Color = albaColor(i*s,i*s,i*s);
    m_LutEntry[i].m_Selected = false;
  }

  int m = 255;
  m_LutEntry[0].m_Color = albaColor(0,0,0);
  m_LutEntry[1].m_Color = albaColor(m,m,m);
  m_LutEntry[2].m_Color = albaColor(m,0,0);
  m_LutEntry[3].m_Color = albaColor(m,m,0);
  m_LutEntry[4].m_Color = albaColor(0,m,0);
  m_LutEntry[5].m_Color = albaColor(0,m,m);
  m_LutEntry[6].m_Color = albaColor(0,0,m);
  m_LutEntry[7].m_Color = albaColor(m,0,m);

  InitBitmaps();
  DrawEntries();
  DrawSelection();
}
//----------------------------------------------------------------------------
void albaGUILutWidget::SetNumEntry( int num )
//----------------------------------------------------------------------------
{
  if(num<1)   num=1;
  if(num>256) num=256;
  m_NumEntry = num;

  if(m_Lut) m_Lut->SetNumberOfTableValues(m_NumEntry);

  InitBitmaps();        
  DrawEntries(); 
  DrawSelection();
  Refresh();
}
//----------------------------------------------------------------------------
void albaGUILutWidget::InitBitmaps()
//----------------------------------------------------------------------------
{
  m_Bmp = wxBitmap(m_BmpSize.GetWidth(), m_BmpSize.GetHeight() );
  m_Bmp2= wxBitmap(m_BmpSize.GetWidth(), m_BmpSize.GetHeight() );
  
  wxMemoryDC dc;  
  dc.SelectObject(m_Bmp);

  wxBrush brush( GetBackgroundColour(),wxSOLID );
  dc.SetBackground(brush);
  dc.Clear();
}
//----------------------------------------------------------------------------
void albaGUILutWidget::OnPaint(wxPaintEvent &event)
//----------------------------------------------------------------------------
{
  wxPaintDC pdc(this);
  wxMemoryDC mdc;
  mdc.SelectObject(m_Bmp2);
  pdc.Blit(m_BmpPosition.x, m_BmpPosition.y, m_BmpSize.GetWidth(), m_BmpSize.GetHeight(), &mdc, 0,0);
}
//----------------------------------------------------------------------------
void albaGUILutWidget::DrawEntries()
//----------------------------------------------------------------------------
{
  for( int i=0; i<m_NumEntry; i++ )
   DrawEntry(i);
}
//----------------------------------------------------------------------------
void albaGUILutWidget::DrawEntry(int idx)
//----------------------------------------------------------------------------
{
  int h  = m_EntryH;
  int w  = m_EntryW;
  int m  = m_EntryM;

  unsigned char *data = new unsigned char[h*w*3];
  unsigned char *p = data;

  int x,y;
  for(y=0; y<h; y++)
  {
    for(x=0; x<w; x++)
    {
      albaColor c = albaColor::CheckeredColor(m_LutEntry[idx].m_Color, x,y);
      *p++ = c.m_Red;
      *p++ = c.m_Green;
      *p++ = c.m_Blue;
    }
  }

  wxImage img(w,h,data); // Data will be freed by the image
  wxBitmap bmp(img);

  wxMemoryDC dc1;
  dc1.SelectObject(bmp);

  wxMemoryDC dc2;
  dc2.SelectObject(m_Bmp);
  
  int x0 = m + (w+m) * (idx - m_EntryPerRow * (idx/m_EntryPerRow) );
  int y0 = m + (h+m) * (idx/m_EntryPerRow);
  
  dc2.Blit(x0, y0, w, h, &dc1, 0, 0);

}
//----------------------------------------------------------------------------
void albaGUILutWidget::DrawSelection()
//----------------------------------------------------------------------------
{
  wxMemoryDC dc1;
  dc1.SelectObject(m_Bmp);

  wxMemoryDC dc2;
  dc2.SelectObject(m_Bmp2);
  
  dc2.Blit(0, 0, m_BmpSize.GetWidth(), m_BmpSize.GetHeight(), &dc1, 0, 0);

  wxPen pen(*wxBLACK,1,wxSOLID);
  dc2.SetPen(*wxBLACK_PEN);
  dc2.SetBrush(*wxWHITE_BRUSH);

  for( int i=0; i<m_NumEntry; i++ )
  {
    if(m_LutEntry[i].m_Selected)
    {
      int h  = m_EntryH;
      int w  = m_EntryW;
      int m  = m_EntryM;
      int sz = 10;
      int x1 = m+(w+m) * (i - m_EntryPerRow * (i/m_EntryPerRow) ) ;
      int y1 = m+(w+m) * (i/m_EntryPerRow)                        ;
      x1 += w/2 - sz/2;
      y1 += h/2 - sz/2;

      dc2.DrawEllipse(x1,y1,sz,sz);
    }
  }

  dc1.SelectObject(wxNullBitmap);
  dc2.SelectObject(wxNullBitmap);
}
//----------------------------------------------------------------------------
int albaGUILutWidget::MouseToIndex(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  int r = (event.m_y - m_BmpPosition.x )/(m_EntryH+m_EntryM);
  int c = (event.m_x - m_BmpPosition.y )/(m_EntryW+m_EntryM);
  int idx = c + m_EntryPerRow*r;
  if( idx<0 || idx>m_NumEntry) idx =-1;
  return idx;
}
//----------------------------------------------------------------------------
void albaGUILutWidget::OnEvent( albaEventBase *event )
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(event))
  {
    switch(e->GetId())
    {
    case 0:
    default:
      e->Log();
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void albaGUILutWidget::SetSelectionColor(albaColor col)
//----------------------------------------------------------------------------
{
  for( int i=0; i<m_NumEntry; i++ )
  {
    if(m_LutEntry[i].m_Selected)
    {
			 m_LutEntry[i].m_Color = col;
			
			 // here LUT is kept in sync
			if(m_Lut)
			{
				double r,g,b,a;
				m_LutEntry[i].m_Color.GetFloatRGB(&r,&g,&b,&a);
				m_Lut->SetTableValue(i,r,g,b,a);
			}
     
      DrawEntry(i);
    }
  }
  DrawSelection();
  Refresh();
}
//----------------------------------------------------------------------------
albaColor albaGUILutWidget::GetSelectionColor()
//----------------------------------------------------------------------------
{
  albaColor c;
  for( int i=0; i<m_NumEntry; i++ )
    if(m_LutEntry[i].m_Selected)
      c = m_LutEntry[i].m_Color;
  return c;
}
//----------------------------------------------------------------------------
void albaGUILutWidget::ShadeSelectionInHSV()
//----------------------------------------------------------------------------
{
  int min, max, num;
  GetSelection(&min, &max, &num);
  if( num < 2) return; //nothing to do

  albaColor c1 = m_LutEntry[min].m_Color;
  albaColor c2 = m_LutEntry[max].m_Color;
  
  int i;
  int counter = 0;
  for( i=min; i<=max; i++ )
  {
    if(m_LutEntry[i].m_Selected)
    {
			double r,g,b,a;
      float t = (1.0*counter)/num;
      m_LutEntry[i].m_Color = albaColor::InterpolateHSV(c1,c2,t);
			m_LutEntry[i].m_Color.GetFloatRGB(&r,&g,&b,&a);
			m_Lut->SetTableValue(i,r,g,b,a);
      DrawEntry(i);
      counter++;
    }
  }
  DrawSelection();
  Refresh();
}
//----------------------------------------------------------------------------
void albaGUILutWidget::ShadeSelectionInRGB()
//----------------------------------------------------------------------------
{
  int min, max, num;
  GetSelection(&min, &max, &num);
  if( num < 2) return; //nothing to do

  albaColor c1 = m_LutEntry[min].m_Color;
  albaColor c2 = m_LutEntry[max].m_Color;
  
  int i;
  int counter = 0;
  for( i=min; i<=max; i++ )
  {
    if(m_LutEntry[i].m_Selected)
    {
			double r,g,b,a;
      float t = (1.0*counter)/num;
      m_LutEntry[i].m_Color = albaColor::InterpolateRGB(c1,c2,t);
			m_LutEntry[i].m_Color.GetFloatRGB(&r,&g,&b,&a);
			m_Lut->SetTableValue(i,r,g,b,a);
      DrawEntry(i);
      counter++;
    }
  }
  DrawSelection();
  Refresh();
}
//----------------------------------------------------------------------------
// no selection        -> min = max  = -1
// single selection    -> min = max  
// multiple selection  -> max <> max
// num == number of selected entries
void albaGUILutWidget::GetSelection(int *min, int *max, int *num)
//----------------------------------------------------------------------------
{
  *min = *max = -1;
  if(num != NULL) *num =0;
  int i;

  for( i=0; i<m_NumEntry; i++ )
  {
    if(m_LutEntry[i].m_Selected)
    {
        if(num != NULL) (*num)++;
        if(*min == -1) *min = i; 
        *max = i;
    } 
  }
}
//----------------------------------------------------------------------------
void albaGUILutWidget::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  CaptureMouse();
  m_Dragging = true;

  int idx = MouseToIndex(event);
  if (idx != -1)
  {
    if(! event.ControlDown() )
    {
      for(int i=0; i<m_NumEntry; i++ )
        m_LutEntry[i].m_Selected = false; 
    }

    if(! event.ControlDown() )
       m_LutEntry[idx].m_Selected = true; 
    else
       m_LutEntry[idx].m_Selected = ! m_LutEntry[idx].m_Selected; 

    DrawSelection();
    Refresh();
    m_DragBeginIdx = idx;
  }
  else
    m_DragBeginIdx = -1;
}
//----------------------------------------------------------------------------
void albaGUILutWidget::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if(m_Dragging)
  {
    int idx = MouseToIndex(event);
    if (idx != -1 && idx != m_DragBeginIdx)
    {
      int i;
			for(i=0; i<m_NumEntry; i++ )
        m_LutEntry[i].m_Selected = false; 

      int min = (idx < m_DragBeginIdx) ? idx : m_DragBeginIdx;
      int max = (idx > m_DragBeginIdx) ? idx : m_DragBeginIdx;

      for( i=min; i<=max; i++ )
           m_LutEntry[i].m_Selected = true; 
      
      DrawSelection();
      Refresh();
    }
  }
}
//----------------------------------------------------------------------------
void albaGUILutWidget::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if(GetCapture() == this) ReleaseMouse();
  m_SelectionRowMin=m_SelectionColMin=m_SelectionRowMax=m_SelectionColMax=-1;

  if(m_Dragging)
  {
    m_Dragging = false;

    // signal to Listener that the selection has changed
    albaEventMacro(albaEvent(this,GetId()));
  }
}
//----------------------------------------------------------------------------
void albaGUILutWidget::SetLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  m_Lut = lut;
  if(m_Lut == NULL)
  {
    SetNumEntry(0);
    return;
  }

  int n = m_Lut->GetNumberOfTableValues();
  if (n>256) n=256;

  albaColor c;
  for( int i=0; i<n; i++ )
  {
    double *p = NULL;
    p = lut->GetTableValue(i);
    assert(p != NULL);
    c.SetFloatRGB(p[0], p[1], p[2], p[3]);
    m_LutEntry[i].m_Color = c; 
  }
  SetNumEntry(n);
}


