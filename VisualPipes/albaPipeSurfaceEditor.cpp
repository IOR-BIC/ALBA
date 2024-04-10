/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurfaceEditor
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

#include "albaPipeSurfaceEditor.h"

#include "albaDecl.h"
#include "albaGUI.h"

#include "albaVME.h"
#include "albaVMEOutputSurfaceEditor.h"
#include "mmaMaterial.h"

#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"

#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkLookupTable.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkCellData.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeSurfaceEditor);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeSurfaceEditor::albaPipeSurfaceEditor()
:albaPipe()
//----------------------------------------------------------------------------
{
	m_LUT = NULL;
	m_Actor = NULL;
	m_Mapper = NULL;
}
//----------------------------------------------------------------------------
void albaPipeSurfaceEditor::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	m_Selected = false;

	albaVMEOutputSurfaceEditor *out_polyline = albaVMEOutputSurfaceEditor::SafeDownCast(m_Vme->GetOutput());
	assert(out_polyline);
	m_Vme->AddObserver(this);
	vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
	assert(data);

	double range[2];
	data->GetScalarRange(range);

	vtkNEW(m_LUT);
	m_LUT->SetTableValue(0.0, 1.0, 1.0, 1.0, 1.0);
	//m_LUT->SetTableValue(1.0, 1.0, 0.0, 0.0, 1.0);
	m_LUT->Build();

	vtkNEW(m_Mapper);
	m_Mapper->SetInputData(data);
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
albaPipeSurfaceEditor::~albaPipeSurfaceEditor()
//----------------------------------------------------------------------------
{
	m_AssemblyFront->RemovePart(m_Actor);
	m_Vme->RemoveObserver(this);

	vtkDEL(m_Actor);
	vtkDEL(m_Mapper);
	vtkDEL(m_LUT);
}
//----------------------------------------------------------------------------
void albaPipeSurfaceEditor::Show(bool show)
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void albaPipeSurfaceEditor::Select(bool sel)
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
albaGUI *albaPipeSurfaceEditor::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new albaGUI(this);

	return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeSurfaceEditor::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		/*switch(e->GetId()) 
		{
		default:
			{*/
				albaEventMacro(*e);
			//}
		//}
	}
	else if (alba_event->GetSender() == m_Vme)
	{
		if(alba_event->GetId() == VME_OUTPUT_DATA_UPDATE)
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
			GetLogicManager()->CameraUpdate();
		}
	}
}