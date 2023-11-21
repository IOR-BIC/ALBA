/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateAveragePlane
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved.

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

#include "albaOpCreateAveragePlane.h"
#include "albaDecl.h"
#include "albaEvent.h"

#include "albaVMERoot.h"
#include "albaVME.h"
#include "albaVMESurfaceParametric.h"
#include "albaVect3d.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateAveragePlane);


albaVME *glo_CurrentVME = NULL;

//----------------------------------------------------------------------------
albaOpCreateAveragePlane::albaOpCreateAveragePlane(const wxString &label) :
albaOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

}
//----------------------------------------------------------------------------
albaOpCreateAveragePlane::~albaOpCreateAveragePlane()
{

}
//----------------------------------------------------------------------------
albaOp* albaOpCreateAveragePlane::Copy()   
{
	return new albaOpCreateAveragePlane(m_Label);
}

//----------------------------------------------------------------------------
int albaOpCreateAveragePlane::PlaneAccept(albaVME *vme)
{
	albaVMESurfaceParametric *plane = albaVMESurfaceParametric::SafeDownCast(vme);
	if (plane && plane != glo_CurrentVME && plane->GetGeometryType() == albaVMESurfaceParametric::PARAMETRIC_PLANE)
		return true;
	else
		return false;
}

//----------------------------------------------------------------------------
bool albaOpCreateAveragePlane::InternalAccept(albaVME *node)
{
	return PlaneAccept(node);
}

//----------------------------------------------------------------------------
void albaOpCreateAveragePlane::OpRun()
{
	albaString title = _("Choose Plane");
	albaEvent e(this, VME_CHOOSE, &title);
	e.SetPointer(&PlaneAccept);

	glo_CurrentVME = m_Input;
	albaEventMacro(e);
	glo_CurrentVME = NULL;
	
	m_SelVME = e.GetVme();

	if (m_SelVME)
	{
		albaEventMacro(albaEvent(this, OP_RUN_OK));
	}
	else
	{
		albaEventMacro(albaEvent(this, OP_RUN_CANCEL));
	}
}

//----------------------------------------------------------------------------
void albaOpCreateAveragePlane::OpDo()
{
	
	albaVMESurfaceParametric *firstPlane = (albaVMESurfaceParametric *)m_Input;
	albaVMESurfaceParametric *secondPlane = (albaVMESurfaceParametric *)m_SelVME;

	albaVect3d pl1origin = firstPlane->GetPlaneOrigin();
	albaVect3d pl1p1 = firstPlane->GetPlanePoint1();
	albaVect3d pl1p2 = firstPlane->GetPlanePoint2();

	albaVect3d pl2origin = secondPlane->GetPlaneOrigin();
	albaVect3d pl2p1 = secondPlane->GetPlanePoint1();
	albaVect3d pl2p2 = secondPlane->GetPlanePoint2();

	albaMatrix *firstMatrix = firstPlane->GetOutput()->GetAbsMatrix();
	albaMatrix *secondMatrix = secondPlane->GetOutput()->GetAbsMatrix();

	pl1origin = firstMatrix->MultiplyPoint(pl1origin);
	pl1p1 = firstMatrix->MultiplyPoint(pl1p1);
	pl1p2 = firstMatrix->MultiplyPoint(pl1p2);

	pl2origin = secondMatrix->MultiplyPoint(pl2origin);
	pl2p1 = secondMatrix->MultiplyPoint(pl2p1);
	pl2p2 = secondMatrix->MultiplyPoint(pl2p2);

	albaVect3d outOrigin = (pl1origin + pl2origin) / 2.0;
	albaVect3d outP1 = (pl1p1 + pl2p1) / 2.0;
	albaVect3d outP2 = (pl1p2 + pl2p2) / 2.0;

	albaString plName;
	plName.Printf("Average [%s,%s]", firstPlane->GetName(), secondPlane->GetName());

	albaVMESurfaceParametric *outPlane;

	albaNEW(outPlane);
	outPlane->SetName(plName.GetCStr());
	outPlane->SetPlaneOrigin(outOrigin.GetVect());
	outPlane->SetPlanePoint1(outP1.GetVect());
	outPlane->SetPlanePoint2(outP2.GetVect());
	outPlane->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_PLANE);
	outPlane->ReparentTo(m_Input);
	m_Output = outPlane;
	albaDEL(outPlane);
			
	GetLogicManager()->VmeShow(m_Output, true);
	GetLogicManager()->CameraUpdate();

	return;
}

//----------------------------------------------------------------------------
void albaOpCreateAveragePlane::OpUndo()
{
	m_Output->ReparentTo(NULL);

	//Restore previous selection
	GetLogicManager()->VmeSelect(m_Input);
	GetLogicManager()->CameraUpdate();
}
