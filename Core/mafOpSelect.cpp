/*=========================================================================

 Program: MAF2
 Module: mafOpSelect
 Authors: Silvano Imboden, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOpSelect.h"
#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafString.h"
#ifdef MAF_USE_VTK
  #include "mafVMELandmarkCloud.h"
#endif
#include "vtkDataSet.h"
#include "mafOpReparentTo.h"

static mafAutoPointer<mafVME> glo_Clipboard = NULL;
static	mafAutoPointer<mafVME> glo_SelectionParent=NULL;

//////////////////
// mafOpSelect ://
//////////////////
//----------------------------------------------------------------------------
mafOpSelect::mafOpSelect(wxString label) 
//----------------------------------------------------------------------------
{
  m_Canundo = true; 
  m_OpType  = OPTYPE_EDIT; 
  m_Label   = label;
  m_NewNodeSelected = NULL;
  m_OldNodeSelected = NULL;
}
//----------------------------------------------------------------------------
mafOpSelect::~mafOpSelect()
//----------------------------------------------------------------------------
{
} 
//----------------------------------------------------------------------------
mafOp* mafOpSelect::Copy() 
//----------------------------------------------------------------------------
{
  mafOpSelect *cp  = new mafOpSelect();
  cp->m_OldNodeSelected  = m_OldNodeSelected;
  cp->m_NewNodeSelected  = m_NewNodeSelected;
  return cp;
}
//----------------------------------------------------------------------------
bool mafOpSelect::Accept(mafVME* vme)     
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
void mafOpSelect::SetInput(mafVME* vme)   
//----------------------------------------------------------------------------
{
  m_OldNodeSelected = vme;
}
//----------------------------------------------------------------------------
void mafOpSelect::SetNewSel(mafVME* vme)  
//----------------------------------------------------------------------------
{
  m_NewNodeSelected = vme;
}
//----------------------------------------------------------------------------
void mafOpSelect::OpDo()
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,VME_SELECTED,m_NewNodeSelected));
};
//----------------------------------------------------------------------------
void mafOpSelect::OpUndo()
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,VME_SELECTED,m_OldNodeSelected));
};


////////////////
// mafOpEdit: //
////////////////
//----------------------------------------------------------------------------
mafOpEdit::mafOpEdit(wxString label)
: m_Backup(NULL)
//----------------------------------------------------------------------------
{
  m_Canundo = true; 
  m_OpType = OPTYPE_EDIT; 
  m_Selection = NULL; 
}
//----------------------------------------------------------------------------
mafOpEdit::~mafOpEdit()
//----------------------------------------------------------------------------
{
} 
//----------------------------------------------------------------------------
bool mafOpEdit::ClipboardIsEmpty()
//----------------------------------------------------------------------------
{
  return GetClipboard() == NULL;
}
//----------------------------------------------------------------------------
void mafOpEdit::ClipboardClear()
//----------------------------------------------------------------------------
{
  SetClipboard(NULL);
}
//----------------------------------------------------------------------------
void mafOpEdit::ClipboardBackup()
//----------------------------------------------------------------------------
{
  assert(m_Backup.GetPointer() == NULL);
  m_Backup = GetClipboard();
  SetClipboard(NULL);
}
//----------------------------------------------------------------------------
void mafOpEdit::ClipboardRestore()
//----------------------------------------------------------------------------
{
  //no assert here, I may make a backup of an empty clipboard
  SetClipboard(m_Backup);
  m_Backup = NULL;
}
//----------------------------------------------------------------------------
mafVME* mafOpEdit::GetClipboard()
//----------------------------------------------------------------------------
{
  return glo_Clipboard;
}
//----------------------------------------------------------------------------
void mafOpEdit::SetClipboard(mafVME *node)
//----------------------------------------------------------------------------
{
  glo_Clipboard = node;
}

//----------------------------------------------------------------------------
void mafOpEdit::SetSelectionParent(mafVME *parent)
{
	glo_SelectionParent = parent;
}

//----------------------------------------------------------------------------
void mafOpEdit::RemoveBackLinksForTheSubTree(mafVME *vme)
{
	mafVMEIterator *iter = vme->NewIterator();
	for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		mafVME::mafLinksMap *links = node->GetLinks();
		for (mafVME::mafLinksMap::iterator it = links->begin(); it != links->end(); it++)
		{
			mafVMELink &link = it->second;
			if (link.m_Type == MANDATORY_LINK)
			{
				link.m_Node->RemoveBackLink(it->first, node);
			}
		}
	}
}

//----------------------------------------------------------------------------
void mafOpEdit::RestoreBackLinksForTheSubTree(mafVME *vme)
{
	mafVMEIterator *iter = vme->NewIterator();
	for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		mafVME::mafLinksMap *links = node->GetLinks();
		for (mafVME::mafLinksMap::iterator it = links->begin(); it != links->end(); it++)
		{
			mafVMELink &link = it->second;
			if (link.m_Type == MANDATORY_LINK)
			{
				link.m_Node->AddBackLink(it->first, node);
			}
		}
	}
}

//----------------------------------------------------------------------------
bool mafOpEdit::CanRestoreBackLinksForTheSubTree(mafVME *vme, mafVME *root)
{
	mafVMEIterator *iter = vme->NewIterator();
	for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		mafVME::mafLinksMap *links = node->GetLinks();
		for (mafVME::mafLinksMap::iterator it = links->begin(); it != links->end(); it++)
		{
			mafVMELink &link = it->second;
			if (link.m_Type == MANDATORY_LINK && root->FindInTreeById(link.m_NodeId) == NULL)
			{
				return false;
			}
		}
	}

	return true;
}

///////////////
// mafOpCut ://
///////////////
//----------------------------------------------------------------------------
mafOpCut::mafOpCut(wxString label) 
//----------------------------------------------------------------------------
{
  m_Label=label;
  
}
//----------------------------------------------------------------------------
mafOpCut::~mafOpCut() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCut::Copy() 
//----------------------------------------------------------------------------
{
  return new mafOpCut(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpCut::Accept(mafVME* vme)
//----------------------------------------------------------------------------
{
  return ((vme!=NULL) && (!vme->IsMAFType(mafVMERoot)));
}
//----------------------------------------------------------------------------
void mafOpCut::OpDo()
//----------------------------------------------------------------------------
/**
backup the clipboard
Move (doesn't make a copy) the selected vme (and it's subtree) in the Clipboard
Select the vme parent
*/
{

	mafVME::mafVMESet dependenciesVMEs = m_Selection->GetDependenciesVMEs();

	//Assure there is no VME depending on this
	if (dependenciesVMEs.empty())
	{
		m_Cutted = true;

		ClipboardBackup();
		m_SelectionParentBackup = glo_SelectionParent;
		SetSelectionParent(m_Selection->GetParent());

		SetClipboard(m_Selection);

		//////////////////////////////////////////////////////////////////////////
		// It is necessary load all vtk data of the vme time varying otherwise paste or undo cause an application crash
		//////////////////////////////////////////////////////////////////////////
		LoadVTKData(m_Selection);

		//////////////////////////////////////////////////////////////////////////
		// It is necessary to load all vtk data of children vme otherwise paste or undo cause an application crash
		//////////////////////////////////////////////////////////////////////////
		mafVME *m_SelectionVme = m_Selection;
		if (m_SelectionVme)
			LoadChild(m_SelectionVme);
		//////////////////////////////////////////////////////////////////////////

		GetLogicManager()->VmeRemove(m_Selection);
		mafEventMacro(mafEvent(this, VME_SELECTED, glo_SelectionParent));

		glo_SelectionParent.GetPointer()->GetOutput()->Update();

		RemoveBackLinksForTheSubTree(m_Selection);
	}
	else
	{
		wxString message;
		message << "You cannot cut this VME. There are some VMEs depending on this.\n";
		message << "\n\n You can delete or reparent this VME.";
		wxMessageBox(message, "Cannot Cut", wxOK | wxICON_WARNING | wxCENTRE | wxSTAY_ON_TOP);

		m_Cutted = false;
	}
}
//----------------------------------------------------------------------------
void mafOpCut::LoadVTKData(mafVME *vme)
//----------------------------------------------------------------------------
{
  // Added by Losi on 03.06.2010
  if (vme)
  {
    mafTimeStamp oldTime = vme->GetTimeStamp();

    std::vector<mafTimeStamp> subKFrames;
    vme->GetTimeStamps(subKFrames);
    for (int i=0;i<subKFrames.size();i++)
    {
      vme->SetTimeStamp(subKFrames[i]);
      vme->GetOutput()->GetVTKData();
    }

    vme->SetTimeStamp(oldTime);
  }
}
//----------------------------------------------------------------------------
void mafOpCut::LoadChild(mafVME *vme)
//----------------------------------------------------------------------------
{
  // Added by Di Cosmo on 24.05.2012
  // it needs load vtk data for all objects in the tree
  const mafVME::mafChildrenVector *children = vme->GetChildren();
  if (children)
  {
    for(int c = 0; c < children->size(); c++)
    {
      mafVME *child = children->at(c);
      LoadVTKData(child);
      LoadChild(child);
    }
  }
}

//----------------------------------------------------------------------------
void mafOpCut::OpUndo()
//----------------------------------------------------------------------------
/**
Move the vme in the Clipboard under it's old parent
Restore the Clipboard
Restore the Selection
*/
{
	if (m_Cutted)
	{
		m_Selection = GetClipboard();

		m_Selection->ReparentTo(glo_SelectionParent);

		RestoreBackLinksForTheSubTree(m_Selection);

		glo_SelectionParent.GetPointer()->GetOutput()->Update();

		SetSelectionParent(m_SelectionParentBackup);

		mafEventMacro(mafEvent(this, VME_SELECTED, m_Selection));
		ClipboardRestore();
	}
}



////////////////
// mafOpCopy ://
////////////////
//----------------------------------------------------------------------------
mafOpCopy::mafOpCopy(wxString label) 
//----------------------------------------------------------------------------
{
  m_Label=label;
}
//----------------------------------------------------------------------------
mafOpCopy::~mafOpCopy()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCopy::Copy()
//----------------------------------------------------------------------------
{
  return new mafOpCopy(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpCopy::Accept(mafVME* vme)
//----------------------------------------------------------------------------
{
  bool res = (vme!=NULL) && (!vme->IsMAFType(mafVMERoot));
  if(GetClipboard() != NULL)
    res = res && GetClipboard()->CanCopy(vme);
  return res;
}
//----------------------------------------------------------------------------
void mafOpCopy::OpDo()
//----------------------------------------------------------------------------
/**
make the clipboard backup
copy the selected VME and its subtree into the clipboard
*/
{
  ClipboardBackup();
	SetSelectionParent(m_Selection->GetParent());
  SetClipboard(m_Selection->CopyTree());
  mafString copy_name;
  copy_name = "copy of ";
  copy_name += GetClipboard()->GetName();
  GetClipboard()->SetName(copy_name.GetCStr());
	RemoveBackLinksForTheSubTree(GetClipboard());
}
//----------------------------------------------------------------------------
void mafOpCopy::OpUndo()
//----------------------------------------------------------------------------
/**
destroy current clipboard
restore previous clipboard
*/
{
  ClipboardRestore();
}



/////////////////
// mafOpPaste ://
/////////////////
//----------------------------------------------------------------------------
mafOpPaste::mafOpPaste(wxString label) 
//----------------------------------------------------------------------------
{
  m_Label=label;
  m_PastedVme = NULL; 
}
//----------------------------------------------------------------------------
mafOp* mafOpPaste::Copy() 
//----------------------------------------------------------------------------
{                    
  return new mafOpPaste(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpPaste::Accept(mafVME* vme)       
//----------------------------------------------------------------------------
{
  //paste may be executed if
  // - vme is not NULL
  // - the clipboard is not empty
  // - THE vme on the clipboard can be re-parented under vme
  // - (this cover also restrictions imposed on Landmarks)

  if(ClipboardIsEmpty()) return false;
  if(vme == NULL) return false;
  mafVME *cv = GetClipboard();
  return cv->CanReparentTo(vme);
};
//----------------------------------------------------------------------------
void mafOpPaste::OpDo()                    
//----------------------------------------------------------------------------
/**
We want that Cut+Paste doesn't change the identity of a vme,so the implementation
of Paste is a little different than usual.

Paste make a copy of the object on the clipboard,
but place in the scene the original and keep the copy in the clipboard.

Them a vme is added, selection is not changed
*/
{
	m_PastedVme = GetClipboard();
	if (CanRestoreBackLinksForTheSubTree(m_PastedVme, m_Selection->GetRoot()))
	{
		mafOpReparentTo::ReparentTo(m_PastedVme, m_Selection, glo_SelectionParent);
		RestoreBackLinksForTheSubTree(m_PastedVme);
		SetClipboard(m_PastedVme->CopyTree());
		RemoveBackLinksForTheSubTree(GetClipboard());
		m_Pasted = true;
	}
	else
	{
		wxString message;
		message << "The clipporad contains some VME that depends on deleted VME\n";
		message << "Paste Operation cannot be executed.";
		wxMessageBox(message, "Cannot Paste", wxOK | wxICON_WARNING | wxCENTRE | wxSTAY_ON_TOP);

		m_Pasted = false;
	}
}
//----------------------------------------------------------------------------
void mafOpPaste::OpUndo()                  
//----------------------------------------------------------------------------
/**
Remove the pasted vme from the scene and place it in the clipboard.
The copy in the clipboard will be automatically deleted
*/
{
	if (m_Pasted)
	{
		SetClipboard(m_PastedVme);
		GetLogicManager()->VmeRemove(m_PastedVme);
	}
}
