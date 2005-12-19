/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLutSwatch.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-19 15:26:31 $
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

#include <math.h>
#include <wx/image.h>
#include <wx/dc.h>
#include <wx/dcclient.h>

#include "mmgLutSwatch.h"
#include "mafColor.h"
#include "mmgLutEditor.h"

// ugly hack to make DrawText Work
// if you remove this line you will have a Compile-Error "DrawTextA is not defined for wxPaintDC"
// .... waiting a better workaround. SIL 30/11/05 
#ifdef WIN32
	#include <wx/msw/winundef.h> 
#endif
// end of hack

//----------------------------------------------------------------------------
// mmgLutSwatch
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgLutSwatch,wxPanel)
  EVT_PAINT(mmgLutSwatch::OnPaint)
  EVT_ERASE_BACKGROUND(mmgLutSwatch::OnEraseBackground) 

  EVT_LEFT_DOWN(mmgLutSwatch::OnLeftMouseButtonDown)
  //EVT_LEFT_UP(mmgLutSwatch::OnLeftMouseButtonUp)
  EVT_MOTION(mmgLutSwatch::OnMouseMotion)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgLutSwatch::mmgLutSwatch(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)         
//----------------------------------------------------------------------------
{
	m_Listener = NULL;
  m_editable = false;
  SetLut(NULL);

  m_mouse_in_window = false;
  m_tip = "";
  SetCursor(*wxCROSS_CURSOR);

  m_font = wxFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
  m_font.SetPointSize(9);
}
//----------------------------------------------------------------------------
void mmgLutSwatch::OnPaint(wxPaintEvent &event)
//----------------------------------------------------------------------------
{
  wxPaintDC pdc(this);

  if(!m_lut) return;
  if(m_lut->GetMTime() > m_updatetime ) Update();

  int w = GetSize().GetWidth();   
  int h = GetSize().GetHeight();   

  wxMemoryDC mdc;
  mdc.SelectObject(m_bmp);
  pdc.Blit(0, 0, w, h, &mdc, 0,0);
  mdc.SelectObject(wxNullBitmap);
 
  if(m_lut && m_mouse_in_window)
  {
    pdc.SetFont(m_font);
    pdc.SetTextForeground(*wxBLACK);
    pdc.SetTextBackground(*wxWHITE);
    pdc.SetBackgroundMode(wxSOLID);
    
    wxCoord tw,th;
    pdc.GetTextExtent(m_tip, &tw, &th);
   
    //******************  SIL 29/11/05 - to be reinserted
    if(m_mouse_x < w/2)
      pdc.DrawText(m_tip,wxPoint(m_mouse_x,1) );
    else
      pdc.DrawText(m_tip,wxPoint(m_mouse_x-tw,1));
    //******************/
  }
}
//----------------------------------------------------------------------------
void mmgLutSwatch::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if( m_lut && m_editable  )
  {
    mmgLutEditor::ShowLutDialog(m_lut);
    Update();
    Refresh();
  }

  //notify the user
  mafEventMacro(mafEvent(this,GetId()));
}
/*
//----------------------------------------------------------------------------
void mmgLutSwatch::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  ReleaseMouse();  
}
*/
//----------------------------------------------------------------------------
void mmgLutSwatch::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  int x = event.m_x;
  int y = event.m_y;
  int w = GetSize().GetWidth();   
  int h = GetSize().GetHeight();   


  // this is to simulate the non-existing event "MouseLeave"
  if(! m_mouse_in_window )
  {
    CaptureMouse();
    m_mouse_in_window = true;
  }
  else
  {
    if( x<0 || x>w || y<0 || y>h )
    {
      ReleaseMouse();  
      m_mouse_in_window = false;
      Refresh();
    }
  }

  //alter the tooltip text according to mouse position
  if(m_lut && m_mouse_in_window ) 
  {
    double *range = m_lut->GetRange();
    double v = range[0] + ((range[1]-range[0]) * x ) / w;
    m_tip = wxString::Format(" value=%g ",v);
    m_mouse_x = x;
    Refresh();
  }
}
//----------------------------------------------------------------------------
void mmgLutSwatch::SetLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  m_lut = lut;
  Update();
  Refresh();
}
//----------------------------------------------------------------------------
void mmgLutSwatch::Update()
//----------------------------------------------------------------------------
{
  int w = GetSize().GetWidth();   
  int h = GetSize().GetHeight();   

  if( m_lut == NULL )
  {
    m_bmp = wxBitmap(w,h);
    return;
  }
  
  float num = m_lut->GetNumberOfTableValues();

  unsigned char *data = new unsigned char[w*h*3];
  unsigned char *p = data;

  int x,y;
  for(y=0; y<h; y++)
  {
    for(x=0; x<w; x++)
    {
      float i = ( num * x ) / w;
      mafColor col  = mafColor( m_lut->GetTableValue(i) );
      mafColor col2 = mafColor::CheckeredColor(col,x,y);

      *p++ = col2.m_r;
      *p++ = col2.m_g;
      *p++ = col2.m_b;
    }
  }
  wxImage img(w,h,data); // data will be freed by the image
  m_bmp = img.ConvertToBitmap();

  m_updatetime = m_lut->GetMTime();
}