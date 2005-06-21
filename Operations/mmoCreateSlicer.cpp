/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateSlicer.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-21 11:35:29 $
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


#include "mmoCreateSlicer.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEVolume.h"
#include "mafVMESlicer.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoCreateSlicer::mmoCreateSlicer(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Slicer   = NULL;
}
//----------------------------------------------------------------------------
mmoCreateSlicer::~mmoCreateSlicer( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Slicer);
}
//----------------------------------------------------------------------------
mafOp* mmoCreateSlicer::Copy()   
//----------------------------------------------------------------------------
{
	return new mmoCreateSlicer(m_Label);
}
//----------------------------------------------------------------------------
bool mmoCreateSlicer::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node != NULL && node->IsMAFType(mafVMEVolume));
}
//----------------------------------------------------------------------------
void mmoCreateSlicer::OpRun()
//----------------------------------------------------------------------------
{
  mafNEW(m_Slicer);
  m_Slicer->SetName("slicer");
  m_Output = m_Slicer;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mmoCreateSlicer::OpDo()
//----------------------------------------------------------------------------
{
  m_Slicer->ReparentTo(m_Input);
}
