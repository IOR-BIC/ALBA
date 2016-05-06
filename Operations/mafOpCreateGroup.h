/*=========================================================================

 Program: MAF2
 Module: mafOpCreateGroup
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateGroup_H__
#define __mafOpCreateGroup_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEGroup;
class mafVME;
//----------------------------------------------------------------------------
// mafOpCreateGroup :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateGroup: public mafOp
{
public:
  mafOpCreateGroup(const wxString &label = "CreateGroup");
  ~mafOpCreateGroup(); 

  mafTypeMacro(mafOpCreateGroup, mafOp);

  mafOp* Copy();

  bool Accept(mafVME*node);
  void OpRun();
  void OpDo();

protected: 
  mafVMEGroup *m_Group;
};
#endif
