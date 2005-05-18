/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgVMEChooserTree.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-18 16:15:55 $
  Version:   $Revision: 1.4 $
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
#include "mmgVMEChooserAccept.h"
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
mmgVMEChooserTree::mmgVMEChooserTree( wxWindow *parent, mmgCheckTree *tree, long vme_accept_function,wxWindowID id, bool CloseButton, bool HideTitle)
:mmgCheckTree(parent, id, CloseButton, HideTitle)
//----------------------------------------------------------------------------
{
  m_ChoosedNode = NULL;

  if(vme_accept_function == 0)
    m_AcceptFunction = NULL;
  else
    m_AcceptFunction = (mmgVmeChooserAccept *)vme_accept_function;
  
  InitializeImageList();

  tree->FillTree(this);
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
  if(m_AcceptFunction == NULL)
  {
    if (!node->IsMAFType(mafVMERoot))
    {
      return NODE_VISIBLE_ON;
    }
  }
  else
  {
    if(m_AcceptFunction->Validate(node))
      return NODE_VISIBLE_ON;
  }

  return NODE_NON_VISIBLE;
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
    wxLogMessage("mmgVMEChooserTree:  Warning - no vme-icons defined");
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
  if(m_prevent_notify) return;

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