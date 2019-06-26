/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIColorSwatch
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

#include "albaGUIColorSwatch.h"

//----------------------------------------------------------------------------
// albaGUIColorSwatch
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIColorSwatch,wxPanel)
  EVT_PAINT(albaGUIColorSwatch::OnPaint)
  EVT_ERASE_BACKGROUND(albaGUIColorSwatch::OnEraseBackground) 

  //EVT_LEFT_DOWN(albaGUIColorSwatch::OnLeftMouseButtonDown)
  //EVT_LEFT_UP(albaGUIColorSwatch::OnLeftMouseButtonUp)
  //EVT_MOTION(albaGUIColorSwatch::OnMouseMotion)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUIColorSwatch::albaGUIColorSwatch(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)         
//----------------------------------------------------------------------------
{
	//m_Listener = NULL;
  SetColor( albaColor(0,0,0));
}
//----------------------------------------------------------------------------
void albaGUIColorSwatch::SetColor(albaColor c)
//----------------------------------------------------------------------------
{
  m_Color = c;
  int w = GetSize().GetWidth();   
  int h = GetSize().GetHeight();   

  unsigned char *data = new unsigned char[w*h*3];
  unsigned char *p = data;

  
  albaColor c0 = albaColor::CheckeredColor(m_Color, 0,0);
  albaColor c1 = albaColor::CheckeredColor(m_Color, 8,0);

  int x,y;
  for(y=0; y<h; y++)
  {
    for(x=0; x<w; x++)
    {
      int a = x%16 >= 8;
      int b = y%16 >= 8;
      if( a+b == 1 )
      {
        *p++ = c0.m_Red;
        *p++ = c0.m_Green;
        *p++ = c0.m_Blue;
      }
      else
      {
        *p++ = c1.m_Red;
        *p++ = c1.m_Green;
        *p++ = c1.m_Blue;
      }
    }
  }
  wxImage img(w,h,data); // data will be freed by the image
  //m_Bmp = img.ConvertToBitmap(); // changed in passing from wx242 -> wx263
  m_Bmp = wxBitmap(img);
}
//----------------------------------------------------------------------------
void albaGUIColorSwatch::OnPaint(wxPaintEvent &event)
//----------------------------------------------------------------------------
{
  wxPaintDC pdc(this);

  int w = GetSize().GetWidth();   
  int h = GetSize().GetHeight();   

  wxMemoryDC mdc;
  mdc.SelectObject(m_Bmp);
  pdc.Blit(0, 0, w, h, &mdc, 0,0);
  mdc.SelectObject(wxNullBitmap);
}
/*
//----------------------------------------------------------------------------
void albaGUIColorSwatch::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  int x = event.m_x;
  int y = event.m_y;
  CaptureMouse();
}
//----------------------------------------------------------------------------
void albaGUIColorSwatch::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  ReleaseMouse();  
}
//----------------------------------------------------------------------------
void albaGUIColorSwatch::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
}
*/
