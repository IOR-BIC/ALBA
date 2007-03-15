/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateProber.cpp,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
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


#include "mmoCreateProber.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEVolume.h"
#include "mafVMEProber.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoCreateProber);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoCreateProber::mmoCreateProber(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Prober  = NULL;
}
//----------------------------------------------------------------------------
mmoCreateProber::~mmoCreateProber( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Prober);
}
//----------------------------------------------------------------------------
mafOp* mmoCreateProber::Copy()   
//----------------------------------------------------------------------------
{
	return new mmoCreateProber(m_Label);
}
//----------------------------------------------------------------------------
bool mmoCreateProber::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node != NULL);
}
//----------------------------------------------------------------------------
void mmoCreateProber::OpRun()
//----------------------------------------------------------------------------
{
  mafNEW(m_Prober);
  m_Prober->SetName("prober");
  m_Output = m_Prober;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mmoCreateProber::OpDo()
//----------------------------------------------------------------------------
{
  m_Prober->ReparentTo(mafVME::SafeDownCast(m_Input));
}
