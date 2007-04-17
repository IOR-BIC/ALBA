/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeMesh.cpp,v $
Language:  C++
Date:      $Date: 2007-04-17 10:17:06 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi - Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafPipeMesh.h"
#include "mafSceneNode.h"
#include "mmgGui.h"
#include "mmgMaterialButton.h"
#include "mmgLutPreset.h"
#include "mafAxes.h"
#include "mmaMaterial.h"
#include "mafVMEOutputMesh.h"

#include "mafDataVector.h"
#include "mafVMEGenericAbstract.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"

#include "vtkPointData.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataSetMapper.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkRenderer.h"

#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeMesh);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeMesh::mafPipeMesh()
:mafPipe()
//----------------------------------------------------------------------------
{
	m_Mapper          = NULL;
	m_Actor           = NULL;
	m_OutlineBox      = NULL;
	m_OutlineMapper   = NULL;
	m_OutlineProperty = NULL;
	m_OutlineActor    = NULL;
	m_Gui             = NULL;
}
//----------------------------------------------------------------------------
void mafPipeMesh::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	m_Selected = false;
	m_Mapper          = NULL;
	m_Actor           = NULL;
	m_OutlineBox      = NULL;
	m_OutlineMapper   = NULL;
	m_OutlineProperty = NULL;
	m_OutlineActor    = NULL;
	m_Axes            = NULL;

	m_Vme->Update();
	assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputMesh));
	mafVMEOutputMesh *mesh_output = mafVMEOutputMesh::SafeDownCast(m_Vme->GetOutput());
	assert(mesh_output);
	mesh_output->Update();
	vtkUnstructuredGrid *data = vtkUnstructuredGrid::SafeDownCast(mesh_output->GetVTKData());
	assert(data);
	data->Update();
	vtkDataArray *scalars = data->GetPointData()->GetScalars();
	double sr[2] = {0,1};

	vtkNEW(m_Mapper);
	m_Mapper->SetInput(data);
	m_Mapper->SetScalarRange(sr);

	vtkNEW(m_Actor);
	m_Actor->SetMapper(m_Mapper);

	m_AssemblyFront->AddPart(m_Actor);

	// selection highlight
	vtkNEW(m_OutlineBox);
	m_OutlineBox->SetInput(data);  

	vtkNEW(m_OutlineMapper);
	m_OutlineMapper->SetInput(m_OutlineBox->GetOutput());

	vtkNEW(m_OutlineProperty);
	m_OutlineProperty->SetColor(1,1,1);
	m_OutlineProperty->SetAmbient(1);
	m_OutlineProperty->SetRepresentationToWireframe();
	m_OutlineProperty->SetInterpolationToFlat();

	vtkNEW(m_OutlineActor);
	m_OutlineActor->SetMapper(m_OutlineMapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(m_OutlineProperty);

	m_AssemblyFront->AddPart(m_OutlineActor);

	m_Axes = new mafAxes(m_RenFront, m_Vme);
}
//----------------------------------------------------------------------------
mafPipeMesh::~mafPipeMesh()
//----------------------------------------------------------------------------
{
	m_AssemblyFront->RemovePart(m_Actor);
	m_AssemblyFront->RemovePart(m_OutlineActor);

	vtkDEL(m_Mapper);
	vtkDEL(m_Actor);
	vtkDEL(m_OutlineBox);
	vtkDEL(m_OutlineMapper);
	vtkDEL(m_OutlineProperty);
	vtkDEL(m_OutlineActor);
	cppDEL(m_Axes);
}
//----------------------------------------------------------------------------
void mafPipeMesh::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
void mafPipeMesh::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mmgGui *mafPipeMesh::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new mmgGui(this);
	m_Gui->Label("");
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeMesh::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId()) 
		{
			default:
				mafEventMacro(*e);
				break;
		}
	}
}
//----------------------------------------------------------------------------
void mafPipeMesh::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
	m_Actor->SetPickable(enable);
	m_Actor->Modified();
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
