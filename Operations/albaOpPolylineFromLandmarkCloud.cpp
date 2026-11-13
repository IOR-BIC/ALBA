/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpPolylineFromLandmarkCloud
 Authors: Gianluigi Crimi

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

#include "albaOpPolylineFromLandmarkCloud.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaVMEPolyline.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEOutput.h"
#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaSmartPointer.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpPolylineFromLandmarkCloud);

//----------------------------------------------------------------------------
albaOpPolylineFromLandmarkCloud::albaOpPolylineFromLandmarkCloud(const wxString &label) : albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
	m_Polyline = NULL;
}

//----------------------------------------------------------------------------
albaOpPolylineFromLandmarkCloud::~albaOpPolylineFromLandmarkCloud()
{
	albaDEL(m_Polyline);
}

//----------------------------------------------------------------------------
albaOp* albaOpPolylineFromLandmarkCloud::Copy()
{
	return new albaOpPolylineFromLandmarkCloud(m_Label);
}

//----------------------------------------------------------------------------
bool albaOpPolylineFromLandmarkCloud::InternalAccept(albaVME *node)
{
	albaVMELandmarkCloud *cloud = albaVMELandmarkCloud::SafeDownCast(node);
	return cloud && cloud->GetNumberOfLandmarks() >= 2;
}

//----------------------------------------------------------------------------
void albaOpPolylineFromLandmarkCloud::OpRun()
{
	albaVMELandmarkCloud *cloud = albaVMELandmarkCloud::SafeDownCast(m_Input);

	vtkALBASmartPointer<vtkPolyData> poly;
	CreatePolyData(cloud, poly);

	albaString name;
	name.Printf("%s Polyline", cloud->GetName());

	albaNEW(m_Polyline);
	m_Polyline->SetName(name.GetCStr());
	m_Polyline->SetData(poly, cloud->GetTimeStamp());
	// Points are in cloud coordinates: as a sibling, the polyline gets the same pose with the cloud matrix
	m_Polyline->SetMatrix(*cloud->GetOutput()->GetMatrix());
	m_Polyline->Update();
	m_Output = m_Polyline;

	albaEventMacro(albaEvent(this, OP_RUN_OK));
}

//----------------------------------------------------------------------------
void albaOpPolylineFromLandmarkCloud::OpDo()
{
	// A landmark cloud cannot be the root, so it always has a parent
	m_Polyline->ReparentTo(m_Input->GetParent());
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpPolylineFromLandmarkCloud::CreatePolyData(albaVMELandmarkCloud *cloud, vtkPolyData *poly)
{
	int numberOfLandmarks = cloud->GetNumberOfLandmarks();

	vtkALBASmartPointer<vtkPoints> points;
	vtkALBASmartPointer<vtkCellArray> lines;
	lines->InsertNextCell(numberOfLandmarks);

	// GetLandmark returns absolute positions: bring them back in cloud coordinates
	albaSmartPointer<albaTransform> absToCloud;
	absToCloud->SetMatrix(*cloud->GetOutput()->GetAbsMatrix());
	absToCloud->Invert();

	double xyz[3];
	for (int i = 0; i < numberOfLandmarks; i++)
	{
		cloud->GetLandmark(i, xyz, cloud->GetTimeStamp());
		absToCloud->TransformPoint(xyz, xyz);
		points->InsertNextPoint(xyz);
		lines->InsertCellPoint(i);
	}

	poly->SetPoints(points);
	poly->SetLines(lines);
}
//----------------------------------------------------------------------------
char **albaOpPolylineFromLandmarkCloud::GetIcon()
{
#include "pic/MENU_OP_CREATE_POLYLINE.xpm"
	return MENU_OP_CREATE_POLYLINE_xpm;
}