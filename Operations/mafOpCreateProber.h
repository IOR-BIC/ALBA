/*=========================================================================

 Program: MAF2
 Module: mafOpCreateProber
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateProber_H__
#define __mafOpCreateProber_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEProber;
class mafGUI;
class mafEvent;
//----------------------------------------------------------------------------
// mafOpCreateProber :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateProber: public mafOp
{
public:
  mafOpCreateProber(const wxString &label = "CreateProber");
 ~mafOpCreateProber(); 

  mafTypeMacro(mafOpCreateProber, mafOp);

  mafOp* Copy();

  bool Accept(mafVME*node);
  void OpRun();
  void OpDo();

protected: 
  mafVMEProber *m_Prober;
};
#endif
