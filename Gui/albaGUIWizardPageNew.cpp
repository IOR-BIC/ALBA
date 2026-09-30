/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizardPageNew
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
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
#include <wx/statline.h>

#include "albaDecl.h"
#include "albaGUIWizardPageNew.h"
#include "albaGUIWizard.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"
#include "albaGUILutSlider.h"
#include "albaSceneGraph.h"
#include "albaVME.h"
#include "albaVMEOutputVolume.h"

#include "vtkLookupTable.h"
#include "vtkRenderer.h"
#include "vtkMapper.h"
#include "vtkTexture.h"
#include "vtkImageData.h"
#include "mmaVolumeMaterial.h"

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
//BEGIN_EVENT_TABLE(albaGUIWizardPageNew, wxWizardPageSimple)
//EVT_CLOSE(albaGUIWizardPageNew::nvOnCloseWindow)
//END_EVENT_TABLE()

enum ID
{
	ID_SCAN_SLICE_LOAD_PAGE=MINID,
};

//----------------------------------------------------------------------------
albaGUIWizardPageNew::albaGUIWizardPageNew(albaGUIWizard *wizardParent,long style, bool ZCrop, wxString label)
: wxWizardPageSimple(wizardParent)
//----------------------------------------------------------------------------
{
	m_Listener = NULL;
  m_ColorLUT = NULL;
  m_ZCropSlider = NULL;
  m_ZCropOn = false;

  m_GUISizer = new wxBoxSizer( wxHORIZONTAL );
  m_GUIUnderSizer = new wxBoxSizer( wxHORIZONTAL );
  m_LUTSizer = new wxBoxSizer( wxHORIZONTAL );
	m_RwiSizer = new wxBoxSizer( wxHORIZONTAL );
  m_SizerAll = new wxBoxSizer( wxVERTICAL );
  m_GuiView = new albaGUI(this);

	m_Rwi = NULL;
  m_GuiLowerLeft = NULL;
  m_GuiLowerCenter = NULL;
  m_GuiLowerUnderLeft = NULL;
  m_GuiLowerUnderCenter = NULL;

	if(style & albaWIZARDUSERWI)
  {
    m_Rwi = new albaRWI(this);
    m_Rwi->SetSize(0,0,512,384);
    m_Rwi->Show(true);
    m_Rwi->CameraSet(CAMERA_CT);
    m_Rwi->CameraUpdate();

    
    m_LutSlider = new albaGUILutSlider(this,-1,wxPoint(0,0),wxSize(300,24));
    m_LutSlider->SetListener(this);

    m_GuiView->Add(m_LutSlider);
    if (ZCrop)
    {
      m_ZCropSlider = new albaGUILutSlider(this,-1,wxPoint(0,0),wxSize(300,24),0,"Z crop");
      m_ZCropSlider->SetListener(this);
      m_GuiView->Add(m_ZCropSlider);
      m_ZCropOn = true;
    }
    m_GuiView->Reparent(this);
   
    m_RwiSizer->Add(m_Rwi->m_RwiBase,1,wxEXPAND);
    m_SizerAll->Add(m_RwiSizer,0,wxEXPAND);

    m_LUTSizer->Add(m_GuiView,1, wxEXPAND);
    m_SizerAll->Add(m_LUTSizer,0,wxEXPAND|wxALL);
	}
	if(style & albaWIZARDUSEGUI)
	{
		m_GuiLowerLeft = new albaGUI(this);
    m_GuiLowerLeft->FitGui();
		m_GuiLowerLeft->Reparent(this);

    m_GuiLowerCenter = new albaGUI(this);
    m_GuiLowerCenter->FitGui();
    m_GuiLowerCenter->Reparent(this);

    m_GuiLowerUnderLeft = new albaGUI(this);
    m_GuiLowerUnderLeft->FitGui();
    m_GuiLowerUnderLeft->Reparent(this);

    m_GuiLowerUnderCenter = new albaGUI(this);
    m_GuiLowerUnderCenter->FitGui();
    m_GuiLowerUnderCenter->Reparent(this);

		m_GUISizer->Add(m_GuiLowerLeft,0,wxEXPAND);
		m_GUISizer->AddSpacer(82);
		m_GUISizer->Add(m_GuiLowerCenter, 0, wxEXPAND);

    m_GUIUnderSizer->Add(m_GuiLowerUnderLeft,1,wxALL);
    m_GUIUnderSizer->Add(m_GuiLowerUnderCenter,1,wxALL);
  
    m_SizerAll->Add(m_GUISizer,0,wxEXPAND|wxALL);
    m_SizerAll->Add(m_GUIUnderSizer,0,wxEXPAND|wxALL);
	}
	
	SetSizer(m_SizerAll,true);
	m_SizerAll->Fit(this);
}

//----------------------------------------------------------------------------
albaGUIWizardPageNew::~albaGUIWizardPageNew()
//----------------------------------------------------------------------------
{
	cppDEL(m_Rwi);
}
//--------------------------------------------------------------------------------
void albaGUIWizardPageNew::OnEvent(albaEventBase *alba_event)
//--------------------------------------------------------------------------------
{
  switch(alba_event->GetId()) 
  {
  case ID_RANGE_MODIFIED:
    {
      if(alba_event->GetSender() == this->m_LutSlider)
        UpdateActor();  //Windowing
      else if(alba_event->GetSender() == this->m_ZCropSlider)
        albaEventMacro(*alba_event);
    }
    break;

  default:
    albaEventMacro(*alba_event);
  }
}
//----------------------------------------------------------------------------
void albaGUIWizardPageNew::UpdateWindowing()
//----------------------------------------------------------------------------
{
  double *tableRange;
  double scalarRange[2];

  if(m_Rwi == NULL) return;

  vtkActorCollection *actorCollection = m_Rwi->m_RenFront->GetActors();
  actorCollection->InitTraversal();
  actorCollection->GetNextItem();
  tableRange=actorCollection->GetNextItem()->GetTexture()->GetLookupTable()->GetRange();

  actorCollection->InitTraversal();
  actorCollection->GetNextItem();
  actorCollection->GetNextItem()->GetTexture()->GetInput()->GetScalarRange(scalarRange);

  m_LutSlider->SetRange(scalarRange[0],scalarRange[1]);
  m_LutSlider->SetSubRange(tableRange[0], tableRange[1]);
}

//--------------------------------------------------------------------------------
void albaGUIWizardPageNew::UpdateWindowing( double *scalarRange, double *scalarSubRange )
//--------------------------------------------------------------------------------
{
  
  if(m_Rwi == NULL) return;

  m_LutSlider->SetRange(scalarRange[0],scalarRange[1]);
  m_LutSlider->SetSubRange(scalarSubRange[0], scalarSubRange[1]);

  UpdateActor();
}

//--------------------------------------------------------------------------------
void albaGUIWizardPageNew::GetWindowing( double *scalarRange, double *scalarSubRange )
//--------------------------------------------------------------------------------
{
  m_LutSlider->GetRange(&scalarRange[0],&scalarRange[1]);
  m_LutSlider->GetSubRange(&scalarSubRange[0],&scalarSubRange[1]);
}




//----------------------------------------------------------------------------
void albaGUIWizardPageNew::UpdateActor()
//----------------------------------------------------------------------------
{

  if(m_Rwi == NULL) return;

  double low, hi;
  m_LutSlider->GetSubRange(&low,&hi);
  vtkActorCollection *actorCollection = m_Rwi->m_RenFront->GetActors();
  actorCollection->InitTraversal();
  actorCollection->GetNextItem();
  actorCollection->GetNextItem()->GetTexture()->GetLookupTable()->SetRange(low,hi);
  m_Rwi->CameraUpdate();
}
//--------------------------------------------------------------------------------
void albaGUIWizardPageNew::AddGuiLowerLeft(albaGUI *gui)
//--------------------------------------------------------------------------------
{
	m_GuiLowerLeft->AddGui(gui);
	m_GuiLowerLeft->FitGui();
	m_GuiLowerLeft->Update();
}
//--------------------------------------------------------------------------------
void albaGUIWizardPageNew::RemoveGuiLowerLeft(albaGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerLeft->Remove(gui);
  m_GuiLowerLeft->FitGui();
  m_GuiLowerLeft->Update();
}

//--------------------------------------------------------------------------------
void albaGUIWizardPageNew::AddGuiLowerCenter(albaGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerCenter->AddGui(gui);
  m_GuiLowerCenter->FitGui();
  m_GuiLowerCenter->Update();
}

//--------------------------------------------------------------------------------
void albaGUIWizardPageNew::AddGuiLowerUnderCenter(albaGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerUnderCenter->AddGui(gui);
  m_GuiLowerUnderCenter->FitGui();
  m_GuiLowerUnderCenter->Update();
}

//--------------------------------------------------------------------------------
void albaGUIWizardPageNew::AddGuiLowerUnderLeft(albaGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerUnderLeft->AddGui(gui);
  m_GuiLowerUnderLeft->FitGui();
  m_GuiLowerUnderLeft->Update();
}

//--------------------------------------------------------------------------------
void albaGUIWizardPageNew::RemoveGuiLowerUnderLeft(albaGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerUnderLeft->Remove(gui);
  m_GuiLowerUnderLeft->FitGui();
  m_GuiLowerUnderLeft->Update();
}

//----------------------------------------------------------------------------
void albaGUIWizardPageNew::RemoveGuiLowerCenter(albaGUI *gui)
{
	m_GuiLowerCenter->Remove(gui);
	m_GuiLowerCenter->FitGui();
	m_GuiLowerCenter->Update();
}

//--------------------------------------------------------------------------------
void albaGUIWizardPageNew::SetNextPage(albaGUIWizardPageNew *nextPage)
//--------------------------------------------------------------------------------
{
	this->SetNext(nextPage);
  nextPage->SetPrev(this);
}
//--------------------------------------------------------------------------------
void albaGUIWizardPageNew::SetZCropBounds(double ZMin, double ZMax)
//--------------------------------------------------------------------------------
{ 
  if(m_ZCropSlider == NULL) return;
  if (ZMax <= ZMin && m_ZCropOn)
  {
    m_GuiView->Remove(m_ZCropSlider);
    m_ZCropOn = false;
  } 
  if(ZMin >= 0 && ZMax > ZMin)
  {
    if (!m_ZCropOn)
    {
      m_GuiView->Add(m_ZCropSlider);
      m_ZCropOn = true;
    }
    m_ZCropSlider->SetRange(ZMin,ZMax);
    m_ZCropSlider->SetSubRange(ZMin, ZMax);
    
  }
}
//--------------------------------------------------------------------------------
void albaGUIWizardPageNew::GetZCropBounds(double ZCropBounds[2])
//--------------------------------------------------------------------------------
{ 
  if(m_ZCropSlider == NULL) return;
  m_ZCropSlider->GetSubRange(&ZCropBounds[0],&ZCropBounds[1]);

}

