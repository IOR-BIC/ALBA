/*=========================================================================

 Program: MAF2Medical
 Module: medLogicWithManagers
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
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
#include "medGUIMDIFrame.h"
#include "mafOpManager.h"
#include "medWizardManager.h"
#include "mafGUIApplicationSettings.h"
#include "mafView.h"
#include "mafGUIDialogRemoteFile.h"
#include "mafGUISettingsStorage.h"

#ifdef MAF_USE_VTK
  #include "mafInteractionManager.h"
#endif


//----------------------------------------------------------------------------
medLogicWithManagers::medLogicWithManagers()
: mafLogicWithManagers( new medGUIMDIFrame("maf", wxDefaultPosition, wxSize(800, 600)) )
//----------------------------------------------------------------------------
{
  //Set default values
  m_UseWizardManager  = false;
  m_WizardRunning = false;
  m_WizardManager = NULL;
}

//----------------------------------------------------------------------------
medLogicWithManagers::~medLogicWithManagers()
//----------------------------------------------------------------------------
{
  //free mem
  if(m_WizardManager)
    delete m_WizardManager;
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
     case MENU_WIZARD:
      //The event from the application menu
      if(m_WizardManager) 
        m_WizardManager->WizardRun(e->GetArg());
     break;
     case WIZARD_RUN_STARTING:
      {
        //Manage start event from the wizard lock window close button
        //and disabling toolbar
        mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
        if (c != NULL)
          c->SetAllowCloseWindow(false);
        WizardRunStarting();
      }
     break; 
     case WIZARD_RUN_TERMINATED:
      {
        //Manage end event from the wizard unlock window close button
        //and enabling toolbar
        mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
        if (c != NULL)
          c->SetAllowCloseWindow(true);
        WizardRunTerminated();
        UpdateFrameTitle();
      }
    break; 
    case WIZARD_UPDATE_WINDOW_TITLE:
       {
         UpdateFrameTitle();
       }
    break;
    case WIZARD_REQUIRED_VIEW:
       {
         //The wizard requires a specific view
         //searching on open views or open a new one
         mafView *view;
         const char *viewStr=e->GetString()->GetCStr();
         
         view=m_ViewManager->GetFromList(viewStr);
         if (view)
           m_ViewManager->Activate(view);
         else
           m_ViewManager->ViewCreate(viewStr);
       }
    break;
    case WIZARD_RUN_OP:
      {
        //Running an op required from the wizard
        mafString *tmp=e->GetString();
        mafLogMessage("wiz starting :%s",tmp->GetCStr());
        m_CancelledBeforeOpStarting=true;
        UpdateFrameTitle();
        m_OpManager->OpRun(*(e->GetString()));
        //If the op is started the value of m_CancelledBeforeOpStarting 
        //is changed by OP_RUN_STARTING event
        if (m_CancelledBeforeOpStarting)
        {
          m_CancelledBeforeOpStarting=false;
           m_WizardManager->WizardContinue(false);
        }
        
      }
    break;
    case WIZARD_OP_DELETE:
      {
        //Running an op required from the wizard
        m_CancelledBeforeOpStarting=true;
        UpdateFrameTitle();
        m_OpManager->OpRun(OP_DELETE);
        m_WizardManager->WizardContinue(true);
      }
    break;
    case WIZARD_OP_NEW:
      {
        //Running an op required from the wizard
        if(m_VMEManager)
          m_VMEManager->MSFNew();
        m_WizardManager->WizardContinue(true);
      }
    break;
    case WIZARD_PAUSE:
      {
        UpdateFrameTitle();
        m_OpManager->OpRun(e->GetOp());
      }
      break;
    case WIZARD_RELOAD_MSF:
      {
        UpdateFrameTitle();
        wxString file;
        file=m_VMEManager->GetFileName().GetCStr();
        if(file.IsEmpty())
        {
          mafLogMessage ("Reload requested whitout opened MSF");
          //continue wizard with error
          m_WizardManager->WizardContinue(false);
        }
        else
        {
          int opened=m_VMEManager->MSFOpen(file);
          //continue wizard after open operation
          m_WizardManager->WizardContinue(opened!=MAF_ERROR);
        }
      }
      break;
    case OP_RUN_STARTING:
      {
        mafLogMessage("run starting");
        m_CancelledBeforeOpStarting=false;
        mafLogicWithManagers::OnEvent(maf_event);
      }
    break;
    case OP_RUN_TERMINATED:
      {
        //if the operation was started from the wizard we continue the wizard execution
        if (m_WizardManager && m_WizardRunning)
        {
          m_WizardManager->WizardContinue(e->GetArg());
        }
        //else we manage the operation end by unlock the close button and so on
        else
        {
          mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
          if (c != NULL)
            c->SetAllowCloseWindow(true);
          OpRunTerminated();
        }
      }
      break; 
	case MENU_VIEW_TOOLBAR:
		m_Win->ShowDockPane("wizardgauge",!m_Win->DockPaneIsShown("wizardgauge") );
		m_Win->ShowDockPane("tmpwithtest",!m_Win->DockPaneIsShown("tmpwithtest") );
		m_Win->ShowDockPane("separator",!m_Win->DockPaneIsShown("separator") );
		mafLogicWithManagers::OnEvent(maf_event);
		break;
    case PROGRESSBAR_SHOW:
     {
       if (e->GetSender()==m_WizardManager)
       {
         m_WizardLabel->Enable();
         m_WizardGauge->Enable();
       }
       else
         mafLogicWithManagers::OnEvent(maf_event);
     }
    break;
    case PROGRESSBAR_HIDE:
      {
        if (e->GetSender()==m_WizardManager)
        {
          m_WizardGauge->SetValue(0);
          m_WizardGauge->Enable(false);
          m_WizardLabel->Enable(false);
        }
        else
          mafLogicWithManagers::OnEvent(maf_event);
      }
    break;
    case PROGRESSBAR_SET_VALUE:
      {
        if (e->GetSender()==m_WizardManager)
          m_WizardGauge->SetValue(e->GetArg());
        else
          mafLogicWithManagers::OnEvent(maf_event);
      }
    break;
		default:
      //Call parent event manager
			mafLogicWithManagers::OnEvent(maf_event);
			break; 
		} // end switch case
	} // end if SafeDowncast
}


//----------------------------------------------------------------------------
void medLogicWithManagers::Init(int argc, char **argv)
//----------------------------------------------------------------------------
{
  if (m_WizardManager)
    m_WizardManager->FillSettingDialog(m_SettingsDialog);
  
  mafLogicWithManagers::Init(argc,argv);
}


//----------------------------------------------------------------------------
void medLogicWithManagers::Plug( mafOp *op, wxString menuPath /*= ""*/, bool canUndo /*= true*/, mafGUISettings *setting /*= NULL*/ )
//----------------------------------------------------------------------------
{
  //plug functions needs to be redefined to avoid overload ambiguity
  mafLogicWithManagers::Plug(op,menuPath,canUndo,setting);
}

//----------------------------------------------------------------------------
void medLogicWithManagers::Plug( mafView* view, bool visibleInMenu /*= true*/ )
//----------------------------------------------------------------------------
{
  //plug functions needs to be redefined to avoid overload ambiguity
  mafLogicWithManagers::Plug(view,visibleInMenu);
}

//----------------------------------------------------------------------------
void medLogicWithManagers::Plug( medWizard *wizard, wxString menuPath /*= ""*/ )
//----------------------------------------------------------------------------
{
  //Plugging the wizard    
  if(m_WizardManager)
  {
    m_WizardManager->WizardAdd(wizard, menuPath);
  }
  else 
  {
    mafLogMessage("Enable wizard pluggin to plug wizards"); 
  }
}

//----------------------------------------------------------------------------
void medLogicWithManagers::Configure()
//----------------------------------------------------------------------------
{
  mafLogicWithManagers::Configure();

  //Setting wizard specific data
  if(m_UseWizardManager)
  {
    CreateWizardToolbar();
    m_WizardManager = new medWizardManager();
    m_WizardManager->SetListener(this);
    m_WizardManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
  }

}

//----------------------------------------------------------------------------
void medLogicWithManagers::Show()
//----------------------------------------------------------------------------
{
  mafLogicWithManagers::Show();

  //setting gui pointers to the Wizard Manager
  if(m_WizardManager)
  {
    if(m_MenuBar)
    {
      m_WizardManager->FillMenu(m_WizardMenu);
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
  }
  OnQuit();
}

//----------------------------------------------------------------------------
void medLogicWithManagers::OnQuit()
//----------------------------------------------------------------------------
{
  if (m_WizardManager && m_WizardRunning)
  {
    wxMessageBox(_("Please exit wizard before quit."), _("Wizard running"), wxOK|wxCENTER|wxICON_STOP);
    return;
  }
  if (m_OpManager && m_OpManager->Running())
  {
    wxMessageBox(_("Please exit operation before quit."), _("Operation running"), wxOK|wxCENTER|wxICON_STOP);
    return;
  }
  mafLogicWithManagers::OnQuit();
}


//----------------------------------------------------------------------------
void medLogicWithManagers::VmeSelected( mafNode *vme )
//----------------------------------------------------------------------------
{
  //if a wizard manager was plugged we tell it about vme selection
  if(m_WizardManager)  
    m_WizardManager->VmeSelected(vme);
  mafLogicWithManagers::VmeSelected(vme);
}


void medLogicWithManagers::CreateMenu()
{
  mafLogicWithManagers::CreateMenu();

  //add the wizard menu between "operation" and "settings"
  if (m_UseWizardManager)
  {
    m_WizardMenu = new wxMenu;
    m_MenuBar->Insert(4,m_WizardMenu, _("&Wizard"));
  }
}

//----------------------------------------------------------------------------
void medLogicWithManagers::WizardRunStarting()
//----------------------------------------------------------------------------
{
  //Disabling menu, toolbars and selection by interacion manager and sidebar
  m_WizardRunning=true;

  EnableMenuAndToolbar(false);
  // currently mafInteraction is strictly dependent on VTK (marco)
  #ifdef MAF_USE_VTK
    if(m_InteractionManager) m_InteractionManager->EnableSelect(false);
  #endif
    if(m_SideBar)    m_SideBar->EnableSelect(false);

}

//----------------------------------------------------------------------------
void medLogicWithManagers::WizardRunTerminated()
//----------------------------------------------------------------------------
{
  //Enabling menu, toolbars and selection by interacion manager and sidebar
  m_WizardRunning=false;

  EnableMenuAndToolbar(true);
  // currently mafInteraction is strictly dependent on VTK (marco)
  #ifdef MAF_USE_VTK
    if(m_InteractionManager) 
      m_InteractionManager->EnableSelect(true);
  #endif
    if(m_SideBar)
      m_SideBar->EnableSelect(true);

}


//----------------------------------------------------------------------------
void medLogicWithManagers::UpdateFrameTitle()
//----------------------------------------------------------------------------
{
  //Special Window title management during wizards
  if (m_WizardRunning)
  {
    wxString title(m_AppTitle);
    title += "   " + m_WizardManager->GetDescription();
    m_Win->SetTitle(title);
  }
  else 
    mafLogicWithManagers::UpdateFrameTitle();
}

//----------------------------------------------------------------------------
void medLogicWithManagers::OnFileOpen(const char *file_to_open)
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
  {
    if(m_VMEManager->AskConfirmAndSave())
    {
      wxString file;
      if (m_StorageSettings->GetStorageType() == mafGUISettingsStorage::HTTP)
      {
        if (file_to_open != NULL)
        {
          file = file_to_open;
        }
        else
        {
          mafGUIDialogRemoteFile remoteFile;
          remoteFile.ShowModal();
          file = remoteFile.GetFile().GetCStr();
          mafString protocol;
          if (IsRemote(file.c_str(),protocol))
          {
            m_VMEManager->SetHost(remoteFile.GetHost());
            m_VMEManager->SetRemotePort(remoteFile.GetPort());//
            m_VMEManager->SetUser(remoteFile.GetUser());
            m_VMEManager->SetPassword(remoteFile.GetPassword());
          }
        }
      }
      else      
      {
        wxString wildc    = _("MAF Storage Format file (*." + m_Extension + ")|*." + m_Extension +"|Compressed file (*.z" + m_Extension + ")|*.z" + m_Extension + "");
        if (file_to_open != NULL)
        {
          file = file_to_open;
        }
        else
        {
          file = mafGetOpenFile("", wildc).c_str();
        }
      }

      if(file.IsEmpty() && m_WizardManager && m_WizardRunning)
        m_WizardManager->WizardContinue(false);
      else if(file.IsEmpty())
        return;
        

      int opened=m_VMEManager->MSFOpen(file);
      //If there is a wizzard running we need to continue it after open operation
      if (m_WizardManager && m_WizardRunning)
        m_WizardManager->WizardContinue(opened!=MAF_ERROR);
    }
  }
}


//----------------------------------------------------------------------------
void medLogicWithManagers::OnFileSave()
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
  {
	mafString save_default_folder = m_StorageSettings->GetDefaultSaveFolder();
	save_default_folder.ParsePathName();
	m_VMEManager->SetDirName(save_default_folder);
    int saved=m_VMEManager->MSFSave();
    //If there is a wizard running we need to continue it after save operation
    if (m_WizardManager && m_WizardRunning)
      m_WizardManager->WizardContinue(saved!=MAF_ERROR);
    UpdateFrameTitle();
  }
}

//----------------------------------------------------------------------------
void medLogicWithManagers::OnFileSaveAs()
//----------------------------------------------------------------------------
{
  if(m_VMEManager) 
  {
	mafString save_default_folder = m_StorageSettings->GetDefaultSaveFolder();
	save_default_folder.ParsePathName();
	m_VMEManager->SetDirName(save_default_folder);
    int saved=m_VMEManager->MSFSaveAs();
    //If there is a wizard running we need to continue it after save operation
    if (m_WizardManager && m_WizardRunning)
      m_WizardManager->WizardContinue(saved!=MAF_ERROR);
    UpdateFrameTitle();
  }
}

//----------------------------------------------------------------------------
void medLogicWithManagers::CreateWizardToolbar()
  //----------------------------------------------------------------------------
{
 
  wxToolBar *serparatorBar = new wxToolBar(m_Win,-1,wxPoint(0,0),wxSize(-1,-1),wxTB_FLAT | wxTB_NODIVIDER );
  serparatorBar->SetMargins(0,0);
  serparatorBar->SetToolSeparation(2);
  serparatorBar->SetToolBitmapSize(wxSize(20,20));
  serparatorBar->AddSeparator();
  serparatorBar->Update();
  serparatorBar->Realize();

  m_WizardGauge;
  m_WizardGauge = new wxGauge(m_Win,-1, 100,wxDefaultPosition,wxDefaultSize,wxGA_SMOOTH);
  m_WizardGauge->SetForegroundColour( *wxBLUE );
  m_WizardGauge->Disable();

  

  wxWindow *tmp=new wxWindow(m_Win,-1, wxDefaultPosition,wxSize(50,20));
  
  m_WizardLabel = new wxStaticText(tmp, -1, " Wizard\n Progress",wxDefaultPosition,wxDefaultSize,wxALIGN_LEFT);
  //lab->Show(false);
  m_WizardLabel->Disable();
 

  m_Win->AddDockPane(m_WizardGauge,  wxPaneInfo()
    .Name("wizardgauge")
    //.Caption(wxT("ToolBar1"))
    .Top()
    .Layer(2)
    .ToolbarPane()
    .LeftDockable(false)
    .RightDockable(false)
    .Floatable(false)
    .Movable(false)
    .Gripper(false)
    );
  

  m_Win->AddDockPane(tmp,  wxPaneInfo()
    .Name("tmpwithtest")
    //.Caption(wxT("ToolBar2"))
    .Top()
    .Layer(2)
    .ToolbarPane()
    .LeftDockable(false)
    .RightDockable(false)
    .Floatable(false)
    .Movable(false)
    .Gripper(false)
    );

  m_Win->AddDockPane(serparatorBar,  wxPaneInfo()
    .Name("separator")
    //.Caption(wxT("ToolBar3"))
    .Top()
    .Layer(2)
    .ToolbarPane()
    .LeftDockable(false)
    .RightDockable(false)
    .Floatable(false)
    .Movable(false)
    .Gripper(false)
    );
 
  m_WizardGauge->Show(false);
}