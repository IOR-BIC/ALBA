/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgVMEChooser.h,v $
  Language:  C++
  Date:      $Date: 2005-05-05 15:24:13 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgVmeChooser_H__
#define __mmgVmeChooser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mmgDialog;
class mmgCheckTree;

/** mmgVmeChooser : Display a modal dialog filled with a vme tree and
return the choosed vme. The tree is filled from
the vme passed in the constructor with its children.*/
class mmgVMEChooser : public mafObserver
{

public:
           mmgVMEChooser(mmgCheckTree *tree, wxString dialog_title = "Vme Chooser", long vme_accept_function = 0);  
          ~mmgVMEChooser();
	mafVME  *ShowModal();
  void     OnEvent(mafEventBase *event);

protected:
  mmgDialog *m_Dialog;
  mafVME    *m_ChoosedVME;
	wxButton  *m_OkButton;
};
#endif