/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgCheckTree.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-08 18:01:05 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mmgCheckTree.h" //always include the class being defined as first
#include "mafPics.h" 

#include "mafNode.h"
#include "mafView.h"

#include <vector>

//#include "mafDecl.h"
//#include "mafEvent.h"
//#include "mafVmeData.h"
//#include "mafSceneNode.h"
//#include "mafSceneGraph.h" 
//#include "mafNodeIterator.h" //used in CryptSubTree

//=========================================================================================
// Helper class to subclass the wxTreeCtrl
// in order to intercept mouse clicks. 
//=========================================================================================
class mmgCheckTreeEvtHandler : public wxEvtHandler
{
public:
  mmgCheckTreeEvtHandler() : m_tree(0) { }
  mmgCheckTreeEvtHandler(mmgCheckTree* tree) : m_tree(tree) { }
  
  void OnMouseDown(wxMouseEvent& event)        {m_tree->OnMouseDown(event);};
  void OnMouseUp(wxMouseEvent& event)          {m_tree->OnMouseUp(event);};
  void OnMouseEvent(wxMouseEvent& event)       {m_tree->OnMouseEvent(event);};
	void ShowContextualMenu(wxMouseEvent& event) {m_tree->ShowContextualMenu(event);};
		
private:
  mmgCheckTree  *m_tree;
  
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
		EVT_MENU_RANGE(RMENU_START, RMENU_STOP, mmgCheckTree::OnContextualMenu)	//Added by Paolo 15-9-2003
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgCheckTree::mmgCheckTree( wxWindow* parent,wxWindowID id, bool CloseButton, bool HideTitle)
:mmgTree(parent,id,CloseButton,HideTitle)
//----------------------------------------------------------------------------
{
  m_view = NULL;
	m_clicked_vme = NULL;
  m_canSelect	= true;
  m_RMenu	= NULL;

  m_tree->PushEventHandler( new mmgCheckTreeEvtHandler(this) );

  InitializeImageList();
}
//----------------------------------------------------------------------------
mmgCheckTree::~mmgCheckTree( )
//----------------------------------------------------------------------------
{
	if(m_RMenu) delete m_RMenu;
	m_tree->PopEventHandler(true);
}
//----------------------------------------------------------------------------
void mmgCheckTree::ShowContextualMenu(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
/*
  //	if(m_view == NULL) return;
  if(m_RMenu != NULL) return; //this is a quick double click 

	int flag;
	wxTreeItemId i = m_tree->HitTest(wxPoint(event.GetX(),event.GetY()),flag);

	m_RMenu = new wxMenu;
	
	if(i.IsOk() && ((flag == wxTREE_HITTEST_ONITEMICON) || (flag == wxTREE_HITTEST_ONITEMLABEL)))
	{
		m_clicked_vme = (mafNode*) (NodeFromItem(i));
		int status = GetVmeStatus(m_clicked_vme);
    bool enable;

    if(m_view != NULL)
    {
		  m_RMenu->Append(RMENU_SHOW_VME, "Show","",true);
		  m_RMenu->AppendSeparator();
		  m_RMenu->Append(RMENU_SHOW_SUBTREE,	 "Show sub-tree");
		  m_RMenu->Append(RMENU_SHOW_SAMETYPE, "Show same type");
		  m_RMenu->AppendSeparator();
		  m_RMenu->Append(RMENU_HIDE_SUBTREE,  "Hide sub-tree");
		  m_RMenu->Append(RMENU_HIDE_SAMETYPE, "Hide same type");
		  m_RMenu->AppendSeparator();

		  // check visibility 
		  m_RMenu->FindItem(RMENU_SHOW_VME)->Check(status==NODE_VISIBLE_ON || status==NODE_MUTEX_ON);
		  m_RMenu->FindItem(RMENU_SHOW_VME)->Enable(status != NODE_NON_VISIBLE );
		  
		  // enable show/hide subtree 
      enable = (m_clicked_vme->GetNumberOfChildren() > 0 );
		  m_RMenu->FindItem(RMENU_SHOW_SUBTREE)->Enable(enable);
		  m_RMenu->FindItem(RMENU_HIDE_SUBTREE)->Enable(enable);

		  // enable show/hide sametype - must be visualizable and not mutex
		  enable = (status==NODE_VISIBLE_ON || status==NODE_VISIBLE_OFF);								
		  m_RMenu->FindItem(RMENU_SHOW_SAMETYPE)->Enable(enable);
		  m_RMenu->FindItem(RMENU_HIDE_SAMETYPE)->Enable(enable);
    }

	  m_RMenu->Append(RMENU_CRYPT_VME, "Crypt","",true);
	  m_RMenu->Append(RMENU_ENABLE_CRYPT_SUBTREE, "Enable crypt sub-tree");
	  m_RMenu->Append(RMENU_DISABLE_CRYPT_SUBTREE, "Disable crypt sub-tree");
	  m_RMenu->AppendSeparator();
    m_RMenu->Append(RMENU_SORT_TREE,		 "Sort children nodes");

    m_check_crypto = m_clicked_vme->GetCrypting() != 0;
    m_RMenu->FindItem(RMENU_CRYPT_VME)->Check(m_check_crypto);
		enable = (m_clicked_vme->GetNumberOfChildren() > 0 );
    m_RMenu->FindItem(RMENU_ENABLE_CRYPT_SUBTREE)->Enable(enable);
		m_RMenu->FindItem(RMENU_DISABLE_CRYPT_SUBTREE)->Enable(enable);
	}

	m_RMenu->Append(RMENU_AUTO_SORT,		 "Keep tree nodes sorted","",true);
	m_RMenu->FindItem(RMENU_AUTO_SORT)->Check(m_autosort);
	
	// show the popup menù
	wxPoint p(event.GetPosition());
	p.y = p.y + 16;
	PopupMenu(m_RMenu, p); // is like ShowModal - can delete the menù now
  	
  wxDEL(m_RMenu);
	return;
*/
}
//----------------------------------------------------------------------------
void mmgCheckTree::OnContextualMenu(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
/*
	mafSceneGraph *sg = NULL;
  if(m_view != NULL)
		sg = m_view->GetSceneGraph();
	
	assert(sg);

	switch(event.GetId())
	{
	  case RMENU_SHOW_VME:
		{
      bool show = !this->m_RMenu->IsChecked(RMENU_SHOW_VME);
			mafEventMacro(mafEvent(this, VME_SHOW, m_clicked_vme, show));
    }
		break;
		case RMENU_SHOW_SUBTREE:
			sg->VmeShowSubTree(m_clicked_vme, true);
		break;
		case RMENU_HIDE_SUBTREE:
			sg->VmeShowSubTree(m_clicked_vme, false);
		break;
		case RMENU_SHOW_SAMETYPE:
			sg->VmeShowByType(m_clicked_vme, true);
		break;
		case RMENU_HIDE_SAMETYPE:
			sg->VmeShowByType(m_clicked_vme, false);
		break;
    case RMENU_CRYPT_VME:
    {
      m_check_crypto = !m_check_crypto;
      m_clicked_vme->SetCrypting(m_check_crypto);
      ((mafVmeData *)m_clicked_vme->GetClientData())->UpdateFromTag();
    }
    break;
    case RMENU_ENABLE_CRYPT_SUBTREE:
      CryptSubTree(true);
    break;
    case RMENU_DISABLE_CRYPT_SUBTREE:
      CryptSubTree(false);
    break;
		case RMENU_SORT_TREE:
			SortTree((long)m_clicked_vme);
		break;
		case RMENU_AUTO_SORT:
			m_autosort = ! m_autosort;
			if(m_autosort) SortTree();
		break;
		default:
			assert(false);
		break;
	}
  if(m_view != NULL)
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
*/
}
//----------------------------------------------------------------------------
void mmgCheckTree::EnableSelect(bool enable)
//----------------------------------------------------------------------------
{
  m_canSelect = enable;
}
//----------------------------------------------------------------------------
void mmgCheckTree::OnMouseDown( wxMouseEvent& event )
//----------------------------------------------------------------------------
{
  //pourpose: intercept and notify if the icon was clicked,
  //prevent node selection if the icon was clicked,
  //prevent node selection anyway, if the selection is disabled,
  int flag;
	wxTreeItemId i = m_tree->HitTest(wxPoint(event.GetX(),event.GetY()),flag);
	if(i.IsOk() && flag == wxTREE_HITTEST_ONITEMICON )
	{
    OnIconClick(i); 
		return;//eat message
	} 
	if(!this->m_canSelect)	
	  return; //also eat message if selection is disabled
	event.Skip();//process event as usual
}
//----------------------------------------------------------------------------
void mmgCheckTree::OnMouseUp( wxMouseEvent& event )
//----------------------------------------------------------------------------
{
  //pourpose: prevent selection if I clicked on the icon.
  //to select you must click the node name
  int flag;
	wxTreeItemId i = m_tree->HitTest(wxPoint(event.GetX(),event.GetY()),flag);
	if(flag == wxTREE_HITTEST_ONITEMICON )
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
	 wxTreeItemId i = m_tree->HitTest(wxPoint(event.GetX(),event.GetY()),flag);
	 if(flag == wxTREE_HITTEST_ONITEMICON )
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
    mafEventMacro(mafEvent(this, VME_SHOW, vme, show));
    mafEventMacro(mafEvent(this, CAMERA_UPDATE));
  }
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
}
//----------------------------------------------------------------------------
void mmgCheckTree::VmeSelected(mafNode *vme)   
//----------------------------------------------------------------------------
{
  this->SelectNode((long)vme);
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
  static int foo =0;
  foo++;
  if (foo>=5) foo=0;
  return foo;
  /*
  if(!m_view) return NODE_NON_VISIBLE;
  mafSceneGraph *sg = m_view->GetSceneGraph();
  if(!sg)     return NODE_NON_VISIBLE;
  return sg->GetNodeStatus(vme);
  */
}
//----------------------------------------------------------------------------
void mmgCheckTree::VmeUpdateIcon(mafNode *vme)   
//----------------------------------------------------------------------------
{
  int icon_index = ClassNameToIcon(vme->GetTypeName()) + GetVmeStatus(vme);
  SetNodeIcon( (long)vme, icon_index );

  /*
  int type = mafGetBaseType(vme);

  if(type == VME_WIDGET) type = VME_GIZMO;//SIL. 18-11-2004: 

  if(type == VME_POINTSET && vme->IsA("mafNodeLandmark")) type++;

  if(vme->IsA("mafNodeExField")           ==1) type = VME_VOLUME;            

  if(type == VME_TOOL ) 
  {
    	   if(vme->IsA("mafNodePointSet")==1) type = VME_POINTSET;
	  else if(vme->IsA("mafNodeSurface") ==1) type = VME_SURFACE;
	  else if(vme->IsA("mafNodeImage")   ==1) type = VME_IMAGE;
	  else if(vme->IsA("mafNodeGrayVolume")  ==1) type = VME_GRAY_VOLUME;
	  else if(vme->IsA("mafNodeVolume")  ==1) type = VME_VOLUME;
	  else if(vme->IsA("mafNodeGizmo")   ==1) type = VME_GIZMO;
    else if(vme->IsA("mafNodeExternalData")  ==1) type = VME_EXTERNAL_DATA;
	  else if(vme->IsA("mafNodefem")     ==1) type = VME_FEM;
	  else if(vme->IsA("mafNodeScalar")     ==1) type = VME_SCALAR;
	  else type = VME_GENERIC;
  }

#ifdef MAF_SHOW_GIZMO
	if(type == VME_GIZMO) type = VME_SURFACE;
#endif

	int status = GetVmeStatus(vme);
  int icon = type * 5 + status;

	SetNodeIcon((long)vme,icon);
  */
}
//----------------------------------------------------------------------------
void mmgCheckTree::ViewSelected(mafView *view)
//----------------------------------------------------------------------------
{
	/*
  m_view=view;

	m_table->BeginFind();
	while(wxNode* node = m_table->Next())
	{
		mmgTreeTableElement* el = (mmgTreeTableElement*)node->Data();
		assert(el);
		wxTreeItemId i = el->GetItem(); 
		if(i.IsOk())
			VmeUpdateIcon((mafNode*)NodeFromItem(i));
	}
  */
}
//----------------------------------------------------------------------------
void mmgCheckTree::ViewDeleted(mafView *view)
//----------------------------------------------------------------------------
{
	if(view != m_view) return;
	ViewSelected(NULL);
}
//----------------------------------------------------------------------------
void mmgCheckTree::CryptSubTree(bool crypt)
//----------------------------------------------------------------------------
{
	/*
  mafVmeData *vd = NULL;
  mafNodeIterator *iter = m_clicked_vme->NewIterator();

	for(mafNode *v=iter->GetFirstNode();v;v=iter->GetNextNode())
	{
    if(mafGetBaseType(v) == VME_GIZMO)
      continue;
    v->SetCrypting(crypt);
    vd = (mafVmeData *)v->GetClientData();
    if (vd == NULL)
    {
      mafEventMacro(mafEvent(this,VME_CREATE_CLIENT_DATA,v));
      vd = (mafVmeData *)v->GetClientData();
    }
    vd->UpdateFromTag();
	}
	iter->Delete();
  */
}
//----------------------------------------------------------------------------
int mmgCheckTree::ClassNameToIcon(wxString classname)
//----------------------------------------------------------------------------
{
  MapClassNameToIcon::iterator it=m_MapClassNameToIcon.find(classname.c_str());
  if (it!= m_MapClassNameToIcon.end())
    return int((*it).second);
  else
  {
    wxLogMessage("mafPictureFactory::ClassNameToIcon: cant find = %s ",classname);
    return 0;
  }
}
//----------------------------------------------------------------------------
void mmgCheckTree::InitializeImageList()
//----------------------------------------------------------------------------
{
  // pourpose:
  // each vme-picture is combined with each state-picture,
  // all the combined picture are inserted in the imagelist.
  // given a vme-class-name and a vme-state
  // the corresponding icon index can be retrieved as 
  // ClassNameToIcon(vme-class-name) + vme-state

  std::vector<wxString>  v;
  mafPics.GetVmeNames(v);

  const int num_of_status = 5; 
  int num_types = v.size();
  int num_icons = num_types * num_of_status;

  if(num_types <= 0)
  {
    wxLogMessage("mmgCheckTree:  Warning - no vme-icons defined");
    return;
  }
  //retrieve state icons
  //I assume all state-icon to have the same size
  wxBitmap state_ico[num_of_status];
  state_ico[0] = mafPics.GetBmp("DISABLED");
  state_ico[1] = mafPics.GetBmp("CHECK_OFF");
  state_ico[2] = mafPics.GetBmp("CHECK_ON");
  state_ico[3] = mafPics.GetBmp("RADIO_ON");
  state_ico[4] = mafPics.GetBmp("RADIO_OFF");
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
    m_MapClassNameToIcon[name]=i*num_of_status;
    
    for( int s=0; s<num_of_status; s++)
    {
      wxBitmap vmeico = mafPics.GetVmePic(name);
      if(s==0) vmeico = GrayScale(vmeico);
      wxBitmap merged = MergeIcons(state_ico[s],vmeico);
      imgs->Add(merged);
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
wxBitmap mmgCheckTree::GrayScale(wxBitmap bmp)
//----------------------------------------------------------------------------
{
  wxImage img = bmp.ConvertToImage();
  unsigned char *p = img.GetData();
  unsigned char *max = p + img.GetWidth() * img.GetHeight() * 3;
  unsigned char *r, *g, *b;
  unsigned int gray ;
  while( p < max )
  {
     r = p++;
     g = p++;
     b = p++;
     gray = *r + *g + *b;
     *r = *g = *b = gray / 3;
  }
  return wxBitmap(img);
}


