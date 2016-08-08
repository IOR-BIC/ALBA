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


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOpCreateRefSys.h"
#include "mafDecl.h"

#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMERefSys.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateRefSys);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateRefSys::mafOpCreateRefSys(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  
  m_RefSys     = NULL;
}

//----------------------------------------------------------------------------
mafOpCreateRefSys::~mafOpCreateRefSys()
//----------------------------------------------------------------------------
{
  mafDEL(m_RefSys);
}

//----------------------------------------------------------------------------
mafOp* mafOpCreateRefSys::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateRefSys(m_Label);
}

//----------------------------------------------------------------------------
bool mafOpCreateRefSys::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}

//----------------------------------------------------------------------------
void mafOpCreateRefSys::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_RefSys);
  m_RefSys->SetName("Reference System");
  m_Output = m_RefSys;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}

//----------------------------------------------------------------------------
void mafOpCreateRefSys::OpDo()
//----------------------------------------------------------------------------
{
  if (!m_Input->IsMAFType(mafVMERoot))
  {
    m_RefSys->ReparentTo(m_Input);
    m_RefSys->SetAbsMatrix(*m_Input->GetOutput()->GetAbsMatrix());

		// Calculate Scale Factor
		double b[6];
		m_Input->GetOutput()->GetVMELocalBounds(b);
		double diffX = fabs(b[1] - b[0]);
		double diffY = fabs(b[3] - b[2]);
		double diffZ = fabs(b[5] - b[4]);
		double mainDiagonal = sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);
		m_RefSys->SetScaleFactor(mainDiagonal/3.0);

		mafEventMacro(mafEvent(this,VME_SHOW,m_RefSys,true));
  }
  else
    mafEventMacro(mafEvent(this, VME_ADD, m_RefSys));
}
