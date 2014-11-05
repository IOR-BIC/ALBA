/*=========================================================================

 Program: MAF2Medical
 Module: medOpCreateWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "medOpCreateWrappedMeter.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "medVMEWrappedMeter.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpCreateWrappedMeter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpCreateWrappedMeter::medOpCreateWrappedMeter(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Meter   = NULL;
}
//----------------------------------------------------------------------------
medOpCreateWrappedMeter::~medOpCreateWrappedMeter( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Meter);
}
//----------------------------------------------------------------------------
mafOp* medOpCreateWrappedMeter::Copy()   
//----------------------------------------------------------------------------
{
	return new medOpCreateWrappedMeter(m_Label);
}
//----------------------------------------------------------------------------
bool medOpCreateWrappedMeter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void medOpCreateWrappedMeter::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_Meter);
  m_Meter->SetName("Wrapped Meter");
  m_Output = m_Meter;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void medOpCreateWrappedMeter::OpDo()
//----------------------------------------------------------------------------
{
  m_Meter->ReparentTo(m_Input);
}
