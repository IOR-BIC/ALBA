/*=========================================================================

 Program: MAF2Medical
 Module: medOpCreateSurface
 Authors: Matteo Giacomoni
 
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

#include "medOpCreateSurface.h"

#include "mafGUI.h"

#include "mafNode.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMESurface.h"

#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpCreateSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpCreateSurface::medOpCreateSurface(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = true;
	m_InputPreserving = true;
	m_Surface = NULL;

}
//----------------------------------------------------------------------------
medOpCreateSurface::~medOpCreateSurface()
//----------------------------------------------------------------------------
{
	mafDEL(m_Surface);
}
//----------------------------------------------------------------------------
mafOp* medOpCreateSurface::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new medOpCreateSurface(m_Label);
}
//----------------------------------------------------------------------------
bool medOpCreateSurface::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	return vme != NULL && vme->IsMAFType(mafVMESurfaceParametric);
}
//----------------------------------------------------------------------------
void medOpCreateSurface::OpRun()
//----------------------------------------------------------------------------
{
	mafVMESurfaceParametric *inputSurface=mafVMESurfaceParametric::SafeDownCast(m_Input);
	
	mafNEW(m_Surface);
	m_Surface->SetName(inputSurface->GetName());
	m_Surface->SetData(vtkPolyData::SafeDownCast(inputSurface->GetOutput()->GetVTKData()),inputSurface->GetTimeStamp());
	m_Surface->Update();

	m_Output=m_Surface;
	
	OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void medOpCreateSurface::OpDo()   
//----------------------------------------------------------------------------
{
	if (m_Output)
	{
		m_Output->ReparentTo(m_Input);
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void medOpCreateSurface::OpStop(int result)   
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,result));
}