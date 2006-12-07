/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgApplicationLayoutSettings.cpp,v $
Language:  C++
Date:      $Date: 2006-12-07 14:42:02 $
Version:   $Revision: 1.7 $
Authors:   Paolo Quadrani
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

#include "mmgApplicationLayoutSettings.h"
#include <wx/config.h>
#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgMDIFrame.h"
#include "mafViewManager.h"
#include "mafView.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafSmartPointer.h"
#include "mafVMEGeneric.h"
#include "mafNodeIterator.h"
#include "mafTagArray.h"
#include "mafViewVTK.h"
#include "mafRWIBase.h"
#include "vtkCamera.h"

#include "mmaApplicationLayout.h"
#include "mafNode.h"

//----------------------------------------------------------------------------
mmgApplicationLayoutSettings::mmgApplicationLayoutSettings(mafObserver *listener)
//----------------------------------------------------------------------------
{
	m_Listener      = listener;
  m_DefaultLayout = "Default Layout";
  m_ViewManager   = NULL;
  m_Layout        = NULL;
  m_Win           = NULL;
  m_List		      = NULL;
  m_Storage       = NULL;

  m_VisibilityVme = false;
  m_ModifiedLayouts = false;
  m_LayoutFileSave = "";

  m_Gui = new mmgGui(this);
  //application layout msf
  m_Gui->Label(_("Application layout msf"), true);
  m_Gui->Label("For becoming definitive the layout saving,");
  m_Gui->Label(" it needs to save the project");
  m_Gui->Button(SAVE_TREE_LAYOUT_ID,_("Save in root"));
  m_Gui->String(LAYOUT_NAME_ID,_("name"),&m_DefaultLayout);
  m_Gui->Bool(LAYOUT_VISIBILITY_VME, _("Visibility"), &m_VisibilityVme ,0,_("If checked the layout will be comprehensive of vme visibility"));
  m_Gui->Button(APPLY_TREE_LAYOUT_ID,_("Apply Root Layout"));
  
  //application layout 
  m_Gui->Divider(2);
  m_Gui->Divider(0);
  m_Gui->Label(_("Application layout"), true);
  m_List= m_Gui->ListBox(ID_LIST_LAYOUT,_(""),100);
  m_Gui->Button(OPEN_LAYOUT_ID,_("Load file"));
  m_Gui->Button(APPLY_LAYOUT_ID,_("Apply layout"));
  m_Gui->Button(ADD_LAYOUT_ID,_("Add Current layout"));
  m_Gui->Button(REMOVE_LAYOUT_ID,_("Remove layout"));
  //m_Gui->Button(SAVE_APPLICATION_LAYOUT_ID,_("Save"));
  m_Gui->FileSave(SAVE_APPLICATION_LAYOUT_ID,_("Browse"), &m_LayoutFileSave);
	m_Gui->Divider();

  InitializeLayout();
}
//----------------------------------------------------------------------------
mmgApplicationLayoutSettings::~mmgApplicationLayoutSettings() 
//----------------------------------------------------------------------------
{
  //mafDEL(m_Layout); // already destroyed by the vme
  m_Gui = NULL; // gui is destroyed by the dialog.
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
  {
    case SAVE_TREE_LAYOUT_ID:
    {
      SaveTreeLayout();
    }
    break;
    case APPLY_TREE_LAYOUT_ID:
      ApplyTreeLayout();
    break;
    case LAYOUT_NAME_ID:
    break;
    case LAYOUT_VISIBILITY_VME:
      m_Gui->Enable(SAVE_APPLICATION_LAYOUT_ID, m_VisibilityVme == 0);
    break;
    case ID_LIST_LAYOUT:
      //select
    break;
    case OPEN_LAYOUT_ID:
      LoadLayout();
    break;
    case APPLY_LAYOUT_ID:
      ApplyLayout();
    break;
    case ADD_LAYOUT_ID:
      AddLayout();
    break;
    case REMOVE_LAYOUT_ID:
      RemoveLayout();
    break;
    case SAVE_APPLICATION_LAYOUT_ID:
      //save
      SaveApplicationLayout();
    break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
  /*wxConfig *config = new wxConfig(wxEmptyString);
  config->Write("DefaultLayout", m_DefaultLayout.GetCStr());
  cppDEL(config);*/
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::SaveTreeLayout()
//----------------------------------------------------------------------------
{
  if (m_ViewManager)
  {
    wxFrame *frame = (wxFrame *)mafGetFrame();
    int pos[2], size[2];
    wxRect rect;
    rect = frame->GetRect();
    pos[0] = rect.GetPosition().x;
    pos[1] = rect.GetPosition().y;
    size[0] = rect.GetSize().GetWidth();
    size[1] = rect.GetSize().GetHeight();

    mafNode *root = m_ViewManager->GetCurrentRoot();
    m_Layout = mmaApplicationLayout::SafeDownCast(root->GetAttribute("ApplicationLayout"));
    if (m_Layout == NULL)
    {
      mafNEW(m_Layout);
      root->SetAttribute(m_Layout->GetName(), m_Layout);
    }
    else
    {
      m_Layout->ClearLayout();
    }
    m_Layout->SetVisibilityVme(m_VisibilityVme);
    m_Layout->SetApplicationInfo(frame->IsMaximized(), pos, size);
    wxPaneInfo toolbar = m_Win->GetDockManager().GetPane("toolbar");
    bool toolbar_vis = toolbar.IsShown();
    m_Layout->SetInterfaceElementVisibility("toolbar", toolbar_vis);
    wxPaneInfo sidebar = m_Win->GetDockManager().GetPane("sidebar");
    bool sidebar_vis = sidebar.IsShown();
    m_Layout->SetInterfaceElementVisibility("sidebar", sidebar_vis);
    wxPaneInfo logbar = m_Win->GetDockManager().GetPane("logbar");
    bool logbar_vis = logbar.IsShown();
    m_Layout->SetInterfaceElementVisibility("logbar", logbar_vis);
    m_Layout->SetLayoutName(m_DefaultLayout.GetCStr());
    mafView *v = m_ViewManager->GetList();
    for(; v; v = v->m_Next)
    {
      m_Layout->AddView(v);
    }
  }
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::InitializeLayout()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  wxString layout_name;
  if(config->Read("DefaultLayout", &layout_name))
  {
    m_DefaultLayout = layout_name.c_str();
  }
  cppDEL(config);

  m_Storage = mafVMEStorage::New();
  m_Storage->GetRoot()->SetName("ApplicationLayout");
  m_Storage->GetRoot()->Initialize();
  
  //reg key for application layout
  wxConfig *configApp = new wxConfig(wxEmptyString);
  wxString layout_filename;
  if(configApp->Read("DefaultLayoutFile", &layout_filename))
  {
    m_DefaultLayoutFile = layout_filename.c_str();
  }
  else
  {
    wxString layout_dir  = mafGetApplicationDirectory().c_str();
    layout_dir << "\\Layout\\layout.msf";
    configApp->Write("DefaultLayoutFile", layout_dir);
    m_DefaultLayoutFile = layout_dir;
  }
  cppDEL(configApp);

  if(wxFileExists(m_DefaultLayoutFile.GetCStr()))
  {
    LoadLayout(true);
  }
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::AddLayout()
//----------------------------------------------------------------------------
{
  wxString name;
	
	wxTextEntryDialog *dlg = new wxTextEntryDialog(NULL,_("please enter a name"), _("New Layout"), name );
	dlg->SetValue(_("new layout"));
	int result = dlg->ShowModal(); 
	name = dlg->GetValue();
	cppDEL(dlg);
	if(result != wxID_OK) return;

  //check for equal names
  if(m_List->FindString(name) != -1)
	{
		wxString msg = _("this name is already used, do you wanto to overwrite this layout ?");
		int res = wxMessageBox(msg,_("Save Layout"), wxOK|wxCANCEL|wxICON_QUESTION, NULL);
		if(res == wxCANCEL) return;

    // delete old child which will be substituted
		m_List->Delete(m_List->FindString(name));
    mafNode *root = m_Storage->GetRoot();
    mafSmartPointer<mafVMEGeneric> child;
    root->RemoveChild(root->FindInTreeByName(name));
	}

  // storage
  if (m_Storage)
  {
    wxFrame *frame = (wxFrame *)mafGetFrame();
    int pos[2], size[2];
    wxRect rect;
    rect = frame->GetRect();
    pos[0] = rect.GetPosition().x;
    pos[1] = rect.GetPosition().y;
    size[0] = rect.GetSize().GetWidth();
    size[1] = rect.GetSize().GetHeight();

    mafNode *root = m_Storage->GetRoot();
    mafSmartPointer<mafVMEGeneric> child;
    root->AddChild(child);
    child->GetTagArray()->SetTag("VISIBLE_IN_THE_TREE",0);
    if(m_Layout = mmaApplicationLayout::SafeDownCast(child->GetAttribute("ApplicationLayout")));
    else
    {
      mafNEW(m_Layout);
      child->SetAttribute(name, m_Layout);
    }
    m_Layout->SetName(name);
    child->SetName(name);
    
    //disable visibility (useless for application layout)
    m_Layout->SetVisibilityVme(0);
   
    m_Layout->SetApplicationInfo(frame->IsMaximized(), pos, size);
    wxPaneInfo toolbar = m_Win->GetDockManager().GetPane("toolbar");
    bool toolbar_vis = toolbar.IsShown();
    m_Layout->SetInterfaceElementVisibility("toolbar", toolbar_vis);
    wxPaneInfo sidebar = m_Win->GetDockManager().GetPane("sidebar");
    bool sidebar_vis = sidebar.IsShown();
    m_Layout->SetInterfaceElementVisibility("sidebar", sidebar_vis);
    wxPaneInfo logbar = m_Win->GetDockManager().GetPane("logbar");
    bool logbar_vis = logbar.IsShown();
    m_Layout->SetInterfaceElementVisibility("logbar", logbar_vis);
    m_Layout->SetLayoutName(m_DefaultLayout.GetCStr());
    mafView *v = m_ViewManager->GetList();
    for(; v; v = v->m_Next)
    {
      m_Layout->AddView(v);
    }
    m_List->Append(name);

    //restore the original visibility
    m_Layout->SetVisibilityVme(m_VisibilityVme);

    m_ModifiedLayouts = true;
  }

  
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::RemoveLayout()
//----------------------------------------------------------------------------
{
  int selection = m_List->GetSelection();
  if(selection != -1)
  {   
    // delete old child which will be substituted
		wxString name = m_List->GetString(selection);
    mafNode *root = m_Storage->GetRoot();
    mafSmartPointer<mafVMEGeneric> child;
    root->RemoveChild(root->FindInTreeByName(name));

    m_List->Delete(selection);
    m_ModifiedLayouts = true;
  }
  // storage
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::SaveApplicationLayout()
//----------------------------------------------------------------------------
{
  if(m_Storage)
  {
    //mafString file = mafGetSaveFile("", _("All Files (*.*)|*.*"), _("Save Layout File"), GetGui()).c_str();
    //m_Storage->SetURL(_(file));

    m_Storage->SetURL(_(m_LayoutFileSave));
    m_Storage->Store();
    m_ModifiedLayouts = false;
  }
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::LoadLayout(bool fileDefault)
//----------------------------------------------------------------------------
{
  if(m_Storage)
  {
    mafString file = "";

    if(fileDefault)
      file = m_DefaultLayoutFile;
    else
      file = mafGetOpenFile("", _("All Files (*.*)|*.*"), _("Open Layout File"), GetGui()).c_str();
    
    if(file.IsEmpty()) return;
    
    //clear tree
    m_Storage->GetRoot()->CleanTree();
    m_List->Clear();

    m_Storage->SetURL(_(file));
    m_Storage->Restore();

    //fill listbox
    mafNodeIterator *iter = m_Storage->GetRoot()->NewIterator();
    for(mafNode *vme = iter->GetFirstNode(); vme; vme = iter->GetNextNode())
    {
      if(!vme->IsMAFType(mafVMERoot))
        m_List->Append(vme->GetName());
    }
    iter->Delete();

    m_ModifiedLayouts = true;
    m_LayoutFileSave = file;

    if(m_Gui)
    {
      m_Gui->Update();

      //apply first layout
      if(m_List->GetCount() != 0)
      {
        m_List->SetSelection(0);
        m_Gui->Update();

        if(!fileDefault)
          ApplyLayout();
      }
    }
  }
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::ApplyLayout()
//----------------------------------------------------------------------------
{
  int selection = m_List->GetSelection();
  if(selection == -1)
  {
    return;
  }

  wxString name = m_List->GetString(selection);

  // Retrieve the saved layout.
  mafNode *root = m_Storage->GetRoot();
  mafVMEGeneric *vme = mafVMEGeneric::SafeDownCast(root->FindInTreeByName(name));
  mmaApplicationLayout *app_layout = mmaApplicationLayout::SafeDownCast(vme->GetAttribute(name));
  if (app_layout)
  {
    int answer = wxMessageBox(_("Do you want to apply the layout?"), _("Warning"), wxYES_NO);
    if (answer == wxNO)
    {
      return;
    }
    m_ViewManager->ViewDeleteAll();
    int maximized, pos[2], size[2];
    app_layout->GetApplicationInfo(maximized, pos, size);
    if (maximized != 0)
    {
      m_Win->Maximize();
    }
    else
    {
      wxRect rect(pos[0],pos[1],size[0],size[1]);
      m_Win->SetSize(rect);
    }
    bool tb_vis = app_layout->GetToolBarVisibility() != 0;
    bool sb_vis = app_layout->GetSideBarVisibility() != 0;
    bool lb_vis = app_layout->GetLogBarVisibility() != 0;
    m_Win->ShowDockPane("toolbar", tb_vis);
    m_Win->ShowDockPane("logbar", lb_vis);
    m_Win->ShowDockPane("sidebar", sb_vis);
    int num = app_layout->GetNumberOfViewsInLayout();
    std::vector<mmaApplicationLayout::ViewLayoutInfo>::iterator iter = app_layout->GetLayoutList();
    mafView *v = NULL;
    for (int i = 0; i < num; i++, iter++)
    {
      if(m_ViewManager)
      {
        mafView* v = m_ViewManager->ViewCreate((*iter).m_Id);

        /*
        if(m_OpManager) 
        {
          VmeShow(m_OpManager->GetSelectedVme(),true);
        }
        */
      }
      
      mafYield();
      v = m_ViewManager->GetSelectedView();
      if (v)
      {
        v->SetName((*iter).m_Label.GetCStr());
        pos[0] = (*iter).m_Position[0];
        pos[1] = (*iter).m_Position[1];
        size[0] = (*iter).m_Size[0];
        size[1] = (*iter).m_Size[1];
        wxRect rect(pos[0],pos[1],size[0],size[1]);
        v->GetFrame()->SetSize(rect);
      }
    }
  }
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::ApplyTreeLayout()
//----------------------------------------------------------------------------
{
  // Retrieve the saved layout.
  mafNode *vme = m_ViewManager->GetCurrentRoot();
  mmaApplicationLayout *app_layout = mmaApplicationLayout::SafeDownCast(vme->GetAttribute("ApplicationLayout")); 
  
  if (app_layout)
  {
    int answer = wxMessageBox(_("Do you want to load the layout?"), _("Warning"), wxYES_NO);
    if (answer == wxNO)
    {
      return;
    }
    m_ViewManager->ViewDeleteAll();
    int maximized, pos[2], size[2];
    app_layout->GetApplicationInfo(maximized, pos, size);
    if (maximized != 0)
    {
      m_Win->Maximize();
    }
    else
    {
      wxRect rect(pos[0],pos[1],size[0],size[1]);
      m_Win->SetSize(rect);
    }
    bool tb_vis = app_layout->GetToolBarVisibility() != 0;
    bool sb_vis = app_layout->GetSideBarVisibility() != 0;
    bool lb_vis = app_layout->GetLogBarVisibility() != 0;
    m_Win->ShowDockPane("toolbar", tb_vis);
    m_Win->ShowDockPane("logbar", lb_vis);
    m_Win->ShowDockPane("sidebar", sb_vis);
    int num = app_layout->GetNumberOfViewsInLayout();
    std::vector<mmaApplicationLayout::ViewLayoutInfo>::iterator iter = app_layout->GetLayoutList();
    mafView *v = NULL;
    for (int i = 0; i < num; i++, iter++)
    {
      if(m_ViewManager)
      {
        mafView* v = m_ViewManager->ViewCreate((*iter).m_Id);

        /*
        if(m_OpManager) 
        {
          VmeShow(m_OpManager->GetSelectedVme(),true);
        }
        */
      }
      mafYield();
      v = m_ViewManager->GetSelectedView();
      if (v)
      {
        v->SetName((*iter).m_Label.GetCStr());
        pos[0] = (*iter).m_Position[0];
        pos[1] = (*iter).m_Position[1];
        size[0] = (*iter).m_Size[0];
        size[1] = (*iter).m_Size[1];
        wxRect rect(pos[0],pos[1],size[0],size[1]);
        v->GetFrame()->SetSize(rect);

        for (int i=0; i<(*iter).m_VisibleVmes.size();i++)
        {
          mafNode *node_restored = m_ViewManager->GetCurrentRoot()->FindInTreeById((*iter).m_VisibleVmes[i]);
          if (node_restored)
          {
            mafEventMacro(mafEvent(this, VME_SHOW, node_restored, true));
          }
        }

        if((*iter).m_VisibleVmes.size() > 0)
        {
          if(v->IsMAFType(mafViewVTK))
          {
            double view_up[3], position[3], focal_point[3];
            view_up[0] = (*iter).m_CameraParameters[0];
            view_up[1] = (*iter).m_CameraParameters[1];
            view_up[2] = (*iter).m_CameraParameters[2];

            position[0] = (*iter).m_CameraParameters[3];
            position[1] = (*iter).m_CameraParameters[4];
            position[2] = (*iter).m_CameraParameters[5];

            focal_point[0] = (*iter).m_CameraParameters[6];
            focal_point[1] = (*iter).m_CameraParameters[7];
            focal_point[2] = (*iter).m_CameraParameters[8];

            v->GetRWI()->GetCamera()->SetViewUp(view_up);
            v->GetRWI()->GetCamera()->SetPosition(position);
            v->GetRWI()->GetCamera()->SetFocalPoint(focal_point);
          }
          else //compound
          {;}
        }
          

      }
    }
  }
}