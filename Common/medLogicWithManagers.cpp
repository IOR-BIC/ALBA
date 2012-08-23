/*=========================================================================

 Program: MAF2Medical
 Module: medLogicWithManagers
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medDecl.h"
#include "medLogicWithManagers.h"
#include "medGUIContextualMenu.h"
#include "mafGUIMDIChild.h"
#include "mafViewManager.h"
#include "mafGUIMDIFrame.h"
#include "mafOpManager.h"
#include "medWizardManager.h"
#include "mafGUIApplicationSettings.h"

//----------------------------------------------------------------------------
medLogicWithManagers::medLogicWithManagers()
: mafLogicWithManagers()
//----------------------------------------------------------------------------
{
  m_UseWizardManager  = false;

  m_WizardManager = NULL;

}

//----------------------------------------------------------------------------
medLogicWithManagers::~medLogicWithManagers()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void medLogicWithManagers::ViewContextualMenu(bool vme_menu)
//----------------------------------------------------------------------------
{
  // Create and visualize the contextual menu for the active vme
  medGUIContextualMenu *contextMenu = new medGUIContextualMenu();
  contextMenu->SetListener(this);
  mafView *v = m_ViewManager->GetSelectedView();
  mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
  if(c != NULL)
    contextMenu->ShowContextualMenu(c,v,vme_menu);
  /*  else
  //m_extern_view->ShowContextualMenu(vme_menu);
  contextMenu->ShowContextualMenu(m_extern_view,v,vme_menu);*/
  cppDEL(contextMenu);
}

//----------------------------------------------------------------------------
void medLogicWithManagers::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		case ABOUT_APPLICATION:
			{
        // trap the ABOUT_APPLICATION event and shows the about window with the application infos
				wxString message = m_AppTitle.GetCStr();
				message += _(" Application ");
				message += m_Revision;
				wxMessageBox(message, "About Application");
				mafLogMessage(wxString::Format("%s",m_Revision.GetCStr()));
			}
			break;
		default:
			mafLogicWithManagers::OnEvent(maf_event);
			break; 
    //G,G MANAGE WIZARD EVENTS!!!!!!!!!!!!!!!!!!!

		} // end switch case
	} // end if SafeDowncast
}

//----------------------------------------------------------------------------
void medLogicWithManagers::Plug( mafOp *op, wxString menuPath /*= ""*/, bool canUndo /*= true*/, mafGUISettings *setting /*= NULL*/ )
//----------------------------------------------------------------------------
{
  mafLogicWithManagers::Plug(op,menuPath,canUndo,setting);

  if(m_WizardManager) 
  {
    m_WizardManager->OpAdd(op, menuPath, canUndo, setting);
  }

}

//----------------------------------------------------------------------------
void medLogicWithManagers::Plug( mafView* view, bool visibleInMenu /*= true*/ )
//----------------------------------------------------------------------------
{
  mafLogicWithManagers::Plug(view,visibleInMenu);
}

//----------------------------------------------------------------------------
void medLogicWithManagers::Configure()
//----------------------------------------------------------------------------
{
  mafLogicWithManagers::Configure();

  if(m_UseWizardManager)
  {
    m_WizardManager = new medWizardManager();
    m_WizardManager->SetListener(this);
    m_WizardManager->SetMouse(m_Mouse);
    m_WizardManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
  }

}

//----------------------------------------------------------------------------
void medLogicWithManagers::Show()
//----------------------------------------------------------------------------
{
  mafLogicWithManagers::Show();

  if(m_WizardManager)
  {
    if(m_MenuBar && (m_ImportMenu || m_OpMenu || m_ExportMenu))
    {
      m_WizardManager->FillMenu(m_ImportMenu, m_ExportMenu, m_OpMenu);
      m_WizardManager->SetMenubar(m_MenuBar);
    }
    if(m_ToolBar)
      m_WizardManager->SetToolbar(m_ToolBar);
  }

}

//----------------------------------------------------------------------------
void medLogicWithManagers::HandleException()
//----------------------------------------------------------------------------
{
  int answare = wxMessageBox(_("Do you want to try to save the unsaved work ?"), _("Fatal Exception!!"), wxYES_NO|wxCENTER);
  if(answare == wxYES)
  {
    OnFileSaveAs();
    if (m_OpManager->Running())
      m_OpManager->StopCurrentOperation();
    else if (m_WizardManager->Running());
      m_WizardManager->StopCurrentOperation();
  }
  OnQuit();
}

//----------------------------------------------------------------------------
void medLogicWithManagers::OnQuit()
//----------------------------------------------------------------------------
{



  if (m_OpManager && m_OpManager->Running())
  {

    int answer = wxMessageBox
      (
      _("There are an running operation, are you sure ?"),
      _("Confirm"), 
      wxYES_NO|wxCANCEL|wxICON_QUESTION , m_Win
      );
    if(answer == wxYES) 
      m_OpManager->StopCurrentOperation();
  }

  else if (m_WizardManager && m_WizardManager->Running())
  {
    int answer = wxMessageBox
      (
      _("There are an running wizzard, are you sure ?"),
      _("Confirm"), 
      wxYES_NO|wxCANCEL|wxICON_QUESTION , m_Win
      );
    if(answer == wxYES) 
      m_WizardManager->StopCurrentOperation();
  }

  mafLogicWithManagers::OnQuit();
}

//----------------------------------------------------------------------------
void medLogicWithManagers::VmeSelect( mafEvent &e )
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void medLogicWithManagers::VmeSelected( mafNode *vme )
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void medLogicWithManagers::ViewSelect()
//----------------------------------------------------------------------------
{

}
