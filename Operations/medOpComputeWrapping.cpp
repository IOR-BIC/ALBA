/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpComputeWrapping.cpp,v $
  Language:  C++
  Date:      $Date: 2008-12-02 12:00:44 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Anupam Agrawal and Hui Wei
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "medOpComputeWrapping.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "medVMEComputeWrapping.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpComputeWrapping);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpComputeWrapping::medOpComputeWrapping(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Meter   = NULL;
}
//----------------------------------------------------------------------------
medOpComputeWrapping::~medOpComputeWrapping( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Meter);
}
//----------------------------------------------------------------------------
mafOp* medOpComputeWrapping::Copy()   
//----------------------------------------------------------------------------
{
	return new medOpComputeWrapping(m_Label);
}
//----------------------------------------------------------------------------
bool medOpComputeWrapping::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void medOpComputeWrapping::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_Meter);
  m_Meter->SetName("Multi-Obj wrapping");
  m_Output = m_Meter;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void medOpComputeWrapping::OpDo()
//----------------------------------------------------------------------------
{
  m_Meter->ReparentTo(m_Input);
}
