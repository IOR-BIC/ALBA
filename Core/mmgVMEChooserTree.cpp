/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgVMEChooserTree.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-07 08:35:58 $
  Version:   $Revision: 1.8 $
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


#include "mmgVMEChooserTree.h" 
#include "mafDecl.h"
#include "mmgDialog.h"
#include "mafPics.h" 
#include "mafEvent.h"
#include "mafNode.h"
#include "mafView.h"
#include "mafVMERoot.h"

#include <vector>

//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgVMEChooserTree,wxPanel)
  EVT_TREE_SEL_CHANGED(ID_TREE, mmgVMEChooserTree::OnSelectionChanged)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgVMEChooserTree::mmgVMEChooserTree( wxWindow *parent, mmgCheckTree *tree, ValidateCallBackType vme_accept_function,wxWindowID id, bool CloseButton, bool HideTitle, long style)
:mmgCheckTree(parent, id, CloseButton, HideTitle)
//----------------------------------------------------------------------------
{
  m_ChoosedNode       = NULL;
  m_ChooserTreeStyle  = style;

  if(vme_accept_function == 0)
    m_ValidateFunction = NULL;
  else
    m_ValidateFunction = vme_accept_function;
  
  InitializeImageList();

  CloneSubTree(tree, &tree->GetTree()->GetRootItem(), (wxTreeItemId *)NULL);
}
//----------------------------------------------------------------------------
mmgVMEChooserTree::~mmgVMEChooserTree()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
int mmgVMEChooserTree::GetVmeStatus(mafNode *node)
//----------------------------------------------------------------------------
{
  int image_id;
  if(m_ValidateFunction == NULL)
  {
    if (!node->IsMAFType(mafVMERoot))
    {
      image_id = ClassNameToIcon(node->GetTypeName()) + NODE_VISIBLE_ON;
      return image_id;
    }
  }
  else
  {
    if(m_ValidateFunction(node))
    {
      image_id = ClassNameToIcon(node->GetTypeName()) + NODE_VISIBLE_ON;
      return image_id;
    }
  }

  image_id = ClassNameToIcon(node->GetTypeName()) + NODE_NON_VISIBLE;
  return image_id;
}
//----------------------------------------------------------------------------
void mmgVMEChooserTree::InitializeImageList()
//----------------------------------------------------------------------------
{
  // pourpose:
  // each vme-picture is combined with each state-picture,
  // all the combined picture are inserted in the image-list.
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
    mafLogMessage("mmgVMEChooserTree:  Warning - no vme-icons defined");
    return;
  }

  //retrieve state icons
  wxBitmap state_ico[num_of_status];
  state_ico[NODE_NON_VISIBLE] = mafPics.GetBmp("DISABLED");
  state_ico[NODE_VISIBLE_OFF] = mafPics.GetBmp("DISABLED");
  state_ico[NODE_VISIBLE_ON]  = mafPics.GetBmp("DISABLED");
  state_ico[NODE_MUTEX_OFF]   = mafPics.GetBmp("DISABLED");
  state_ico[NODE_MUTEX_ON]    = mafPics.GetBmp("DISABLED");
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
  //int mw = sw+w; 
  //int mh = (sh>h) ? sh : h;
  wxImageList *imgs = new wxImageList(w,h,FALSE,num_icons);

  for(int i=0; i<num_types; i++)
  {
    wxString name = v[i];
    m_MapClassNameToIcon[name]=i*num_of_status;
    
    for( int s=0; s<num_of_status; s++)
    {
      wxBitmap vmeico = mafPics.GetVmePic(name);
      if(s==0) vmeico = GrayScale(vmeico);
      imgs->Add(vmeico);
    }
  }
  SetImageList(imgs);
}
//----------------------------------------------------------------------------
void mmgVMEChooserTree::OnSelectionChanged(wxTreeEvent& event)
//----------------------------------------------------------------------------
{
  wxTreeItemId i;
  if(m_PreventNotify) return;

  i = event.GetItem();
  if(i.IsOk())
  {
    m_ChoosedNode = (mafNode *)NodeFromItem(i);
  }
  event.Skip();

  int status = GetVmeStatus(m_ChoosedNode);
  mafEventMacro(mafEvent(this,VME_SELECTED,status != NODE_NON_VISIBLE));
}
//----------------------------------------------------------------------------
void mmgVMEChooserTree::OnIconClick(wxTreeItemId item)
//----------------------------------------------------------------------------
{
/*  mafNode* vme = (mafNode*) (NodeFromItem(item));
  int status = GetVmeStatus(vme);

  mafEventMacro(mafEvent(this,VME_SELECTED,status != NODE_NON_VISIBLE));*/
}
//----------------------------------------------------------------------------
void mmgVMEChooserTree::ShowContextualMenu(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgVMEChooserTree::CloneSubTree(mmgCheckTree *source_tree, wxTreeItemId *source_item, wxTreeItemId *dest_parent_item)
//----------------------------------------------------------------------------
{
  wxString  text  = source_tree->GetTree()->GetItemText(*source_item);
  long      node  = source_tree->NodeFromItem(*source_item);
  int       image = GetVmeStatus((mafNode *)node);

  wxTreeItemId current_item;
  bool         expanded = source_tree->GetTree()->IsExpanded(*source_item);

  if (dest_parent_item == NULL)
  {
    m_NodeTree->DeleteAllItems();
    current_item = m_NodeTree->AddRoot(text,image,image, new mmgTreeItemData(node));
  }
  else
  {
    if (m_ChooserTreeStyle == REPRESENTATION_AS_TREE)
    {
      current_item = m_NodeTree->AppendItem(*dest_parent_item,text,image,image, new mmgTreeItemData(node));
    }
    else 
    {
      // Flat tree of acceptable VMEs
      if (image == NODE_VISIBLE_ON)
      {
        m_NodeTree->AppendItem(m_NodeTree->GetRootItem(),text,image,image, new mmgTreeItemData(node));
      }
      current_item = m_NodeTree->GetRootItem();
    }
  }

  wxTreeItemIdValue cookie = 0;
  wxTreeItemId child = source_tree->GetTree()->GetFirstChild(*source_item, cookie);
  while( child.IsOk() )
  {
    CloneSubTree(source_tree, &child, &current_item);
    child = source_tree->GetTree()->GetNextChild(*source_item, cookie); 
  }

  if (m_ChooserTreeStyle == REPRESENTATION_AS_TREE)
  {
    if(expanded) 
      m_NodeTree->Expand(current_item); 
    else 
      m_NodeTree->Collapse(current_item);
  }
  else
  {
    // in flat representation the root's children are sorted
    m_NodeTree->Expand(current_item);
    m_NodeTree->SortChildren(current_item);
  }
}
