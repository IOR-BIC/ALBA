/*=========================================================================

 Program: MAF2
 Module: mafPipeSurfaceEditor
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

#include "mafPipeSurfaceEditor.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mafVME.h"
#include "mafVMEOutputSurfaceEditor.h"
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
mafCxxTypeMacro(mafPipeSurfaceEditor);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeSurfaceEditor::mafPipeSurfaceEditor()
:mafPipe()
//----------------------------------------------------------------------------
{
	m_LUT = NULL;
	m_Actor = NULL;
	m_Mapper = NULL;
}
//----------------------------------------------------------------------------
void mafPipeSurfaceEditor::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	m_Selected = false;

	mafVMEOutputSurfaceEditor *out_polyline = mafVMEOutputSurfaceEditor::SafeDownCast(m_Vme->GetOutput());
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
mafPipeSurfaceEditor::~mafPipeSurfaceEditor()
//----------------------------------------------------------------------------
{
	m_AssemblyFront->RemovePart(m_Actor);
	m_Vme->GetEventSource()->RemoveObserver(this);

	vtkDEL(m_Actor);
	vtkDEL(m_Mapper);
	vtkDEL(m_LUT);
}
//----------------------------------------------------------------------------
void mafPipeSurfaceEditor::Show(bool show)
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void mafPipeSurfaceEditor::Select(bool sel)
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
mafGUI *mafPipeSurfaceEditor::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new mafGUI(this);

	return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeSurfaceEditor::OnEvent(mafEventBase *maf_event)
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