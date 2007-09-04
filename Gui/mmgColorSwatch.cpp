/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgColorSwatch.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-04 16:22:15 $
  Version:   $Revision: 1.3 $
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

#include "mmgColorSwatch.h"

//----------------------------------------------------------------------------
// mmgColorSwatch
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgColorSwatch,wxPanel)
  EVT_PAINT(mmgColorSwatch::OnPaint)
  EVT_ERASE_BACKGROUND(mmgColorSwatch::OnEraseBackground) 

  //EVT_LEFT_DOWN(mmgColorSwatch::OnLeftMouseButtonDown)
  //EVT_LEFT_UP(mmgColorSwatch::OnLeftMouseButtonUp)
  //EVT_MOTION(mmgColorSwatch::OnMouseMotion)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgColorSwatch::mmgColorSwatch(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)         
//----------------------------------------------------------------------------
{
	//m_Listener = NULL;
  SetColor( mafColor(0,0,0));
}
//----------------------------------------------------------------------------
void mmgColorSwatch::SetColor(mafColor c)
//----------------------------------------------------------------------------
{
  m_Color = c;
  int w = GetSize().GetWidth();   
  int h = GetSize().GetHeight();   

  unsigned char *data = new unsigned char[w*h*3];
  unsigned char *p = data;

  
  mafColor c0 = mafColor::CheckeredColor(m_Color, 0,0);
  mafColor c1 = mafColor::CheckeredColor(m_Color, 8,0);

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
void mmgColorSwatch::OnPaint(wxPaintEvent &event)
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
void mmgColorSwatch::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  int x = event.m_x;
  int y = event.m_y;
  CaptureMouse();
}
//----------------------------------------------------------------------------
void mmgColorSwatch::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  ReleaseMouse();  
}
//----------------------------------------------------------------------------
void mmgColorSwatch::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
}
*/
