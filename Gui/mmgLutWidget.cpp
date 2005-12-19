/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgLutWidget.cpp,v $
Language:  C++
Date:      $Date: 2005-12-19 15:46:07 $
Version:   $Revision: 1.2 $
Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <wx/image.h>
#include <math.h>

#include "mmgLutWidget.h"
#include "mmgColorWidget.h" 
#include "mmgValidator.h"

//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------
  
//----------------------------------------------------------------------------
// mmgLutWidget
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgLutWidget,wxPanel)
  EVT_PAINT(mmgLutWidget::OnPaint)
  EVT_ERASE_BACKGROUND(mmgLutWidget::OnEraseBackground) 

  EVT_LEFT_DOWN(mmgLutWidget::OnLeftMouseButtonDown)
  EVT_LEFT_UP(mmgLutWidget::OnLeftMouseButtonUp)
  EVT_MOTION(mmgLutWidget::OnMouseMotion)

END_EVENT_TABLE()

//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum 
{
};
//----------------------------------------------------------------------------
mmgLutWidget::mmgLutWidget(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)         
//----------------------------------------------------------------------------
{
	m_Listener = NULL;
  m_lut = NULL;

  m_dragging = false;
  m_r1=m_c1=m_r2=m_c2=0;
  
  m_numEntry    = 256;
  m_EntryW      = 15;
  m_EntryH      = 15;
  m_EntryM      = 3;
  m_EntryPerRow = 16;

  m_bp = wxPoint(0,0);
  m_bs = wxSize((m_EntryW + m_EntryM)*m_EntryPerRow + m_EntryM, (m_EntryH + m_EntryM)*16 + m_EntryM);      
  
  float s = 255.0/m_numEntry;
  for(int i=0; i<m_numEntry; i++)
  {
    m_lutEntry[i].m_c = mafColor(i*s,i*s,i*s);
    m_lutEntry[i].m_selected = false;
  }

  int m = 255;
  m_lutEntry[0].m_c = mafColor(0,0,0);
  m_lutEntry[1].m_c = mafColor(m,m,m);
  m_lutEntry[2].m_c = mafColor(m,0,0);
  m_lutEntry[3].m_c = mafColor(m,m,0);
  m_lutEntry[4].m_c = mafColor(0,m,0);
  m_lutEntry[5].m_c = mafColor(0,m,m);
  m_lutEntry[6].m_c = mafColor(0,0,m);
  m_lutEntry[7].m_c = mafColor(m,0,m);

  InitBitmaps();
  DrawEntries();
  DrawSelection();
}
//----------------------------------------------------------------------------
void mmgLutWidget::SetNumEntry( int num )
//----------------------------------------------------------------------------
{
  if(num<1)   num=1;
  if(num>256) num=256;
  m_numEntry = num;

  if(m_lut) m_lut->SetNumberOfTableValues(m_numEntry);

  InitBitmaps();        
  DrawEntries(); 
  DrawSelection();
  Refresh();
}
//----------------------------------------------------------------------------
void mmgLutWidget::InitBitmaps()
//----------------------------------------------------------------------------
{
  m_bmp = wxBitmap(m_bs.GetWidth(), m_bs.GetHeight() );
  m_bmp2= wxBitmap(m_bs.GetWidth(), m_bs.GetHeight() );
  
  wxMemoryDC dc;  
  dc.SelectObject(m_bmp);

  wxBrush brush( GetBackgroundColour(),wxSOLID );
  dc.SetBackground(brush);
  dc.Clear();
}
//----------------------------------------------------------------------------
void mmgLutWidget::OnPaint(wxPaintEvent &event)
//----------------------------------------------------------------------------
{
  wxPaintDC pdc(this);
  wxMemoryDC mdc;
  mdc.SelectObject(m_bmp2);
  pdc.Blit(m_bp.x, m_bp.y, m_bs.GetWidth(), m_bs.GetHeight(), &mdc, 0,0);
}
//----------------------------------------------------------------------------
void mmgLutWidget::DrawEntries()
//----------------------------------------------------------------------------
{
  for( int i=0; i<m_numEntry; i++ )
   DrawEntry(i);
}
//----------------------------------------------------------------------------
void mmgLutWidget::DrawEntry(int idx)
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
      mafColor c = mafColor::CheckeredColor(m_lutEntry[idx].m_c, x,y);
      *p++ = c.m_r;
      *p++ = c.m_g;
      *p++ = c.m_b;
    }
  }

  wxImage img(w,h,data); // Data will be freed by the image
  wxBitmap bmp(img);

  wxMemoryDC dc1;
  dc1.SelectObject(bmp);

  wxMemoryDC dc2;
  dc2.SelectObject(m_bmp);
  
  int x0 = m + (w+m) * (idx - m_EntryPerRow * (idx/m_EntryPerRow) );
  int y0 = m + (h+m) * (idx/m_EntryPerRow);
  
  dc2.Blit(x0, y0, w, h, &dc1, 0, 0);


  // here LUT is kept in sync
  if(m_lut)
  {
    double r,g,b,a;
    m_lutEntry[idx].m_c.GetFloatRGB(&r,&g,&b,&a);
    m_lut->SetTableValue(idx,r,g,b,a);
  }
}
//----------------------------------------------------------------------------
void mmgLutWidget::DrawSelection()
//----------------------------------------------------------------------------
{
  wxMemoryDC dc1;
  dc1.SelectObject(m_bmp);

  wxMemoryDC dc2;
  dc2.SelectObject(m_bmp2);
  
  dc2.Blit(0, 0, m_bs.GetWidth(), m_bs.GetHeight(), &dc1, 0, 0);

  wxPen pen(*wxBLACK,1,wxSOLID);
  dc2.SetPen(*wxBLACK_PEN);
  dc2.SetBrush(*wxWHITE_BRUSH);

  for( int i=0; i<m_numEntry; i++ )
  {
    if(m_lutEntry[i].m_selected)
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
int mmgLutWidget::MouseToIndex(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  int r = (event.m_y - m_bp.x )/(m_EntryH+m_EntryM);
  int c = (event.m_x - m_bp.y )/(m_EntryW+m_EntryM);
  int idx = c + m_EntryPerRow*r;
  if( idx<0 || idx>m_numEntry) idx =-1;
  return idx;
}
//----------------------------------------------------------------------------
void mmgLutWidget::OnEvent( mafEventBase *event )
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(event))
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
void mmgLutWidget::SetSelectionColor(mafColor col)
//----------------------------------------------------------------------------
{
  for( int i=0; i<m_numEntry; i++ )
  {
    if(m_lutEntry[i].m_selected)
    {
      m_lutEntry[i].m_c = col;
      DrawEntry(i);
    }
  }
  DrawSelection();
  Refresh();
}
//----------------------------------------------------------------------------
mafColor mmgLutWidget::GetSelectionColor()
//----------------------------------------------------------------------------
{
  mafColor c;
  for( int i=0; i<m_numEntry; i++ )
    if(m_lutEntry[i].m_selected)
      c = m_lutEntry[i].m_c;
  return c;
}
//----------------------------------------------------------------------------
void mmgLutWidget::ShadeSelectionInHSV()
//----------------------------------------------------------------------------
{
  int min, max, num;
  GetSelection(&min, &max, &num);
  if( num < 2) return; //nothing to do

  mafColor c1 = m_lutEntry[min].m_c;
  mafColor c2 = m_lutEntry[max].m_c;
  
  int i;
  int counter = 0;
  for( i=min; i<=max; i++ )
  {
    if(m_lutEntry[i].m_selected)
    {
      float t = (1.0*counter)/num;
      m_lutEntry[i].m_c = mafColor::InterpolateHSV(c1,c2,t);
      DrawEntry(i);
      counter++;
    }
  }
  DrawSelection();
  Refresh();
}
//----------------------------------------------------------------------------
void mmgLutWidget::ShadeSelectionInRGB()
//----------------------------------------------------------------------------
{
  int min, max, num;
  GetSelection(&min, &max, &num);
  if( num < 2) return; //nothing to do

  mafColor c1 = m_lutEntry[min].m_c;
  mafColor c2 = m_lutEntry[max].m_c;
  
  int i;
  int counter = 0;
  for( i=min; i<=max; i++ )
  {
    if(m_lutEntry[i].m_selected)
    {
      float t = (1.0*counter)/num;
      m_lutEntry[i].m_c = mafColor::InterpolateRGB(c1,c2,t);
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
void mmgLutWidget::GetSelection(int *min, int *max, int *num)
//----------------------------------------------------------------------------
{
  *min = *max = -1;
  if(num != NULL) *num =0;
  int i;

  for( i=0; i<m_numEntry; i++ )
  {
    if(m_lutEntry[i].m_selected)
    {
        if(num != NULL) (*num)++;
        if(*min == -1) *min = i; 
        *max = i;
    } 
  }
}
//----------------------------------------------------------------------------
void mmgLutWidget::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  CaptureMouse();
  m_dragging = true;

  int idx = MouseToIndex(event);
  if (idx != -1)
  {
    if(! event.ControlDown() )
    {
      for(int i=0; i<m_numEntry; i++ )
        m_lutEntry[i].m_selected = false; 
    }

    if(! event.ControlDown() )
       m_lutEntry[idx].m_selected = true; 
    else
       m_lutEntry[idx].m_selected = ! m_lutEntry[idx].m_selected; 

    DrawSelection();
    Refresh();
    m_drag_begin_idx = idx;
  }
  else
    m_drag_begin_idx = -1;
}
//----------------------------------------------------------------------------
void mmgLutWidget::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if(m_dragging)
  {
    int idx = MouseToIndex(event);
    if (idx != -1 && idx != m_drag_begin_idx)
    {
      int i;
			for(i=0; i<m_numEntry; i++ )
        m_lutEntry[i].m_selected = false; 

      int min = (idx < m_drag_begin_idx) ? idx : m_drag_begin_idx;
      int max = (idx > m_drag_begin_idx) ? idx : m_drag_begin_idx;

      for( i=min; i<=max; i++ )
           m_lutEntry[i].m_selected = true; 
      
      DrawSelection();
      Refresh();
    }
  }
}
//----------------------------------------------------------------------------
void mmgLutWidget::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  ReleaseMouse();
  m_r1=m_c1=m_r2=m_c2=-1;

  if(m_dragging)
  {
    m_dragging = false;

    // signal to Listener that the selection has changed
    mafEventMacro(mafEvent(this,GetId()));
  }
}
//----------------------------------------------------------------------------
void mmgLutWidget::SetLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  m_lut = lut;
  if(m_lut == NULL)
  {
    SetNumEntry(0);
    return;
  }

  int n = m_lut->GetNumberOfTableValues();
  if (n>256) n=256;

  mafColor c;
  for( int i=0; i<n; i++ )
  {
    double *p = NULL;
    p = lut->GetTableValue(i);
    assert(p != NULL);
    c.SetFloatRGB(p[0], p[1], p[2], p[3]);
    m_lutEntry[i].m_c = c; 
  }
  SetNumEntry(n);
}


