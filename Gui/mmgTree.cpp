/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgTree.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-10 12:03:08 $
  Version:   $Revision: 1.7 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "mmgTree.h"
#include "mafDecl.h"
#include "mafPics.h"
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(mmgTree,wxPanel)
    EVT_TREE_SEL_CHANGED(ID_TREE, mmgTree::OnSelectionChanged)
    EVT_SIZE(mmgTree::OnSize)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgTree::mmgTree( wxWindow* parent,wxWindowID id, bool CloseButton, bool HideTitle)
:mmgNamedPanel(parent,id,CloseButton,HideTitle)
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_images = NULL;
  m_table = NULL;
  m_prevent_notify = false;
  m_autosort	= false;

  m_tree = new wxTreeCtrl(this,ID_TREE,wxDefaultPosition,wxSize(100,100),wxNO_BORDER | wxTR_HAS_BUTTONS );
  m_sizer->Add(m_tree,1,wxEXPAND);

  //default imagelist
  wxBitmap bmp = mafPics.GetBmp("NODE_GRAY");
  int w = bmp.GetWidth();
  int h = bmp.GetHeight();

  m_images = new wxImageList(w,h,FALSE,4);
  m_images->Add(bmp);
  m_images->Add(mafPics.GetBmp("NODE_RED"));
  m_images->Add(mafPics.GetBmp("NODE_BLUE"));
  m_images->Add(mafPics.GetBmp("NODE_YELLOW"));
  m_tree->SetImageList(m_images);

  Reset();
}
//----------------------------------------------------------------------------
mmgTree::~mmgTree( )
//----------------------------------------------------------------------------
{
  if (m_table) 
  {
    m_table->DeleteContents(true);
    delete m_table;
  }
  delete m_images;
}
//----------------------------------------------------------------------------
void mmgTree::Reset ()
//----------------------------------------------------------------------------
{
  m_tree->DeleteAllItems();

  if (m_table != NULL) delete m_table; // short way to delete all associated objets
  m_table = new wxHashTable(wxKEY_INTEGER,100);
  m_table->DeleteContents(true);
  m_root  = 0;
}
//----------------------------------------------------------------------------
bool mmgTree::AddNode (long node_id, long parent_id , wxString label, int icon)
//----------------------------------------------------------------------------
{
	/*
	- se parent_id = 0 to create the root
	- icon must be a valid index in the m_images - otherwise it is clamped
	- fails if node_id already exist
	- parent_id must exist
	- return true on success
	*/

  icon = CheckIconId(icon);

  wxTreeItemId  item, parent_item;

  // check if already inserted
  if( NodeExist(node_id) ) return false;

	if( parent_id == 0 && m_root == 0 ) 
	{
		item = m_tree->AddRoot(label, icon, icon, new mmgTreeItemData(node_id));
		m_root = node_id;
	}
  else
  {
    if(!NodeExist(parent_id) ) return false;
    parent_item = ItemFromNode(parent_id);
    //insert normally
    item = m_tree->AppendItem(parent_item,label,icon,icon,new mmgTreeItemData(node_id));
    // expand parent node
    m_tree->SortChildren(parent_item);
    m_tree->SetItemHasChildren(parent_item,true);
    m_tree->Expand(parent_item);
  }

  //insert [node_id -> item] in the table
  mmgTreeTableElement *el = new mmgTreeTableElement( item );  
  m_table->Put(node_id, el);

  return true;
}
//----------------------------------------------------------------------------
bool mmgTree::DeleteNode  (long node_id)
//----------------------------------------------------------------------------
{
	/*
	- node_id must exist
	- return true on success
	- check that node_id exist
	- calls DeleteNode2 to delete all the item in the subtree keeping m_table consistent
	- keep parent->HasChildren consistent
	- if the item was selected, select the parent node (). [ to prevent the shown property-gui to become inconsistent ]
	*/

  if( !NodeExist(node_id) ) return false;

  wxTreeItemId item = ItemFromNode(node_id);
  wxTreeItemId parent_item = m_tree->GetItemParent(item);

  // move the selection
  if (m_tree->IsSelected(item))
  {
    m_tree->SelectItem(parent_item);
  }

  // if the old parent has no other children, set HasChildren to false => hide the widget to expand the subtree
  if( m_tree->GetChildrenCount(parent_item,false) <= 1 )
  {
    m_tree->SetItemHasChildren(parent_item,false);
  }
   
  DeleteNode2(node_id);
   
  if(node_id == m_root) m_root = 0; // if we deleted the root we can create a new one
  return true;
}
//----------------------------------------------------------------------------
void mmgTree::DeleteNode2 (long node_id)
//----------------------------------------------------------------------------
{
	/*
	- private function called by DeleteNode. Delete recursively a node and its subtree
	- doesn't check if node exist
	- doesn't handle the selection
	- keep m_table consistent
	*/

  long cookie;
  wxTreeItemId item  = ItemFromNode(node_id);
  wxTreeItemId child = m_tree->GetFirstChild(item, cookie);
  while(child.IsOk())
	{
    DeleteNode2(NodeFromItem(child)); 
    child = m_tree->GetNextChild(item, cookie);
  }

  m_tree->Delete(item);

  wxObject *el = m_table->Delete(node_id);
  if (el != NULL ) delete el;
}
//----------------------------------------------------------------------------
bool mmgTree::SetNodeLabel (long node_id, wxString label)
//----------------------------------------------------------------------------
{
  if( !NodeExist(node_id) ) return false;
  wxTreeItemId  item = ItemFromNode(node_id);
  m_tree->SetItemText(item,label);

  wxTreeItemId parent = m_tree->GetItemParent(item);
  if(parent.IsOk() )
    if(m_autosort) m_tree->SortChildren(parent); 

  return true;
}
//----------------------------------------------------------------------------
bool mmgTree::SetNodeParent(long node_id, long parent_id )
//----------------------------------------------------------------------------
{
	/*
	- node_id must exist
	- fails if parent_id is a children of node_id
	- return true on success
	- check that node_id exist
	- calls DeleteNode2 to delete all the item in the subtree keeping m_table consistent
	- keep parent->HasChildren consistent
	- if the item was selected, select the parent node. [ to prevent the shown property-gui to become inconsistent ]
	*/

  if( node_id == parent_id )  return false;

  if( !NodeExist(node_id) )   return false;
  if( !NodeExist(parent_id) ) return false;

  // check that node_id is not an ancestor of parent_id
  wxTreeItemId i = ItemFromNode(parent_id);;
  while( i = m_tree->GetItemParent(i) )
  {
    if ( NodeFromItem(i) == node_id ) return false;
  }

  // if the old parent has no other children, set HasChildren to false => hide the widget to expand the subtree
  i = ItemFromNode(node_id);;
  wxTreeItemId old_parent_item = m_tree->GetItemParent(i);
  if( m_tree->GetChildrenCount(old_parent_item,false) <= 1 )
  {
    m_tree->SetItemHasChildren(old_parent_item,false);
  }

  // Now the checks are made - start to move
  SetNodeParent2(node_id,parent_id);

  // Set hasChildren of the new parent and open it
  i = ItemFromNode(parent_id);;
  m_tree->SetItemHasChildren(i,true);
	m_tree->SortChildren(i);
  m_tree->Expand(i);

  return true;
}
//----------------------------------------------------------------------------
void mmgTree::SetNodeParent2(long node_id, long parent_id )
//----------------------------------------------------------------------------
{
	/*
	- private function called by SetNodeParent
	- copy old_node under parent_id
	- move recursively all the children under the new node
	- delete old_node
	- keep m_table,HasChildren,IsExpanded consistent
	*/

  wxTreeItemId item        = ItemFromNode(node_id);
  wxTreeItemId parent_item = ItemFromNode(parent_id);
  int          icon        = m_tree->GetItemImage(item);
  wxString     label       = m_tree->GetItemText(item);
  wxTreeItemId new_item    = m_tree->AppendItem(parent_item,label,icon,icon,new mmgTreeItemData(node_id));
  bool         HasChildren = m_tree->ItemHasChildren(item);
  bool         IsExpanded  = m_tree->IsExpanded(item);

  // update the table
  mmgTreeTableElement* el = (mmgTreeTableElement*)m_table->Get(node_id);
  assert (el != NULL);
  el->SetItem(new_item);

  // move recursively the sub tree under new_item
  long cookie;
  wxTreeItemId child = m_tree->GetFirstChild(item, cookie);
  while(child.IsOk())
  {
    SetNodeParent2( NodeFromItem(child), node_id);
    child = m_tree->GetNextChild(item, cookie);
  }

  //Synchronize HasChildren and Expanded
  m_tree->SetItemHasChildren(new_item,HasChildren);
  if( IsExpanded ) m_tree->Expand(new_item); else m_tree->Collapse(new_item);

  m_tree->Delete(item);
}
//----------------------------------------------------------------------------
bool mmgTree::SetNodeIcon (long node_id, int icon)
//----------------------------------------------------------------------------
{
  icon = CheckIconId(icon);
  if( !NodeExist(node_id) ) return false;
  wxTreeItemId  item = ItemFromNode(node_id);
  m_tree->SetItemImage(item,icon);
  m_tree->SetItemSelectedImage(item,icon);
  return true;
}
//----------------------------------------------------------------------------
bool mmgTree::NodeExist(long node_id)
//----------------------------------------------------------------------------
{
  mmgTreeTableElement* el = (mmgTreeTableElement*)m_table->Get(node_id);
  return el != NULL;
}
//----------------------------------------------------------------------------
wxTreeItemId mmgTree::ItemFromNode(long node_id)
//----------------------------------------------------------------------------
{
  mmgTreeTableElement* el = (mmgTreeTableElement*)m_table->Get(node_id);
  assert (el != NULL);
  return el->GetItem();
}
//----------------------------------------------------------------------------
long mmgTree::NodeFromItem(wxTreeItemId& item)
//----------------------------------------------------------------------------
{
  mmgTreeItemData *nd = (mmgTreeItemData*)m_tree->GetItemData(item);
  if(nd == NULL) return 0;
  return nd->GetNode();
}
//----------------------------------------------------------------------------
void mmgTree::OnSelectionChanged(wxTreeEvent& event)
//----------------------------------------------------------------------------
{
  wxTreeItemId i;
  if(m_prevent_notify) return;

  i = event.GetItem();
  if(i.IsOk())
    mafEventMacro(mafEvent(this, VME_SELECT, NodeFromItem(i)));
  event.Skip();
}
//----------------------------------------------------------------------------
bool mmgTree::SelectNode(long node_id)
//----------------------------------------------------------------------------
{
  if( !NodeExist(node_id) ) return false;
  wxTreeItemId  item = ItemFromNode(node_id);
  m_prevent_notify = true;
  m_tree->SelectItem(item);
  m_prevent_notify = false;
  return true;
}
//----------------------------------------------------------------------------
void mmgTree::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{
	mmgNamedPanel::OnSize(event);
  m_tree->Refresh();
}
//----------------------------------------------------------------------------
int mmgTree::CheckIconId(int icon)
//----------------------------------------------------------------------------
{
  if(!m_images) return 0;
  if( icon <0 )
  {
    mafLogMessage("mmgTree: icon id = %d out of range ", icon);
    return 0;
  }
  if( icon >= m_images->GetImageCount() ) 
  {
    mafLogMessage("mmgTree: icon id = %d out of range ", icon);
    return m_images->GetImageCount()-1;
  }
  return icon;
}
//----------------------------------------------------------------------------
void mmgTree::SetImageList(wxImageList *img)
//----------------------------------------------------------------------------
{
  if(m_root != 0)
  {
    mafLogMessage("warning: mmgTree::SetImageList must be called before adding any node");
    // if you replace the imagelist with a shorter one 
    // the icon-index actually in use by the existing nodes 
    // can become inconsistent 
    
    //return; //SIL. 7-4-2005: - commented 4 testing -- to be reinserted
  }

  if(img == m_images) return;
  cppDEL(m_images);
  m_images = img;
  m_tree->SetImageList(m_images);
}
//----------------------------------------------------------------------------
int mmgTree::GetNodeIcon(long node_id)
//----------------------------------------------------------------------------
{
  if( !NodeExist(node_id) ) return 0;
  wxTreeItemId  item = ItemFromNode(node_id);
  if(!item.IsOk()) return 0;
  return m_tree->GetItemImage(item);
}
//----------------------------------------------------------------------------
void mmgTree::SortChildren(long node_id)
//----------------------------------------------------------------------------
{
  if(node_id == 0)
    m_tree->SortChildren(m_tree->GetRootItem());
  else
    m_tree->SortChildren(ItemFromNode(node_id));
}
//----------------------------------------------------------------------------
void mmgTree::CollapseNode(long node_id)
//----------------------------------------------------------------------------
{
  if( !NodeExist(node_id) ) return;
  wxTreeItemId  item;
  item = ItemFromNode(node_id);
  if(!item.IsOk()) return;
  m_tree->Collapse(item);
}
//----------------------------------------------------------------------------
void mmgTree::ExpandNode(long node_id)
//----------------------------------------------------------------------------
{
  if( !NodeExist(node_id) ) return;
  wxTreeItemId  item;
  item = ItemFromNode(node_id);
  if(!item.IsOk()) return;
  m_tree->Expand(item);
}


