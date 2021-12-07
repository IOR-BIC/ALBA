/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateRefSys
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaOpCreateRefSys.h"
#include "albaDecl.h"

#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaVMERefSys.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateRefSys);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateRefSys::albaOpCreateRefSys(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  
  m_RefSys     = NULL;
}

//----------------------------------------------------------------------------
albaOpCreateRefSys::~albaOpCreateRefSys()
//----------------------------------------------------------------------------
{
  albaDEL(m_RefSys);
}

//----------------------------------------------------------------------------
albaOp* albaOpCreateRefSys::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpCreateRefSys(m_Label);
}

//----------------------------------------------------------------------------
bool albaOpCreateRefSys::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVME));
}

//----------------------------------------------------------------------------
void albaOpCreateRefSys::OpRun()   
//----------------------------------------------------------------------------
{
  albaNEW(m_RefSys);
  m_RefSys->SetName("Reference System");
  m_Output = m_RefSys;
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}

//----------------------------------------------------------------------------
void albaOpCreateRefSys::OpDo()
//----------------------------------------------------------------------------
{
  if (!m_Input->IsALBAType(albaVMERoot))
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

		GetLogicManager()->VmeShow(m_RefSys, true);
  }
  else
    GetLogicManager()->VmeAdd(m_RefSys);
}
