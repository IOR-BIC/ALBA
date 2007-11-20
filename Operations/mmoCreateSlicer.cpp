/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateSlicer.cpp,v $
  Language:  C++
  Date:      $Date: 2007-11-20 14:37:41 $
  Version:   $Revision: 1.13 $
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

#include "mafVMESlicer.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoCreateSlicer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoCreateSlicer::mmoCreateSlicer(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  
  m_Slicer    = NULL;
  m_SlicedVME = NULL;
}
//----------------------------------------------------------------------------
mmoCreateSlicer::~mmoCreateSlicer()
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
  return (node != NULL);
}
//----------------------------------------------------------------------------
void mmoCreateSlicer::OpRun()
//----------------------------------------------------------------------------
{
  mafString title = _("Choose VME to slice");
  mafEvent *e; e = new mafEvent();
  e->SetId(VME_CHOOSE);
  e->SetArg((long)&mmoCreateSlicer::VolumeAccept);
  e->SetString(&title);
  mafEventMacro(*e);

  int result = OP_RUN_CANCEL;

  mafNode *n = e->GetVme();
  if (n != NULL)
  {
		mafNEW(m_Slicer);
		m_Slicer->SetName("slicer");
		m_Output = m_Slicer;

    m_SlicedVME = n;
    m_Slicer->SetSlicedVMELink(m_SlicedVME);
    result = OP_RUN_OK;
  }
  mafEventMacro(mafEvent(this, result));
}
//----------------------------------------------------------------------------
void mmoCreateSlicer::OpDo()
//----------------------------------------------------------------------------
{
  double center[3], rot[3];
  mafOBB b;
  m_Slicer->ReparentTo(mafVME::SafeDownCast(m_Input));
  rot[0] = rot[1] = rot[2] = 0;
  ((mafVME *)m_SlicedVME)->GetOutput()->GetVMELocalBounds(b);
  b.GetCenter(center);
  m_Slicer->SetPose(center,rot,0);
  m_Output = m_Slicer; // This allow the UnDo to work.
}
