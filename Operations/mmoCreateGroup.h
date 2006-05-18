/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateGroup.h,v $
  Language:  C++
  Date:      $Date: 2006-05-18 10:26:47 $
  Version:   $Revision: 1.3 $
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

protected: 
  mafVMEGroup *m_Group;
};
#endif
