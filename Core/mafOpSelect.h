/*=========================================================================

 Program: MAF2
 Module: mafOpSelect
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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

template class MAF_EXPORT mafAutoPointer<mafNode>;

/**
    class name: mafOpSelect
    Operation for the selection of a vme.
*/
class MAF_EXPORT mafOpSelect: public mafOp
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
class MAF_EXPORT mafOpEdit: public mafOp
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
    /** return the mafNode that is in the clipboard */
    mafNode* GetClipboard();
    /** set the clipboard */
    void SetClipboard(mafNode *node);
		/** set the parent of the selection */
		void SetSelectionParent(mafNode *parent);
protected:
  // static   mafAutoPointer<mafNode> m_Clipboard;
  mafAutoPointer<mafNode> m_Backup;
  mafAutoPointer<mafNode> m_Selection;
};
/**
    class name: mafOpCut
    Operation which perform cut on a node input. (copying it into a clipboard)
*/
class MAF_EXPORT mafOpCut: public mafOpEdit
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
    /** Load VTK data for the specified VME (Added by Losi on 03.06.2010) */
    void LoadVTKData(mafNode *vme);

protected:
   
    /** Load all children in the tree (Added by Di Cosmo on 24.05.2012) */
    void LoadChild(mafNode *vme);
};
/**
    class name: mafOpCopy
    Operation which perform copy of a node.
*/
class MAF_EXPORT mafOpCopy: public mafOpEdit
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
class MAF_EXPORT mafOpPaste: public mafOpEdit
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

#endif
