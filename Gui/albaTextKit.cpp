/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTextKit
 Authors: Paolo Quadrani
 
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

#include "albaTextKit.h"

#include "albaDecl.h"
#include "albaGUIDialog.h"
#include "albaGUI.h"
#include "albaRWI.h"

#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"

//----------------------------------------------------------------------------
albaTextKit::albaTextKit(wxWindow* parent, vtkRenderer *renderer, albaObserver *Listener)
//----------------------------------------------------------------------------
{
	m_TextRenderer = renderer;
	
	m_Listener		= Listener;
	m_ParentPanel = parent;
  m_ShowText = 1;
  m_TextAlign = 0;
  m_TextOffset = 0;
  m_TextInView = "";
  m_TextSize = 12;
  
  vtkNEW(m_TextMapper);
  m_TextMapper->SetInput(m_TextInView.GetCStr());
  m_TextMapper->GetTextProperty()->SetFontFamily(VTK_ARIAL);
  m_TextMapper->GetTextProperty()->SetLineOffset(0.5);
  m_TextMapper->GetTextProperty()->SetLineSpacing(1.5);

  vtkNEW(m_TextActor);
  m_TextActor->SetMapper(m_TextMapper);
  m_TextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  m_TextRenderer->AddActor(m_TextActor);

  SetTextAlignment();
  SetTextColor(240,240,240);
  SetTextSize(m_TextSize);
  CreateGui();
}
//----------------------------------------------------------------------------
albaTextKit::~albaTextKit() 
//----------------------------------------------------------------------------
{
  if(m_Gui)	
	  m_Gui->SetListener(NULL);
	cppDEL(m_Gui);

  if (m_TextActor)
  {
    m_TextRenderer->RemoveActor(m_TextActor);
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
  ID_TEXT_ALIGN,
  ID_TEXT_SIZE
};
//----------------------------------------------------------------------------
void albaTextKit::CreateGui()
//----------------------------------------------------------------------------
{
  wxString align[9] = {"Upper left","Upper center","Upper right",
                       "Center left","Center","Center right",
                       "Lower left","Lower center","Lower right"};

  m_Gui = new albaGUI(this);
	m_Gui->Show(true);
  m_Gui->Label("Text kit",true);
  m_Gui->Bool(ID_SHOW_TEXT,"Show",&m_ShowText,0,"Show/hide text");
  m_Gui->String(ID_TEXT_IN_VIEW,"View text",&m_TextInView,"Text visualized into the view");
  m_Gui->Button(ID_NLINE_TEXT_IN_VIEW,"Edit","n-line text");

  m_Gui->Color(ID_TEXT_COLOR,"Color",&m_TextColor);
  m_Gui->Combo(ID_TEXT_ALIGN,"Align",&m_TextAlign,9,align,"Align text inside the view");
  m_Gui->Integer(ID_TEXT_SIZE,"Font size",&m_TextSize,1,300);
	m_Gui->Divider();
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaTextKit::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case ID_NLINE_TEXT_IN_VIEW:
		{
			albaGUIDialog dlg("Edit text", albaCLOSEWINDOW | albaRESIZABLE | albaOK | albaCANCEL);
			wxTextCtrl *text = new wxTextCtrl(&dlg, -1, wxString(m_TextInView.GetCStr()), wxDefaultPosition, wxSize(200, 200), wxTE_MULTILINE);
			dlg.Add(text, 1);
			int answer = dlg.ShowModal();
			wxString s = text->GetValue();
			m_TextInView = s;
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
			SetTextColor(m_TextColor.Red(), m_TextColor.Green(), m_TextColor.Blue());
			break;
		case ID_TEXT_ALIGN:
			SetTextAlignment(m_TextAlign);
			break;
		case ID_TEXT_SIZE:
			SetTextSize(m_TextSize);
			break;
		}
		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaTextKit::SetText(const char *text)
//----------------------------------------------------------------------------
{
  m_TextInView = text;
  m_TextMapper->SetInput(m_TextInView.GetCStr());
}
//----------------------------------------------------------------------------
void albaTextKit::SetTextColor(int textColor[3])
//----------------------------------------------------------------------------
{
  SetTextColor(textColor[0],textColor[1],textColor[2]);
}
//----------------------------------------------------------------------------
void albaTextKit::SetTextColor(int textColorRed,int textColorGreen,int textColorBlue )
//----------------------------------------------------------------------------
{
  m_TextColor.Set(textColorRed,textColorGreen,textColorBlue);
  m_TextMapper->GetTextProperty()->SetColor(m_TextColor.Red()/255.0,m_TextColor.Green()/255.0,m_TextColor.Blue()/255.0);
}
//----------------------------------------------------------------------------
void albaTextKit::SetTextPosition(double nv_x, double nv_y)
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
void albaTextKit::SetTextAlignment(int align)
//----------------------------------------------------------------------------
{
  m_TextAlign = align;

  switch(align)
  {
    case UPPER_LEFT:
      m_TextMapper->GetTextProperty()->SetJustificationToLeft();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToTop();
      SetTextPosition(0.01,1);
    break;
    case UPPER_CENTER:
      m_TextMapper->GetTextProperty()->SetJustificationToCentered ();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToTop();
      SetTextPosition(0.5,1);
    break;
    case UPPER_RIGHT:
      m_TextMapper->GetTextProperty()->SetJustificationToRight();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToTop();
      SetTextPosition(1,1);
    break;

    case CENTER_LEFT:
      m_TextMapper->GetTextProperty()->SetJustificationToLeft();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToCentered();
      SetTextPosition(0.01,0.5);
    break;
    case CENTER_CENTER:
      m_TextMapper->GetTextProperty()->SetJustificationToCentered ();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToCentered();
      SetTextPosition(0.5,0.5);
    break;
    case CENTER_RIGHT:
      m_TextMapper->GetTextProperty()->SetJustificationToRight();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToCentered();
      SetTextPosition(1,0.5);
    break;

    case LOWER_LEFT:
      m_TextMapper->GetTextProperty()->SetJustificationToLeft();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToBottom();
      SetTextPosition(0.01,0.01);
      break;
    case LOWER_CENTER:
      m_TextMapper->GetTextProperty()->SetJustificationToCentered ();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToBottom();
      SetTextPosition(0.5,0.01);
      break;
    case LOWER_RIGHT:
      m_TextMapper->GetTextProperty()->SetJustificationToRight();
      m_TextMapper->GetTextProperty()->SetVerticalJustificationToBottom();
      SetTextPosition(1,0.01);
      break;

    default:
      // incorrect value passed -> set to default 
      SetTextAlignment(UPPER_LEFT);
    break;
  }
}
//----------------------------------------------------------------------------
void albaTextKit::SetTextSize(int size)
//----------------------------------------------------------------------------
{
  m_TextSize = size;
  m_TextMapper->GetTextProperty()->SetFontSize(size);
}
