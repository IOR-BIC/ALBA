/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIWizardPage.cpp,v $
Language:  C++
Date:      $Date: 2009-05-04 13:24:49 $
Version:   $Revision: 1.5.2.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <wx/statline.h>

#include "mafDecl.h"
#include "medGUIWizardPage.h"
#include "medGUIWizard.h"
#include "mafGUI.h"
#include "mafGUIValidator.h"
#include "mafGUILutSlider.h"
#include "mafSceneGraph.h"
#include "mafVME.h"
#include "mafVMEOutputVolume.h"

#include "vtkLookupTable.h"
#include "vtkRenderer.h"
#include "vtkMapper.h"
#include "vtkTexture.h"
#include "vtkImageData.h"
#include "mmaVolumeMaterial.h"

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
//BEGIN_EVENT_TABLE(medGUIWizardPage, wxWizardPageSimple)
//EVT_CLOSE(medGUIWizardPage::nvOnCloseWindow)
//END_EVENT_TABLE()

enum ID
{
	ID_SCAN_SLICE_LOAD_PAGE=MINID,
};

//----------------------------------------------------------------------------
medGUIWizardPage::medGUIWizardPage(medGUIWizard *wizardParent,long style,wxString label)
: wxWizardPageSimple(wizardParent)
//----------------------------------------------------------------------------
{
	m_Listener = NULL;

	m_FirstPage = NULL;
  m_ColorLUT = NULL;

	m_GUISizer = new wxBoxSizer( wxHORIZONTAL );
  m_GUIUnderSizer = new wxBoxSizer( wxHORIZONTAL );
  m_LUTSizer = new wxBoxSizer( wxHORIZONTAL );
	m_RwiSizer = new wxBoxSizer( wxHORIZONTAL );
  m_SizerAll = new wxBoxSizer( wxVERTICAL );

	m_Rwi = NULL;
  m_GuiLowerLeft = NULL;
  m_GuiLowerCenter = NULL;
  m_GuiLowerUnderLeft = NULL;

	if(style & medUSERWI)
  {
    m_Rwi = new mafRWI(this);
    m_Rwi->SetSize(0,0,512,512);
    m_Rwi->Show(true);
    m_Rwi->CameraUpdate();

    mafGUI *m_GuiView= new mafGUI(this);
    m_LutSlider = new mafGUILutSlider(this,-1,wxPoint(0,0),wxSize(300,24));
    m_LutSlider->SetListener(this);
    
    m_GuiView->Add(m_LutSlider);
    m_GuiView->Reparent(this);

    m_RwiSizer->Add(m_Rwi->m_RwiBase,1,wxEXPAND);
    m_SizerAll->Add(m_RwiSizer,0,wxEXPAND);

    m_LUTSizer->Add(m_GuiView,1, wxEXPAND);
    m_SizerAll->Add(m_LUTSizer,0,wxEXPAND|wxALL);
	}
	if(style & medUSEGUI)
	{
		m_GuiLowerLeft = new mafGUI(this);
    m_GuiLowerLeft->FitGui();
		m_GuiLowerLeft->Reparent(this);

    m_GuiLowerCenter = new mafGUI(this);
    m_GuiLowerCenter->FitGui();
    m_GuiLowerCenter->Reparent(this);

    m_GuiLowerUnderLeft = new mafGUI(this);
    m_GuiLowerUnderLeft->FitGui();
    m_GuiLowerUnderLeft->Reparent(this);

		m_GUISizer->Add(m_GuiLowerLeft,0,wxEXPAND);
    m_GUISizer->Add(m_GuiLowerCenter,0,wxEXPAND);

    m_GUIUnderSizer->Add(m_GuiLowerUnderLeft,1,wxALL);
  
    m_SizerAll->Add(m_GUISizer,0,wxEXPAND|wxALL);
    m_SizerAll->Add(m_GUIUnderSizer,0,wxEXPAND|wxALL);
	}
	
	SetSizer(m_SizerAll,true);
	m_SizerAll->Fit(this);
}

//----------------------------------------------------------------------------
medGUIWizardPage::~medGUIWizardPage()
//----------------------------------------------------------------------------
{
	cppDEL(m_Rwi);
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::OnEvent(mafEventBase *maf_event)
//--------------------------------------------------------------------------------
{
  switch(maf_event->GetId()) 
  {
  case ID_RANGE_MODIFIED:
    {
      //Windowing
      UpdateActor();
    }
    break;

  default:
    mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
void medGUIWizardPage::UpdateWindowing()
//----------------------------------------------------------------------------
{
  double tableRange[2];
  double scalarRange[2];

  vtkActorCollection *actorCollection = m_Rwi->m_RenFront->GetActors();
  actorCollection->InitTraversal();
  actorCollection->GetNextItem();
  actorCollection->GetNextItem()->GetTexture()->GetLookupTable()->GetTableRange(tableRange);

  actorCollection->InitTraversal();
  actorCollection->GetNextItem();
  actorCollection->GetNextItem()->GetTexture()->GetInput()->GetScalarRange(scalarRange);

  m_LutSlider->SetRange(scalarRange[0],scalarRange[1]);
  m_LutSlider->SetSubRange(tableRange[0], tableRange[1]);
}
//----------------------------------------------------------------------------
void medGUIWizardPage::UpdateActor()
//----------------------------------------------------------------------------
{
  double low, hi;
  m_LutSlider->GetSubRange(&low,&hi);
  vtkActorCollection *actorCollection = m_Rwi->m_RenFront->GetActors();
  actorCollection->InitTraversal();
  actorCollection->GetNextItem();
  actorCollection->GetNextItem()->GetTexture()->GetLookupTable()->SetTableRange(low,hi);
  m_Rwi->CameraUpdate();
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::AddGuiLowerLeft(mafGUI *gui)
//--------------------------------------------------------------------------------
{
	m_GuiLowerLeft->AddGui(gui);
	m_GuiLowerLeft->FitGui();
	m_GuiLowerLeft->Update();
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::RemoveGuiLowerLeft(mafGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerLeft->Remove(gui);
  m_GuiLowerLeft->FitGui();
  m_GuiLowerLeft->Update();
}

//--------------------------------------------------------------------------------
void medGUIWizardPage::AddGuiLowerCenter(mafGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerCenter->AddGui(gui);
  m_GuiLowerCenter->FitGui();
  m_GuiLowerCenter->Update();
}

//--------------------------------------------------------------------------------
void medGUIWizardPage::AddGuiLowerUnderLeft(mafGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerUnderLeft->AddGui(gui);
  m_GuiLowerUnderLeft->FitGui();
  m_GuiLowerUnderLeft->Update();
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::SetNextPage(medGUIWizardPage *nextPage)
//--------------------------------------------------------------------------------
{
	this->SetNext(nextPage);
  nextPage->SetPrev(this);
}