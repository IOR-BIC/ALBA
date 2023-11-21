/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUICheckTree
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


#include "albaGUICheckTree.h" 
#include "albaDecl.h"
#include "albaView.h"
#include "albaPics.h" 

#include "albaVME.h"
#include "albaVMEIterator.h"
#include <vector>

//=========================================================================================
// Helper class to subclass the wxTreeCtrl
// in order to intercept mouse clicks. 
//=========================================================================================
class albaGUICheckTreeEvtHandler : public wxEvtHandler
{
public:
  albaGUICheckTreeEvtHandler() : m_NodeTree(0) { }
  albaGUICheckTreeEvtHandler(albaGUICheckTree* tree) : m_NodeTree(tree) { }
  
  void OnMouseDown(wxMouseEvent& event)        {m_NodeTree->OnMouseDown(event);};
  void OnMouseUp(wxMouseEvent& event)          {m_NodeTree->OnMouseUp(event);};
  void OnMouseEvent(wxMouseEvent& event)       {m_NodeTree->OnMouseEvent(event);};
	void ShowContextualMenu(wxMouseEvent& event) {m_NodeTree->ShowContextualMenu(event);};
		
private:
  albaGUICheckTree  *m_NodeTree;
  
	DECLARE_DYNAMIC_CLASS(albaGUICheckTreeEvtHandler )
  DECLARE_EVENT_TABLE()
};
IMPLEMENT_DYNAMIC_CLASS( albaGUICheckTreeEvtHandler , wxEvtHandler )
BEGIN_EVENT_TABLE( albaGUICheckTreeEvtHandler , wxEvtHandler )
    EVT_LEFT_DOWN( albaGUICheckTreeEvtHandler ::OnMouseDown )
		EVT_RIGHT_UP( albaGUICheckTreeEvtHandler ::ShowContextualMenu )
    EVT_LEFT_UP( albaGUICheckTreeEvtHandler ::OnMouseUp )
    EVT_MOUSE_EVENTS( albaGUICheckTreeEvtHandler ::OnMouseEvent )
END_EVENT_TABLE()

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum TREE_RMENU_COMMANDS
{
  RMENU_START = MINID,
		RMENU_SHOW_VME,				
		RMENU_SHOW_SUBTREE,
		RMENU_SHOW_SAMETYPE,
		RMENU_HIDE_SUBTREE,
		RMENU_HIDE_SAMETYPE,
		RMENU_SORT_TREE,
		RMENU_AUTO_SORT,
    RMENU_CRYPT_VME, 
    RMENU_ENABLE_CRYPT_SUBTREE, 
    RMENU_DISABLE_CRYPT_SUBTREE,
	RMENU_STOP
};
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUICheckTree,wxPanel)
    EVT_TREE_SEL_CHANGED(ID_TREE, albaGUICheckTree::OnSelectionChanged)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUICheckTree::albaGUICheckTree( wxWindow* parent,wxWindowID id, bool CloseButton, bool HideTitle)
:albaGUITree(parent,id,CloseButton,HideTitle)
//----------------------------------------------------------------------------
{
  m_View = NULL;
  m_CanSelect	= true;
  m_SelectedVME = NULL;
  m_RMenu	= NULL;

  m_NodeTree->PushEventHandler( new albaGUICheckTreeEvtHandler(this) );

  InitializeImageList();
}
//----------------------------------------------------------------------------
albaGUICheckTree::~albaGUICheckTree()
//----------------------------------------------------------------------------
{
	if(m_RMenu)
    delete m_RMenu;
	m_NodeTree->PopEventHandler(true);
}
//----------------------------------------------------------------------------
void albaGUICheckTree::ShowContextualMenu(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
  int flag;
  wxTreeItemId i = m_NodeTree->HitTest(wxPoint(event.GetX(),event.GetY()),flag);
  bool vmeMenu = false;
#ifdef WIN32
  vmeMenu = i.IsOk() && ((flag == wxTREE_HITTEST_ONITEMICON) || (flag == wxTREE_HITTEST_ONITEMLABEL));
#else
  vmeMenu = i.IsOk();
#endif

  albaEvent e;
  e.SetSender(this);
  e.SetId(SHOW_TREE_CONTEXTUAL_MENU);
  e.SetBool(vmeMenu);
  e.SetArg(m_Autosort);
  if(vmeMenu)
    e.SetVme((albaVME *)NodeFromItem(i));
  albaEventMacro(e);
}
//----------------------------------------------------------------------------
void albaGUICheckTree::EnableSelect(bool enable)
//----------------------------------------------------------------------------
{
  m_CanSelect = enable;
}
//----------------------------------------------------------------------------
void albaGUICheckTree::OnMouseDown( wxMouseEvent& event )
//----------------------------------------------------------------------------
{
  //purpose: intercept and notify if the icon was clicked,
  //prevent node selection if the icon was clicked,
  //prevent node selection anyway, if the selection is disabled,
  int flag;
	wxTreeItemId i = m_NodeTree->HitTest(wxPoint(event.GetX(),event.GetY()),flag);
	if(i.IsOk() && flag & wxTREE_HITTEST_ONITEMICON )
	{
    OnIconClick(i); 
		return;//eat message
	}
	if (i.IsOk() && flag & wxTREE_HITTEST_ONITEMINDENT)
	{
		if (m_NodeTree->IsExpanded(i))
			m_NodeTree->Collapse(i);
		else
			m_NodeTree->Expand(i);
		return;//eat message
	}
	if(!this->m_CanSelect)	
	  return; //also eat message if selection is disabled
	event.Skip();//process event as usual
}
//----------------------------------------------------------------------------
void albaGUICheckTree::OnMouseUp( wxMouseEvent& event )
//----------------------------------------------------------------------------
{
  //purpose: prevent selection if I clicked on the icon.
  //to select you must click the node name
  int flag;
	wxTreeItemId i = m_NodeTree->HitTest(wxPoint(event.GetX(),event.GetY()),flag);
	if(flag & wxTREE_HITTEST_ONITEMICON )
		return;//eat message
	event.Skip();//process event as usual
}
//----------------------------------------------------------------------------
void albaGUICheckTree::OnMouseEvent( wxMouseEvent& event )
//----------------------------------------------------------------------------
{
	//pourpose: prevent selection if I clicked on the icon.
  //to select you must click the node name
  if(event.ButtonDown() || event.ButtonUp() || event.ButtonDClick())
	{
	 int flag;
	 wxTreeItemId i = m_NodeTree->HitTest(wxPoint(event.GetX(),event.GetY()),flag);
	 if(flag & wxTREE_HITTEST_ONITEMICON )
			return;//eat message
	}
	if(event.RightDown() || event.RightIsDown() || event.RightUp() || event.RightDClick())
		return;//eat message 
	event.Skip();//process event as usual
}
//----------------------------------------------------------------------------
void albaGUICheckTree::OnIconClick(wxTreeItemId item)
//----------------------------------------------------------------------------
{
  albaVME* vme = (albaVME*) (NodeFromItem(item));
  int status = GetVmeStatus(vme); 

  if(status != NODE_NON_VISIBLE)		
  {
    bool show = !(status == NODE_VISIBLE_ON || status == NODE_MUTEX_ON ); 
    if (!show && !this->m_CanSelect && m_SelectedVME && m_SelectedVME == vme)
    {
      return;
    }

		GetLogicManager()->VmeShow(vme, show);
		GetLogicManager()->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
bool albaGUICheckTree::IsIconChecked(wxTreeItemId item)
//----------------------------------------------------------------------------
{
  albaVME* vme = (albaVME*) (NodeFromItem(item));
  bool checked = GetNodeIcon((long long)vme) == (ClassNameToIcon(vme->GetTypeName()) + NODE_VISIBLE_ON * 2);
  return checked;
}
//----------------------------------------------------------------------------
void albaGUICheckTree::VmeAdd(albaVME *vme)   
//----------------------------------------------------------------------------
{
  AddNode((long long)vme,(long long)vme->GetParent(), vme->GetName(), 0);
	VmeUpdateIcon(vme);
}
//----------------------------------------------------------------------------
void albaGUICheckTree::VmeRemove(albaVME *vme)   
//----------------------------------------------------------------------------
{
  this->DeleteNode((long long)vme);
  if (m_SelectedVME == vme)
  {
    m_SelectedVME = NULL;
  }
}
//----------------------------------------------------------------------------
void albaGUICheckTree::VmeSelected(albaVME *vme)   
//----------------------------------------------------------------------------
{
  this->SelectNode((long long)vme);
  m_SelectedVME = vme;
  VmeUpdateIcon(vme);
}
//----------------------------------------------------------------------------
void albaGUICheckTree::VmeModified(albaVME *vme)
//----------------------------------------------------------------------------
{
  this->SetNodeLabel((long long)vme, vme->GetName());
	VmeUpdateIcon(vme);
}
//----------------------------------------------------------------------------
void albaGUICheckTree::VmeShow(albaVME *vme, bool show)   
//----------------------------------------------------------------------------
{
	VmeUpdateIcon(vme);
}
//----------------------------------------------------------------------------
int albaGUICheckTree::GetVmeStatus(albaVME *vme)
//----------------------------------------------------------------------------
{
  if(!m_View)
    return NODE_NON_VISIBLE;
  return m_View->GetNodeStatus(vme);
}
//----------------------------------------------------------------------------
void albaGUICheckTree::VmeUpdateIcon(albaVME *vme)
//----------------------------------------------------------------------------
{
  albaVMEIterator *iter = vme->NewIterator();
  for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    int dataStatus = 1;
    int icon_index;

    dataStatus = node->IsDataAvailable() ? 0 : 1;
    icon_index = ClassNameToIcon(node->GetTypeName()) + (GetVmeStatus(node)*2) + dataStatus;
    SetNodeIcon( (long long)node, icon_index );

    if (node->GetNumberOfLinks() != 0)
    {
      albaVME::albaLinksMap *links = node->GetLinks();
      albaVME *linkedVME = NULL;
      for (albaVME::albaLinksMap::iterator it = links->begin(); it != links->end(); it++)
      {
        if(it->second.m_Node)
        {
          linkedVME = it->second.m_Node;
          if (linkedVME)
          {
            dataStatus = linkedVME->IsDataAvailable() ? 0 : 1;
            icon_index = ClassNameToIcon(linkedVME->GetTypeName()) + (GetVmeStatus(linkedVME)*2) + dataStatus;
            SetNodeIcon( (long long)linkedVME, icon_index );
          }
        }
      }
    }
  }
  iter->Delete();
//  int icon_index = ClassNameToIcon(vme->GetTypeName()) + (GetVmeStatus(vme)*2) + dataStatus;
//  SetNodeIcon( (long)vme, icon_index );
}
//----------------------------------------------------------------------------
void albaGUICheckTree::ViewSelected(albaView *view)
//----------------------------------------------------------------------------
{
  m_View = view;
  TreeUpdateIcon();
}
//----------------------------------------------------------------------------
void albaGUICheckTree::TreeUpdateIcon()
//----------------------------------------------------------------------------
{
  if (m_SelectedVME != NULL)
  {
  	VmeUpdateIcon(m_SelectedVME->GetRoot());
  }
}
//----------------------------------------------------------------------------
void albaGUICheckTree::ViewDeleted(albaView *view)
//----------------------------------------------------------------------------
{
	if(view != m_View) 
    return;
	ViewSelected(NULL);
}
//----------------------------------------------------------------------------
int albaGUICheckTree::ClassNameToIcon(wxString classname)
//----------------------------------------------------------------------------
{
  MapClassNameToIcon::iterator it = m_MapClassNameToIcon.find(classname.ToAscii());
  if (it != m_MapClassNameToIcon.end())
    return int((*it).second);
  else
  {
    // search superclass's icon
    // if also this icon is not present, "Unknown" icon is used
    albaLogMessage("albaPictureFactory::ClassNameToIcon: cant find = %s ",classname.ToAscii());
    return 0;
  }
}
//----------------------------------------------------------------------------
void albaGUICheckTree::InitializeImageList()
//----------------------------------------------------------------------------
{
  // purpose:
  // each vme-picture is combined with each state-picture,
  // all the combined picture are inserted in the image-list.
  // given a vme-class-name and a vme-state
  // the corresponding icon index can be retrieved as 
  // ClassNameToIcon(vme-class-name) + vme-state

  std::vector<wxString>  v;
  albaPictureFactory::GetPictureFactory()->GetVmeNames(v);

  const int num_of_status = 5; 
  int num_types = v.size();
  int num_icons = num_types * (num_of_status * 2); // Added the status "Data not available"

  if(num_types <= 0)
  {
    wxLogMessage("albaGUICheckTree:  Warning - no vme-icons defined");
    return;
  }
  //retrieve state icons
  //I assume all state-icon to have the same size
  wxBitmap state_ico[num_of_status];
  state_ico[NODE_NON_VISIBLE] = albaPictureFactory::GetPictureFactory()->GetBmp("DISABLED");
  state_ico[NODE_VISIBLE_OFF] = albaPictureFactory::GetPictureFactory()->GetBmp("CHECK_OFF");
  state_ico[NODE_VISIBLE_ON]  = albaPictureFactory::GetPictureFactory()->GetBmp("CHECK_ON");
  state_ico[NODE_MUTEX_OFF]   = albaPictureFactory::GetPictureFactory()->GetBmp("RADIO_OFF");
  state_ico[NODE_MUTEX_ON]    = albaPictureFactory::GetPictureFactory()->GetBmp("RADIO_ON");
  int sw = state_ico[0].GetWidth();
  int sh = state_ico[0].GetHeight();

  //get icon size 
  //I assume all vme-icon to have the same size
  wxBitmap bmp = albaPictureFactory::GetPictureFactory()->GetVmePic(v[0]);
  int w = bmp.GetWidth();
  int h = bmp.GetHeight();
  assert(w>0 && h>0);

  // create the ImageList 
  int mw = sw+w; 
  int mh = (sh>h) ? sh : h;
  wxImageList *imgs = new wxImageList(mw,mh,FALSE,num_icons);

  for(int i=0; i<num_types; i++)
  {
    wxString name = v[i];
    //m_MapClassNameToIcon[name]=i*num_of_status;
    m_MapClassNameToIcon[name]=i*(num_of_status * 2); // Paolo 18/12/2006
    
    int s;
    for( s=0; s<num_of_status; s++)
    {
			wxBitmap vmeico,missingData;

			if(s == 0)
			{
				vmeico = albaWhiteFade(albaPictureFactory::GetPictureFactory()->GetVmePic(name),0.75);
				missingData = albaWhiteFade(albaPictureFactory::GetPictureFactory()->GetVmePic(name),0.4);
			}
			else
			{
				missingData = vmeico = albaPictureFactory::GetPictureFactory()->GetVmePic(name);
				missingData=albaWhiteFade(missingData,0.75);
			}

			vmeico=albaBlueScale(vmeico);
			vmeico = MergeIcons(state_ico[s],vmeico);
			imgs->Add(vmeico);

			missingData=MergeIcons(state_ico[s],missingData); // Same icon as above, but represent a 
			imgs->Add(missingData);                                 // node with no data available.
    }
  }
  SetImageList(imgs);
}
//----------------------------------------------------------------------------
wxBitmap albaGUICheckTree::MergeIcons(wxBitmap state, wxBitmap vme)
//----------------------------------------------------------------------------
{
  int sw = state.GetWidth();
  int sh = state.GetHeight();
  int vw = vme.GetWidth();
  int vh = vme.GetHeight();
  int w = sw+vw;
  int h = vh;
  int hpos = (vh-sh)/2; // state icon should be center-aligned in vertical 

  wxMemoryDC statedc;
  statedc.SelectObject(state);

  wxMemoryDC vmedc;  
  vmedc.SelectObject(vme);

  wxBitmap merge = wxBitmap(w,h);
  wxMemoryDC mergedc;
  mergedc.SelectObject(merge);

  mergedc.SetBackground(*wxWHITE_BRUSH);
  mergedc.Clear();
  mergedc.Blit(0, hpos, sw, sh, &statedc, 0, 0);
  mergedc.Blit(sw, 0, vw, vh, &vmedc, 0, 0);
  mergedc.SelectObject(wxNullBitmap); //merge must be removed from the DC at the end
  return merge;
}
//----------------------------------------------------------------------------
void albaGUICheckTree::OnSelectionChanged(wxTreeEvent& event)
//----------------------------------------------------------------------------
{
  wxTreeItemId i;
  if(m_PreventNotify)
    return;

  i = event.GetItem();
  if(i.IsOk())
    albaEventMacro(albaEvent(this, VME_SELECT, (albaVME *)NodeFromItem(i)));
	event.Skip();
}
