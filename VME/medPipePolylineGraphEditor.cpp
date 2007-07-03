/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medPipePolylineGraphEditor.cpp,v $
Language:  C++
Date:      $Date: 2007-07-03 09:59:44 $
Version:   $Revision: 1.1 $
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
#include "medVMEOutputEditor.h"

#include "vtkMAFAssembly.h"

#include "vtkPolyData.h"
#include "vtkSphereSource.h"
#include "vtkGlyph3D.h"
#include "vtkTubeFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkAppendPolyData.h"
#include "vtkActor.h"
#include "vtkLookupTable.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkCellData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipePolylineGraphEditor);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipePolylineGraphEditor::medPipePolylineGraphEditor()
:mafPipe()
//----------------------------------------------------------------------------
{
	m_SphereRadius = 2.0;
	m_TubeRadius	 = 1.0;
}
//----------------------------------------------------------------------------
void medPipePolylineGraphEditor::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	m_Selected = false;

	medVMEOutputEditor *out_polyline = medVMEOutputEditor::SafeDownCast(m_Vme->GetOutput());
	assert(out_polyline);
	vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
	assert(data);
	data->Update();

	double range[2]={0.0,1.0};

	m_LUT = vtkLookupTable::New();
	m_LUT->SetNumberOfColors(2);
	m_LUT->SetTableValue(0.0, 1.0, 1.0, 1.0, 1.0);
	m_LUT->SetTableValue(1.0, 1.0, 0.0, 0.0, 1.0);
	m_LUT->Build();
	//if(data->GetPointData()->GetScalars())
		//data->GetPointData()->GetScalars()->SetLookupTable(m_LUT);

	vtkNEW(m_Sphere);
	m_Sphere->SetRadius(m_SphereRadius);
	m_Sphere->SetPhiResolution(10);
	m_Sphere->SetThetaResolution(10);

	vtkNEW(m_Glyph);
	m_Glyph->SetInput(data);
	m_Glyph->SetSource(m_Sphere->GetOutput());
	m_Glyph->SetScaleModeToDataScalingOff();
	m_Glyph->SetRange(0.0,1.0);
	m_Glyph->Update();

	vtkNEW(m_Tube);
	m_Tube->UseDefaultNormalOff();
	m_Tube->SetInput(data);
	m_Tube->SetRadius(m_TubeRadius);
	m_Tube->SetCapping(true);
	m_Tube->SetNumberOfSides(10);

	vtkNEW(m_AppendPolydata);
	m_AppendPolydata->AddInput(m_Tube->GetOutput());
	m_AppendPolydata->AddInput(m_Glyph->GetOutput());
	m_AppendPolydata->Update();

	vtkNEW(m_MapperSphere);
	m_MapperSphere->SetInput(data);
	m_MapperSphere->SetLookupTable(m_LUT);
	m_MapperSphere->SetScalarRange(range);
	if(data->GetPointData()->GetScalars())
		m_MapperSphere->SetScalarModeToUsePointData();
	else if(data->GetCellData()->GetScalars())
		m_MapperSphere->SetScalarModeToUseCellData();
	m_MapperSphere->Modified();

	vtkNEW(m_MapperTube);
	m_MapperTube->SetInput(m_Tube->GetOutput());
	/*m_MapperTube->SetLookupTable(m_LUT);
	m_MapperTube->SetScalarRange(data->GetPointData()->GetScalars()->GetRange());
	m_MapperTube->SetScalarModeToUsePointData();*/
	m_MapperTube->SetScalarModeToUseCellData();
	m_MapperTube->Modified();

	vtkNEW(m_ActorSphere);
	m_ActorSphere->SetMapper(m_MapperSphere);
	m_AssemblyFront->AddPart(m_ActorSphere);

	vtkNEW(m_ActorTube);
	m_ActorTube->SetMapper(m_MapperTube);
	//m_AssemblyFront->AddPart(m_ActorTube);

}
//----------------------------------------------------------------------------
medPipePolylineGraphEditor::~medPipePolylineGraphEditor()
//----------------------------------------------------------------------------
{

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
void medPipePolylineGraphEditor::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId()) 
		{
		default:
			{
				mafEventMacro(*e);
			}
		}
	}
}