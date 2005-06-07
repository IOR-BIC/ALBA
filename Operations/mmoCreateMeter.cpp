/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-07 14:42:21 $
  Version:   $Revision: 1.3 $
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


#include "mmoCreateMeter.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEMeter.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoCreateMeter::mmoCreateMeter(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Meter   = NULL;
}
//----------------------------------------------------------------------------
mmoCreateMeter::~mmoCreateMeter( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Meter);
}
//----------------------------------------------------------------------------
mafOp* mmoCreateMeter::Copy()   
//----------------------------------------------------------------------------
{
	return new mmoCreateMeter(m_Label);
}
//----------------------------------------------------------------------------
bool mmoCreateMeter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	if(!node) 
    return false;
  return true;
}
//----------------------------------------------------------------------------
void mmoCreateMeter::OpRun()   
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mmoCreateMeter::OpDo()
//----------------------------------------------------------------------------
{
  mafNEW(m_Meter);
  m_Meter->SetName("meter");
  m_Meter->ReparentTo(m_Input);
  mafEventMacro(mafEvent(this, VME_ADD, m_Meter));
  m_Meter->SetLink("StartVME", m_Input);
}
//----------------------------------------------------------------------------
void mmoCreateMeter::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_Meter);
  mafEventMacro(mafEvent(this, VME_REMOVE, m_Meter));
}
