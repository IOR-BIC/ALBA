/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medPipePolylineGraphEditor.cpp,v $
Language:  C++
Date:      $Date: 2008-03-20 11:30:47 $
Version:   $Revision: 1.6 $
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

#include "medPipePolylineGraphEditor.h"

#include "mafDecl.h"
#include "mmgGui.h"

#include "mafVME.h"
#include "medVMEOutputPolylineEditor.h"

#include "mafAbsMatrixPipe.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"

#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkLookupTable.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkCellData.h"
#include "vtkFixedCutter.h"
#include "vtkPlane.h"
#include "vtkMAFToLinearTransform.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipePolylineGraphEditor);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipePolylineGraphEditor::medPipePolylineGraphEditor()
:mafPipe()
//----------------------------------------------------------------------------
{

	m_Origin[0] = 0;
	m_Origin[1] = 0;
	m_Origin[2] = 0;

	m_Normal[0] = 0;
	m_Normal[1] = 0;
	m_Normal[2] = 0;

	m_Cutter = NULL;
	m_Mapper = NULL;

	m_Modality = ID_PERSPECTIVE;
}
//----------------------------------------------------------------------------
void medPipePolylineGraphEditor::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	m_Selected = false;

	medVMEOutputPolylineEditor *out_polyline = medVMEOutputPolylineEditor::SafeDownCast(m_Vme->GetOutput());
	assert(out_polyline);
	vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
	assert(data);
	data->Update();

	double range[2]={0.0,1.0};

	vtkNEW(m_LUT);
	m_LUT->SetNumberOfColors(2);
	m_LUT->SetTableValue(0.0, 1.0, 1.0, 0.0, 1.0);
	m_LUT->SetTableValue(1.0, 1.0, 0.0, 0.0, 1.0);
	m_LUT->Build();

	vtkNEW(m_Plane);
	vtkNEW(m_Cutter);

	m_Plane->SetOrigin(m_Origin);
	m_Plane->SetNormal(m_Normal);
	vtkMAFSmartPointer<vtkMAFToLinearTransform>VTKTransform;
	VTKTransform->SetInputMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrixPointer());
	m_Plane->SetTransform(VTKTransform);

	m_Cutter->SetInput(data);
	m_Cutter->SetCutFunction(m_Plane);
	m_Cutter->Update();

	vtkNEW(m_Mapper);
	if(m_Modality==ID_SLICE)
		m_Mapper->SetInput(m_Cutter->GetOutput());
	else if(m_Modality==ID_PERSPECTIVE)
		m_Mapper->SetInput(data);
	m_Mapper->SetLookupTable(m_LUT);
	m_Mapper->SetScalarRange(range);
	if(data->GetPointData()->GetScalars())
		m_Mapper->SetScalarModeToUsePointData();
	else if(data->GetCellData()->GetScalars())
		m_Mapper->SetScalarModeToUseCellData();
	m_Mapper->Modified();

	vtkNEW(m_Actor);
  m_Actor->PickableOff();
	m_Actor->SetMapper(m_Mapper);
	m_AssemblyFront->AddPart(m_Actor);
}
//----------------------------------------------------------------------------
medPipePolylineGraphEditor::~medPipePolylineGraphEditor()
//----------------------------------------------------------------------------
{
	m_AssemblyFront->RemovePart(m_Actor);

	vtkDEL(m_Actor);
	vtkDEL(m_Mapper);
	vtkDEL(m_Plane);
	vtkDEL(m_Cutter);
	vtkDEL(m_LUT);
}
//----------------------------------------------------------------------------
void medPipePolylineGraphEditor::Show(bool show)
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void medPipePolylineGraphEditor::Select(bool sel)
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
mmgGui *medPipePolylineGraphEditor::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new mmgGui(this);

	return m_Gui;
}
//----------------------------------------------------------------------------
void medPipePolylineGraphEditor::SetModalityPerspective()
//----------------------------------------------------------------------------
{
	m_Modality = ID_PERSPECTIVE;

	if(m_Mapper)
	{	
		medVMEOutputPolylineEditor *out_polyline = medVMEOutputPolylineEditor::SafeDownCast(m_Vme->GetOutput());
		assert(out_polyline);
		vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
		assert(data);
		data->Update();

		m_Mapper->SetInput(data);
		m_Mapper->Update();
	}
}
//----------------------------------------------------------------------------
void medPipePolylineGraphEditor::SetModalitySlice()
//----------------------------------------------------------------------------
{
	m_Modality = ID_SLICE;

	if(m_Mapper)
	{
		m_Mapper->SetInput(m_Cutter->GetOutput());
		m_Mapper->Update();
	}
}
//----------------------------------------------------------------------------
void medPipePolylineGraphEditor::SetSlice(double *Origin)
//----------------------------------------------------------------------------
{
	m_Origin[0] = Origin[0];
	m_Origin[1] = Origin[1];
	m_Origin[2] = Origin[2];

	if(m_Plane && m_Cutter)
	{
		m_Plane->SetOrigin(m_Origin);
		m_Cutter->SetCutFunction(m_Plane);
		m_Cutter->Update();
	}
}
//----------------------------------------------------------------------------
void medPipePolylineGraphEditor::SetNormal(double *Normal)
//----------------------------------------------------------------------------
{
	m_Normal[0] = Normal[0];
	m_Normal[1] = Normal[1];
	m_Normal[2] = Normal[2];


	if(m_Plane && m_Cutter)
	{
		m_Plane->SetNormal(m_Normal);
		m_Cutter->SetCutFunction(m_Plane);
		m_Cutter->Update();
	}
}
//----------------------------------------------------------------------------
void medPipePolylineGraphEditor::OnEvent(mafEventBase *maf_event)
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
}
