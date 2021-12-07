/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSurfaceParametric
 Authors: Daniele Giunchi
 
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


#include "albaOpCreateSurfaceParametric.h"
#include "albaDecl.h"
#include "albaEvent.h"

#include "albaVMESurfaceParametric.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateSurfaceParametric);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateSurfaceParametric::albaOpCreateSurfaceParametric(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_SurfaceParametric = NULL;
}
//----------------------------------------------------------------------------
albaOpCreateSurfaceParametric::~albaOpCreateSurfaceParametric( ) 
//----------------------------------------------------------------------------
{
  albaDEL(m_SurfaceParametric);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateSurfaceParametric::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpCreateSurfaceParametric(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateSurfaceParametric::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVME));
}
//----------------------------------------------------------------------------
void albaOpCreateSurfaceParametric::OpRun()   
//----------------------------------------------------------------------------
{
  albaNEW(m_SurfaceParametric);
  m_SurfaceParametric->SetName("Parametric Surface");
  m_Output = m_SurfaceParametric;
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void albaOpCreateSurfaceParametric::OpDo()
//----------------------------------------------------------------------------
{
  m_SurfaceParametric->ReparentTo(m_Input);
}
