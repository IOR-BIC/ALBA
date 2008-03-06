/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateGroup.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpCreateGroup_H__
#define __mafOpCreateGroup_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEGroup;
class mafNode;
//----------------------------------------------------------------------------
// mafOpCreateGroup :
//----------------------------------------------------------------------------
/** */
class mafOpCreateGroup: public mafOp
{
public:
  mafOpCreateGroup(const wxString &label = "CreateGroup");
  ~mafOpCreateGroup(); 

  mafTypeMacro(mafOpCreateGroup, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMEGroup *m_Group;
};
#endif
