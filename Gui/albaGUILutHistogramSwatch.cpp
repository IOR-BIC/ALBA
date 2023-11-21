/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILutHistogramSwatch
 Authors: Crimi Gianluigi
 
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

#include <math.h>
#include <wx/image.h>
#include <wx/dc.h>
#include <wx/dcclient.h>

#include "albaGUI.h"
#include "albaGUILutHistogramSwatch.h"
#include "albaColor.h"
#include "albaGUILutHistogramEditor.h"
#include "mmaVolumeMaterial.h"
#include "vtkDataSet.h"

const int  M	= 1;											// margin all around a row  
const int LM	= 5;											// label margin             
const int LH	= 18;											// label/entry height       
const int BH	= 20;											// button height            
const int HM	= 2*M;										// horizontal margin        (2)
#ifdef WIN32
const int LW	= 55;	// label width Windows
#else
const int LW	= 100;	// label width Linux
#endif
const int EW	= 45;											// entry width  - (was 48)  
const int FW	= LW+LM+EW+HM+EW+HM+EW;		// full width               (304)
const int DW	= EW+HM+EW+HM+EW;					// Data Width - Full Width without the Label (184)


// ugly hack to make DrawText Work
// if you remove this line you will have a Compile-Error "DrawTextA is not defined for wxPaintDC"
// .... waiting a better workaround. SIL 30/11/05 
#ifdef WIN32
	#include <wx/msw/winundef.h> 
#endif
// end of hack

//----------------------------------------------------------------------------
// albaGUILutSwatch
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUILutHistogramSwatch,wxPanel)
  EVT_PAINT(albaGUILutHistogramSwatch::OnPaint)
  EVT_ERASE_BACKGROUND(albaGUILutHistogramSwatch::OnEraseBackground) 

  EVT_LEFT_DOWN(albaGUILutHistogramSwatch::OnLeftMouseButtonDown)
  EVT_LEFT_UP(albaGUILutHistogramSwatch::OnLeftMouseButtonUp)
  EVT_MOTION(albaGUILutHistogramSwatch::OnMouseMotion)
  EVT_SIZE(albaGUILutHistogramSwatch::OnSize)
 
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUILutHistogramSwatch::albaGUILutHistogramSwatch(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)         
//----------------------------------------------------------------------------
{
  m_ShowThreshold= false;
	m_Listener = NULL;
  m_Editable = false;
  SetMaterial(NULL);

  m_MouseInWindow = false;
  m_Tip = "";
  SetCursor(*wxCROSS_CURSOR);

  m_Font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  m_Font.SetPointSize(9);
}

albaGUILutHistogramSwatch::albaGUILutHistogramSwatch(albaGUI *parent, wxWindowID id, wxString name, vtkDataSet *dataSet, mmaVolumeMaterial *material, wxSize size, bool showText)
:wxPanel(parent,id,wxDefaultPosition, size,  wxTAB_TRAVERSAL | wxSIMPLE_BORDER )
{

  m_ShowThreshold = false;
  m_OverHighlight = false;
  m_Highlighted = false;
  m_Editable = false;
  
  m_MouseInWindow = false;
  m_Tip = "";
  SetCursor(*wxCROSS_CURSOR);

  m_Font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  m_Font.SetPointSize(9);
  
  SetMaterial(material);
  SetDataSet(dataSet);
  SetListener(parent);

  //add label only if showText is true
  if (showText)
  {
    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText	*lab  = new wxStaticText(parent, id , name ,wxDefaultPosition, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    sizer->Add( lab,  0, wxRIGHT, LM);
    sizer->Add( this, 0, wxLEFT, HM);
    parent->Add(sizer,0,wxEXPAND, M);
  }
  else
  {
//     wxStaticText *foo_l = new wxStaticText(parent,id, "",wxDefaultPosition, wxSize(4,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
//     sizer->Add(foo_l, 0, wxLEFT, 0);
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add( this, 1, wxEXPAND, HM);
    parent->Add( sizer, 0, wxEXPAND, LM);
  }  
}

//----------------------------------------------------------------------------
void albaGUILutHistogramSwatch::OnPaint(wxPaintEvent &event)
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
void albaGUILutHistogramSwatch::OnLeftMouseButtonDown(wxMouseEvent &event)
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
    //run only if editable

    albaGUILutHistogramEditor::ShowLutHistogramDialog(m_DataSet,m_Material,"Histogram Lut Editor",m_Listener,  GetId());

    Modified();
  }

  //notify the user
  albaEventMacro(albaEvent(this,GetId()));
}
//----------------------------------------------------------------------------
void albaGUILutHistogramSwatch::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  //no left mouse button functionality now.
}
//----------------------------------------------------------------------------
void albaGUILutHistogramSwatch::OnMouseMotion(wxMouseEvent &event)
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
    
    m_Tip = albaString::Format(" value=%g ",v);
    m_MouseX = x;
   
    if (m_OverHighlight)
    {
      int i= GetLutIndexByPos(w,x);
      //redraw only if previously was not highlighted
      //or if the highlighted table value index is changed
      if (i!=m_LastHighlighted || !m_Highlighted)
      {
        m_Material->ApplyGammaCorrection(4);
        double newcol[4];
        newcol[0]=0.5;
        newcol[1]=0;
        newcol[2]=0;
        newcol[3]=1.0;
        //updating the values on the lut whit a red color
        //for highlighting on the views
        m_Lut->SetTableValue(i,newcol);
        m_Material->UpdateFromTables();
        GetLogicManager()->CameraUpdate();
        m_LastHighlighted=i;
        m_Highlighted=true;
      }
    }
    Refresh();
  }
  else if (m_Lut && m_Highlighted) 
  {
    //mouse out of window and previously highlighted 
    m_Material->ApplyGammaCorrection(4);
    m_Material->UpdateProp();
    m_Highlighted=0;
		GetLogicManager()->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
void albaGUILutHistogramSwatch::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  m_Material->ApplyGammaCorrection(4);
  m_Material->UpdateProp();
  m_Highlighted=0;
	GetLogicManager()->CameraUpdate();
  Refresh();
}
//----------------------------------------------------------------------------
void albaGUILutHistogramSwatch::SetMaterial(mmaVolumeMaterial *material)
//----------------------------------------------------------------------------
{
  //update lut pointer
  m_Material = material;
  if (m_Material)
    m_Lut=material->m_ColorLut;
  else 
    m_Lut=NULL;
  Modified();
}

//----------------------------------------------------------------------------
void albaGUILutHistogramSwatch::SetDataSet(vtkDataSet *dataSet)
//----------------------------------------------------------------------------
{
  //Update volume pointer
  m_DataSet = dataSet;
  Modified();
}

//----------------------------------------------------------------------------
void albaGUILutHistogramSwatch::ShowThreshold(bool b) 
 //----------------------------------------------------------------------------
{
  m_ShowThreshold =b;
  Modified();
}

//----------------------------------------------------------------------------
void albaGUILutHistogramSwatch::Modified()
//----------------------------------------------------------------------------
{
  Update();
  Refresh();
}

//----------------------------------------------------------------------------
int albaGUILutHistogramSwatch::GetLutIndexByPos(float fullWidth, float x)
//----------------------------------------------------------------------------
{
  float i;
  float num = m_Lut->GetNumberOfTableValues();

  if (m_ShowThreshold)
  {
    //if m_showThreshold is true i need to calculate the table index 
    //inside the sub range
    double *range,*subRange,rangeSize;
    range=m_DataSet->GetScalarRange();
    subRange=subRange=m_Lut->GetTableRange();
    rangeSize=range[1]-range[0];
    float leftLimit, rightLimit;
    leftLimit=((subRange[0]-range[0])/rangeSize)*fullWidth;
    rightLimit=((subRange[1]-range[0])/rangeSize)*fullWidth;
    //Generating the pixel from the Lut Value
    if (x<=leftLimit) i=0;
    else if (x>rightLimit) i=num-1;
    else i = (( x - leftLimit ) / (rightLimit-leftLimit)) * num;
  }
  else 
  {
    i = ( num * x ) / fullWidth;
  }
  return (int) i;
}


//----------------------------------------------------------------------------
void albaGUILutHistogramSwatch::Update()
//----------------------------------------------------------------------------
{
  int w = GetSize().GetWidth();   
  int h = GetSize().GetHeight();   

  if( m_Lut == NULL )
  {
    m_Bmp = wxBitmap(w,h);
    return;
  }
  
  
  unsigned char *data = (unsigned char*)malloc(sizeof(unsigned char)*w*h*3);
  unsigned char *p = data;

  int x,y;
  for(y=0; y<h; y++)
  {
    for(x=0; x<w; x++)
    {
      //updating image whit Lut data
      int i = GetLutIndexByPos(w, x);
      albaColor col  = albaColor( m_Lut->GetTableValue(i) );
      albaColor col2 = albaColor::CheckeredColor(col,x,y);

      *p++ = col2.m_Red;
      *p++ = col2.m_Green;
      *p++ = col2.m_Blue;
    }
  }
  
  wxImage img(w,h,data); // data will be freed by the image
  m_Bmp = wxBitmap(img);

  m_UpdateTime = m_Lut->GetMTime();
}
