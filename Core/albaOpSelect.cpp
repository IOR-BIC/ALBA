/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSelect
 Authors: Silvano Imboden, Gianluigi Crimi
 
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


#include "albaOpSelect.h"
#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaString.h"
#ifdef ALBA_USE_VTK
  #include "albaVMELandmarkCloud.h"
#endif
#include "vtkDataSet.h"
#include "albaOpReparentTo.h"

static albaVME *glo_Clipboard = NULL;
static	albaVME *glo_SelectionParent=NULL;

//////////////////
// albaOpSelect ://
//////////////////

albaCxxTypeMacro(albaOpSelect);

//----------------------------------------------------------------------------
albaOpSelect::albaOpSelect(wxString label) 
//----------------------------------------------------------------------------
{
  m_Canundo = true; 
  m_OpType  = OPTYPE_EDIT; 
  m_Label   = label;
  m_NewNodeSelected = NULL;
  m_OldNodeSelected = NULL;
}
//----------------------------------------------------------------------------
albaOpSelect::~albaOpSelect()
//----------------------------------------------------------------------------
{
} 
//----------------------------------------------------------------------------
albaOp* albaOpSelect::Copy() 
//----------------------------------------------------------------------------
{
  albaOpSelect *cp  = new albaOpSelect();
  cp->m_OldNodeSelected  = m_OldNodeSelected;
  cp->m_NewNodeSelected  = m_NewNodeSelected;
  return cp;
}

//----------------------------------------------------------------------------
void albaOpSelect::OpRun()
{
	OpStop(OP_RUN_OK);
}

//----------------------------------------------------------------------------
bool albaOpSelect::Accept(albaVME* vme)     
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
void albaOpSelect::SetInput(albaVME* vme)   
//----------------------------------------------------------------------------
{
  m_OldNodeSelected = vme;
}
//----------------------------------------------------------------------------
void albaOpSelect::SetNewSel(albaVME* vme)  
//----------------------------------------------------------------------------
{
  m_NewNodeSelected = vme;
}
//----------------------------------------------------------------------------
void albaOpSelect::OpDo()
//----------------------------------------------------------------------------
{
  albaEventMacro(albaEvent(this,VME_SELECTED,m_NewNodeSelected));
};
//----------------------------------------------------------------------------
void albaOpSelect::OpUndo()
//----------------------------------------------------------------------------
{
  albaEventMacro(albaEvent(this,VME_SELECTED,m_OldNodeSelected));
};


////////////////
// albaOpEdit: //
////////////////
//----------------------------------------------------------------------------
albaOpEdit::albaOpEdit(wxString label)
: m_Backup(NULL)
//----------------------------------------------------------------------------
{
  m_Canundo = true; 
  m_OpType = OPTYPE_EDIT; 
  m_Selection = NULL; 
}
//----------------------------------------------------------------------------
albaOpEdit::~albaOpEdit()
//----------------------------------------------------------------------------
{
} 
//----------------------------------------------------------------------------
bool albaOpEdit::ClipboardIsEmpty()
//----------------------------------------------------------------------------
{
  return GetClipboard() == NULL;
}
//----------------------------------------------------------------------------
void albaOpEdit::ClipboardClear()
//----------------------------------------------------------------------------
{
  SetClipboard(NULL);
}
//----------------------------------------------------------------------------
void albaOpEdit::ClipboardBackup()
//----------------------------------------------------------------------------
{
  assert(m_Backup.GetPointer() == NULL);
  m_Backup = GetClipboard();
  SetClipboard(NULL);
}
//----------------------------------------------------------------------------
void albaOpEdit::ClipboardRestore()
//----------------------------------------------------------------------------
{
  //no assert here, I may make a backup of an empty clipboard
  SetClipboard(m_Backup);
  m_Backup = NULL;
}
//----------------------------------------------------------------------------
albaVME* albaOpEdit::GetClipboard()
//----------------------------------------------------------------------------
{
  return glo_Clipboard;
}
//----------------------------------------------------------------------------
void albaOpEdit::SetClipboard(albaVME *node)
//----------------------------------------------------------------------------
{
  glo_Clipboard = node;
}

//----------------------------------------------------------------------------
void albaOpEdit::SetSelectionParent(albaVME *parent)
{
	glo_SelectionParent = parent;
}

//----------------------------------------------------------------------------
void albaOpEdit::OpRun()
{
	OpStop(OP_RUN_OK);
}

//----------------------------------------------------------------------------
void albaOpEdit::RemoveBackLinksForTheSubTree(albaVME *vme)
{
	albaVMEIterator *iter = vme->NewIterator();
	for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		albaVME::albaLinksMap *links = node->GetLinks();
		for (albaVME::albaLinksMap::iterator it = links->begin(); it != links->end(); it++)
		{
			albaVMELink &link = it->second;
			if (link.m_Type == MANDATORY_LINK)
			{
				link.m_Node->RemoveBackLink(it->first, node);
			}
		}
	}
}

//----------------------------------------------------------------------------
void albaOpEdit::RestoreBackLinksForTheSubTree(albaVME *vme)
{
	albaVMEIterator *iter = vme->NewIterator();
	for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		albaVME::albaLinksMap *links = node->GetLinks();
		for (albaVME::albaLinksMap::iterator it = links->begin(); it != links->end(); it++)
		{
			albaVMELink &link = it->second;
			if (link.m_Type == MANDATORY_LINK)
			{
				link.m_Node->AddBackLink(it->first, node);
			}
		}
	}
}

//----------------------------------------------------------------------------
bool albaOpEdit::CanRestoreBackLinksForTheSubTree(albaVME *vme, albaVME *root)
{
	albaVMEIterator *iter = vme->NewIterator();
	for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		albaVME::albaLinksMap *links = node->GetLinks();
		for (albaVME::albaLinksMap::iterator it = links->begin(); it != links->end(); it++)
		{
			albaVMELink &link = it->second;
			if (link.m_Type == MANDATORY_LINK && root->FindInTreeById(link.m_NodeId) == NULL)
			{
				return false;
			}
		}
	}

	return true;
}

///////////////
// albaOpCut ://
///////////////
//----------------------------------------------------------------------------
albaOpCut::albaOpCut(wxString label) 
//----------------------------------------------------------------------------
{
  m_Label=label;
  
}
//----------------------------------------------------------------------------
albaOpCut::~albaOpCut() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* albaOpCut::Copy() 
//----------------------------------------------------------------------------
{
  return new albaOpCut(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCut::Accept(albaVME* vme)
//----------------------------------------------------------------------------
{
  return ((vme!=NULL) && (!vme->IsALBAType(albaVMERoot)));
}
//----------------------------------------------------------------------------
void albaOpCut::OpDo()
//----------------------------------------------------------------------------
/**
backup the clipboard
Move (doesn't make a copy) the selected vme (and it's subtree) in the Clipboard
Select the vme parent
*/
{

	albaVME::albaVMESet dependenciesVMEs = m_Selection->GetDependenciesVMEs();

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
		albaVME *m_SelectionVme = m_Selection;
		if (m_SelectionVme)
			LoadChild(m_SelectionVme);
		//////////////////////////////////////////////////////////////////////////

		GetLogicManager()->VmeRemove(m_Selection);
		//albaEventMacro(albaEvent(this, VME_SELECTED, glo_SelectionParent));

		glo_SelectionParent->Update();

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
void albaOpCut::LoadVTKData(albaVME *vme)
//----------------------------------------------------------------------------
{
  // Added by Losi on 03.06.2010
  if (vme)
  {
    albaTimeStamp oldTime = vme->GetTimeStamp();

    std::vector<albaTimeStamp> subKFrames;
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
void albaOpCut::LoadChild(albaVME *vme)
//----------------------------------------------------------------------------
{
  // Added by Di Cosmo on 24.05.2012
  // it needs load vtk data for all objects in the tree
  const albaVME::albaChildrenVector *children = vme->GetChildren();
  if (children)
  {
    for(int c = 0; c < children->size(); c++)
    {
      albaVME *child = children->at(c);
      LoadVTKData(child);
      LoadChild(child);
    }
  }
}

//----------------------------------------------------------------------------
void albaOpCut::OpUndo()
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

		glo_SelectionParent->Update();

		SetSelectionParent(m_SelectionParentBackup);

		albaEventMacro(albaEvent(this, VME_SELECTED, m_Selection));
		ClipboardRestore();
	}
}



////////////////
// albaOpCopy ://
////////////////
//----------------------------------------------------------------------------
albaOpCopy::albaOpCopy(wxString label) 
//----------------------------------------------------------------------------
{
  m_Label=label;
}
//----------------------------------------------------------------------------
albaOpCopy::~albaOpCopy()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* albaOpCopy::Copy()
//----------------------------------------------------------------------------
{
  return new albaOpCopy(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCopy::Accept(albaVME* vme)
//----------------------------------------------------------------------------
{
  bool res = (vme!=NULL) && (!vme->IsALBAType(albaVMERoot));
  if(GetClipboard() != NULL)
    res = res && GetClipboard()->CanCopy(vme);
  return res;
}
//----------------------------------------------------------------------------
void albaOpCopy::OpDo()
//----------------------------------------------------------------------------
/**
make the clipboard backup
copy the selected VME and its subtree into the clipboard
*/
{
  ClipboardBackup();
	SetSelectionParent(m_Selection->GetParent());
  SetClipboard(m_Selection->CopyTree());
  albaString copy_name;
  copy_name = "copy of ";
  copy_name += GetClipboard()->GetName();
  GetClipboard()->SetName(copy_name.GetCStr());
	RemoveBackLinksForTheSubTree(GetClipboard());
}


//----------------------------------------------------------------------------
void albaOpCopy::OpUndo()
//----------------------------------------------------------------------------
/**
destroy current clipboard
restore previous clipboard
*/
{
  ClipboardRestore();
}

/////////////////
// albaOpPaste ://
/////////////////
//----------------------------------------------------------------------------
albaOpPaste::albaOpPaste(wxString label) 
//----------------------------------------------------------------------------
{
  m_Label=label;
  m_PastedVme = NULL; 
}
//----------------------------------------------------------------------------
albaOp* albaOpPaste::Copy() 
//----------------------------------------------------------------------------
{                    
  return new albaOpPaste(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpPaste::Accept(albaVME* vme)       
//----------------------------------------------------------------------------
{
  //paste may be executed if
  // - vme is not NULL
  // - the clipboard is not empty
  // - THE vme on the clipboard can be re-parented under vme
  // - (this cover also restrictions imposed on Landmarks)

  if(ClipboardIsEmpty()) return false;
  if(vme == NULL) return false;
  albaVME *cv = GetClipboard();
  return cv->CanReparentTo(vme);
};
//----------------------------------------------------------------------------
void albaOpPaste::OpDo()                    
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
		albaOpReparentTo::ReparentTo(m_PastedVme, m_Selection, glo_SelectionParent);
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
void albaOpPaste::OpUndo()                  
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


//////////////////////////////////////////////////////////////////////////
// albaOpRename
//////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
albaOpRename::albaOpRename(wxString label)
{
	m_Label = label;
	m_OldName = "";
}
//----------------------------------------------------------------------------
albaOp* albaOpRename::Copy()
{
	return new albaOpRename(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpRename::Accept(albaVME* vme)
{
	return (vme != NULL) && (!vme->IsALBAType(albaVMERoot));
};
//----------------------------------------------------------------------------
void albaOpRename::OpDo()
{
	wxTextEntryDialog *dlg = new wxTextEntryDialog(NULL, "Name", "Rename", m_Selection->GetName());
	int result = dlg->ShowModal();
	wxString stringValue = dlg->GetValue();
	cppDEL(dlg);

	if (result == wxID_OK && !stringValue.IsEmpty())
	{
		m_OldName = m_Selection->GetName();

		m_Selection->SetName(stringValue);
		m_Renamed = true;
	}
	else
	{
		m_Renamed = false;
	}
}

//----------------------------------------------------------------------------
void albaOpRename::OpUndo()
{
	if (m_Renamed)
	{
		m_Selection->SetName(m_OldName);
	}
}
