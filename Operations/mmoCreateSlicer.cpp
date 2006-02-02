/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateSlicer.cpp,v $
  Language:  C++
  Date:      $Date: 2006-02-02 16:41:08 $
  Version:   $Revision: 1.6 $
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
  m_Slicer->ReparentTo(mafVME::SafeDownCast(m_Input));
  double b[6], center[3], rot[3];
  rot[0] = rot[1] = rot[2] = 0;
  ((mafVME *)m_Input)->GetOutput()->GetBounds(b);
  center[0] = (b[0]+b[1]) * 0.5;
  center[1] = (b[2]+b[3]) * 0.5;
  center[2] = (b[4]+b[5]) * 0.5;
  m_Slicer->SetPose(center,rot,0);
}
