/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateSurfaceParametric.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi
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


#include "mafOpCreateSurfaceParametric.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMESurfaceParametric.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateSurfaceParametric);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateSurfaceParametric::mafOpCreateSurfaceParametric(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_SurfaceParametric = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateSurfaceParametric::~mafOpCreateSurfaceParametric( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_SurfaceParametric);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateSurfaceParametric::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateSurfaceParametric(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpCreateSurfaceParametric::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateSurfaceParametric::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_SurfaceParametric);
  m_SurfaceParametric->SetName("Surface Parametric");
  m_Output = m_SurfaceParametric;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mafOpCreateSurfaceParametric::OpDo()
//----------------------------------------------------------------------------
{
  m_SurfaceParametric->ReparentTo(m_Input);
}
