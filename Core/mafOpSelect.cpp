/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpSelect.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-26 11:08:35 $
  Version:   $Revision: 1.2 $
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


#include "mafOpSelect.h"
#include "mafNode.h"
#include "mafNodeRoot.h"
#include "vtkMatrix4x4.h"
#include "vtkMath.h"

//initialize the Clipboard
mafAutoPointer<mafNode>  mafOpEdit::m_clipboard(NULL); 

//////////////////
// mafOpSelect ://
//////////////////
//----------------------------------------------------------------------------
mafOpSelect::mafOpSelect(wxString label) 
{
	m_Canundo = true; 
	m_OpType = OPTYPE_EDIT; 
	m_Label=label;
}
//----------------------------------------------------------------------------
mafOpSelect::~mafOpSelect()
{
} 
//----------------------------------------------------------------------------
mafOp* mafOpSelect::Copy() 
{
	mafOpSelect *cp  = new mafOpSelect();
	cp->m_old_sel  = m_old_sel;
	cp->m_new_sel  = m_new_sel;
	return cp;
}
//----------------------------------------------------------------------------
bool mafOpSelect::Accept(mafNode* vme)     
{
	return true;
}
//----------------------------------------------------------------------------
void mafOpSelect::SetInput(mafNode* vme)   
{
	m_old_sel = vme;
}
//----------------------------------------------------------------------------
void mafOpSelect::SetNewSel(mafNode* vme)  
{
	m_new_sel = vme;
}
//----------------------------------------------------------------------------
void mafOpSelect::OpDo()
{
  mafEventMacro(mafEvent(this,VME_SELECTED,m_new_sel));
};
//----------------------------------------------------------------------------
void mafOpSelect::OpUndo()
{
  mafEventMacro(mafEvent(this,VME_SELECTED,m_old_sel));
};


////////////////
// mafOpEdit: //
////////////////
//----------------------------------------------------------------------------
mafOpEdit::mafOpEdit(wxString label)
: m_backup(NULL)
{
	m_Canundo = true; 
	m_OpType = OPTYPE_EDIT; 
	m_selection = NULL; 
}
//----------------------------------------------------------------------------
mafOpEdit::~mafOpEdit()
{
} 
//----------------------------------------------------------------------------
bool mafOpEdit::ClipboardIsEmpty()
{
  return m_clipboard.GetPointer() == NULL;
}
//----------------------------------------------------------------------------
void mafOpEdit::ClipboardClear()
{
  m_clipboard = NULL;
}
//----------------------------------------------------------------------------
void mafOpEdit::ClipboardBackup()
{
  assert(m_backup.GetPointer() == NULL);
	m_backup = m_clipboard;
  m_clipboard = NULL;
}
//----------------------------------------------------------------------------
void mafOpEdit::ClipboardRestore()
{
  //assert(m_backup.GetPointer() ); - //SIL. 6-11-2003: assert removed, I may make a backup of an empy clipboard
	m_clipboard = m_backup;
  m_backup = NULL;
}

///////////////
// mafOpCut ://
///////////////
//----------------------------------------------------------------------------
mafOpCut::mafOpCut(wxString label) 
{
  m_Label=label;
  m_selection_parent = NULL; 
}
//----------------------------------------------------------------------------
mafOpCut::~mafOpCut() 
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCut::Copy() 
{
	return new mafOpCut(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpCut::Accept(mafNode* vme)
{
  return ((vme!=NULL) && (!vme->IsA("mafNodeRoot")));
}
//----------------------------------------------------------------------------
void mafOpCut::OpDo()
/**
backup the clipboard
Send a VME_REMOVE for the selected vme
Move (doesn't make a copy) the selected vme (and it's subtree) in the Clipboard
Select the vme parent
*/
{
	ClipboardBackup();
	m_selection_parent = m_selection->GetParent(); 
	m_clipboard = m_selection;
	mafEventMacro(mafEvent(this,VME_REMOVE,m_selection));
	mafEventMacro(mafEvent(this,VME_SELECTED,m_selection_parent));
}
//----------------------------------------------------------------------------
void mafOpCut::OpUndo()
/**
Move the vme in the Clipboard under it's old parent
send a VME_ADD
Restore the Clipboard
Restore the Selection
*/
{
	m_selection = m_clipboard.GetPointer();
	
	m_selection->ReparentTo(m_selection_parent);
	mafEventMacro(mafEvent(this,VME_ADD,m_selection));
	mafEventMacro(mafEvent(this,VME_SELECTED,m_selection));
	ClipboardRestore();
}



////////////////
// mafOpCopy ://
////////////////
//----------------------------------------------------------------------------
mafOpCopy::mafOpCopy(wxString label) 
{
  m_Label=label;
}
//----------------------------------------------------------------------------
mafOpCopy::~mafOpCopy()
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCopy::Copy()
{
	return new mafOpCopy(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpCopy::Accept(mafNode* vme)
{
  return ((vme!=NULL) && (!vme->IsA("mafNodeRoot")));
}
//----------------------------------------------------------------------------
void mafOpCopy::OpDo()
/**
fa il backup della clipboard
copia nella clipboard il vme selezionato (e tutto il subtree),
*/
{
	ClipboardBackup();
  m_clipboard = m_selection->CopyTree();
}
//----------------------------------------------------------------------------
void mafOpCopy::OpUndo()
/**
distruggo la clipboard corrente
ripristino la clipboard precedente
*/
{
  ClipboardRestore();
}



/////////////////
// mafOpPaste ://
/////////////////
//----------------------------------------------------------------------------
mafOpPaste::mafOpPaste(wxString label) 
{
  m_Label=label;
  m_pasted_vme = NULL; 
}
//----------------------------------------------------------------------------
mafOp* mafOpPaste::Copy() 
{                    
	return new mafOpPaste(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpPaste::Accept(mafNode* vme)       
{
	//paste may be executed if
	// - vme is not NULL
	// - the clipboard is not empty
	// - THE vme on the clipboard can be reparented under vme
	// - (this cover also restrictions imposed on Landmarks)

  if(ClipboardIsEmpty()) return false;
	if(vme == NULL) return false;
	mafNode *cv = m_clipboard.GetPointer();
  return cv->CanReparentTo(vme) == VTK_OK; //SIL. 28-11-2003:, Thanks To Marco
};
//----------------------------------------------------------------------------
void mafOpPaste::OpDo()                    
/**
We want that Cut+Paste doesn't change the identity of a vme,so the implementaiton
of Paste is a little different than usual.

Paste make a copy of the object on the clipboard,
but place in the scene the original and keep the copy in the clipboard.

Them a VME_ADD is sent, selection is not changed
*/
{
  m_pasted_vme = m_clipboard.GetPointer(); 
	m_pasted_vme->ReparentTo(m_selection);
	mafEventMacro(mafEvent(this,VME_ADD,m_pasted_vme));
	m_clipboard = m_pasted_vme->CopyTree(); 
}
//----------------------------------------------------------------------------
void mafOpPaste::OpUndo()                  
/**
Remove the pasted vme from the scene and place it in the clipboard.
The copy in the clipboard will be automatically deleted
*/
{
  m_clipboard = m_pasted_vme;
  mafEventMacro(mafEvent(this,VME_REMOVE,m_pasted_vme));
}

/*
/////////////////////
// mafOpTransform ://
/////////////////////
//----------------------------------------------------------------------------
mafOpTransform::mafOpTransform(wxString label) 
{
  assert(false); //SIL. 9-4-2005: 
  m_canundo = true; 
  m_optype = OPTYPE_EDIT;     	
  m_label=label;
  m_vme = NULL;
  m_old_matrix = vtkMatrix4x4::New();
  m_new_matrix = vtkMatrix4x4::New();
}
//----------------------------------------------------------------------------
mafOpTransform::~mafOpTransform() 
{
  m_old_matrix->Delete();
  m_new_matrix->Delete();
}
//----------------------------------------------------------------------------
mafOp* mafOpTransform::Copy() 
{                    
	mafOpTransform *cp  = new mafOpTransform();
	cp->m_Listener = m_Listener;
	cp->m_new_matrix->DeepCopy(m_new_matrix);
	cp->m_old_matrix->DeepCopy(m_old_matrix);
	return cp;
}
//----------------------------------------------------------------------------
bool mafOpTransform::Accept(mafNode* vme)       
{
  return vme!=NULL && !vme->IsA("mafNodeRoot");
}
//----------------------------------------------------------------------------
void mafOpTransform::SetInput(mafNode* vme)       
{
  m_vme = vme;
}
//----------------------------------------------------------------------------
void mafOpTransform::SetOldMatrix(vtkMatrix4x4* matrix)
{
  m_old_matrix->DeepCopy(matrix);
}
//----------------------------------------------------------------------------
void mafOpTransform::SetNewMatrix(vtkMatrix4x4* matrix)
{
  m_new_matrix->DeepCopy(matrix);
}
//----------------------------------------------------------------------------
void mafOpTransform::OpDo()                    
{
  assert(false); //temporary commented out - m_vme->SetPose(m_new_matrix,-1);
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpTransform::OpUndo()                  
{
  assert(false); //temporary commented out -m_vme->SetPose(m_old_matrix,-1);
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
*/


