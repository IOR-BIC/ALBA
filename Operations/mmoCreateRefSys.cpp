/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateRefSys.cpp,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mmoCreateRefSys.h"
#include "mafDecl.h"

#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMERefSys.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoCreateRefSys);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoCreateRefSys::mmoCreateRefSys(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  
  m_RefSys     = NULL;
}

//----------------------------------------------------------------------------
mmoCreateRefSys::~mmoCreateRefSys()
//----------------------------------------------------------------------------
{
  mafDEL(m_RefSys);
}

//----------------------------------------------------------------------------
mafOp* mmoCreateRefSys::Copy()   
//----------------------------------------------------------------------------
{
	return new mmoCreateRefSys(m_Label);
}

//----------------------------------------------------------------------------
bool mmoCreateRefSys::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}

//----------------------------------------------------------------------------
void mmoCreateRefSys::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_RefSys);
  m_RefSys->SetName("ref_sys");
  m_Output = m_RefSys;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}

//----------------------------------------------------------------------------
void mmoCreateRefSys::OpDo()
//----------------------------------------------------------------------------
{
  if (!m_Input->IsMAFType(mafVMERoot))
  {
    m_RefSys->ReparentTo(m_Input);
    m_RefSys->SetAbsMatrix(*((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
  }
  else
    mafEventMacro(mafEvent(this, VME_ADD, m_RefSys));
}
