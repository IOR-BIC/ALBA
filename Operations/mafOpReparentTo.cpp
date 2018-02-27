/*=========================================================================

 Program: MAF2
 Module: mafOpReparentTo
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

#include "mafOpReparentTo.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMERoot.h"
#include "mmuTimeSet.h"
#include "mafTransformFrame.h"
#include "mafSmartPointer.h"
#include "mafVMELandmarkCloud.h"
#include "mafAbsMatrixPipe.h"
#include "mafView.h"

#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpReparentTo);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpReparentTo::mafOpReparentTo(const wxString &label) : mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType    = OPTYPE_OP;
  m_Canundo   = true;
  m_OldParent = NULL;
  m_TargetVme = NULL;

	glo_VmeForReparent = NULL;
}
//----------------------------------------------------------------------------
mafOpReparentTo::~mafOpReparentTo( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOpReparentTo::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
  return (node != NULL && node->IsMAFType(mafVME) && !node->IsMAFType(mafVMERoot) /*&& !node->IsMAFType(mafVMEExternalData)*/);
}
//----------------------------------------------------------------------------
mafOp* mafOpReparentTo::Copy()   
//----------------------------------------------------------------------------
{
  mafOpReparentTo *cp = new mafOpReparentTo(m_Label);
	cp->m_OldParent = m_OldParent;
  return cp;
}

//----------------------------------------------------------------------------
bool mafOpReparentTo::VMEAcceptForReparent(mafVME *vme)
{
	return (glo_VmeForReparent->GetParent() != vme && glo_VmeForReparent->CanReparentTo(vme));
}

//----------------------------------------------------------------------------
void mafOpReparentTo::OpRun()   
//----------------------------------------------------------------------------
{
	glo_VmeForReparent = m_Input;

  if (m_TargetVme == NULL)
  {
    mafEvent e(this,VME_CHOOSE);
		e.SetPointer(&VMEAcceptForReparent);
    mafEventMacro(e);
    m_TargetVme = e.GetVme();
  }
	
	int result = OP_RUN_CANCEL;
	if((m_TargetVme != NULL) && (m_Input->CanReparentTo(m_TargetVme)))
		result = OP_RUN_OK;

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mafOpReparentTo::SetTargetVme(mafVME *target)
//----------------------------------------------------------------------------
{
  m_TargetVme = target;

  if((m_TargetVme == NULL) || !m_Input->CanReparentTo(m_TargetVme))
  {
    mafMessage(_("Cannot re-parent to specified node"),_("Error"),wxICON_ERROR);
    mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
  }
}
//----------------------------------------------------------------------------
void mafOpReparentTo::OpDo()
//----------------------------------------------------------------------------
{
	m_OldParent = m_Input->GetParent();

	mafEvent e(this, VIEW_SELECTED);
	mafEventMacro(e);
	int showed = e.GetView() && e.GetView()->IsVmeShowed(m_Input);
	
	int reparentOK=ReparentTo(m_Input, m_TargetVme, m_OldParent);

  if (reparentOK == MAF_OK)
  {
		mafEventMacro(mafEvent(this, VME_SELECT, m_Input));

		if(showed)
	    GetLogicManager()->VmeShow(m_Input, true);
  }
  else
  {
    mafLogMessage(_("Something went wrong while re-parenting (bad pointer or memory errors)"));
  }
}

//----------------------------------------------------------------------------
int mafOpReparentTo::ReparentTo(mafVME * input, mafVME * targetVme, mafVME * oldParent)
{
	int num, t;
	mmuTimeVector input_time;
	mmuTimeVector target_time;
	mmuTimeVector time;
	mafTimeStamp cTime, startTime;

	input->GetAbsTimeStamps(input_time);
	targetVme->GetAbsTimeStamps(target_time);
	mmuTimeSet::Merge(input_time,target_time,time);
	num = time.size();

	startTime = targetVme->GetTimeStamp();

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

		input->SetTimeStamp(cTime);
		targetVme->SetTimeStamp(cTime);
		oldParent->SetTimeStamp(cTime);

		transform->SetTimeStamp(cTime);
		mafMatrixPipe *mp = input->GetMatrixPipe();
		if (mp == NULL)
		{
			transform->SetInput(input->GetOutput()->GetMatrix());
		}
		else
		{
			transform->SetInput(mp);
		}
		transform->SetInputFrame(oldParent->GetAbsMatrixPipe());
		transform->SetTargetFrame(targetVme->GetAbsMatrixPipe());
		transform->Update();

		new_input_pose[t]->DeepCopy(transform->GetMatrixPointer());
	}

	input->SetTimeStamp(startTime);
	targetVme->SetTimeStamp(startTime);
	oldParent->SetTimeStamp(startTime);

	for (t = 0; t < num; t++)
	{
		input->SetMatrix(*new_input_pose[t]);
	}
	
	return input->ReparentTo(targetVme);
}

//----------------------------------------------------------------------------
void mafOpReparentTo::OpUndo()
//----------------------------------------------------------------------------
{
  mafVME *tmp = m_TargetVme;
  m_TargetVme = m_OldParent;
  OpDo();
  m_TargetVme = tmp;
}
