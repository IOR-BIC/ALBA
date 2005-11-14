/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTextKit.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-14 16:53:21 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafTextKit.h"

#include "mafDecl.h"
#include "mmgDialog.h"
#include "mmgGui.h"
#include "mafRWI.h"

#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"

//----------------------------------------------------------------------------
mafTextKit::mafTextKit(wxWindow* parent, mafRWI *rwi, mafObserver *Listener)
//----------------------------------------------------------------------------
{
	m_Rwi = rwi;
	
	m_Listener		= Listener;
	m_ParentPanel = parent;
  m_ShowText = 1;
  m_TextAlign = 0;
  m_TextOffset = 0;
  m_TextInView = "";
  
  vtkNEW(m_TextMapper);
  m_TextMapper->SetInput(m_TextInView.GetCStr());
  m_TextMapper->GetTextProperty()->AntiAliasingOff();
  m_TextMapper->GetTextProperty()->SetFontFamily(VTK_TIMES);
  m_TextMapper->GetTextProperty()->SetColor(0.8,0.8,0.8);
  m_TextMapper->GetTextProperty()->SetLineOffset(0.5);
  m_TextMapper->GetTextProperty()->SetLineSpacing(1.5);

  vtkNEW(m_TextActor);
  m_TextActor->SetMapper(m_TextMapper);
  m_TextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  m_Rwi->m_RenFront->AddActor(m_TextActor);

  SetTextAlignment();
  SetTextColor(240,240,240);
  CreateGui();
}
//----------------------------------------------------------------------------
mafTextKit::~mafTextKit() 
//----------------------------------------------------------------------------
{
  if(m_Gui)	
	  m_Gui->SetListener(NULL);
	cppDEL(m_Gui);

  if (m_TextActor)
  {
    m_Rwi->m_RenFront->RemoveActor(m_TextActor);
  }
  vtkDEL(m_TextMapper);
  vtkDEL(m_TextActor);
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum TEXT_KIT_WIDGET_ID
{
	ID_TEXT_IN_VIEW = MINID,
  ID_NLINE_TEXT_IN_VIEW,
  ID_SHOW_TEXT,
  ID_TEXT_POSITION,
  ID_TEXT_COLOR,
  ID_TEXT_ALIGN
};
//----------------------------------------------------------------------------
void mafTextKit::CreateGui()
//----------------------------------------------------------------------------
{
  wxString align[4] = {"upper left","upper right","lower left","lower right"};
  
  m_Gui = new mmgGui(this);
	m_Gui->Show(true);
  m_Gui->Bool(ID_SHOW_TEXT,"show",&m_ShowText,0,"Show/hide text");
  m_Gui->String(ID_TEXT_IN_VIEW,"view text",&m_TextInView,"Text visualized into the view");
  m_Gui->Button(ID_NLINE_TEXT_IN_VIEW,"edit","n-line text");

  m_Gui->Color(ID_TEXT_COLOR,"color",&m_TextColor);
  m_Gui->Combo(ID_TEXT_ALIGN,"align",&m_TextAlign,4,align,"Align text inside the view");
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafTextKit::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case ID_NLINE_TEXT_IN_VIEW:
      {
        mmgDialog dlg("Edit text",mafCLOSEWINDOW|mafRESIZABLE|mafOK|mafCANCEL);
        wxTextCtrl *text = new wxTextCtrl(&dlg,-1,wxString(m_TextInView.GetCStr()),wxDefaultPosition,wxSize(200,200),wxTE_MULTILINE);
        dlg.Add(text,1);
        int answer = dlg.ShowModal();
        wxString s = text->GetValue();
        m_TextInView = s.c_str();
        m_TextMapper->SetInput(m_TextInView.GetCStr());
      }
      break;
      case ID_TEXT_IN_VIEW:
        m_TextMapper->SetInput(m_TextInView.GetCStr());
      break;
      case ID_SHOW_TEXT:
        m_TextActor->SetVisibility(m_ShowText);
      break;
      case ID_TEXT_COLOR:
        SetTextColor(m_TextColor.Red(),m_TextColor.Green(),m_TextColor.Blue());
        break;
      case ID_TEXT_ALIGN:
        SetTextAlignment(m_TextAlign);
      break;
    }
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void mafTextKit::SetText(const char *text)
//----------------------------------------------------------------------------
{
  m_TextInView = text;
  m_TextMapper->SetInput(m_TextInView.GetCStr());
}
//----------------------------------------------------------------------------
void mafTextKit::SetTextColor(int textColor[3])
//----------------------------------------------------------------------------
{
  SetTextColor(textColor[0],textColor[1],textColor[2]);
}
//----------------------------------------------------------------------------
void mafTextKit::SetTextColor(int textColorRed,int textColorGreen,int textColorBlue )
//----------------------------------------------------------------------------
{
  m_TextColor.Set(textColorRed,textColorGreen,textColorBlue);
  m_TextMapper->GetTextProperty()->SetColor(m_TextColor.Red()/255.0,m_TextColor.Green()/255.0,m_TextColor.Blue()/255.0);
}
//----------------------------------------------------------------------------
void mafTextKit::SetTextPosition(double nv_x, double nv_y)
//----------------------------------------------------------------------------
{
  double pos_x, pos_y;
  pos_x = nv_x > 1 ? 1 : nv_x;
  pos_x = nv_x < -1 ? -1 : nv_x;
  pos_y = nv_y > 1 ? 1 : nv_y;
  pos_y = nv_y < -1 ? -1 : nv_y;
  m_TextPosition[0] = pos_x;
  m_TextPosition[1] = pos_y;

  m_TextActor->SetPosition(m_TextPosition[0]+m_TextOffset, m_TextPosition[1]-m_TextOffset);
}
//----------------------------------------------------------------------------
void mafTextKit::SetTextAlignment(int align)
//----------------------------------------------------------------------------
{
  switch(align)
  {
    case UPPER_RIGHT:
      m_TextAlign = align;
      m_TextMapper->GetTextProperty()->SetJustificationToRight();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToTop();
      SetTextPosition(1,1);
  	break;
    case LOWER_LEFT:
      m_TextAlign = align;
      m_TextMapper->GetTextProperty()->SetJustificationToLeft();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToBottom();
      SetTextPosition(0,0.01);
    break;
    case LOWER_RIGHT:
      m_TextAlign = align;
      m_TextMapper->GetTextProperty()->SetJustificationToRight();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToBottom();
      SetTextPosition(1,0.01);
    break;
    default:
      // UPPER_LEFT
      m_TextAlign = align;
      m_TextMapper->GetTextProperty()->SetJustificationToLeft();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToTop();
      SetTextPosition(0,1);
    break;
  }
}
