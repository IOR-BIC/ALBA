/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpSelect.h,v $
  Language:  C++
  Date:      $Date: 2010-04-21 09:49:57 $
  Version:   $Revision: 1.7.2.1 $
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

/**
    class name: mafOpSelect
    Operation for the selection of a vme.
*/
class mafOpSelect: public mafOp
{
public:
    /** Constructor. */
    mafOpSelect(wxString label=_("Select"));
    /** Destructor. */
   ~mafOpSelect(); 
    /** check if node can be input of the operation. */
    bool Accept(mafNode* vme);
    /** set input node to the operation. */
    void SetInput(mafNode* vme);
    /** retrieve the input node. */
    mafNode* GetInput(){return m_OldNodeSelected;};
    /** selection of another node. */
    void SetNewSel(mafNode* vme);
    /** retrieve new selected node. */
    mafNode* GetNewSel(){return m_NewNodeSelected;};
    /** execute the operation.  */
    void OpDo();
    /** undo the operation. */
    void OpUndo();
    /** return a instance of current object. */
    mafOp* Copy();
protected:
    mafAutoPointer<mafNode> m_OldNodeSelected;
    mafAutoPointer<mafNode> m_NewNodeSelected;
};
/**
    class name: mafOpEdit
    Interface operation for cut, copy, and paste operation.
*/
class mafOpEdit: public mafOp
{
public:
    /** Constructor. */
    mafOpEdit(wxString label="");
    /** Destructor. */
    ~mafOpEdit(); 
    /** set input node to the operation. */
    void     SetInput(mafNode* vme) {m_Selection = vme;};
    /** check if the clipboard is empty.*/
    bool     ClipboardIsEmpty();
    /** clear the clipboard. */
		void     ClipboardClear();
    /** store clipboard for backup  */
		void     ClipboardBackup();
    /** restore clipboard from backup */
		void     ClipboardRestore();

    static   mafAutoPointer<mafNode> m_Clipboard;
protected:
             mafAutoPointer<mafNode> m_Backup;
             mafAutoPointer<mafNode> m_Selection;
};
/**
    class name: mafOpCut
    Operation which perform cut on a node input. (copying it into a clipboard)
*/
class mafOpCut: public mafOpEdit
{
public:
    /** Constructor. */
    mafOpCut(wxString label=_("Cut"));
    /** Destructor. */
    ~mafOpCut();
    /** check if node can be input of the operation. */
    bool Accept(mafNode* vme);
    /** execute the operation.  */
    void OpDo();
    /** undo the operation. */
    void OpUndo();
    /** return a instance of current object. */
    mafOp* Copy(); 
protected:
    mafAutoPointer<mafNode> m_SelectionParent;
};
/**
    class name: mafOpCopy
    Operation which perform copy of a node.
*/
class mafOpCopy: public mafOpEdit
{
public:
    /** Constructor. */
    mafOpCopy(wxString label=_("Copy"));
    /** Destructor. */
    ~mafOpCopy();
    /** check if node can be input of the operation. */
    bool Accept(mafNode* vme);
    /** execute the operation.  */
    void OpDo();
    /** undo the operation. */
    void OpUndo();
    /** return a instance of current object. */
    mafOp* Copy();
};
/**
    class name: mafOpPaste
    Operation which perform paste of a node previously copied or cut.
*/
class mafOpPaste: public mafOpEdit
{
public:
    /** Constructor. */
    mafOpPaste(wxString label=_("Paste"));
    /** check if node can be input of the operation. */
    bool Accept(mafNode* vme);
    /** execute the operation.  */    
    void OpDo();
    /** undo the operation. */
    void OpUndo();
    /** return a instance of current object. */
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
