/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medPipeSurfaceEditor.cpp,v $
Language:  C++
Date:      $Date: 2008-07-25 11:19:42 $
Version:   $Revision: 1.5 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/


#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medPipeSurfaceEditor.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mafVME.h"
#include "medVMEOutputSurfaceEditor.h"
#include "mafEventSource.h"
#include "mmaMaterial.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"

#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkLookupTable.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkCellData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeSurfaceEditor);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeSurfaceEditor::medPipeSurfaceEditor()
:mafPipe()
//----------------------------------------------------------------------------
{
	m_LUT = NULL;
	m_Actor = NULL;
	m_Mapper = NULL;
}
//----------------------------------------------------------------------------
void medPipeSurfaceEditor::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	m_Selected = false;

	medVMEOutputSurfaceEditor *out_polyline = medVMEOutputSurfaceEditor::SafeDownCast(m_Vme->GetOutput());
	assert(out_polyline);
	m_Vme->GetEventSource()->AddObserver(this);
	vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
	assert(data);
	data->Update();

	double range[2];
	data->GetScalarRange(range);

	vtkNEW(m_LUT);
	m_LUT->SetTableValue(0.0, 1.0, 1.0, 1.0, 1.0);
	//m_LUT->SetTableValue(1.0, 1.0, 0.0, 0.0, 1.0);
	m_LUT->Build();

	vtkNEW(m_Mapper);
	m_Mapper->SetInput(data);
	m_Mapper->SetLookupTable(out_polyline->GetMaterial()->m_ColorLut);
	m_Mapper->SetScalarRange(range);
	m_Mapper->ScalarVisibilityOn();
	if(data->GetPointData()->GetScalars())
		m_Mapper->SetScalarModeToUsePointData();
	if(data->GetCellData()->GetScalars())
		m_Mapper->SetScalarModeToUseCellData();
	m_Mapper->Modified();

	vtkNEW(m_Actor);
	m_Actor->SetMapper(m_Mapper);
	m_AssemblyFront->AddPart(m_Actor);
}
//----------------------------------------------------------------------------
medPipeSurfaceEditor::~medPipeSurfaceEditor()
//----------------------------------------------------------------------------
{
	m_AssemblyFront->RemovePart(m_Actor);
	m_Vme->GetEventSource()->RemoveObserver(this);

	vtkDEL(m_Actor);
	vtkDEL(m_Mapper);
	vtkDEL(m_LUT);
}
//----------------------------------------------------------------------------
void medPipeSurfaceEditor::Show(bool show)
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void medPipeSurfaceEditor::Select(bool sel)
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
mafGUI *medPipeSurfaceEditor::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new mafGUI(this);

	return m_Gui;
}
//----------------------------------------------------------------------------
void medPipeSurfaceEditor::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		/*switch(e->GetId()) 
		{
		default:
			{*/
				mafEventMacro(*e);
			//}
		//}
	}
	else if (maf_event->GetSender() == m_Vme)
	{
		if(maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
		{
			vtkPolyData *data = (vtkPolyData *)m_Vme->GetOutput()->GetVTKData();
			assert(data);
			double range[2];
			data->GetScalarRange(range);
			m_LUT->SetNumberOfColors(4);
			double step=(range[1]-range[0])/3;
			//m_LUT->SetTableValue(range[0], 0.0, 0.0, 1.0, 1.0);
			m_LUT->SetTableValue(range[0], 1.0, 1.0, 1.0, 1.0);
			m_LUT->SetTableValue(range[0]+step, 0.0, 0.0, 1.0, 1.0);
			m_LUT->SetTableValue(range[0]+2*step, 0.0, 1.0, 0.0, 1.0);
			m_LUT->SetTableValue(range[1], 1.0, 0.0, 0.0, 1.0);
			m_LUT->Build();
			//m_Mapper->SetLookupTable(m_LUT);
			m_Mapper->SetScalarRange(range);
			m_Mapper->Update();
			mafEventMacro(mafEvent(this,CAMERA_UPDATE));
		}
	}
}