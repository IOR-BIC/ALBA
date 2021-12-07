/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSlicer
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


#include "albaOpCreateSlicer.h"
#include "albaDecl.h"
#include "albaEvent.h"

#include "albaVMESlicer.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateSlicer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateSlicer::albaOpCreateSlicer(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  
  m_Slicer    = NULL;
  m_SlicedVME = NULL;
}
//----------------------------------------------------------------------------
albaOpCreateSlicer::~albaOpCreateSlicer()
//----------------------------------------------------------------------------
{
  albaDEL(m_Slicer);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateSlicer::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpCreateSlicer(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateSlicer::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node != NULL);
}
//----------------------------------------------------------------------------
void albaOpCreateSlicer::OpRun()
//----------------------------------------------------------------------------
{
  albaString title = _("Choose VME to slice");
  albaEvent e;
  e.SetId(VME_CHOOSE);
  e.SetPointer(&albaOpCreateSlicer::VolumeAccept);
  e.SetString(&title);
  albaEventMacro(e);

  int result = OP_RUN_CANCEL;

  albaVME *n = e.GetVme();
  if (n != NULL)
  {
		albaNEW(m_Slicer);
		m_Slicer->SetName("slicer");
		m_Output = m_Slicer;

    m_SlicedVME = n;
    m_Slicer->SetSlicedVMELink(m_SlicedVME);
    result = OP_RUN_OK;
  }
  albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpCreateSlicer::OpDo()
//----------------------------------------------------------------------------
{
  double center[3] = {0.0,0.0,0.0}, rot[3] = {0.0,0.0,0.0};
  albaOBB b;
  m_Slicer->ReparentTo(m_Input);
  rot[0] = rot[1] = rot[2] = 0;
  m_SlicedVME->GetOutput()->GetVMELocalBounds(b);
  if (b.IsValid())
  {
    b.GetCenter(center);
  }
  m_Slicer->SetPose(center,rot,0);
}
