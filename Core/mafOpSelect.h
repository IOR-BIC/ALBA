/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpSelect.h,v $
  Language:  C++
  Date:      $Date: 2005-07-22 13:54:44 $
  Version:   $Revision: 1.5 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafOpSelect_H__
#define __mafOpSelect_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafOp.h"
#include "mafNodeIterator.h"
#include "mafSmartPointer.h" //for mafAutoPointer
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class vtkMatrix4x4;

//----------------------------------------------------------------------------
// mafOpSelect :
//----------------------------------------------------------------------------
class mafOpSelect: public mafOp
{
public:
    mafOpSelect(wxString label="Select");
   ~mafOpSelect(); 
    bool Accept(mafNode* vme);
    void SetInput(mafNode* vme);
    void SetNewSel(mafNode* vme);
    void OpDo();
    void OpUndo();
    mafOp* Copy();
protected:
    mafAutoPointer<mafNode> m_OldNodeSelected;
    mafAutoPointer<mafNode> m_NewNodeSelected;
};
//----------------------------------------------------------------------------
// mafOpEdit :
//----------------------------------------------------------------------------
class mafOpEdit: public mafOp
{
public:
             mafOpEdit(wxString label="");
            ~mafOpEdit(); 
    void     SetInput(mafNode* vme) {m_Selection = vme;};

    bool     ClipboardIsEmpty();
		void     ClipboardClear();
		void     ClipboardBackup();
		void     ClipboardRestore();

    static   mafAutoPointer<mafNode> m_Clipboard;
protected:
             mafAutoPointer<mafNode> m_Backup;
             mafAutoPointer<mafNode> m_Selection;
};
//----------------------------------------------------------------------------
// mafOpCut :
//----------------------------------------------------------------------------
class mafOpCut: public mafOpEdit
{
public:
    mafOpCut(wxString label="Cut"); 
    ~mafOpCut(); 
    bool Accept(mafNode* vme);
    void OpDo();
    void OpUndo();
    mafOp* Copy(); 
protected:
    mafAutoPointer<mafNode> m_SelectionParent;
};
//----------------------------------------------------------------------------
// mafOpCopy :
//----------------------------------------------------------------------------
class mafOpCopy: public mafOpEdit
{
public:
    mafOpCopy(wxString label="Copy"); 
    ~mafOpCopy();
    bool Accept(mafNode* vme);
    void OpDo();
    void OpUndo();
    mafOp* Copy();
};
//----------------------------------------------------------------------------
// mafOpPaste :
//----------------------------------------------------------------------------
class mafOpPaste: public mafOpEdit
{
public:
    mafOpPaste(wxString label="Paste"); 
    bool Accept(mafNode* vme);       
    void OpDo();
    void OpUndo();
    mafOp* Copy(); 
protected:
    mafAutoPointer<mafNode> m_PastedVme;
};
/*
//----------------------------------------------------------------------------
// mafOpTransform :
//----------------------------------------------------------------------------

class mafOpTransform: public mafOp
{
public:
    mafOpTransform(wxString label="Transform"); 
   ~mafOpTransform(); 
    bool Accept(mafNode* vme);
    void SetInput(mafNode* vme);       
    void SetOldMatrix(vtkMatrix4x4* matrix);
    void SetNewMatrix(vtkMatrix4x4* matrix);
    void OpDo();
    void OpUndo();
    mafOp* Copy();
protected:
  	mafNode*       m_vme; 
  	vtkMatrix4x4* m_new_matrix; 
  	vtkMatrix4x4* m_old_matrix; 
};
*/
#endif
