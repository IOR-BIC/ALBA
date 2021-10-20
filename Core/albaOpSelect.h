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
#ifndef __albaOpSelect_H__
#define __albaOpSelect_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaOp.h"
#include "albaVMEIterator.h"
#include "albaSmartPointer.h" //for albaAutoPointer
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;

template class ALBA_EXPORT albaAutoPointer<albaVME>;

/**
    class name: albaOpSelect
    Operation for the selection of a vme.
*/
class ALBA_EXPORT albaOpSelect: public albaOp
{
public:
		albaTypeMacro(albaOpSelect, albaOp);

    /** Constructor. */
    albaOpSelect(wxString label=_("Select"));
    /** Destructor. */
   ~albaOpSelect(); 
    /** check if node can be input of the operation. */
    bool Accept(albaVME* vme);
    /** set input node to the operation. */
    void SetInput(albaVME* vme);
    /** retrieve the input node. */
    albaVME* GetInput(){return m_OldNodeSelected;};
    /** selection of another node. */
    void SetNewSel(albaVME* vme);
    /** retrieve new selected node. */
    albaVME* GetNewSel(){return m_NewNodeSelected;};
    /** execute the operation.  */
    void OpDo();
    /** undo the operation. */
    void OpUndo();
    /** return a instance of current object. */
    albaOp* Copy();
		/** runs operation. */
		void OpRun();
protected:
    albaAutoPointer<albaVME> m_OldNodeSelected;
    albaAutoPointer<albaVME> m_NewNodeSelected;
};
/**
    class name: albaOpEdit
    Interface operation for cut, copy, and paste operation.
*/
class ALBA_EXPORT albaOpEdit: public albaOp
{
public:
    /** Constructor. */
    albaOpEdit(wxString label="");
    /** Destructor. */
    ~albaOpEdit(); 
    /** set input node to the operation. */
    void     SetInput(albaVME* vme) {m_Selection = vme;};
    /** check if the clipboard is empty.*/
    bool     ClipboardIsEmpty();
    /** clear the clipboard. */
		void     ClipboardClear();
    /** store clipboard for backup  */
		void     ClipboardBackup();
    /** restore clipboard from backup */
		void     ClipboardRestore();
    /** return the albaVME that is in the clipboard */
    albaVME* GetClipboard();
    /** set the clipboard */
    void SetClipboard(albaVME *node);
		/** set the parent of the selection */
		void SetSelectionParent(albaVME *parent);
		/** runs operation. */
		void OpRun();
protected:

	void RemoveBackLinksForTheSubTree(albaVME *vme);
	void RestoreBackLinksForTheSubTree(albaVME *vme);
	bool CanRestoreBackLinksForTheSubTree(albaVME *vme, albaVME *root);
  
	albaAutoPointer<albaVME> m_Backup;
  albaAutoPointer<albaVME> m_Selection;
};
/**
    class name: albaOpCut
    Operation which perform cut on a node input. (copying it into a clipboard)
*/
class ALBA_EXPORT albaOpCut: public albaOpEdit
{
public:
  /** Constructor. */
  albaOpCut(wxString label=_("Cut"));
  /** Destructor. */
  ~albaOpCut();
  /** check if node can be input of the operation. */
  bool Accept(albaVME* vme);
  /** execute the operation.  */
  void OpDo();

	/** undo the operation. */
  void OpUndo();
  /** return a instance of current object. */
  albaOp* Copy(); 
  /** Load VTK data for the specified VME (Added by Losi on 03.06.2010) */
  void LoadVTKData(albaVME *vme);

protected: 
	/** Load all children in the tree (Added by Di Cosmo on 24.05.2012) */
	void LoadChild(albaVME *vme);
	albaAutoPointer<albaVME> m_SelectionParentBackup;
	bool m_Cutted;
};
/**
    class name: albaOpCopy
    Operation which perform copy of a node.
*/
class ALBA_EXPORT albaOpCopy: public albaOpEdit
{
public:
    /** Constructor. */
    albaOpCopy(wxString label=_("Copy"));
    /** Destructor. */
    ~albaOpCopy();
    /** check if node can be input of the operation. */
    bool Accept(albaVME* vme);
    /** execute the operation.  */
    void OpDo();
		/** undo the operation. */
    void OpUndo();
    /** return a instance of current object. */
    albaOp* Copy();
};
/**
    class name: albaOpPaste
    Operation which perform paste of a node previously copied or cut.
*/
class ALBA_EXPORT albaOpPaste: public albaOpEdit
{
public:
    /** Constructor. */
    albaOpPaste(wxString label=_("Paste"));
    /** check if node can be input of the operation. */
    bool Accept(albaVME* vme);
    /** execute the operation.  */    
    void OpDo();
    /** undo the operation. */
    void OpUndo();
    /** return a instance of current object. */
    albaOp* Copy(); 
protected:
    albaAutoPointer<albaVME> m_PastedVme;
		bool m_Pasted;
};

/**
class name: albaOpPaste
Operation which perform paste of a node previously copied or cut.
*/
class ALBA_EXPORT albaOpRename : public albaOpEdit
{
public:
	/** Constructor. */
	albaOpRename(wxString label = _("Rename"));
	/** check if node can be input of the operation. */
	bool Accept(albaVME* vme);
	/** execute the operation.  */
	void OpDo();
	/** undo the operation. */
	void OpUndo();
	/** return a instance of current object. */
	albaOp* Copy();
protected:
	albaString m_OldName;
	bool m_Renamed;
};

#endif
