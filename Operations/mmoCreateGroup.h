/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateGroup.h,v $
  Language:  C++
  Date:      $Date: 2005-06-21 09:47:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoCreateGroup_H__
#define __mmoCreateGroup_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEGroup;
class mafNode;
//----------------------------------------------------------------------------
// mmoCreateGroup :
//----------------------------------------------------------------------------
/** */
class mmoCreateGroup: public mafOp
{
public:
  mmoCreateGroup(wxString label);
 ~mmoCreateGroup(); 
  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();
  void OpUndo();

protected: 
	void OpStop(int result);

  mafVMEGroup *m_Group;
};
#endif
