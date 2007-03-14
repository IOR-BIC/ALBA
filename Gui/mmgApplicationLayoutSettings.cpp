/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgApplicationLayoutSettings.cpp,v $
Language:  C++
Date:      $Date: 2007-03-14 17:13:23 $
Version:   $Revision: 1.10 $
Authors:   Daniele Giunchi
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
#include "mafVMEManager.h"
#include "mafView.h"
#include "mafVMEStorage.h"
#include "mafXMLStorage.h"
#include "mafVMERoot.h"
#include "mafSmartPointer.h"
#include "mafNodeLayout.h"
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
  m_DefaultLayoutName = " - ";

  m_ActiveLayoutName  = "";
  m_LayoutType        = " - ";
  m_DefaultFlag       = 0;
  
  m_ViewManager   = NULL;
  m_Layout        = NULL;
  m_Win           = NULL;
  m_List		      = NULL;
//  m_Storage       = NULL;
	m_XMLStorage    = NULL;
	m_XMLRoot       = NULL;

	m_SelectedItem  = -1;
  m_VisibilityVme = false;
  m_ModifiedLayouts = false;
  m_LayoutFileSave = "";

  m_Gui = new mmgGui(this);

  // layout editor
  m_Gui->Label(_("Layout Editor"), true);
  m_Gui->Divider(1);
  m_Gui->String(LAYOUT_NAME_ID,_("name"),&m_ActiveLayoutName);
  m_Gui->Label(_("Type"), &m_LayoutType);
  m_Gui->Divider(1);
   
  //application layout msf
  m_Gui->Label(_("MSF"), true);
  //m_Gui->Label("For becoming definitive the layout saving,");
  //m_Gui->Label(" it needs to save the project");
  
  //m_Gui->String(LAYOUT_NAME_ID,_("name"),&m_DefaultLayout);
  m_Gui->Button(APPLY_TREE_LAYOUT_ID,_("Apply Root Layout"));
  m_Gui->Bool(LAYOUT_VISIBILITY_VME, _("Visibility"), &m_VisibilityVme ,0,_("If checked the layout will be comprehensive of vme visibility"));
  m_Gui->Button(SAVE_TREE_LAYOUT_ID,_("Save in root"));

  m_Gui->Enable(APPLY_TREE_LAYOUT_ID,false);

  //application layout 
  m_Gui->Divider(1);
  m_Gui->Label(_("Application"), true);
	m_Gui->Label(_("default:"), &m_DefaultLayoutName);
  
  m_Gui->Button(OPEN_LAYOUT_ID,_("Load file"));
  m_Gui->Button(ADD_LAYOUT_ID,_("Add Current layout"));
  m_Gui->Button(REMOVE_LAYOUT_ID,_("Remove layout"));
  m_List= m_Gui->ListBox(ID_LIST_LAYOUT,_(""),60);
  //m_Gui->Button(SAVE_APPLICATION_LAYOUT_ID,_("Save"));
  m_Gui->Button(APPLY_LAYOUT_ID,_("Apply layout"));
  m_Gui->Bool(DEFAULT_LAYOUT_ID,_("Default Layout"), &m_DefaultFlag, 1);
  //here lies the button for set as default
  
  m_Gui->FileSave(SAVE_APPLICATION_LAYOUT_ID,_("File"), &m_LayoutFileSave,_("All Files (*.mly)|*.mly"));
	m_Gui->Label(_(""));

  InitializeLayout();
}
//----------------------------------------------------------------------------
mmgApplicationLayoutSettings::~mmgApplicationLayoutSettings() 
//----------------------------------------------------------------------------
{
  //mafDEL(m_Layout); // already destroyed by the vme
  vtkDEL(m_XMLStorage);
  mafDEL(m_XMLRoot);
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
    {
      m_SelectedItem = m_List->GetSelection();
      mafString sel = m_List->GetStringSelection().c_str();
      m_DefaultFlag = (sel == m_DefaultLayoutName) ? 1 : 0;
      m_Gui->Update();
    }
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
    case DEFAULT_LAYOUT_ID:
      SetLayoutAsDefault();
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
    m_Layout->SetLayoutName("Layout"); //m_DefaultLayout.GetCStr()
    mafView *v = m_ViewManager->GetList();
    for(; v; v = v->m_Next)
    {
      m_Layout->AddView(v);
    }

    m_Gui->Enable(APPLY_TREE_LAYOUT_ID,true);
    
  }
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::InitializeLayout()
//----------------------------------------------------------------------------
{
  /*wxConfig *config = new wxConfig(wxEmptyString);
  wxString layout_name;
  if(config->Read("DefaultLayout", &layout_name))
  {
    m_DefaultLayout = layout_name.c_str();
  }
  cppDEL(config);*/

//  m_Storage = mafVMEStorage::New();
//  m_Storage->GetRoot()->SetName("ApplicationLayout");
//  m_Storage->GetRoot()->Initialize();
  
	//m_XMLStorage = mafXMLStorage::New();
  vtkNEW(m_XMLStorage);
	m_XMLStorage->SetFileType("MLY");
	m_XMLStorage->SetVersion("2.0");
	mafNEW(m_XMLRoot);
	m_XMLRoot->SetName("ApplicationLayout");
	m_XMLRoot->Initialize();
	m_XMLStorage->SetDocument(m_XMLRoot);
	//((mafNode *)m_XMLStorage->GetDocument())->SetName("ApplicationLayout");

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
    layout_dir << "\\Layout\\layout.mly";
    configApp->Write("DefaultLayoutFile", layout_dir);
    m_DefaultLayoutFile = layout_dir;
  }
  cppDEL(configApp);

  /*if(wxFileExists(m_DefaultLayoutFile.GetCStr()))
  {
    LoadLayout();
  }*/
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
    mafNode *root = ((mafNode *)m_XMLStorage->GetDocument());
    mafSmartPointer<mafNodeLayout> child;
    root->RemoveChild(root->FindInTreeByName(name));
	}

  // storage
  if (m_XMLStorage)
  {
    wxFrame *frame = (wxFrame *)mafGetFrame();
    int pos[2], size[2];
    wxRect rect;
    rect = frame->GetRect();
    pos[0] = rect.GetPosition().x;
    pos[1] = rect.GetPosition().y;
    size[0] = rect.GetSize().GetWidth();
    size[1] = rect.GetSize().GetHeight();

    mafNode *root = ((mafNode *)m_XMLStorage->GetDocument());
    mafSmartPointer<mafNodeLayout> child;
    root->AddChild(child);
    
    if(m_Layout = mmaApplicationLayout::SafeDownCast(child->GetLayout()));
    else
    {
      mafNEW(m_Layout);
      child->SetAttribute("ApplicationLayout", m_Layout); //mettere application layout
    }
    m_Layout->SetName("ApplicationLayout");
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
    m_Layout->SetLayoutName("Layout"); //m_DefaultLayout.GetCStr()
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
  if(m_SelectedItem != -1)
  {   
    // delete old child which will be substituted
		wxString name = m_List->GetString(m_SelectedItem);
    mafNode *root = ((mafNode *)m_XMLStorage->GetDocument());
		if(mafString(((mafNodeLayout *)root->FindInTreeByName(name))->GetLayout()->GetLayoutName()) == mafString("Default"))
			m_DefaultLayoutName = " - ";

		m_Gui->Update();
    root->RemoveChild(root->FindInTreeByName(name));

    m_List->Delete(m_SelectedItem);
    m_SelectedItem = -1;
    m_ModifiedLayouts = true;
  }
  // storage
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::SaveApplicationLayout()
//----------------------------------------------------------------------------
{
  if(m_XMLStorage)
  {
    //mafString file = mafGetSaveFile("", _("All Files (*.*)|*.*"), _("Save Layout File"), GetGui()).c_str();
    //m_Storage->SetURL(_(file));

    m_XMLStorage->SetURL(m_LayoutFileSave);
    m_XMLStorage->Store();
    m_ModifiedLayouts = false;
  }
  /*
	mafXMLStorage storage;
	storage.SetFileType("MLY");
	storage.SetVersion("2.0");
	storage.SetDocument(m_Storage->GetRoot());

	wxString newname;
	newname = m_LayoutFileSave.GetCStr();
	newname = newname.BeforeLast('.');
	newname += ".mly";
	storage.SetURL(newname);
	storage.Store();*/
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::LoadLayout(bool fileDefault)
//----------------------------------------------------------------------------
{  
  if(m_XMLStorage && wxFileExists(m_DefaultLayoutFile.GetCStr()))
  {
    mafString file = "";

    if(fileDefault)
      file = m_DefaultLayoutFile;
    else
      file = mafGetOpenFile("", _("All Files (*.*)|*.*"), _("Open Layout File"), GetGui()).c_str();
    
    if(file.IsEmpty()) return;
    
    //clear tree
    ((mafNode *)m_XMLStorage->GetDocument())->CleanTree();
    m_List->Clear();

    m_XMLStorage->SetURL(file);
    m_XMLStorage->Restore();

    //fill listbox
    mafNodeIterator *iter = ((mafNode *)m_XMLStorage->GetDocument())->NewIterator();
    for(mafNode *vme = iter->GetFirstNode(); vme; vme = iter->GetNextNode())
    {
      if(!vme->IsMAFType(mafVMERoot))
			{
        m_List->Append(vme->GetName());
				if(mafString(((mafNodeLayout *)vme)->GetLayout()->GetLayoutName()) == mafString("Default"))
          m_DefaultLayoutName = ((mafNodeLayout *)vme)->GetName();
			}
    }
    iter->Delete();

    m_LayoutFileSave = file;
    
    if(m_Gui)
    {
      m_Gui->Update();
			m_List->Update();

      //apply default layout
      if(m_List->GetCount() != 0)
      {
			  m_SelectedItem = m_List->FindString(m_DefaultLayoutName.GetCStr());
        if(m_SelectedItem != -1)
          m_List->SetSelection(m_SelectedItem, true);
        m_Gui->Update();

        if(fileDefault && m_SelectedItem == -1)
          return;

        ApplyLayout();
        mafString sel = m_List->GetStringSelection().c_str();
        m_DefaultFlag = (sel == m_DefaultLayoutName) ? 1 : 0;
				m_SelectedItem = m_List->GetSelection();
        m_Gui->Update();
      }
    }

     
  }
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::ApplyLayout()
//----------------------------------------------------------------------------
{
  if(m_SelectedItem == -1)
  {
    return;
  }

  wxString name = m_List->GetString(m_SelectedItem);

  // Retrieve the saved layout.
  mafNode *root = ((mafNode *)m_XMLStorage->GetDocument());
  mafNodeLayout *vme = mafNodeLayout::SafeDownCast(root->FindInTreeByName(name));
  mmaApplicationLayout *app_layout = mmaApplicationLayout::SafeDownCast(vme->GetLayout()); //application layout
  m_ActiveLayoutName = vme->GetName();
  m_LayoutType       = _("Application Layout");
  if(m_Gui) m_Gui->Update();

  if (app_layout)
  {
    /*int answer = wxMessageBox(_("Do you want to apply the layout?"), _("Warning"), wxYES_NO);
    if (answer == wxNO)
    {
      return;
    }*/
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
    /*int answer = wxMessageBox(_("Do you want to load the layout?"), _("Warning"), wxYES_NO);
    if (answer == wxNO)
    {
      return;
    }*/

    m_ActiveLayoutName = app_layout->GetLayoutName();
    m_LayoutType       = _("MSF Layout");
    if(m_Gui) m_Gui->Update();

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

        if(m_VisibilityVme)
        {
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
  else
  {
    m_Gui->Enable(APPLY_TREE_LAYOUT_ID,false);
    m_Gui->Enable(SAVE_TREE_LAYOUT_ID,false);
  }
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::SetLayoutAsDefault()
//----------------------------------------------------------------------------
{
  if(m_SelectedItem != -1)
  {   
    // delete old child which will be substituted
		/*wxString name = m_List->GetString(selection);
    mafNode *root = m_Storage->GetRoot();
    mafSmartPointer<mafNodeLayout> child;
    root->RemoveChild(root->FindInTreeByName(name));*/

    //m_List->GetItem(selection)->SetBackgroundColour(wxColor(256, 0, 0));
    m_ModifiedLayouts = true;
    mafNodeIterator *iter = ((mafNode *)m_XMLStorage->GetDocument())->NewIterator();
		for(mafNode *vme = iter->GetFirstNode(); vme; vme = iter->GetNextNode())
		{
			if(!vme->IsMAFType(mafVMERoot))
			  ((mafNodeLayout *)vme)->GetLayout()->SetLayoutName("Layout");
		}
		iter->Delete();

    if(m_DefaultFlag != 0)
      m_DefaultLayoutName = m_List->GetString(m_SelectedItem);
    else
    {
      m_DefaultLayoutName = " - ";
      m_Gui->Update();
      return;
    }

		mafNode *root = ((mafNode *)m_XMLStorage->GetDocument());		
		((mafNodeLayout *)root->FindInTreeByName(m_DefaultLayoutName))->GetLayout()->SetLayoutName("Default"); //m_DefaultLayout.GetCStr()
		
		
    m_ModifiedLayouts = true;

		m_Gui->Update();
  }
}