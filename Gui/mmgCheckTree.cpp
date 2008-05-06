/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgCheckTree.cpp,v $
  Language:  C++
  Date:      $Date: 2008-05-06 14:58:18 $
  Version:   $Revision: 1.27 $
  Authors:   Silvano Imboden
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


#include "mmgCheckTree.h" 
#include "mafDecl.h"
#include "mafView.h"
#include "mafPics.h" 

#include "mafNode.h"
#include "mafVME.h"
#include "mafNodeIterator.h"
#include <vector>

//=========================================================================================
// Helper class to subclass the wxTreeCtrl
// in order to intercept mouse clicks. 
//=========================================================================================
class mmgCheckTreeEvtHandler : public wxEvtHandler
{
public:
  mmgCheckTreeEvtHandler() : m_NodeTree(0) { }
  mmgCheckTreeEvtHandler(mmgCheckTree* tree) : m_NodeTree(tree) { }
  
  void OnMouseDown(wxMouseEvent& event)        {m_NodeTree->OnMouseDown(event);};
  void OnMouseUp(wxMouseEvent& event)          {m_NodeTree->OnMouseUp(event);};
  void OnMouseEvent(wxMouseEvent& event)       {m_NodeTree->OnMouseEvent(event);};
	void ShowContextualMenu(wxMouseEvent& event) {m_NodeTree->ShowContextualMenu(event);};
		
private:
  mmgCheckTree  *m_NodeTree;
  
	DECLARE_DYNAMIC_CLASS(mmgCheckTreeEvtHandler )
  DECLARE_EVENT_TABLE()
};
IMPLEMENT_DYNAMIC_CLASS( mmgCheckTreeEvtHandler , wxEvtHandler )
BEGIN_EVENT_TABLE( mmgCheckTreeEvtHandler , wxEvtHandler )
    EVT_LEFT_DOWN( mmgCheckTreeEvtHandler ::OnMouseDown )
		EVT_RIGHT_UP( mmgCheckTreeEvtHandler ::ShowContextualMenu )
    EVT_LEFT_UP( mmgCheckTreeEvtHandler ::OnMouseUp )
    EVT_MOUSE_EVENTS( mmgCheckTreeEvtHandler ::OnMouseEvent )
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
BEGIN_EVENT_TABLE(mmgCheckTree,wxPanel)
    EVT_TREE_SEL_CHANGED(ID_TREE, mmgCheckTree::OnSelectionChanged)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgCheckTree::mmgCheckTree( wxWindow* parent,wxWindowID id, bool CloseButton, bool HideTitle)
:mmgTree(parent,id,CloseButton,HideTitle)
//----------------------------------------------------------------------------
{
  m_View = NULL;
  m_CanSelect	= true;
  m_SelectedNode = NULL;
  m_RMenu	= NULL;

  m_NodeTree->PushEventHandler( new mmgCheckTreeEvtHandler(this) );

  InitializeImageList();
}
//----------------------------------------------------------------------------
mmgCheckTree::~mmgCheckTree()
//----------------------------------------------------------------------------
{
	if(m_RMenu)
    delete m_RMenu;
	m_NodeTree->PopEventHandler(true);
}
//----------------------------------------------------------------------------
void mmgCheckTree::ShowContextualMenu(wxMouseEvent& event)
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

  mafEvent e;
  e.SetSender(this);
  e.SetId(SHOW_CONTEXTUAL_MENU);
  e.SetBool(vmeMenu);
  e.SetArg(m_Autosort);
  if(vmeMenu)
    e.SetVme((mafNode *) (NodeFromItem(i)));
  mafEventMacro(e);
}
//----------------------------------------------------------------------------
void mmgCheckTree::EnableSelect(bool enable)
//----------------------------------------------------------------------------
{
  m_CanSelect = enable;
}
//----------------------------------------------------------------------------
void mmgCheckTree::OnMouseDown( wxMouseEvent& event )
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
	if(!this->m_CanSelect)	
	  return; //also eat message if selection is disabled
	event.Skip();//process event as usual
}
//----------------------------------------------------------------------------
void mmgCheckTree::OnMouseUp( wxMouseEvent& event )
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
void mmgCheckTree::OnMouseEvent( wxMouseEvent& event )
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
void mmgCheckTree::OnIconClick(wxTreeItemId item)
//----------------------------------------------------------------------------
{
  mafNode* vme = (mafNode*) (NodeFromItem(item));
  int status = GetVmeStatus(vme); 

  if(status != NODE_NON_VISIBLE)		
  {
    bool show = !(status == NODE_VISIBLE_ON || status == NODE_MUTEX_ON ); 
    if (!show && !this->m_CanSelect && m_SelectedNode && m_SelectedNode == vme)
    {
      return;
    }
    mafEventMacro(mafEvent(this, VME_SHOW, vme, show));
    mafEventMacro(mafEvent(this, CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
bool mmgCheckTree::IsIconChecked(wxTreeItemId item)
//----------------------------------------------------------------------------
{
  mafNode* vme = (mafNode*) (NodeFromItem(item));
  bool checked = GetNodeIcon((long)vme) == (ClassNameToIcon(vme->GetTypeName()) + NODE_VISIBLE_ON * 2);
  return checked;
}
//----------------------------------------------------------------------------
void mmgCheckTree::VmeAdd(mafNode *vme)   
//----------------------------------------------------------------------------
{
  AddNode((long)vme,(long)vme->GetParent(), vme->GetName(), 0);
	VmeUpdateIcon(vme);
}
//----------------------------------------------------------------------------
void mmgCheckTree::VmeRemove(mafNode *vme)   
//----------------------------------------------------------------------------
{
  this->DeleteNode((long)vme);
  if (m_SelectedNode == vme)
  {
    m_SelectedNode = NULL;
  }
}
//----------------------------------------------------------------------------
void mmgCheckTree::VmeSelected(mafNode *vme)   
//----------------------------------------------------------------------------
{
  this->SelectNode((long)vme);
  m_SelectedNode = vme;
  VmeUpdateIcon(vme);
}
//----------------------------------------------------------------------------
void mmgCheckTree::VmeModified(mafNode *vme)
//----------------------------------------------------------------------------
{
  this->SetNodeLabel((long)vme, vme->GetName());
	VmeUpdateIcon(vme);
}
//----------------------------------------------------------------------------
void mmgCheckTree::VmeShow(mafNode *vme, bool show)   
//----------------------------------------------------------------------------
{
	VmeUpdateIcon(vme);
}
//----------------------------------------------------------------------------
int mmgCheckTree::GetVmeStatus(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(!m_View)
    return NODE_NON_VISIBLE;
  return m_View->GetNodeStatus(vme);
}
//----------------------------------------------------------------------------
void mmgCheckTree::VmeUpdateIcon(mafNode *vme)   
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = vme->NewIterator();
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    int dataStatus = 1;
    int icon_index;

    dataStatus = ((mafVME *)node)->IsDataAvailable() ? 0 : 1;
    icon_index = ClassNameToIcon(node->GetTypeName()) + (GetVmeStatus(node)*2) + dataStatus;
    SetNodeIcon( (long)node, icon_index );

    if (node->GetNumberOfLinks() != 0)
    {
      mafNode::mafLinksMap *links = node->GetLinks();
      mafVME *linkedVME = NULL;
      for (mafNode::mafLinksMap::iterator it = links->begin(); it != links->end(); it++)
      {
        if(it->second.m_Node)
        {
          linkedVME = mafVME::SafeDownCast(it->second.m_Node);
          if (linkedVME)
          {
            dataStatus = linkedVME->IsDataAvailable() ? 0 : 1;
            icon_index = ClassNameToIcon(linkedVME->GetTypeName()) + (GetVmeStatus(linkedVME)*2) + dataStatus;
            SetNodeIcon( (long)linkedVME, icon_index );
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
void mmgCheckTree::ViewSelected(mafView *view)
//----------------------------------------------------------------------------
{
  m_View = view;
  TreeUpdateIcon();
}
//----------------------------------------------------------------------------
void mmgCheckTree::TreeUpdateIcon()
//----------------------------------------------------------------------------
{
  if (m_SelectedNode != NULL)
  {
  	VmeUpdateIcon(m_SelectedNode->GetRoot());
  }
}
//----------------------------------------------------------------------------
void mmgCheckTree::ViewDeleted(mafView *view)
//----------------------------------------------------------------------------
{
	if(view != m_View) 
    return;
	ViewSelected(NULL);
}
//----------------------------------------------------------------------------
int mmgCheckTree::ClassNameToIcon(wxString classname)
//----------------------------------------------------------------------------
{
  MapClassNameToIcon::iterator it = m_MapClassNameToIcon.find(classname.c_str());
  if (it != m_MapClassNameToIcon.end())
    return int((*it).second);
  else
  {
    // search superclass's icon
    // if also this icon is not present, "Unknown" icon is used
    wxLogMessage("mafPictureFactory::ClassNameToIcon: cant find = %s ",classname.c_str());
    return 0;
  }
}
//----------------------------------------------------------------------------
void mmgCheckTree::InitializeImageList()
//----------------------------------------------------------------------------
{
  // purpose:
  // each vme-picture is combined with each state-picture,
  // all the combined picture are inserted in the image-list.
  // given a vme-class-name and a vme-state
  // the corresponding icon index can be retrieved as 
  // ClassNameToIcon(vme-class-name) + vme-state

  std::vector<wxString>  v;
  mafPics.GetVmeNames(v);

  const int num_of_status = 5; 
  int num_types = v.size();
  int num_icons = num_types * (num_of_status * 2); // Added the status "Data not available"

  if(num_types <= 0)
  {
    wxLogMessage("mmgCheckTree:  Warning - no vme-icons defined");
    return;
  }
  //retrieve state icons
  //I assume all state-icon to have the same size
  wxBitmap state_ico[num_of_status];
  state_ico[NODE_NON_VISIBLE] = mafPics.GetBmp("DISABLED");
  state_ico[NODE_VISIBLE_OFF] = mafPics.GetBmp("CHECK_OFF");
  state_ico[NODE_VISIBLE_ON]  = mafPics.GetBmp("CHECK_ON");
  state_ico[NODE_MUTEX_OFF]   = mafPics.GetBmp("RADIO_OFF");
  state_ico[NODE_MUTEX_ON]    = mafPics.GetBmp("RADIO_ON");
  int sw = state_ico[0].GetWidth();
  int sh = state_ico[0].GetHeight();

  //get icon size 
  //I assume all vme-icon to have the same size
  wxBitmap bmp = mafPics.GetVmePic(v[0]);
  assert(bmp != wxNullBitmap);
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
      wxBitmap vmeico = mafPics.GetVmePic(name);
      if(s == 0)
        vmeico = mafGrayScale(vmeico);
      wxBitmap merged = MergeIcons(state_ico[s],vmeico);
      imgs->Add(merged);

      // Icons for missing data
      if(s != 0)
        vmeico = mafGrayScale(vmeico);
      vmeico = mafRedScale(vmeico);
      wxBitmap missingData = MergeIcons(state_ico[s],vmeico); // Same icon as above, but represent a 
      imgs->Add(missingData);                                 // node with no data available.
    }
  }
  SetImageList(imgs);
}
//----------------------------------------------------------------------------
wxBitmap mmgCheckTree::MergeIcons(wxBitmap state, wxBitmap vme)
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
void mmgCheckTree::OnSelectionChanged(wxTreeEvent& event)
//----------------------------------------------------------------------------
{
  wxTreeItemId i;
  if(m_PreventNotify)
    return;

  i = event.GetItem();
  if(i.IsOk())
    mafEventMacro(mafEvent(this, VME_SELECT, NodeFromItem(i)));
  event.Skip();
}
