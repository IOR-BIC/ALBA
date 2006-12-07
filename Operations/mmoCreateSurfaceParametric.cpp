/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateSurfaceParametric.cpp,v $
  Language:  C++
  Date:      $Date: 2006-12-07 15:01:25 $
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


#include "mmoCreateSurfaceParametric.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMESurfaceParametric.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoCreateSurfaceParametric);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoCreateSurfaceParametric::mmoCreateSurfaceParametric(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_SurfaceParametric = NULL;
}
//----------------------------------------------------------------------------
mmoCreateSurfaceParametric::~mmoCreateSurfaceParametric( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_SurfaceParametric);
}
//----------------------------------------------------------------------------
mafOp* mmoCreateSurfaceParametric::Copy()   
//----------------------------------------------------------------------------
{
	return new mmoCreateSurfaceParametric(m_Label);
}
//----------------------------------------------------------------------------
bool mmoCreateSurfaceParametric::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mmoCreateSurfaceParametric::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_SurfaceParametric);
  m_SurfaceParametric->SetName("Surface Parametric");
  m_Output = m_SurfaceParametric;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mmoCreateSurfaceParametric::OpDo()
//----------------------------------------------------------------------------
{
  m_SurfaceParametric->ReparentTo(m_Input);
}
