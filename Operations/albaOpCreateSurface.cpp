/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSurface
 Authors: Matteo Giacomoni
 
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

#include "albaOpCreateSurface.h"

#include "albaGUI.h"

#include "albaVME.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMESurface.h"

#include "vtkPolyData.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateSurface::albaOpCreateSurface(wxString label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = true;
	m_InputPreserving = true;
	m_Surface = NULL;

}
//----------------------------------------------------------------------------
albaOpCreateSurface::~albaOpCreateSurface()
//----------------------------------------------------------------------------
{
	albaDEL(m_Surface);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateSurface::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new albaOpCreateSurface(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateSurface::InternalAccept(albaVME* vme)
//----------------------------------------------------------------------------
{
	return vme != NULL && vme->IsALBAType(albaVMESurfaceParametric);
}
//----------------------------------------------------------------------------
void albaOpCreateSurface::OpRun()
//----------------------------------------------------------------------------
{
	albaVMESurfaceParametric *inputSurface=albaVMESurfaceParametric::SafeDownCast(m_Input);
	
	albaNEW(m_Surface);
	m_Surface->SetName(inputSurface->GetName());
	m_Surface->SetData(vtkPolyData::SafeDownCast(inputSurface->GetOutput()->GetVTKData()),inputSurface->GetTimeStamp());
	m_Surface->Update();

	m_Output=m_Surface;
	
	OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void albaOpCreateSurface::OpDo()   
//----------------------------------------------------------------------------
{
	if (m_Output)
	{
		m_Output->ReparentTo(m_Input);
		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaOpCreateSurface::OpStop(int result)   
//----------------------------------------------------------------------------
{
	albaEventMacro(albaEvent(this,result));
}