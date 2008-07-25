/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUILutSwatch.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
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

#include "mafGUILutSwatch.h"
#include "mafColor.h"
#include "mafGUILutEditor.h"

// ugly hack to make DrawText Work
// if you remove this line you will have a Compile-Error "DrawTextA is not defined for wxPaintDC"
// .... waiting a better workaround. SIL 30/11/05 
#ifdef WIN32
	#include <wx/msw/winundef.h> 
#endif
// end of hack

//----------------------------------------------------------------------------
// mafGUILutSwatch
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUILutSwatch,wxPanel)
  EVT_PAINT(mafGUILutSwatch::OnPaint)
  EVT_ERASE_BACKGROUND(mafGUILutSwatch::OnEraseBackground) 

  EVT_LEFT_DOWN(mafGUILutSwatch::OnLeftMouseButtonDown)
  EVT_LEFT_UP(mafGUILutSwatch::OnLeftMouseButtonUp)
  EVT_MOTION(mafGUILutSwatch::OnMouseMotion)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUILutSwatch::mafGUILutSwatch(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)         
//----------------------------------------------------------------------------
{
	m_Listener = NULL;
  m_Editable = false;
  SetLut(NULL);

  m_MouseInWindow = false;
  m_Tip = "";
  SetCursor(*wxCROSS_CURSOR);

  m_Font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  m_Font.SetPointSize(9);
}
//----------------------------------------------------------------------------
void mafGUILutSwatch::OnPaint(wxPaintEvent &event)
//----------------------------------------------------------------------------
{
  wxPaintDC pdc(this);

  if(!m_Lut) return;
  if(m_Lut->GetMTime() > m_UpdateTime ) Update();

  int w = GetSize().GetWidth();   
  int h = GetSize().GetHeight();   

  wxMemoryDC mdc;
  mdc.SelectObject(m_Bmp);
  pdc.Blit(0, 0, w, h, &mdc, 0,0);
  mdc.SelectObject(wxNullBitmap);
 
  if(m_Lut && m_MouseInWindow)
  {
    pdc.SetFont(m_Font);
    pdc.SetTextForeground(*wxBLACK);
    pdc.SetTextBackground(*wxWHITE);
    pdc.SetBackgroundMode(wxSOLID);
    
    wxCoord tw,th;
    pdc.GetTextExtent(m_Tip, &tw, &th);
   
    //******************  SIL 29/11/05 - to be reinserted
    if(m_MouseX < w/2)
      pdc.DrawText(m_Tip,wxPoint(m_MouseX,1) );
    else
      pdc.DrawText(m_Tip,wxPoint(m_MouseX-tw,1));
    //******************/
  }
}
//----------------------------------------------------------------------------
void mafGUILutSwatch::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if( m_Lut && m_Editable  )
  {
    if(m_MouseInWindow )
    {
      if(GetCapture() == this) ReleaseMouse();  
      m_MouseInWindow = false;
      Refresh();
    }

    mafGUILutEditor::ShowLutDialog(m_Lut,m_Listener, GetId() );
    Update();
    Refresh();
  }

  //notify the user
  mafEventMacro(mafEvent(this,GetId()));
}
//----------------------------------------------------------------------------
void mafGUILutSwatch::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUILutSwatch::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  int x = event.m_x;
  int y = event.m_y;
  int w = GetSize().GetWidth();   
  int h = GetSize().GetHeight();   


  // this is to simulate the non-existing event "MouseLeave"
  if(! m_MouseInWindow )
  {
    CaptureMouse();   // Paolo 30/01/2006 
    m_MouseInWindow = true;
  }
  else
  {
    if( x<0 || x>w || y<0 || y>h )
    {
      if(GetCapture() == this) ReleaseMouse();  // Paolo 30/01/2006 
      m_MouseInWindow = false;
      Refresh();
    }
  }
  
	//alter the tooltip text according to mouse position
  if(m_Lut && m_MouseInWindow ) 
  {
    double *range = m_Lut->GetRange();
    double v = range[0] + ((range[1]-range[0]) * x ) / w;
    m_Tip = wxString::Format(" value=%g ",v);
    m_MouseX = x;
    Refresh();
  }
}
//----------------------------------------------------------------------------
void mafGUILutSwatch::SetLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  m_Lut = lut;
  Update();
  Refresh();
}
//----------------------------------------------------------------------------
void mafGUILutSwatch::Update()
//----------------------------------------------------------------------------
{
  int w = GetSize().GetWidth();   
  int h = GetSize().GetHeight();   

  if( m_Lut == NULL )
  {
    m_Bmp = wxBitmap(w,h);
    return;
  }
  
  float num = m_Lut->GetNumberOfTableValues();

  unsigned char *data = (unsigned char*)malloc(sizeof(unsigned char)*w*h*3);
  unsigned char *p = data;

  int x,y;
  for(y=0; y<h; y++)
  {
    for(x=0; x<w; x++)
    {
      float i = ( num * x ) / w;
      mafColor col  = mafColor( m_Lut->GetTableValue(i) );
      mafColor col2 = mafColor::CheckeredColor(col,x,y);

      *p++ = col2.m_Red;
      *p++ = col2.m_Green;
      *p++ = col2.m_Blue;
    }
  }
  wxImage img(w,h,data); // data will be freed by the image
  //m_Bmp = img.ConvertToBitmap(); // changed in passing from wx242 -> wx263
  m_Bmp = wxBitmap(img);

  m_UpdateTime = m_Lut->GetMTime();
}
