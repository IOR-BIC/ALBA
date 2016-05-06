/*=========================================================================

 Program: MAF2
 Module: mafOpCreateRefSys
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateRefSys_H__
#define __mafOpCreateRefSys_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMERefSys;
class mafVME;

//----------------------------------------------------------------------------
// mafOpCreateRefSys :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateRefSys: public mafOp
{
public:
  mafOpCreateRefSys(const wxString &label = "CreateRefSys");
 ~mafOpCreateRefSys(); 

  mafTypeMacro(mafOpCreateRefSys, mafOp);

  mafOp* Copy();

  bool Accept(mafVME*node);
  void OpRun();
  void OpDo();

protected: 
  mafVMERefSys *m_RefSys;
};
#endif
