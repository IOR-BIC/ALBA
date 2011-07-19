/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGUILutHistogramSwatch.cpp,v $
  Language:  C++
  Date:      $Date: 2011-07-19 10:25:27 $
  Version:   $Revision: 1.1.2.5 $
  Authors:   Crimi Gianluigi
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

#include "medGUILutHistogramSwatch.h"
#include "mafColor.h"
#include "medGUILutHistogramEditor.h"
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
// mafGUILutSwatch
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(medGUILutHistogramSwatch,wxPanel)
  EVT_PAINT(medGUILutHistogramSwatch::OnPaint)
  EVT_ERASE_BACKGROUND(medGUILutHistogramSwatch::OnEraseBackground) 

  EVT_LEFT_DOWN(medGUILutHistogramSwatch::OnLeftMouseButtonDown)
  EVT_LEFT_UP(medGUILutHistogramSwatch::OnLeftMouseButtonUp)
  EVT_MOTION(medGUILutHistogramSwatch::OnMouseMotion)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
medGUILutHistogramSwatch::medGUILutHistogramSwatch(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
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

medGUILutHistogramSwatch::medGUILutHistogramSwatch(mafGUI *parent, wxWindowID id, wxString name, vtkDataSet *dataSet, mmaVolumeMaterial *material, wxSize size, bool showText)
:wxPanel(parent,id,wxDefaultPosition, size,  wxTAB_TRAVERSAL | wxSIMPLE_BORDER )
{

  m_ShowThreshold = false;
  m_Editable = false;
  
  m_MouseInWindow = false;
  m_Tip = "";
  SetCursor(*wxCROSS_CURSOR);

  m_Font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  m_Font.SetPointSize(9);
  
  SetMaterial(material);
  SetDataSet(dataSet);
  SetListener(parent);

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

  //add label only if showText is true
  if (showText)
  {
    wxStaticText	*lab  = new wxStaticText(parent, id , name ,wxDefaultPosition, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    sizer->Add( lab,  0, wxRIGHT, LM);
    sizer->Add( this, 0, wxEXPAND, HM);
  }
  else
  {
    wxStaticText *foo_l = new wxStaticText(parent,id, "",wxDefaultPosition, wxSize(4,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
    sizer->Add(foo_l, 0, wxLEFT, 0);
    sizer->Add( this, 0, wxEXPAND, 0);
  }
  
  parent->Add(sizer,0,wxALL, M); 
}

//----------------------------------------------------------------------------
void medGUILutHistogramSwatch::OnPaint(wxPaintEvent &event)
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
void medGUILutHistogramSwatch::OnLeftMouseButtonDown(wxMouseEvent &event)
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

    medGUILutHistogramEditor::ShowLutHistogramDialog(m_DataSet,m_Material,"Histogram Lut Editor",m_Listener,  GetId());
    Update();
    Refresh();
  }

  //notify the user
  mafEventMacro(mafEvent(this,GetId()));
}
//----------------------------------------------------------------------------
void medGUILutHistogramSwatch::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  //no left mouse button functionality now.
}
//----------------------------------------------------------------------------
void medGUILutHistogramSwatch::OnMouseMotion(wxMouseEvent &event)
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
void medGUILutHistogramSwatch::SetMaterial(mmaVolumeMaterial *material)
//----------------------------------------------------------------------------
{
  //update lut pointer
  m_Material = material;
  if (m_Material)
    m_Lut=material->m_ColorLut;
  else 
    m_Lut=NULL;
  Update();
  Refresh();
}

//----------------------------------------------------------------------------
void medGUILutHistogramSwatch::SetDataSet(vtkDataSet *dataSet)
//----------------------------------------------------------------------------
{
  //Update volume pointer
  m_DataSet = dataSet;
  Update();
  Refresh();
}

//----------------------------------------------------------------------------
voif medGUILutHistogramSwatch::showThreshold(bool b) 
 //----------------------------------------------------------------------------
{
  m_ShowThreshold =b;
  Update();
  Refresh()
}

//----------------------------------------------------------------------------
void medGUILutHistogramSwatch::Modified()
//----------------------------------------------------------------------------
{
  Update();
  Refresh();
}


//----------------------------------------------------------------------------
void medGUILutHistogramSwatch::Update()
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
  if (m_ShowThreshold)
    for(y=0; y<h; y++)
    {
      for(x=0; x<w; x++)
      {
        double *range,*subRange,rangeSize;
        float i;
        range=m_DataSet->GetScalarRange();
        subRange=subRange=m_Lut->GetTableRange();
        rangeSize=range[1]-range[0];
        float leftLimit, rightLimit;
        leftLimit=((subRange[0]-range[0])/rangeSize)*w;
        rightLimit=((subRange[1]-range[0])/rangeSize)*w;

        
        //Generating the pixel from the Lut Value
        if (x<leftLimit) i=0;
        else if (x>rightLimit) i=num;
        else i = (( x - leftLimit ) / (rightLimit-leftLimit)) * num;
        mafColor col  = mafColor( m_Lut->GetTableValue(i) );
        mafColor col2 = mafColor::CheckeredColor(col,x,y);

        *p++ = col2.m_Red;
        *p++ = col2.m_Green;
        *p++ = col2.m_Blue;
      }
    }
  else
    for(y=0; y<h; y++)
    {
      for(x=0; x<w; x++)
      {
        //Generating the pixel from the Lut Value
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
