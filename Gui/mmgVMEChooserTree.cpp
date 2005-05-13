/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgVMEChooserTree.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-13 16:15:38 $
  Version:   $Revision: 1.2 $
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
#include "mafPics.h" 
#include "mafNode.h"
#include "mafView.h"

#include <vector>

//----------------------------------------------------------------------------
mmgVMEChooserTree::mmgVMEChooserTree( wxWindow* parent,wxWindowID id, bool CloseButton, bool HideTitle, long vme_accept_function)
:mmgCheckTree(parent, id, CloseButton, HideTitle)
//----------------------------------------------------------------------------
{
  if(vme_accept_function == 0)
    m_AcceptFunction = NULL;
  else
    m_AcceptFunction = (mmgVMEChooserAccept *)vme_accept_function;
}
//----------------------------------------------------------------------------
mmgVMEChooserTree::~mmgVMEChooserTree()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgVMEChooserTree::FillTree(mmgCheckTree *tree)
//----------------------------------------------------------------------------
{
/*  mafVMEIterator *iter=vme_in->NewIterator();
  for(mafVME *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
  {
    if (vme->GetTagArray()->FindTag("VISIBLE_IN_THE_TREE") == -1 || (vme->GetTagArray()->GetTag("VISIBLE_IN_THE_TREE")->GetValueAsDouble() == 1.0))
    {
      if(m_AcceptFunction == NULL)
        m_Tree->AddNode((long)vme,(long)vme->GetParent(),vme->GetName(),NODE_RED);
      else
        if(m_AcceptFunction->Validate(vme))
          m_Tree->AddNode((long)vme,(long)vme->GetParent(),vme->GetName(),NODE_RED);
        else
          m_Tree->AddNode((long)vme,(long)vme->GetParent(),vme->GetName());
    }
  }
  iter->Delete();*/
}
//----------------------------------------------------------------------------
void mmgVMEChooserTree::OnIconClick(wxTreeItemId item)
//----------------------------------------------------------------------------
{
  mafNode* vme = (mafNode*) (NodeFromItem(item));
  int status = GetVmeStatus(vme); 
}
//----------------------------------------------------------------------------
int mmgVMEChooserTree::GetVmeStatus(mafNode *node)
//----------------------------------------------------------------------------
{
  
  return 1;
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
  state_ico[NODE_VISIBLE_OFF] = mafPics.GetBmp("CHECK_ON");
  state_ico[NODE_VISIBLE_ON]  = mafPics.GetBmp("CHECK_ON");
  state_ico[NODE_MUTEX_OFF]   = mafPics.GetBmp("CHECK_ON");
  state_ico[NODE_MUTEX_ON]    = mafPics.GetBmp("CHECK_ON");
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
      imgs->Add(vmeico);
    }
  }
  SetImageList(imgs);
}
//----------------------------------------------------------------------------
void mmgVMEChooserTree::OnSelectionChanged(wxTreeEvent& event)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgVMEChooserTree::ShowContextualMenu(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
}