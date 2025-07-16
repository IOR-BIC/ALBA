/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISettingsDialog
 Authors: Silvano Imboden
 
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


#include <wx/event.h>
#include "albaGUIBusyInfo.h"
#include <wx/settings.h>
#include <wx/laywin.h>
#include <wx/imaglist.h>

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaGUISettingsDialog.h"
#include "albaGUITree.h"
#include "albaGUI.h"
#include "albaGUIHolder.h"
#include "albaPics.h"
#include "albaGUIDialog.h"

//----------------------------------------------------------------------------
// constant :
//----------------------------------------------------------------------------
enum albaGUISettingsDialog_IDS
{
	ID_SELECT_PAGE = MINID,
  ID_ROOT,
};
//----------------------------------------------------------------------------
albaGUISettingsDialog::albaGUISettingsDialog(wxString dialog_title)
//----------------------------------------------------------------------------
{  
  #include <pic/FOLDER.xpm>
  albaPictureFactory::GetPictureFactory()->Add("FOLDER", FOLDER_xpm);
  wxImageList *tree_images = new wxImageList(20,20,false,1);
  tree_images->Add(albaPictureFactory::GetPictureFactory()->GetBmp("FOLDER"));

  m_Dialog = new albaGUIDialog(dialog_title);
	m_Tree   = new albaGUITree(m_Dialog,ID_SELECT_PAGE,false,true);
	m_Guih   = new albaGUIHolder(m_Dialog,-1,false,true);

  m_Tree->SetListener(this);
	//m_Tree->SetTreeStyle( wxSIMPLE_BORDER | wxTR_HAS_BUTTONS );//| wxTR_HIDE_ROOT );
	m_Tree->SetSize(270,400);
	m_Tree->SetMinSize(wxSize(270,400));
  m_Tree->SetImageList(tree_images);

  m_Guih->SetSize(270,400);
  m_Guih->SetMinSize(wxSize(270,400));
	
  wxBoxSizer *vsz = new wxBoxSizer(wxVERTICAL);
  vsz->Add(m_Guih,1,wxEXPAND);

	wxBoxSizer *hsz = new wxBoxSizer(wxHORIZONTAL);
	hsz->Add(m_Tree,1,wxEXPAND | wxALL, 6);
	hsz->Add(vsz,0,wxEXPAND | wxALL, 6);

  m_Dialog->Add(hsz,1,wxEXPAND);

  AddRoot();
}
//----------------------------------------------------------------------------
albaGUISettingsDialog::~albaGUISettingsDialog()
//----------------------------------------------------------------------------
{
  for (int i = 0; i < m_List.size(); i++)
  {
    delete m_List[i];
  }
  m_List.clear();
};
//----------------------------------------------------------------------------
bool albaGUISettingsDialog::ShowModal()
//----------------------------------------------------------------------------
{
  m_Guih->Update();
  return m_Dialog->ShowModal() != 0;
}
//----------------------------------------------------------------------------
void albaGUISettingsDialog::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case VME_SELECT:
      {
        Page *page = (Page*)(e->GetVme());
        if(page)
          m_Guih->Put(page->ui);
      }
      break; 
      default:
        e->Log();
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void albaGUISettingsDialog::AddRoot()
//----------------------------------------------------------------------------
{
  // the Root is m_List[0]
  
  Page *page = new Page();
  page->label = "General";
  page->parent = 0;
  page->ui = new albaGUI(this);
  m_List.push_back(page);

	//Root should be expanded
  m_Tree->AddNode((long long)page,0,page->label,true,0);
}
//----------------------------------------------------------------------------
void albaGUISettingsDialog::AddPage(albaGUI *ui, wxString label, wxString parent_label)
//----------------------------------------------------------------------------
{
  if(GetPageByName(label)) return; // page already exist

  Page *parent = GetPageByName(parent_label);
  if(!parent) parent = m_List[0];

  Page *page = new Page();
  page->label = label;
  page->parent = parent;
  page->ui = ui;
  m_List.push_back(page);
  
	//pages should be expanded
  m_Tree->AddNode((long long)page,(long long)parent,label,true,0);
}
//----------------------------------------------------------------------------
void albaGUISettingsDialog::RemovePage(wxString label)
//----------------------------------------------------------------------------
{
  if(label == "General") return; // prevent removing the Root
  
  Page *page = GetPageByName(label);
  if(!page) return;
  m_Tree->DeleteNode((long long)(page));

  // remove page from the List --- and shift up all following elements
  int i;
  for(i=0 ; i < m_List.size(); i++)
  {
    Page *p = m_List[i];
    if(p && p->label == label) delete p;
  }
  for( ; i < m_List.size()-1; i++ )
  {
    m_List[i] = m_List[i+1];
  }
  m_List.pop_back();
  
}
//----------------------------------------------------------------------------
albaGUISettingsDialog::Page* albaGUISettingsDialog::GetPageByName(wxString name)
//----------------------------------------------------------------------------
{
  for(int i=0 ; i < m_List.size(); i++)
  {
    Page *p = m_List[i];
    if(p && p->label == name) return p;
  }
  return NULL;
}
