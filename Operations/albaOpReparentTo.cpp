/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpReparentTo
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
#include "albaOpReparentTo.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaVMERoot.h"
#include "mmuTimeSet.h"
#include "albaTransformFrame.h"
#include "albaSmartPointer.h"
#include "albaVMELandmarkCloud.h"
#include "albaAbsMatrixPipe.h"
#include "albaView.h"

#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include <vector>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpReparentTo);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpReparentTo::albaOpReparentTo(const wxString &label) : albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType    = OPTYPE_OP;
  m_Canundo   = true;
  m_OldParent = NULL;
  m_TargetVme = NULL;

	glo_VmeForReparent = NULL;
}
//----------------------------------------------------------------------------
albaOpReparentTo::~albaOpReparentTo( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool albaOpReparentTo::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node != NULL && node->IsALBAType(albaVME) && !node->IsALBAType(albaVMERoot) /*&& !node->IsALBAType(albaVMEExternalData)*/);
}
//----------------------------------------------------------------------------
albaOp* albaOpReparentTo::Copy()   
//----------------------------------------------------------------------------
{
  albaOpReparentTo *cp = new albaOpReparentTo(m_Label);
	cp->m_OldParent = m_OldParent;
  return cp;
}

//----------------------------------------------------------------------------
bool albaOpReparentTo::VMEAcceptForReparent(albaVME *vme)
{
	return (glo_VmeForReparent->GetParent() != vme && glo_VmeForReparent->CanReparentTo(vme));
}

//----------------------------------------------------------------------------
void albaOpReparentTo::OpRun()   
//----------------------------------------------------------------------------
{
	glo_VmeForReparent = m_Input;

  if (m_TargetVme == NULL)
  {
    albaEvent e(this,VME_CHOOSE);
		e.SetPointer(&VMEAcceptForReparent);
    albaEventMacro(e);
    m_TargetVme = e.GetVme();
  }
	
	int result = OP_RUN_CANCEL;
	if((m_TargetVme != NULL) && (m_Input->CanReparentTo(m_TargetVme)))
		result = OP_RUN_OK;

	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpReparentTo::SetTargetVme(albaVME *target)
//----------------------------------------------------------------------------
{
  m_TargetVme = target;

  if((m_TargetVme == NULL) || !m_Input->CanReparentTo(m_TargetVme))
  {
    albaMessage(_("Cannot re-parent to specified node"),_("Error"),wxICON_ERROR);
    albaEventMacro(albaEvent(this,OP_RUN_CANCEL));
  }
}
//----------------------------------------------------------------------------
void albaOpReparentTo::OpDo()
//----------------------------------------------------------------------------
{
	m_OldParent = m_Input->GetParent();

	albaEvent e(this, VIEW_SELECTED);
	albaEventMacro(e);
	int showed = e.GetView() && e.GetView()->IsVmeShowed(m_Input);
	
	int reparentOK=ReparentTo(m_Input, m_TargetVme, m_OldParent);

  if (reparentOK == ALBA_OK)
  {
		albaEventMacro(albaEvent(this, VME_SELECT, m_Input));

		if(showed)
	    GetLogicManager()->VmeShow(m_Input, true);
  }
  else
  {
    albaLogMessage(_("Something went wrong while re-parenting (bad pointer or memory errors)"));
  }
}

//----------------------------------------------------------------------------
int albaOpReparentTo::ReparentTo(albaVME * input, albaVME * targetVme, albaVME * oldParent)
{
	int num, t;
	mmuTimeVector input_time;
	mmuTimeVector target_time;
	mmuTimeVector time;
	albaTimeStamp cTime, startTime;

	input->GetAbsTimeStamps(input_time);
	targetVme->GetAbsTimeStamps(target_time);
	mmuTimeSet::Merge(input_time,target_time,time);
	num = time.size();

	startTime = targetVme->GetTimeStamp();

	std::vector< albaAutoPointer<albaMatrix> > new_input_pose;
	new_input_pose.resize(num);

	for (t = 0; t < num; t++)
	{
		new_input_pose[t] = albaMatrix::New();
	}

	//change reference system
	albaSmartPointer<albaTransformFrame> transform;
	for (t = 0; t < num; t++)
	{
		cTime = time[t];

		input->SetTimeStamp(cTime);
		targetVme->SetTimeStamp(cTime);
		oldParent->SetTimeStamp(cTime);

		transform->SetTimeStamp(cTime);
		albaMatrixPipe *mp = input->GetMatrixPipe();
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
void albaOpReparentTo::OpUndo()
//----------------------------------------------------------------------------
{
  albaVME *tmp = m_TargetVme;
  m_TargetVme = m_OldParent;
  OpDo();
  m_TargetVme = tmp;
}
