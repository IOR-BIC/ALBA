/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateGroup.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.5 $
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
  mmoCreateGroup(const wxString &label = "CreateGroup");
  ~mmoCreateGroup(); 

  mafTypeMacro(mmoCreateGroup, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMEGroup *m_Group;
};
#endif
