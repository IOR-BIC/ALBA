/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpSelect.cpp,v $
  Language:  C++
  Date:      $Date: 2006-11-02 11:25:50 $
  Version:   $Revision: 1.8 $
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
#include "mafVMERoot.h"
#include "mafString.h"
#ifdef MAF_USE_VTK
  #include "mafVMELandmarkCloud.h"
#endif
//#include "vtkMatrix4x4.h"
//#include "vtkMath.h"

//initialize the Clipboard
mafAutoPointer<mafNode>  mafOpEdit::m_Clipboard(NULL); 

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
bool mafOpSelect::Accept(mafNode* vme)     
//----------------------------------------------------------------------------
{
	return true;
}
//----------------------------------------------------------------------------
void mafOpSelect::SetInput(mafNode* vme)   
//----------------------------------------------------------------------------
{
	m_OldNodeSelected = vme;
}
//----------------------------------------------------------------------------
void mafOpSelect::SetNewSel(mafNode* vme)  
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
  return m_Clipboard.GetPointer() == NULL;
}
//----------------------------------------------------------------------------
void mafOpEdit::ClipboardClear()
//----------------------------------------------------------------------------
{
  m_Clipboard = NULL;
}
//----------------------------------------------------------------------------
void mafOpEdit::ClipboardBackup()
//----------------------------------------------------------------------------
{
  assert(m_Backup.GetPointer() == NULL);
	m_Backup = m_Clipboard;
  m_Clipboard = NULL;
}
//----------------------------------------------------------------------------
void mafOpEdit::ClipboardRestore()
//----------------------------------------------------------------------------
{
  //assert(m_Backup.GetPointer() ); - //SIL. 6-11-2003: assert removed, I may make a backup of an empy clipboard
	m_Clipboard = m_Backup;
  m_Backup = NULL;
}

///////////////
// mafOpCut ://
///////////////
//----------------------------------------------------------------------------
mafOpCut::mafOpCut(wxString label) 
//----------------------------------------------------------------------------
{
  m_Label=label;
  m_SelectionParent = NULL; 
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
bool mafOpCut::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  return ((vme!=NULL) && (!vme->IsMAFType(mafVMERoot)));
}
//----------------------------------------------------------------------------
void mafOpCut::OpDo()
//----------------------------------------------------------------------------
/**
backup the clipboard
Send a VME_REMOVE for the selected vme
Move (doesn't make a copy) the selected vme (and it's subtree) in the Clipboard
Select the vme parent
*/
{
	ClipboardBackup();
	m_SelectionParent = m_Selection->GetParent(); 
	m_Clipboard = m_Selection;
  mafEventMacro(mafEvent(this,VME_REMOVE,m_Selection));
	mafEventMacro(mafEvent(this,VME_SELECTED,m_SelectionParent));
  if (mafVME::SafeDownCast(m_SelectionParent.GetPointer()))
  {
    ((mafVME *)m_SelectionParent.GetPointer())->GetOutput()->Update();
  }
}
//----------------------------------------------------------------------------
void mafOpCut::OpUndo()
//----------------------------------------------------------------------------
/**
Move the vme in the Clipboard under it's old parent
send a VME_ADD
Restore the Clipboard
Restore the Selection
*/
{
  m_Selection = m_Clipboard.GetPointer();

#ifdef MAF_USE_VTK
  if (m_SelectionParent->IsMAFType(mafVMELandmarkCloud) && !((mafVMELandmarkCloud *)m_SelectionParent.GetPointer())->IsOpen())
  {
    ((mafVMELandmarkCloud *)m_SelectionParent.GetPointer())->Open();
    m_Selection->ReparentTo(m_SelectionParent);
    ((mafVMELandmarkCloud *)m_SelectionParent.GetPointer())->Close();
  }
  else
  {
    m_Selection->ReparentTo(m_SelectionParent);
  }
#else
    m_Selection->ReparentTo(m_SelectionParent);
#endif

  if (mafVME::SafeDownCast(m_SelectionParent.GetPointer()))
  {
    ((mafVME *)m_SelectionParent.GetPointer())->GetOutput()->Update();
  }
	mafEventMacro(mafEvent(this,VME_SELECTED,m_Selection));
	ClipboardRestore();
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
bool mafOpCopy::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  bool res = (vme!=NULL) && (!vme->IsMAFType(mafVMERoot));
  if(m_Clipboard)
    res = res && m_Clipboard->CanCopy(vme);
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
  m_Clipboard = m_Selection->CopyTree();
  mafString copy_name;
  copy_name = "copy of ";
  copy_name += m_Clipboard->GetName();
  m_Clipboard->SetName(copy_name.GetCStr());
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
bool mafOpPaste::Accept(mafNode* vme)       
//----------------------------------------------------------------------------
{
	//paste may be executed if
	// - vme is not NULL
	// - the clipboard is not empty
	// - THE vme on the clipboard can be re-parented under vme
	// - (this cover also restrictions imposed on Landmarks)

  if(ClipboardIsEmpty()) return false;
	if(vme == NULL) return false;
	mafNode *cv = m_Clipboard.GetPointer();
  return cv->CanReparentTo(vme) == MAF_OK;
};
//----------------------------------------------------------------------------
void mafOpPaste::OpDo()                    
//----------------------------------------------------------------------------
/**
We want that Cut+Paste doesn't change the identity of a vme,so the implementation
of Paste is a little different than usual.

Paste make a copy of the object on the clipboard,
but place in the scene the original and keep the copy in the clipboard.

Them a VME_ADD is sent, selection is not changed
*/
{
  m_PastedVme = m_Clipboard.GetPointer(); 
  m_PastedVme->ReparentTo(m_Selection);
	//mafEventMacro(mafEvent(this,VME_ADD,m_PastedVme));
	m_Clipboard = m_PastedVme->CopyTree(); 
}
//----------------------------------------------------------------------------
void mafOpPaste::OpUndo()                  
//----------------------------------------------------------------------------
/**
Remove the pasted vme from the scene and place it in the clipboard.
The copy in the clipboard will be automatically deleted
*/
{
  m_Clipboard = m_PastedVme;
  mafEventMacro(mafEvent(this,VME_REMOVE,m_PastedVme));
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
