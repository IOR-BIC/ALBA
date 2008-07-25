/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIColorWidget.cpp,v $
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
#include <wx/image.h>
#include <math.h>

#include "mafGUIColorWidget.h"
#include "mafGUIValidator.h"
#include "mafGUIPicButton.h"
#include "mafGUIButton.h"

//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------
  const double pi = 3.141592;
  const double ppi = 2 * pi;
  const int DRAG_NONE =0;
  const int DRAG_H    =1;
  const int DRAG_SV   =2;
  const int DRAG_A    =3;
//----------------------------------------------------------------------------
// mafGUIColorWidget
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIColorWidget,wxPanel)
  EVT_PAINT(mafGUIColorWidget::OnPaint)

  EVT_LEFT_DOWN(mafGUIColorWidget::OnLeftMouseButtonDown)
  EVT_LEFT_UP(mafGUIColorWidget::OnLeftMouseButtonUp)
  EVT_MOTION(mafGUIColorWidget::OnMouseMotion)

END_EVENT_TABLE()

//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum COLOR_WIDGET_ID
{
  ID_R = MINID,
  ID_G,
  ID_B,
  ID_A,
  ID_H,
  ID_S,
  ID_V,
  ID_COPY,
  ID_PASTE,
};
//----------------------------------------------------------------------------
mafGUIColorWidget::mafGUIColorWidget(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)         
//----------------------------------------------------------------------------
{
	m_Listener = NULL;

  /*
  m_HsvSize    = 160;                          //size of the hsv chooser  
  m_HsvInnerRadius    = m_HsvSize/2 - m_HsvSize/10;         //inner radius of the h cirle 
  m_HsvOuterRadius    = m_HsvSize/2;                     //outer radius of the h cirle 

  m_AlphaBmpPosition   = wxPoint(3,0);                 //position of the alpha chooser  
  m_AlphaBmpSize    = wxSize(m_HsvOuterRadius-m_HsvInnerRadius,m_HsvSize); //size of the alpha chooser  

  m_HsvPosition   = wxPoint(m_AlphaBmpPosition.x + m_AlphaBmpSize.GetWidth()+3,0); //position of the hsv chooser  
  m_SvSize   = m_HsvInnerRadius *1.4;                    //size of the sv chooser  
  m_SvPosition  = wxPoint((m_HsvSize-m_SvSize)/2,(m_HsvSize-m_SvSize)/2); //position of the sv chooser (within the hsv bmp)
  m_CursorSize = 5;
  */

  m_HsvSize    = 128;                          //size of the hsv chooser  
  m_HsvOuterRadius    = m_HsvSize/2;                     //outer radius of the h cirle 
  m_HsvInnerRadius    = m_HsvSize/2 - 16;                //inner radius of the h cirle 

  m_AlphaBmpPosition   = wxPoint(3,0);                 //position of the alpha chooser  
  m_AlphaBmpSize    = wxSize(16,m_HsvSize);            //size of the alpha chooser  

  m_HsvPosition   = wxPoint(m_AlphaBmpPosition.x + m_AlphaBmpSize.GetWidth()+3,0);     //position of the hsv chooser  
  m_SvSize   = m_HsvInnerRadius *1.4;                                    //size of the sv chooser  
  m_SvPosition  = wxPoint((m_HsvSize-m_SvSize)/2,(m_HsvSize-m_SvSize)/2); //position of the sv chooser (within the hsv bmp)
  m_CursorSize = 5;
  
  m_Dragging = DRAG_NONE;

  ///////////////// widgets ///////////////////
  int sh = 14;
  int w1 = 30;
  int w2 = 30;

  wxStaticText *lab;
  wxTextCtrl   *text;

  int py  =0 ;//2*m_r2+5;
  int px = m_HsvPosition.x + m_HsvSize + 5;
  lab  = new wxStaticText(this, ID_R, "red:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_R, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mafGUIValidator(this,ID_R,text,&m_Color.m_Red,0,255) );

  py += (sh+3);
  lab  = new wxStaticText(this, ID_G, "green:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_G, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mafGUIValidator(this,ID_G,text,&m_Color.m_Green,0,255) );

  py += (sh+3);
  lab  = new wxStaticText(this, ID_B, "blue:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_B, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mafGUIValidator(this,ID_B,text,&m_Color.m_Blue,0,255) );

  py += (sh+3);
  lab  = new wxStaticText(this, ID_B, "alpha:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_B, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mafGUIValidator(this,ID_A,text,&m_Color.m_Alpha,0,255) );

  w1 = 26;
  py  = 0;//2*m_r2+5;
  px += (w1+w2+10);
  lab  = new wxStaticText(this, ID_H, "hue:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_H, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mafGUIValidator(this,ID_H,text,&m_Color.m_Hue,0,360) );

  py += (sh+3);
  lab  = new wxStaticText(this, ID_S, "sat:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_S, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mafGUIValidator(this,ID_S,text,&m_Color.m_Saturation,0,255) );

  py += (sh+3);
  lab  = new wxStaticText(this, ID_V, "val:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_V, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mafGUIValidator(this,ID_V,text,&m_Color.m_Value,0,255) );

  w1 = 30;
  px = m_HsvPosition.x + m_HsvSize + 5;
  py += 2*(sh+3) + 5;
  lab   = new wxStaticText(this, -1, "color:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  m_ColorSwatch = new mafGUIColorSwatch(this, -1, wxPoint(px+w1+2,py), wxSize(96,16) );

  py += 16;
  lab   = new wxStaticText(this, -1, "prev:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  m_PrevColorSwatch = new mafGUIColorSwatch(this, -1, wxPoint(px+w1+2,py), wxSize(96,16) );

  py= m_HsvPosition.y+ m_HsvSize - 20;
  px = px+w1+2;
  mafGUIButton *b1 = new mafGUIButton(this,ID_COPY,"copy",wxPoint(px,py),wxSize(48,20));  
	b1->SetValidator( mafGUIValidator(this,ID_COPY,b1) );
  px += 47;
  mafGUIButton *b2 = new mafGUIButton(this,ID_PASTE,"paste",wxPoint(px,py), wxSize(48,20));  
	b2->SetValidator( mafGUIValidator(this,ID_PASTE,b2) );


  ///////////////// bitmaps ///////////////////

  InitBitmap();
  UpdateHSVBitmap();
  UpdateAlphaBitmap();
  OnColorChanged(false);
}
//----------------------------------------------------------------------------
void mafGUIColorWidget::SetColor(mafColor c)
//----------------------------------------------------------------------------
{
  m_Color = c;
  m_PrevColorSwatch->SetColor(c);
  OnColorChanged(false);
}
//----------------------------------------------------------------------------
mafColor mafGUIColorWidget::GetColor()
//----------------------------------------------------------------------------
{
  return m_Color;
}
//----------------------------------------------------------------------------
void mafGUIColorWidget::OnPaint(wxPaintEvent &event)
//----------------------------------------------------------------------------
{
  wxPaintDC pdc(this);
  wxMemoryDC mdc;

  mdc.SelectObject(m_AlphaBmp);
  pdc.Blit(m_AlphaBmpPosition.x, m_AlphaBmpPosition.y, m_AlphaBmpSize.GetWidth(), m_AlphaBmpSize.GetHeight(), &mdc, 0,0);

  mdc.SelectObject(m_HsvBmp);
  pdc.Blit(m_HsvPosition.x, m_HsvPosition.y, m_HsvSize, m_HsvSize, &mdc, 0,0);
  
  mdc.SelectObject(wxNullBitmap);
  PaintHandles(pdc);  // paint handles is called @ every paint 
}
//----------------------------------------------------------------------------
void mafGUIColorWidget::InitBitmap()
//----------------------------------------------------------------------------
{
  int r0 = GetBackgroundColour().Red();
  int g0 = GetBackgroundColour().Green();
  int b0 = GetBackgroundColour().Blue();

  unsigned char *data = new unsigned char[m_HsvSize*m_HsvSize*3];
  unsigned char *p = data;

  double r1sq = m_HsvInnerRadius*m_HsvInnerRadius;
  double r2sq = m_HsvOuterRadius*m_HsvOuterRadius;
  
  int x,y;
  for(y=0; y<m_HsvSize; y++)
  {
    unsigned char v = 255 - (255.0 * y ) / m_HsvSize;
    for(x=0; x<m_HsvSize; x++)
    {
      int xc = x-m_HsvSize/2; // coords with respct to the center
      int yc = y-m_HsvSize/2; // coords with respct to the center

      int distsq = xc*xc + yc*yc;
      int r,g,b;
      if( distsq < r1sq || distsq > r2sq)
      {
        r=r0 ; g=g0; b=b0;
      }
      else
      {
        double angle = atan2( 1.0*yc, 1.0*xc);
        int h = angle * 360.0 / ppi + 90;
        if (h>360) h-=360;
        if (h<0) h+=360;
        mafColor::HSVToRGB(h,255,255,&r,&g,&b);

        if( distsq == r1sq || distsq == r2sq)
        {
           r = (r+r0)/2;
           g = (g+g0)/2;
           b = (b+b0)/2;
        }
      }
      
      *p++ = r;
      *p++ = g;
      *p++ = b;
    }
  }
  
  wxImage img(m_HsvSize,m_HsvSize,data); // data will be freed by the image
  //m_HsvBmp = img.ConvertToBitmap(); // changed in passing from wx242 -> wx263
  m_HsvBmp = wxBitmap(img);


  m_AlphaBmp = wxBitmap( m_AlphaBmpSize.GetWidth(), m_AlphaBmpSize.GetHeight() );
}
//----------------------------------------------------------------------------
void mafGUIColorWidget::UpdateHSVBitmap()
//----------------------------------------------------------------------------
{
  unsigned char *data = new unsigned char[m_SvSize*m_SvSize*3];
  unsigned char *p = data;

  int x,y;
  for(y=0; y<m_SvSize; y++)
  {
    unsigned char v = 255 - (255.0 * y ) / m_SvSize;
    for(x=0; x<m_SvSize; x++)
    {
      unsigned char s = (255.0 * x ) / m_SvSize;
      int r,g,b;
      mafColor::HSVToRGB(m_Color.m_Hue,s,v,&r,&g,&b);
      
      *p++ = r;
      *p++ = g;
      *p++ = b;
    }
  }
  wxImage img(m_SvSize,m_SvSize,data); // Data will be freed by the image

  wxBitmap bmp(img);
  wxMemoryDC dc1;
  dc1.SelectObject(bmp);

  wxMemoryDC dc2;
  dc2.SelectObject(m_HsvBmp);
  
  dc2.Blit(m_SvPosition.x, m_SvPosition.y, m_SvSize, m_SvSize, &dc1, 0, 0);

  dc1.SelectObject(wxNullBitmap);
  dc2.SelectObject(wxNullBitmap);
}
//----------------------------------------------------------------------------
void mafGUIColorWidget::UpdateAlphaBitmap()
//----------------------------------------------------------------------------
{
  int w = m_AlphaBmpSize.GetWidth();
  int h = m_AlphaBmpSize.GetHeight();

  int y0 = m_CursorSize;
  int y1 = h - 2*m_CursorSize;

  unsigned char *data = new unsigned char[w*h*3];
  unsigned char *p = data;

  int x,y;
  for(y=0; y<h; y++)
  {
    mafColor c0 = m_Color;

    float a = (y-y0*1.0)/(y1-y0);
    if(y<=y0) a=0;
    if(y>=y1) a=1;
    c0.m_Alpha = a*255;;

    for(x=0; x<w; x++)
    {
      mafColor c = mafColor::CheckeredColor(c0,x,y);
      *p++ = c.m_Red;
      *p++ = c.m_Green;
      *p++ = c.m_Blue;
    }
  }
  wxImage img(w,h,data); // Data will be freed by the image

  m_AlphaBmp = wxBitmap(img);
}
//----------------------------------------------------------------------------
void mafGUIColorWidget::PaintHandles(wxPaintDC &dc)
//----------------------------------------------------------------------------
{
  //rgb,hsv widgets are kept updated here 
  TransferDataToWindow();  

  //m_colorswatch is kept updated here 
  m_ColorSwatch->SetColor(m_Color);

  //m_HsvCursorPosition, m_SvCursorPosition and m_AlphaCursorPosition  are kept updated here 
  
  wxPen pen(*wxBLACK_PEN);
  pen.SetWidth(1);
  dc.SetPen(pen);
  dc.SetBrush(*wxWHITE_BRUSH);

  int cx = m_HsvPosition.x + m_HsvSize/2;
  int cy = m_HsvPosition.y + m_HsvSize/2;
  
  int r       = (m_HsvInnerRadius+m_HsvOuterRadius)/2;
  double angle= (m_Color.m_Hue - 90) * ppi /360.0;
  m_HsvCursorPosition.x = cx + r * cos(angle);
  m_HsvCursorPosition.y = cy + r * sin(angle);    
  
  dc.DrawEllipse(m_HsvCursorPosition.x-m_CursorSize, m_HsvCursorPosition.y-m_CursorSize, 2*m_CursorSize, 2*m_CursorSize);

  m_SvCursorPosition.x = m_HsvPosition.x + m_SvPosition.x + (m_Color.m_Saturation /255.0) * m_SvSize;
  m_SvCursorPosition.y = m_HsvPosition.y + m_SvPosition.y + m_SvSize - (m_Color.m_Value /255.0) * m_SvSize;

  dc.DrawEllipse(m_SvCursorPosition.x-m_CursorSize, m_SvCursorPosition.y-m_CursorSize, 2*m_CursorSize, 2*m_CursorSize);

  m_AlphaCursorPosition.x = m_AlphaBmpPosition.x + m_AlphaBmpSize.GetWidth()/2;
  int y0 = m_AlphaBmpPosition.y + m_CursorSize;
  int y1 = m_AlphaBmpSize.GetHeight() - 2 * m_CursorSize;
  m_AlphaCursorPosition.y = y0 + (m_Color.m_Alpha /255.0) * y1;

  dc.DrawEllipse(m_AlphaCursorPosition.x-m_CursorSize, m_AlphaCursorPosition.y-m_CursorSize, 2*m_CursorSize, 2*m_CursorSize);
  
  dc.SetPen(wxNullPen);
  dc.SetBrush(wxNullBrush); 
}
//----------------------------------------------------------------------------
void mafGUIColorWidget::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  int x = event.m_x;
  int y = event.m_y;

  int cx = m_HsvPosition.x + m_HsvSize/2;
  int cy = m_HsvPosition.y + m_HsvSize/2;
  int x1 = m_HsvPosition.x + m_SvPosition.x;
  int x2 = x1 + m_SvSize;
  int y1 = m_HsvPosition.y + m_SvPosition.y;
  int y2 = y1 + m_SvSize;
  
  int ax1 = m_AlphaBmpPosition.x;
  int ax2 = x1 + m_AlphaBmpSize.GetWidth();
  int ay1 = m_AlphaBmpPosition.y /*+ m_CursorSize*/ ;
  int ay2 = y1 + m_AlphaBmpSize.GetHeight() /*- 2 * m_CursorSize*/;
  
  double r= sqrt( 1.0*(x-cx)*(x-cx)+1.0*(y-cy)*(y-cy) );

  CaptureMouse();
  
  if( abs(x -m_SvCursorPosition.x) <= m_CursorSize && abs(y -m_SvCursorPosition.y) <= m_CursorSize )
  {
    // clicked the SV cursor --- (which can be partially outside the SV area)
    m_Dragging = DRAG_SV;
    OnMouseMotion(event);
  }
  else
  if( x>x1 && x<x2 && y>y1 && y<y2 )
  {
    // clicked in the SV area
    m_Dragging = DRAG_SV;
    OnMouseMotion(event);
  }
  else
  if( r > m_HsvInnerRadius && r < m_HsvOuterRadius )
  {
    // clicked in the H area - move the Hcursor here
    m_Dragging = DRAG_H;
    OnMouseMotion(event);
  }
  else
  if( x>ax1 && x<ax2 && y>ay1 && y<ay2 )
  {
    // clicked in the Alpha chooser
    m_Dragging = DRAG_A;
    OnMouseMotion(event);
  }
}
//----------------------------------------------------------------------------
void mafGUIColorWidget::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  ReleaseMouse();  
  
  if(m_Dragging)
  {
    m_Dragging = DRAG_NONE;
    OnColorChanged();
  }
}
//----------------------------------------------------------------------------
void mafGUIColorWidget::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if(m_Dragging == DRAG_NONE) return;

  int cx = m_HsvPosition.x + m_HsvSize/2;
  int cy = m_HsvPosition.y + m_HsvSize/2;

  if(m_Dragging == DRAG_H)
  {
    double angle = atan2( 1.0*(event.m_y - cy) , 1.0*(event.m_x- cx) );
    int h = angle * 360.0 / ppi + 90;
    m_Color.SetHSV( h, m_Color.m_Saturation, m_Color.m_Value );
  }
  else 
  if(m_Dragging == DRAG_SV)
  {
    int x0 = m_HsvPosition.x + m_SvPosition.x;
    int x1 = x0 + m_SvSize;
    int y0 = m_HsvPosition.y + m_SvPosition.y;
    int y1 = y0 + m_SvSize;
    
    float normalized_x  =     ( 1.0 * (event.m_x-x0) ) / (x1-x0);
    float normalized_y  = 1 - ( 1.0 * (event.m_y-y0) ) / (y1-y0);

    if (normalized_x < 0) normalized_x = 0;
    if (normalized_x > 1) normalized_x = 1;
    if (normalized_y < 0) normalized_y = 0;
    if (normalized_y > 1) normalized_y = 1;

    m_Color.SetHSV( m_Color.m_Hue, normalized_x *255, normalized_y *255 );
  }
  else 
  if(m_Dragging == DRAG_A)
  {
    float y0 = m_AlphaBmpPosition.y + m_CursorSize ;
    float y1 = y0 + m_AlphaBmpSize.GetHeight() - 2 * m_CursorSize;
    float normalized_y  = (event.m_y-y0) / (y1-y0);

    if (normalized_y < 0) normalized_y = 0;
    if (normalized_y > 1) normalized_y = 1;

    m_Color.SetRGB( m_Color.m_Red, m_Color.m_Green, m_Color.m_Blue, normalized_y*255);
  }

  OnColorChanged(false);
}
//----------------------------------------------------------------------------
void mafGUIColorWidget::OnEvent( mafEventBase *event )
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(event))
  {
    switch(e->GetId())
    {
    case ID_A:
      OnColorChanged();
      break;
    case ID_R:
    case ID_G:
    case ID_B:
      m_Color.RGBToHSV(); // keep RGB and HSV in sync.
      OnColorChanged();
      break;
    case ID_H:
    case ID_S:
    case ID_V:
      m_Color.HSVToRGB(); // keep RGB and HSV in sync.
      OnColorChanged();
      break;
    case ID_COPY:
      m_Clipboard = m_Color;
      break;
    case ID_PASTE:
      m_Color = m_Clipboard ;
      OnColorChanged();
      break;
    default:
      e->Log();
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void mafGUIColorWidget::OnColorChanged(bool notify)
//----------------------------------------------------------------------------
{
  static mafColor old_col; 
  if( m_Color.m_Hue != old_col.m_Hue )  
    //h changed - update the SV chooser
    UpdateHSVBitmap();    
  //if( m_Color.m_Hue != old_col.m_Hue || m_Color.m_Saturation != old_col.m_Saturation || m_Color.m_Value != old_col.m_Value )  
    //any changes except alpha - update the Alpha chooser
    UpdateAlphaBitmap();
  old_col = m_Color;

  TransferDataToWindow();  // update the widgets contents
  Refresh();               // call paint, which draws the birmap and the cursors  

  if(notify)
    mafEventMacro(mafEvent(this,GetId())); // notify the user that the color has changed
}
