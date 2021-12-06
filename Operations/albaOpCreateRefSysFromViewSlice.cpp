/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateRefSysFromViewSliceFromViewSlice
 Authors: Nicola Vanella
 
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

#include "albaOpCreateRefSysFromViewSlice.h"
#include "albaDecl.h"

#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaVMERefSys.h"
#include "albaView.h"
#include "albaEvent.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"
#include "albaViewManager.h"
#include "albaViewArbitrarySlice.h"
#include "albaViewArbitraryOrthoSlice.h"
#include "albaTagArray.h"
#include "albaTagItem.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateRefSysFromViewSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateRefSysFromViewSlice::albaOpCreateRefSysFromViewSlice(const wxString &label) :
albaOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;  
  m_RefSys  = NULL;

	m_View = NULL;
}

//----------------------------------------------------------------------------
albaOpCreateRefSysFromViewSlice::~albaOpCreateRefSysFromViewSlice()
{
  albaDEL(m_RefSys);
}

//----------------------------------------------------------------------------
albaOp* albaOpCreateRefSysFromViewSlice::Copy()   
{
	return new albaOpCreateRefSysFromViewSlice(m_Label);
}

//----------------------------------------------------------------------------
bool albaOpCreateRefSysFromViewSlice::InternalAccept(albaVME*node)
{
	bool hasView = false;

	albaEvent e(this, VIEW_SELECTED);
	albaEventMacro(e);
	if (e.GetBool())
	{	
		hasView = e.GetView()->IsA("albaViewArbitraryOrthoSlice") || e.GetView()->IsA("albaViewArbitrarySlice");
	}

	return (hasView && node && node->IsALBAType(albaVME));
}

//----------------------------------------------------------------------------
void albaOpCreateRefSysFromViewSlice::OpRun()   
{
	m_View = GetLogicManager()->GetViewManager()->GetSelectedView();

	if (m_View && (m_View->IsA("albaViewArbitraryOrthoSlice") || m_View->IsA("albaViewArbitrarySlice")))
	{
		albaNEW(m_RefSys);
		m_RefSys->SetName("Reference System");
		m_Output = m_RefSys;
		albaEventMacro(albaEvent(this, OP_RUN_OK));
	}
}

//----------------------------------------------------------------------------
void albaOpCreateRefSysFromViewSlice::OpDo()
{
	if (m_RefSys)
	{
		m_RefSys->ReparentTo(m_Input);
		albaMatrix *matrix;

		if(m_View->IsA("albaViewArbitrarySlice"))
		{ 
			matrix= ((albaViewArbitrarySlice *)m_View)->GetSlicerMatrix();
			((albaViewArbitrarySlice *)m_View)->SetRestoreTagToVME(m_RefSys);
		}
		else if (m_View->IsA("albaViewArbitraryOrthoSlice"))
		{
			matrix = ((albaViewArbitraryOrthoSlice *)m_View)->GetSlicerMatrix();
			((albaViewArbitraryOrthoSlice *)m_View)->SetRestoreTagToVME(m_RefSys);
		}

		m_RefSys->SetAbsMatrix(*matrix);
		m_RefSys->SetScaleFactor(100.0);
		GetLogicManager()->VmeShow(m_RefSys, true);
	}
}
