/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIVMEChooserTree
 Authors: Paolo Quadrani
 
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


#include "albaGUIVMEChooserTree.h" 
#include "albaDecl.h"
#include "albaGUIDialog.h"
#include "albaPics.h" 
#include "albaEvent.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaView.h"
#include "albaVMERoot.h"
#include "albaVMELandmarkCloud.h"

#include <vector>
#include <algorithm>

//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIVMEChooserTree,wxPanel)
  EVT_TREE_SEL_CHANGED(ID_TREE, albaGUIVMEChooserTree::OnSelectionChanged)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUIVMEChooserTree::albaGUIVMEChooserTree( wxWindow *parent, albaGUICheckTree *tree, ValidateCallBackType vme_accept_function,wxWindowID id, bool CloseButton, bool HideTitle, long style, bool multiSelect, albaVME *subTree)
:albaGUICheckTree(parent, id, CloseButton, HideTitle)
//----------------------------------------------------------------------------
{
  m_ChoosedNode       = NULL;
  m_ChooserTreeStyle  = style;
  m_CheckedNode.clear();

  m_MultipleSelection = multiSelect;
 
  if(vme_accept_function == 0)
    m_ValidateFunction = NULL;
  else
    m_ValidateFunction = vme_accept_function;
  
  if (!multiSelect)
    InitializeImageList();
  else
    albaGUICheckTree::InitializeImageList();

	if (subTree && tree->NodeExist((long long)subTree) )
	{
		wxTreeItemId  item = tree->ItemFromNode((long long)subTree);
		CloneSubTree(tree, &item, (wxTreeItemId *)NULL);
	}
	else
		CloneSubTree(tree, &tree->GetTree()->GetRootItem(), (wxTreeItemId *)NULL);
}
//----------------------------------------------------------------------------
albaGUIVMEChooserTree::~albaGUIVMEChooserTree()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
int albaGUIVMEChooserTree::GetVmeStatus(albaVME *node)
//----------------------------------------------------------------------------
{
  int image_id;
  if(m_ValidateFunction == NULL)
  {
    if (m_MultipleSelection)
    {
      image_id = ClassNameToIcon(node->GetTypeName()) + NODE_VISIBLE_ON;
      return image_id;
    }
    else
    {
      if (!node->IsALBAType(albaVMERoot))
      {
        image_id = ClassNameToIcon(node->GetTypeName()) + NODE_VISIBLE_ON;
        return image_id;
      }
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
std::vector<albaVME*> albaGUIVMEChooserTree::GetChoosedNode()
//----------------------------------------------------------------------------
{
  if (!m_MultipleSelection)
  {
    m_CheckedNode.clear();
    m_CheckedNode.push_back(m_ChoosedNode);
  }
  return m_CheckedNode;
}

//----------------------------------------------------------------------------
void albaGUIVMEChooserTree::InitializeImageList()
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
  int num_icons = num_types * num_of_status;

  if(num_types <= 0)
  {
    albaLogMessage("albaGUIVMEChooserTree:  Warning - no vme-icons defined");
    return;
  }

  //retrieve state icons
  wxBitmap state_ico[num_of_status];
  state_ico[NODE_NON_VISIBLE] = albaPictureFactory::GetPictureFactory()->GetBmp("DISABLED");
  state_ico[NODE_VISIBLE_OFF] = albaPictureFactory::GetPictureFactory()->GetBmp("DISABLED");
  state_ico[NODE_VISIBLE_ON]  = albaPictureFactory::GetPictureFactory()->GetBmp("DISABLED");
  state_ico[NODE_MUTEX_OFF]   = albaPictureFactory::GetPictureFactory()->GetBmp("DISABLED");
  state_ico[NODE_MUTEX_ON]    = albaPictureFactory::GetPictureFactory()->GetBmp("DISABLED");
  int sw = state_ico[0].GetWidth();
  int sh = state_ico[0].GetHeight();

  //get icon size 
  //I assume all vme-icon to have the same size
  wxBitmap bmp = albaPictureFactory::GetPictureFactory()->GetVmePic(v[0]);
  int w = bmp.GetWidth();
  int h = bmp.GetHeight();
  assert(w>0 && h>0);

  // create the ImageList 
  //int mw = sw+w; 
  //int mh = (sh>h) ? sh : h;
  wxImageList *imgs = new wxImageList(w,h,false,num_icons);

  for(int i=0; i<num_types; i++)
  {
    wxString name = v[i];
    m_MapClassNameToIcon[name]=i*num_of_status;

    for( int s=0; s<num_of_status; s++)
    {
      wxBitmap vmeico = albaPictureFactory::GetPictureFactory()->GetVmePic(name);
      if(s==0) vmeico = albaWhiteFade(vmeico);
			else vmeico = albaBlueScale(vmeico);
      imgs->Add(vmeico);
    }
  }
  SetImageList(imgs);
}
//----------------------------------------------------------------------------
void albaGUIVMEChooserTree::OnSelectionChanged(wxTreeEvent& event)
//----------------------------------------------------------------------------
{
  int status = false;
  wxTreeItemId i;
  if(m_PreventNotify) return;

  i = event.GetItem();
  if(i.IsOk())
  {
    m_ChoosedNode = (albaVME *)NodeFromItem(i);
  }
  event.Skip();

  if (m_ChoosedNode != NULL)
  {
     status = GetVmeStatus(m_ChoosedNode);
  }
  
  bool enable_ok = false;
  if (!m_MultipleSelection)
  {
    if (m_ValidateFunction && m_ChoosedNode != NULL)
    {
      enable_ok = m_ValidateFunction(m_ChoosedNode);
    }
    else
    {
      enable_ok = status != NODE_NON_VISIBLE;
    }
    albaEventMacro(albaEvent(this,VME_SELECTED, enable_ok));
  }
}
//----------------------------------------------------------------------------
void albaGUIVMEChooserTree::OnIconClick(wxTreeItemId item)
//----------------------------------------------------------------------------
{
// overrided albaGUICheckTree this version must do nothing
}
//----------------------------------------------------------------------------
void albaGUIVMEChooserTree::ShowContextualMenu(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIVMEChooserTree::CloneSubTree(albaGUICheckTree *source_tree, wxTreeItemId *source_item, wxTreeItemId *dest_parent_item)
//----------------------------------------------------------------------------
{
  wxString  text  = source_tree->GetTree()->GetItemText(*source_item);
  long long     node  = source_tree->NodeFromItem(*source_item);
  int       image = GetVmeStatus((albaVME *)node);

  wxTreeItemId current_item;
  bool         expanded = source_tree->GetTree()->IsExpanded(*source_item);

  if (dest_parent_item == NULL)
  {
    m_NodeTree->DeleteAllItems();
    current_item = m_NodeTree->AddRoot(text,image,image, new albaGUITreeItemData(node));
  }
  else
  {
    if (m_ChooserTreeStyle == REPRESENTATION_AS_TREE)
    {
      current_item = m_NodeTree->AppendItem(*dest_parent_item,text,image,image, new albaGUITreeItemData(node));
    }
    else 
    {
      // Flat tree of acceptable VMEs
      if (image == NODE_VISIBLE_ON)
      {
        m_NodeTree->AppendItem(m_NodeTree->GetRootItem(),text,image,image, new albaGUITreeItemData(node));
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
  albaGUITreeTableElement *el = new albaGUITreeTableElement( current_item );  
  m_NodeTable->Put(node, el);
}
