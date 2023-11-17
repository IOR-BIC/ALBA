/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaLightKit
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

#include "albaLightKit.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaGUIButton.h"

#include "vtkRenderWindow.h"
#include "vtkLight.h"
#include "vtkRenderer.h"

//----------------------------------------------------------------------------
albaLightKit::albaLightKit(wxWindow* parent, vtkRenderer *r, albaObserver *Listener)
//----------------------------------------------------------------------------
{
	m_LightRenderer = r;
	
	m_LightList       = NULL;
	for(int i = 0; i < MAX_NUM_LIGHT; i++) 
    m_LightItemList[i] = NULL;
	m_LightAzimuth	  = 0.0;
	m_LightElevation	= 0.0;
	m_LightIntensity	= 1.0;
	m_LightOnOff		  = 1;
	m_LightColor		  = wxColour(255,255,255);
	m_SelectedLight	  = NULL;
	m_LightCounter		= 0;

	m_Listener				= Listener;
	m_LightParentPanel= parent;
	
	CreateGui();
}
//----------------------------------------------------------------------------
albaLightKit::~albaLightKit() 
//----------------------------------------------------------------------------
{
	if(m_Gui)	
    m_Gui->SetListener(NULL);
	cppDEL(m_Gui);
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum LIGHTKIT_WIDGET_ID
{
	ID_LIGHT_LIST = MINID,
	ID_ADD_LIGHT,
	ID_REMOVE_LIGHT,
	ID_LIGHT_AZIMUTH,
	ID_LIGHT_ELEVATION,
	ID_LIGHT_INTENSITY,
	ID_LIGHT_COLOR,
	ID_LIGHT_ON_OFF,
};
//----------------------------------------------------------------------------
void albaLightKit::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new albaGUI(this);
	m_Gui->Show(true);

	LightItem *item = NULL;

  int rm = m_Gui->GetMetrics(GUI_ROW_MARGIN);
  int dw = m_Gui->GetMetrics(GUI_DATA_WIDTH);
  int lw = m_Gui->GetMetrics(GUI_LABEL_WIDTH);
  int lm = m_Gui->GetMetrics(GUI_LABEL_MARGIN);
  int wm = m_Gui->GetMetrics(GUI_WIDGET_MARGIN);
  int bh = m_Gui->GetMetrics(GUI_BUTTON_HEIGHT);
  wxSize bs(dw/2-wm,bh);
  wxSize ls(lw,bh);
	wxPoint dp = wxDefaultPosition;
	
  wxStaticText *lab = new wxStaticText(m_Gui,-1,"Lights",dp,ls, wxALIGN_RIGHT);
	
	m_ButtonAdd = new albaGUIButton (m_Gui,ID_ADD_LIGHT,"Add",dp,bs);
  m_ButtonRemove = new albaGUIButton (m_Gui,ID_REMOVE_LIGHT,"Remove",dp,bs);
	m_ButtonAdd->SetListener(this);
	m_ButtonRemove->SetListener(this);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add( lab,        0, wxALIGN_CENTRE|wxRIGHT, lm);
	sizer->Add( m_ButtonAdd, 0, wxRIGHT, wm);
	sizer->Add( m_ButtonRemove, 0, wxRIGHT, wm);
  m_Gui->Add(sizer,0,wxALL,rm); 
	
	m_LightList = m_Gui->ListBox(ID_LIGHT_LIST, " ");

	m_Gui->Bool(ID_LIGHT_ON_OFF,"On-Off",&m_LightOnOff,0);
	m_Gui->FloatSlider(ID_LIGHT_AZIMUTH,"Azimuth",&m_LightAzimuth,-90.0,90.0);
	m_Gui->FloatSlider(ID_LIGHT_ELEVATION,"Elevation",&m_LightElevation,-90.0,90.0);
	m_Gui->FloatSlider(ID_LIGHT_INTENSITY,"Intensity",&m_LightIntensity,0.0,1.0);
	m_Gui->Color(ID_LIGHT_COLOR,"Color",&m_LightColor);

	//init the listbox with the existent light
	for(int i=0; i<MAX_NUM_LIGHT; i++)
	{
		item = m_LightItemList[i];
		if(item)
			m_LightList->Append(item->m_Name, item);
	}
	m_Gui->Divider();

	m_Gui->Update();

  //if(m_LightList->Number() == 0)
 	if(m_LightList->GetCount() == 0)
	{
		//let enabled only add light button
		EnableWidget(false);
	}
	else
	{
		m_LightList->SetSelection(0,true);
		OnEvent(&albaEvent(m_Gui,ID_LIGHT_LIST));
	}
}
//----------------------------------------------------------------------------
void albaLightKit::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
    switch(e->GetId())
    {
      case ID_LIGHT_LIST:
      {
        //light selected in the listbox
        int sel = m_LightList->GetSelection();
        if (sel != -1)
        {
          m_SelectedLight   = (LightItem *)m_LightList->GetClientData(sel);
          m_LightOnOff			= m_SelectedLight->m_OnOff;
          m_LightAzimuth		= m_SelectedLight->m_Azimuth;
          m_LightElevation	= m_SelectedLight->m_Elevation;
          m_LightIntensity	= m_SelectedLight->m_Intensity;
          m_LightColor			= m_SelectedLight->m_Colour;
          m_Gui->Update();
        }
      }
      break;
      case ID_ADD_LIGHT:
        AddLight();
      break;
      case ID_REMOVE_LIGHT:
      {
        int sel = m_LightList->GetSelection();
        if (sel != -1)
          RemoveLight(sel);

        //select the first light of the list if there is almost one light
        //if(m_LightList->Number() > 0)
        if(m_LightList->GetCount() > 0)
        {
          m_LightList->SetSelection(0,true);
          OnEvent(&albaEvent(m_Gui,ID_LIGHT_LIST));
        }
      }
      break;
      case ID_LIGHT_ON_OFF:
        if(m_SelectedLight)
        {
          m_SelectedLight->m_Light->SetSwitch(m_LightOnOff);
          m_SelectedLight->m_OnOff = m_LightOnOff;
        }
      break;
      case ID_LIGHT_AZIMUTH:
      case ID_LIGHT_ELEVATION:
        if(m_SelectedLight)
        {
          m_SelectedLight->m_Light->SetDirectionAngle(m_LightElevation, m_LightAzimuth);
          m_SelectedLight->m_Azimuth = m_LightAzimuth;
          m_SelectedLight->m_Elevation = m_LightElevation;
        }
      break;
      case ID_LIGHT_INTENSITY:
        if(m_SelectedLight)
        {
          m_SelectedLight->m_Light->SetIntensity(m_LightIntensity);
          m_SelectedLight->m_Intensity = m_LightIntensity;
        }
      break;
      case ID_LIGHT_COLOR:
        if(m_SelectedLight)
        {
          m_SelectedLight->m_Light->SetColor(m_LightColor.Red()/255.0,m_LightColor.Green()/255.0,m_LightColor.Blue()/255.0);
          m_SelectedLight->m_Colour = m_LightColor;
        }
      break;
    }
    
		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaLightKit::AddLight()
//----------------------------------------------------------------------------
{
//	static int m_LightCounter = 0;
	//if(m_LightList->Number() >= MAX_NUM_LIGHT) return;
  if(m_LightList->GetCount() >= MAX_NUM_LIGHT) return;

	//initialize light parameters
//	ParametersInit();
	//build new light
	vtkLight *Light = vtkLight::New();
	Light->SetLightTypeToCameraLight();
	Light->SetColor(m_LightColor.Red()/255.0,m_LightColor.Green()/255.0,m_LightColor.Blue()/255.0);
	Light->SetIntensity(m_LightIntensity);
	
	//name of the light
	wxString light_name = "light " + albaString::Format("%d",m_LightCounter);
	
	//build LightItem and add it to the list
	m_SelectedLight = new LightItem(light_name,m_LightOnOff,m_LightAzimuth,m_LightElevation,m_LightIntensity,m_LightColor,Light);
	m_LightItemList[m_LightCounter] = m_SelectedLight;
	m_LightList->Append(light_name,m_SelectedLight);  
  //m_LightList->SetSelection(m_LightList->Number() - 1,true);
  m_LightList->SetSelection(m_LightList->GetCount() - 1,true);
	m_LightRenderer->AddLight(m_SelectedLight->m_Light);

	m_LightCounter++;
	EnableWidget(true);
}
//----------------------------------------------------------------------------
void albaLightKit::RemoveLight(int sel)
//----------------------------------------------------------------------------
{
	m_SelectedLight = (LightItem *)m_LightList->GetClientData(sel);
	for(int l=0;l<MAX_NUM_LIGHT;l++)
	{
		if(m_LightItemList[l] == m_SelectedLight)
		{
			m_LightItemList[l] = NULL;
			break;
		}
	}
	m_LightRenderer->RemoveLight(m_SelectedLight->m_Light);
	m_LightList->Delete(sel);
	delete m_SelectedLight;
	m_SelectedLight = NULL;
	
	//if(m_LightList->Number() == 0)
  if(m_LightList->GetCount() == 0)
	{
		EnableWidget(false);
		ParametersInit();
	}
}
//----------------------------------------------------------------------------
void albaLightKit::EnableWidget(bool enable)
//----------------------------------------------------------------------------
{
	m_ButtonRemove->Enable(enable);
	m_Gui->Enable(ID_LIGHT_ON_OFF ,enable);
	m_Gui->Enable(ID_LIGHT_AZIMUTH ,enable);
	m_Gui->Enable(ID_LIGHT_ELEVATION ,enable);
	m_Gui->Enable(ID_LIGHT_INTENSITY ,enable);
	m_Gui->Enable(ID_LIGHT_COLOR ,enable);
}
//----------------------------------------------------------------------------
void albaLightKit::ParametersInit()
//----------------------------------------------------------------------------
{
	m_LightAzimuth = 0.0;
	m_LightElevation = 0.0;
	m_LightIntensity = 1.0;
	m_LightOnOff = 1;
	m_LightColor = wxColour(255,255,255);
	m_LightCounter = 0;
	m_Gui->Update();
}
