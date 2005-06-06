/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoReparentTo.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-06 14:48:35 $
  Version:   $Revision: 1.1 $
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

#include "mmoReparentTo.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMERoot.h"
#include "mmuTimeSet.h"
#include "mafTransformFrame.h"
#include "mafSmartPointer.h"
#include "mafVMELandmarkCloud.h"
#include "mafAbsMatrixPipe.h"

#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include <vector>

//----------------------------------------------------------------------------
mmoReparentTo::mmoReparentTo(wxString label) : mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType    = OPTYPE_OP;
  m_Canundo   = true;
  m_OldParent = NULL;
}
//----------------------------------------------------------------------------
mmoReparentTo::~mmoReparentTo( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmoReparentTo::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node != NULL && node->IsMAFType(mafVME) && !node->IsMAFType(mafVMERoot) /*&& !node->IsMAFType(mafVMEExternalData)*/);
}
//----------------------------------------------------------------------------
mafOp* mmoReparentTo::Copy()   
//----------------------------------------------------------------------------
{
  mmoReparentTo *cp = new mmoReparentTo(m_Label);
	cp->m_OldParent = m_OldParent;
  return cp;
}
//----------------------------------------------------------------------------
void mmoReparentTo::OpRun()   
//----------------------------------------------------------------------------
{
	mafEvent e(this,VME_CHOOSE);
	mafEventMacro(e);
  m_TargetVme = mafVME::SafeDownCast(e.GetVme());
	
	int result = OP_RUN_CANCEL;
	if((m_TargetVme != NULL) && (m_Input->CanReparentTo(m_TargetVme)))
		result = OP_RUN_OK;
  else
    wxMessageBox("Cannot reparent to specified node","Reparent Error",wxICON_ERROR );

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoReparentTo::SetTargetVme(mafVME *target)
//----------------------------------------------------------------------------
{
  m_TargetVme = target;

  if((m_TargetVme == NULL) || !m_Input->CanReparentTo(m_TargetVme))
  {
    wxMessageBox("Cannot reparent to specified node","Reparent Error",wxICON_ERROR );
    mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
  }
}
//----------------------------------------------------------------------------
void mmoReparentTo::OpDo()
//----------------------------------------------------------------------------
{
  int num, t;
  mmuTimeVector input_time;
	mmuTimeVector target_time;
	mmuTimeVector time;
	mafTimeStamp cTime, startTime;
	
  m_OldParent = mafVME::SafeDownCast(m_Input->GetParent());

  ((mafVME *)m_Input)->GetAbsTimeStamps(input_time);
	m_TargetVme->GetAbsTimeStamps(target_time);
  mmuTimeSet::Merge(input_time,target_time,time);
	num = time.size();

	startTime = m_TargetVme->GetTimeStamp();

  std::vector< mafAutoPointer<mafMatrix> > new_input_pose;
  new_input_pose.resize(num);

  for (t = 0; t < num; t++)
  {
    new_input_pose[t] = mafMatrix::New();
  }
  
  //change reference system
  mafSmartPointer<mafTransformFrame> transform;
  for (t = 0; t < num; t++)
	{
		cTime = time[t];
		
		((mafVME *)m_Input)->SetTimeStamp(cTime);
		m_TargetVme->SetTimeStamp(cTime);
    m_OldParent->SetTimeStamp(cTime);
		
    transform->SetTimeStamp(cTime);
    transform->SetInput(((mafVME *)m_Input)->GetMatrixPipe());
    transform->SetInputFrame(m_OldParent->GetAbsMatrixPipe());
    transform->SetTargetFrame(m_TargetVme->GetAbsMatrixPipe());
		transform->Update();

    new_input_pose[t]->DeepCopy(transform->GetMatrixPointer());
	}
	
  ((mafVME *)m_Input)->SetTimeStamp(startTime);
  m_TargetVme->SetTimeStamp(startTime);
  m_OldParent->SetTimeStamp(startTime);

  for (t = 0; t < num; t++)
  {
    ((mafVME *)m_Input)->SetMatrix(*new_input_pose[t]);
  }
  if (m_Input->ReparentTo(m_TargetVme) == MAF_OK)
  {
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
  else
  {
    wxLogMessage("Something went wrong while reparenting (bad pointer or memory errors)"); 
  }
}
//----------------------------------------------------------------------------
void mmoReparentTo::OpUndo()
//----------------------------------------------------------------------------
{
  mafVME *tmp = m_TargetVme;
  m_TargetVme = m_OldParent;
  OpDo();
  m_TargetVme = tmp;
}
