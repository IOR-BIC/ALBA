/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgColorWidget.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-01 15:22:05 $
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

#include "mmgColorWidget.h"
#include "mmgValidator.h"
#include "mmgPicButton.h"
#include "mmgButton.h"

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
// mmgColorWidget
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgColorWidget,wxPanel)
  EVT_PAINT(mmgColorWidget::OnPaint)
  EVT_ERASE_BACKGROUND(mmgColorWidget::OnEraseBackground) 

  EVT_LEFT_DOWN(mmgColorWidget::OnLeftMouseButtonDown)
  EVT_LEFT_UP(mmgColorWidget::OnLeftMouseButtonUp)
  EVT_MOTION(mmgColorWidget::OnMouseMotion)

END_EVENT_TABLE()

//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum 
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
mmgColorWidget::mmgColorWidget(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)         
//----------------------------------------------------------------------------
{
	m_Listener = NULL;

  /*
  m_h_sz    = 160;                          //size of the hsv chooser  
  m_h_r1    = m_h_sz/2 - m_h_sz/10;         //inner radius of the h cirle 
  m_h_r2    = m_h_sz/2;                     //outer radius of the h cirle 

  m_a_pos   = wxPoint(3,0);                 //position of the alpha chooser  
  m_a_sz    = wxSize(m_h_r2-m_h_r1,m_h_sz); //size of the alpha chooser  

  m_h_pos   = wxPoint(m_a_pos.x + m_a_sz.GetWidth()+3,0); //position of the hsv chooser  
  m_sv_sz   = m_h_r1 *1.4;                    //size of the sv chooser  
  m_sv_pos  = wxPoint((m_h_sz-m_sv_sz)/2,(m_h_sz-m_sv_sz)/2); //position of the sv chooser (within the hsv bmp)
  m_cursor_size = 5;
  */

  m_h_sz    = 128;                          //size of the hsv chooser  
  m_h_r2    = m_h_sz/2;                     //outer radius of the h cirle 
  m_h_r1    = m_h_sz/2 - 16;                //inner radius of the h cirle 

  m_a_pos   = wxPoint(3,0);                 //position of the alpha chooser  
  m_a_sz    = wxSize(16,m_h_sz);            //size of the alpha chooser  

  m_h_pos   = wxPoint(m_a_pos.x + m_a_sz.GetWidth()+3,0);     //position of the hsv chooser  
  m_sv_sz   = m_h_r1 *1.4;                                    //size of the sv chooser  
  m_sv_pos  = wxPoint((m_h_sz-m_sv_sz)/2,(m_h_sz-m_sv_sz)/2); //position of the sv chooser (within the hsv bmp)
  m_cursor_size = 5;
  
  m_dragging = DRAG_NONE;

  ///////////////// widgets ///////////////////
  int sh = 14;
  int w1 = 30;
  int w2 = 30;

  wxStaticText *lab;
  wxTextCtrl   *text;

  int py  =0 ;//2*m_r2+5;
  int px = m_h_pos.x + m_h_sz + 5;
  lab  = new wxStaticText(this, ID_R, "red:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_R, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mmgValidator(this,ID_R,text,&m_color.m_r,0,255) );

  py += (sh+3);
  lab  = new wxStaticText(this, ID_G, "green:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_G, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mmgValidator(this,ID_G,text,&m_color.m_g,0,255) );

  py += (sh+3);
  lab  = new wxStaticText(this, ID_B, "blue:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_B, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mmgValidator(this,ID_B,text,&m_color.m_b,0,255) );

  py += (sh+3);
  lab  = new wxStaticText(this, ID_B, "alpha:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_B, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mmgValidator(this,ID_A,text,&m_color.m_a,0,255) );

  w1 = 26;
  py  = 0;//2*m_r2+5;
  px += (w1+w2+10);
  lab  = new wxStaticText(this, ID_H, "hue:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_H, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mmgValidator(this,ID_H,text,&m_color.m_h,0,360) );

  py += (sh+3);
  lab  = new wxStaticText(this, ID_S, "sat:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_S, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mmgValidator(this,ID_S,text,&m_color.m_s,0,255) );

  py += (sh+3);
  lab  = new wxStaticText(this, ID_V, "val:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  text = new wxTextCtrl  (this, ID_V, "", wxPoint(px+w1+2,py), wxSize(w2,sh), wxNO_BORDER  );
	text->SetValidator( mmgValidator(this,ID_V,text,&m_color.m_v,0,255) );

  w1 = 30;
  px = m_h_pos.x + m_h_sz + 5;
  py += 2*(sh+3) + 5;
  lab   = new wxStaticText(this, -1, "color:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  m_col_swatch = new mmgColorSwatch(this, -1, wxPoint(px+w1+2,py), wxSize(96,16) );

  py += 16;
  lab   = new wxStaticText(this, -1, "prev:",wxPoint(px,py), wxSize(w1,sh), wxALIGN_RIGHT );
  m_prevcol_swatch = new mmgColorSwatch(this, -1, wxPoint(px+w1+2,py), wxSize(96,16) );

  py= m_h_pos.y+ m_h_sz - 20;
  px = px+w1+2;
  mmgButton *b1 = new mmgButton(this,ID_COPY,"copy",wxPoint(px,py),wxSize(48,20));  
	b1->SetValidator( mmgValidator(this,ID_COPY,b1) );
  px += 47;
  mmgButton *b2 = new mmgButton(this,ID_PASTE,"paste",wxPoint(px,py), wxSize(48,20));  
	b2->SetValidator( mmgValidator(this,ID_PASTE,b2) );


  ///////////////// bitmaps ///////////////////

  InitBitmap();
  UpdateHSVBitmap();
  UpdateAlphaBitmap();
  OnColorChanged(false);
}
//----------------------------------------------------------------------------
void mmgColorWidget::SetColor(mafColor c)
//----------------------------------------------------------------------------
{
  m_color = c;
  m_prevcol_swatch->SetColor(c);
  OnColorChanged(false);
}
//----------------------------------------------------------------------------
mafColor mmgColorWidget::GetColor()
//----------------------------------------------------------------------------
{
  return m_color;
}
//----------------------------------------------------------------------------
void mmgColorWidget::OnPaint(wxPaintEvent &event)
//----------------------------------------------------------------------------
{
  wxPaintDC pdc(this);
  wxMemoryDC mdc;

  mdc.SelectObject(m_alpha_bmp);
  pdc.Blit(m_a_pos.x, m_a_pos.y, m_a_sz.GetWidth(), m_a_sz.GetHeight(), &mdc, 0,0);

  mdc.SelectObject(m_hsv_bmp);
  pdc.Blit(m_h_pos.x, m_h_pos.y, m_h_sz, m_h_sz, &mdc, 0,0);
  
  mdc.SelectObject(wxNullBitmap);
  PaintHandles(pdc);  // paint handles is called @ every paint 
}
//----------------------------------------------------------------------------
void mmgColorWidget::InitBitmap()
//----------------------------------------------------------------------------
{
  int r0 = GetBackgroundColour().Red();
  int g0 = GetBackgroundColour().Green();
  int b0 = GetBackgroundColour().Blue();

  unsigned char *data = new unsigned char[m_h_sz*m_h_sz*3];
  unsigned char *p = data;

  double r1sq = m_h_r1*m_h_r1;
  double r2sq = m_h_r2*m_h_r2;
  
  int x,y;
  for(y=0; y<m_h_sz; y++)
  {
    unsigned char v = 255 - (255.0 * y ) / m_h_sz;
    for(x=0; x<m_h_sz; x++)
    {
      int xc = x-m_h_sz/2; // coords with respct to the center
      int yc = y-m_h_sz/2; // coords with respct to the center

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
  
  wxImage img(m_h_sz,m_h_sz,data); // data will be freed by the image
  m_hsv_bmp = img.ConvertToBitmap();


  m_alpha_bmp = wxBitmap( m_a_sz.GetWidth(), m_a_sz.GetHeight() );
}
//----------------------------------------------------------------------------
void mmgColorWidget::UpdateHSVBitmap()
//----------------------------------------------------------------------------
{
  unsigned char *data = new unsigned char[m_sv_sz*m_sv_sz*3];
  unsigned char *p = data;

  int x,y;
  for(y=0; y<m_sv_sz; y++)
  {
    unsigned char v = 255 - (255.0 * y ) / m_sv_sz;
    for(x=0; x<m_sv_sz; x++)
    {
      unsigned char s = (255.0 * x ) / m_sv_sz;
      int r,g,b;
      mafColor::HSVToRGB(m_color.m_h,s,v,&r,&g,&b);
      
      *p++ = r;
      *p++ = g;
      *p++ = b;
    }
  }
  wxImage img(m_sv_sz,m_sv_sz,data); // Data will be freed by the image

  wxBitmap bmp(img);
  wxMemoryDC dc1;
  dc1.SelectObject(bmp);

  wxMemoryDC dc2;
  dc2.SelectObject(m_hsv_bmp);
  
  dc2.Blit(m_sv_pos.x, m_sv_pos.y, m_sv_sz, m_sv_sz, &dc1, 0, 0);

  dc1.SelectObject(wxNullBitmap);
  dc2.SelectObject(wxNullBitmap);
}
//----------------------------------------------------------------------------
void mmgColorWidget::UpdateAlphaBitmap()
//----------------------------------------------------------------------------
{
  int w = m_a_sz.GetWidth();
  int h = m_a_sz.GetHeight();

  int y0 = m_cursor_size;
  int y1 = h - 2*m_cursor_size;

  unsigned char *data = new unsigned char[w*h*3];
  unsigned char *p = data;

  int x,y;
  for(y=0; y<h; y++)
  {
    mafColor c0 = m_color;

    float a = (y-y0*1.0)/(y1-y0);
    if(y<=y0) a=0;
    if(y>=y1) a=1;
    c0.m_a = a*255;;

    for(x=0; x<w; x++)
    {
      mafColor c = mafColor::CheckeredColor(c0,x,y);
      *p++ = c.m_r;
      *p++ = c.m_g;
      *p++ = c.m_b;
    }
  }
  wxImage img(w,h,data); // Data will be freed by the image

  m_alpha_bmp = wxBitmap(img);
}
//----------------------------------------------------------------------------
void mmgColorWidget::PaintHandles(wxPaintDC &dc)
//----------------------------------------------------------------------------
{
  //rgb,hsv widgets are kept updated here 
  TransferDataToWindow();  

  //m_colorswatch is kept updated here 
  m_col_swatch->SetColor(m_color);

  //m_hcursor, m_svcursor and m_acursor  are kept updated here 
  
  wxPen pen(*wxBLACK_PEN);
  pen.SetWidth(1);
  dc.SetPen(pen);
  dc.SetBrush(*wxWHITE_BRUSH);

  int cx = m_h_pos.x + m_h_sz/2;
  int cy = m_h_pos.y + m_h_sz/2;
  
  int r       = (m_h_r1+m_h_r2)/2;
  double angle= (m_color.m_h - 90) * ppi /360.0;
  m_hcursor.x = cx + r * cos(angle);
  m_hcursor.y = cy + r * sin(angle);    
  
  dc.DrawEllipse(m_hcursor.x-m_cursor_size, m_hcursor.y-m_cursor_size, 2*m_cursor_size, 2*m_cursor_size);

  m_svcursor.x = m_h_pos.x + m_sv_pos.x + (m_color.m_s /255.0) * m_sv_sz;
  m_svcursor.y = m_h_pos.y + m_sv_pos.y + m_sv_sz - (m_color.m_v /255.0) * m_sv_sz;

  dc.DrawEllipse(m_svcursor.x-m_cursor_size, m_svcursor.y-m_cursor_size, 2*m_cursor_size, 2*m_cursor_size);

  m_acursor.x = m_a_pos.x + m_a_sz.GetWidth()/2;
  int y0 = m_a_pos.y + m_cursor_size;
  int y1 = m_a_sz.GetHeight() - 2 * m_cursor_size;
  m_acursor.y = y0 + (m_color.m_a /255.0) * y1;

  dc.DrawEllipse(m_acursor.x-m_cursor_size, m_acursor.y-m_cursor_size, 2*m_cursor_size, 2*m_cursor_size);
  
  dc.SetPen(wxNullPen);
  dc.SetBrush(wxNullBrush); 
}
//----------------------------------------------------------------------------
void mmgColorWidget::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  int x = event.m_x;
  int y = event.m_y;

  int cx = m_h_pos.x + m_h_sz/2;
  int cy = m_h_pos.y + m_h_sz/2;
  int x1 = m_h_pos.x + m_sv_pos.x;
  int x2 = x1 + m_sv_sz;
  int y1 = m_h_pos.y + m_sv_pos.y;
  int y2 = y1 + m_sv_sz;
  
  int ax1 = m_a_pos.x;
  int ax2 = x1 + m_a_sz.GetWidth();
  int ay1 = m_a_pos.y /*+ m_cursor_size*/ ;
  int ay2 = y1 + m_a_sz.GetHeight() /*- 2 * m_cursor_size*/;
  
  double r= sqrt( 1.0*(x-cx)*(x-cx)+1.0*(y-cy)*(y-cy) );

  CaptureMouse();
  
  if( abs(x -m_svcursor.x) <= m_cursor_size && abs(y -m_svcursor.y) <= m_cursor_size )
  {
    // clicked the SV cursor --- (which can be partially outside the SV area)
    m_dragging = DRAG_SV;
    OnMouseMotion(event);
  }
  else
  if( x>x1 && x<x2 && y>y1 && y<y2 )
  {
    // clicked in the SV area
    m_dragging = DRAG_SV;
    OnMouseMotion(event);
  }
  else
  if( r > m_h_r1 && r < m_h_r2 )
  {
    // clicked in the H area - move the Hcursor here
    m_dragging = DRAG_H;
    OnMouseMotion(event);
  }
  else
  if( x>ax1 && x<ax2 && y>ay1 && y<ay2 )
  {
    // clicked in the Alpha chooser
    m_dragging = DRAG_A;
    OnMouseMotion(event);
  }
}
//----------------------------------------------------------------------------
void mmgColorWidget::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  ReleaseMouse();  
  
  if(m_dragging)
  {
    m_dragging = DRAG_NONE;
    OnColorChanged();
  }
}
//----------------------------------------------------------------------------
void mmgColorWidget::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if(m_dragging == DRAG_NONE) return;

  int cx = m_h_pos.x + m_h_sz/2;
  int cy = m_h_pos.y + m_h_sz/2;

  if(m_dragging == DRAG_H)
  {
    double angle = atan2( 1.0*(event.m_y - cy) , 1.0*(event.m_x- cx) );
    int h = angle * 360.0 / ppi + 90;
    m_color.SetHSV( h, m_color.m_s, m_color.m_v );
  }
  else 
  if(m_dragging == DRAG_SV)
  {
    int x0 = m_h_pos.x + m_sv_pos.x;
    int x1 = x0 + m_sv_sz;
    int y0 = m_h_pos.y + m_sv_pos.y;
    int y1 = y0 + m_sv_sz;
    
    float normalized_x  =     ( 1.0 * (event.m_x-x0) ) / (x1-x0);
    float normalized_y  = 1 - ( 1.0 * (event.m_y-y0) ) / (y1-y0);

    if (normalized_x < 0) normalized_x = 0;
    if (normalized_x > 1) normalized_x = 1;
    if (normalized_y < 0) normalized_y = 0;
    if (normalized_y > 1) normalized_y = 1;

    m_color.SetHSV( m_color.m_h, normalized_x *255, normalized_y *255 );
  }
  else 
  if(m_dragging == DRAG_A)
  {
    float y0 = m_a_pos.y + m_cursor_size ;
    float y1 = y0 + m_a_sz.GetHeight() - 2 * m_cursor_size;
    float normalized_y  = (event.m_y-y0) / (y1-y0);

    if (normalized_y < 0) normalized_y = 0;
    if (normalized_y > 1) normalized_y = 1;

    m_color.SetRGB( m_color.m_r, m_color.m_g, m_color.m_b, normalized_y*255);
  }

  OnColorChanged(false);
}
//----------------------------------------------------------------------------
void mmgColorWidget::OnEvent( mafEventBase *event )
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
      m_color.RGBToHSV(); // keep RGB and HSV in sync.
      OnColorChanged();
      break;
    case ID_H:
    case ID_S:
    case ID_V:
      m_color.HSVToRGB(); // keep RGB and HSV in sync.
      OnColorChanged();
      break;
    case ID_COPY:
      m_clipboard = m_color;
      break;
    case ID_PASTE:
      m_color = m_clipboard ;
      OnColorChanged();
      break;
    default:
      e->Log();
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void mmgColorWidget::OnColorChanged(bool notify)
//----------------------------------------------------------------------------
{
  static mafColor old_col; 
  if( m_color.m_h != old_col.m_h )  
    //h changed - update the SV chooser
    UpdateHSVBitmap();    
  //if( m_color.m_h != old_col.m_h || m_color.m_s != old_col.m_s || m_color.m_v != old_col.m_v )  
    //any changes except alpha - update the Alpha chooser
    UpdateAlphaBitmap();
  old_col = m_color;

  TransferDataToWindow();  // update the widgets contents
  Refresh();               // call paint, which draws the birmap and the cursors  

  if(notify)
    mafEventMacro(mafEvent(this,GetId())); // notify the user that the color has changed
}

