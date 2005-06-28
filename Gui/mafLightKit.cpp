/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLightKit.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-28 09:51:05 $
  Version:   $Revision: 1.2 $
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

#include "mafLightKit.h"

#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgButton.h"

#include "vtkRenderWindow.h"
#include "vtkLight.h"
#include "vtkRenderer.h"

//----------------------------------------------------------------------------
mafLightKit::mafLightKit(wxWindow* parent, vtkRenderer *r, mafObserver *Listener)
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
mafLightKit::~mafLightKit() 
//----------------------------------------------------------------------------
{
	if(m_Gui)	m_Gui->SetListener(NULL);
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
void mafLightKit::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mmgGui(this);
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
	
	wxStaticText *lab = new wxStaticText(m_Gui,-1,"lights list",dp,ls);
  lab->SetFont(m_Gui->GetBoldFont());
	
	m_ButtonAdd = new mmgButton (m_Gui,ID_ADD_LIGHT,"add",dp,bs);
  m_ButtonRemove = new mmgButton (m_Gui,ID_REMOVE_LIGHT,"remove",dp,bs);
	m_ButtonAdd->SetListener(this);
	m_ButtonRemove->SetListener(this);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add( lab,        0, wxRIGHT, lm);
	sizer->Add( m_ButtonAdd, 0, wxRIGHT, wm);
	sizer->Add( m_ButtonRemove, 0, wxRIGHT, wm);
  m_Gui->Add(sizer,0,wxALL,rm); 
	
	m_LightList = m_Gui->ListBox(ID_LIGHT_LIST, " ");

	m_Gui->Bool(ID_LIGHT_ON_OFF,"on-off",&m_LightOnOff,0);
	m_Gui->FloatSlider(ID_LIGHT_AZIMUTH,"azimuth",&m_LightAzimuth,-90.0,90.0);
	m_Gui->FloatSlider(ID_LIGHT_ELEVATION,"elevation",&m_LightElevation,-90.0,90.0);
	m_Gui->FloatSlider(ID_LIGHT_INTENSITY,"intensity",&m_LightIntensity,0.0,1.0);
	m_Gui->Color(ID_LIGHT_COLOR,"color",&m_LightColor);

	//init the listbox with the existent light
	for(int i=0; i<MAX_NUM_LIGHT; i++)
	{
		item = m_LightItemList[i];
		if(item)
			m_LightList->Append(item->name, item);
	}
	m_Gui->Update();

 	if(m_LightList->Number() == 0)
	{
		//let enabled only add light button
		EnableWidget(false);
	}
	else
	{
		m_LightList->SetSelection(0,true);
		OnEvent(&mafEvent(m_Gui,ID_LIGHT_LIST));
	}
}
//----------------------------------------------------------------------------
void mafLightKit::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
          m_LightOnOff			= m_SelectedLight->on_off;
          m_LightAzimuth		= m_SelectedLight->azimuth;
          m_LightElevation	= m_SelectedLight->elevation;
          m_LightIntensity	= m_SelectedLight->intensity;
          m_LightColor			= m_SelectedLight->colour;
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
        if(m_LightList->Number() > 0)
        {
          m_LightList->SetSelection(0,true);
          OnEvent(&mafEvent(m_Gui,ID_LIGHT_LIST));
        }
      }
      break;
      case ID_LIGHT_ON_OFF:
        if(m_SelectedLight)
        {
          m_SelectedLight->light->SetSwitch(m_LightOnOff);
          m_SelectedLight->on_off = m_LightOnOff;
        }
      break;
      case ID_LIGHT_AZIMUTH:
      case ID_LIGHT_ELEVATION:
        if(m_SelectedLight)
        {
          m_SelectedLight->light->SetDirectionAngle(m_LightElevation, m_LightAzimuth);
          m_SelectedLight->azimuth = m_LightAzimuth;
          m_SelectedLight->elevation = m_LightElevation;
        }
      break;
      case ID_LIGHT_INTENSITY:
        if(m_SelectedLight)
        {
          m_SelectedLight->light->SetIntensity(m_LightIntensity);
          m_SelectedLight->intensity = m_LightIntensity;
        }
      break;
      case ID_LIGHT_COLOR:
        if(m_SelectedLight)
        {
          m_SelectedLight->light->SetColor(m_LightColor.Red()/255.0,m_LightColor.Green()/255.0,m_LightColor.Blue()/255.0);
          m_SelectedLight->colour = m_LightColor;
        }
      break;
    }
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void mafLightKit::AddLight()
//----------------------------------------------------------------------------
{
//	static int m_LightCounter = 0;
	if(m_LightList->Number() >= MAX_NUM_LIGHT) return;

	//initialize light parameters
//	ParametersInit();
	//build new light
	vtkLight *Light = vtkLight::New();
	Light->SetLightTypeToCameraLight();
	Light->SetColor(m_LightColor.Red()/255.0,m_LightColor.Green()/255.0,m_LightColor.Blue()/255.0);
	Light->SetIntensity(m_LightIntensity);
	
	//name of the light
	wxString light_name = "light " + wxString::Format("%d",m_LightCounter);
	
	//build LightItem and add it to the list
	m_SelectedLight = new LightItem(light_name,m_LightOnOff,m_LightAzimuth,m_LightElevation,m_LightIntensity,m_LightColor,Light);
	m_LightItemList[m_LightCounter] = m_SelectedLight;
	m_LightList->Append(light_name,m_SelectedLight);
	m_LightList->SetSelection(m_LightList->Number() - 1,true);
	m_LightRenderer->AddLight(m_SelectedLight->light);

	m_LightCounter++;
	EnableWidget(true);
}
//----------------------------------------------------------------------------
void mafLightKit::RemoveLight(int sel)
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
	m_LightRenderer->RemoveLight(m_SelectedLight->light);
	m_LightList->Delete(sel);
	delete m_SelectedLight;
	m_SelectedLight = NULL;
	
	if(m_LightList->Number() == 0)
	{
		EnableWidget(false);
		ParametersInit();
	}
}
//----------------------------------------------------------------------------
void mafLightKit::EnableWidget(bool enable)
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
void mafLightKit::ParametersInit()
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
