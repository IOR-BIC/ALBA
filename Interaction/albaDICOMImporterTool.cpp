/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDICOMImporterTool
 Authors: Gianluigi Crimi, from albaInteractorDICOMImporter by Paolo Quadrani, Stefano Perticoni

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

#include "albaDICOMImporterTool.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"
#include "albaRWIBase.h"

#include "vtkActor.h"
#include "vtkALBASmartPointer.h"
#include "vtkOutlineFilter.h"
#include "vtkPlaneSource.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

enum DICOM_IMPORTER_MODALITY
{
	NORMAL_MODALITY,
	RESIZING_MODALITY,
	DRAG_MODALITY,
};
//	  8------------1----------2--->x
//		|												|
//		7												3
//		|												|
//		6------------5----------4
//		|
//	  v y
enum SIDE_DRAGGING
{
	NO_DRAGGING,
	NEW_BOX,
	UP_SIDE,
	UP_RIGHT_SIDE,
	RIGHT_SIDE,
	BOTTOM_RIGHT_SIDE,
	BOTTOM_SIDE,
	BOTTOM_LEFT_SIDE,
	LEFT_SIDE,
	UP_LEFT_SIDE
};

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaDICOMImporterTool);

//------------------------------------------------------------------------------
albaDICOMImporterTool::albaDICOMImporterTool()
{
	vtkNEW(m_CropPlane);

	vtkALBASmartPointer<vtkOutlineFilter>	of;
	of->SetInputConnection(m_CropPlane->GetOutputPort());

	vtkALBASmartPointer<vtkPolyDataMapper> pdm;
	pdm->SetInputConnection(of->GetOutputPort());

	vtkNEW(m_CropActor);
	m_CropActor->GetProperty()->SetColor(0.8, 0, 0);
	m_CropActor->VisibilityOff();
	m_CropActor->SetMapper(pdm);

	m_SideToBeDragged = 0;
	m_GizmoStatus = NORMAL_MODALITY;
	m_SliceSize[0] = m_SliceSize[1] = 0;
	m_Delta = 0;
	m_CurrentArrow = wxCURSOR_NONE;
}

//------------------------------------------------------------------------------
albaDICOMImporterTool::~albaDICOMImporterTool()
{
	if (m_Renderer && m_CropActor)
		m_Renderer->RemoveActor(m_CropActor);

	vtkDEL(m_CropPlane);
	vtkDEL(m_CropActor);
}

//------------------------------------------------------------------------------
bool albaDICOMImporterTool::OnMove(albaEventInteraction *e)
{
	if (GetEventRenderer(e) == NULL)
		return false;

	double mousePos[2], pos[4];
	e->Get2DPosition(mousePos);
	ComputeDisplayToWorld((int)mousePos[0], (int)mousePos[1], -1, pos);
	CalculateSideDragged(pos);

	albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)e->GetSender());
	UpdateCursor(mouse ? mouse->GetRWI() : NULL);

	m_Renderer->GetRenderWindow()->Render();
	return true;
}

//------------------------------------------------------------------------------
bool albaDICOMImporterTool::OnLeftButtonDown(albaEventInteraction *e)
{
	if (GetEventRenderer(e) == NULL)
		return false;

	double mousePos[2], pos[4];
	e->Get2DPosition(mousePos);
	ComputeDisplayToWorld((int)mousePos[0], (int)mousePos[1], -1, pos);

	CalculateSideDragged(pos);

	if (m_SideToBeDragged == NEW_BOX)
	{
		m_GizmoStatus = RESIZING_MODALITY;
		pos[2] = 0;
		m_CropPlane->SetOrigin(pos);
		m_CropPlane->SetPoint1(pos[0], pos[1], pos[2]);
		m_CropPlane->SetPoint2(pos[0], pos[1], pos[2]);
	}
	else if (m_SideToBeDragged != NO_DRAGGING)
		m_GizmoStatus = DRAG_MODALITY;

	m_Renderer->GetRenderWindow()->Render();

	// the drags not used to edit the rectangle move the camera
	return true;
}

//------------------------------------------------------------------------------
bool albaDICOMImporterTool::OnDrag(albaEventInteraction *e)
{
	if (m_GizmoStatus == NORMAL_MODALITY)
		return false;

	double mousePos[2];
	e->Get2DPosition(mousePos);
	MoveCropPlane((int)mousePos[0], (int)mousePos[1]);
	return true;
}

//------------------------------------------------------------------------------
bool albaDICOMImporterTool::OnLeftButtonUp(albaEventInteraction *e)
{
	m_GizmoStatus = NORMAL_MODALITY;

	double *bounds = m_CropPlane->GetOutput()->GetBounds();
	if (bounds[0] == bounds[1] || bounds[2] == bounds[3] || (bounds[0] == 1.0 && bounds[1] == -1))
		SetPlaneFromSliceSize();
	else
	{
		//avoid negative vectors
		m_CropPlane->SetOrigin(bounds[0], bounds[2], 0);
		m_CropPlane->SetPoint1(bounds[1], bounds[2], 0);
		m_CropPlane->SetPoint2(bounds[0], bounds[3], 0);
	}
	return true;
}

//------------------------------------------------------------------------------
void albaDICOMImporterTool::MoveCropPlane(double x, double y)
{
	double pos[4], oldO[3], oldP1[3], oldP2[3];
	ComputeDisplayToWorld(x, y, -1, pos);
	m_CropPlane->GetOrigin(oldO);
	m_CropPlane->GetPoint1(oldP1);
	m_CropPlane->GetPoint2(oldP2);

	if (m_GizmoStatus == RESIZING_MODALITY)
	{
		pos[0] = MAX(pos[0], 0);
		pos[0] = MIN(pos[0], m_SliceSize[0]);
		pos[1] = MAX(pos[1], 0);
		pos[1] = MIN(pos[1], m_SliceSize[1]);

		m_CropPlane->SetPoint1(oldP1[0], pos[1], 0.0);
		m_CropPlane->SetPoint2(pos[0], oldP2[1], 0.0);
	}
	else if (m_GizmoStatus == DRAG_MODALITY)
	{
		pos[0] = MAX(pos[0], 0);
		pos[0] = MIN(pos[0], m_SliceSize[0]);
		pos[1] = MAX(pos[1], 0);
		pos[1] = MIN(pos[1], m_SliceSize[1]);

		switch (m_SideToBeDragged)
		{
			case UP_SIDE:
				m_CropPlane->SetOrigin(oldO[0], pos[1], 0.0);
				m_CropPlane->SetPoint1(oldP1[0], pos[1], 0.0);
				break;
			case BOTTOM_SIDE:
				m_CropPlane->SetPoint2(oldP2[0], pos[1], 0.0);
				break;
			case LEFT_SIDE:
				m_CropPlane->SetOrigin(pos[0], oldO[1], 0.0);
				m_CropPlane->SetPoint2(pos[0], oldP2[1], 0.0);
				break;
			case RIGHT_SIDE:
				m_CropPlane->SetPoint1(pos[0], oldP1[1], 0.0);
				break;
			case UP_LEFT_SIDE:
				m_CropPlane->SetOrigin(pos[0], pos[1], 0.0);
				m_CropPlane->SetPoint1(oldP1[0], pos[1], 0.0);
				m_CropPlane->SetPoint2(pos[0], oldP2[1], 0.0);
				break;
			case UP_RIGHT_SIDE:
				m_CropPlane->SetOrigin(oldO[0], pos[1], 0.0);
				m_CropPlane->SetPoint1(pos[0], pos[1], 0.0);
				break;
			case BOTTOM_LEFT_SIDE:
				m_CropPlane->SetOrigin(pos[0], oldO[1], 0.0);
				m_CropPlane->SetPoint2(pos[0], pos[1], 0.0);
				break;
			case BOTTOM_RIGHT_SIDE:
				m_CropPlane->SetPoint1(pos[0], oldP1[1], 0.0);
				m_CropPlane->SetPoint2(oldP2[0], pos[1], 0.0);
				break;
			default:
				break;
		}
	}

	if (m_Renderer)
		m_Renderer->GetRenderWindow()->Render();
}

//------------------------------------------------------------------------------
void albaDICOMImporterTool::UpdateCursor(albaRWIBase *rwi)
{
	wxStockCursor newArrow = m_CurrentArrow;
	switch (m_SideToBeDragged)
	{
		case UP_SIDE:
		case BOTTOM_SIDE:
			newArrow = wxCURSOR_SIZENS;
			break;
		case UP_RIGHT_SIDE:
		case BOTTOM_LEFT_SIDE:
			newArrow = wxCURSOR_SIZENESW;
			break;
		case BOTTOM_RIGHT_SIDE:
		case UP_LEFT_SIDE:
			newArrow = wxCURSOR_SIZENWSE;
			break;
		case LEFT_SIDE:
		case RIGHT_SIDE:
			newArrow = wxCURSOR_SIZEWE;
			break;
		case NEW_BOX:
			newArrow = wxCURSOR_PENCIL;
			break;
		case NO_DRAGGING:
			newArrow = wxCURSOR_ARROW;
			break;
		default:
			break;
	}

	if (newArrow != m_CurrentArrow && rwi)
	{
		m_CurrentArrow = newArrow;
		rwi->SetCursor(wxCursor(m_CurrentArrow));
	}
}

//------------------------------------------------------------------------------
void albaDICOMImporterTool::SetRendererAndView(vtkRenderer *ren, albaView *view)
{
	Superclass::SetRendererAndView(ren, view);
	if (ren)
		ren->AddActor(m_CropActor);
}

#define INSIDE_DELTA(a,b) ( a>=(b-m_Delta) && a<=(b+m_Delta) )
//------------------------------------------------------------------------------
void albaDICOMImporterTool::CalculateSideDragged(double *pos)
{
	double P1[3], P2[3];
	m_CropPlane->GetPoint1(P1);
	m_CropPlane->GetPoint2(P2);

	double minX, maxX, minY, maxY, x, y;
	x = pos[0];
	y = pos[1];
	minX = P2[0];
	maxX = P1[0];
	minY = P1[1];
	maxY = P2[1];

	if (INSIDE_DELTA(x, minX) && INSIDE_DELTA(y, minY))
		m_SideToBeDragged = UP_LEFT_SIDE;
	else if (INSIDE_DELTA(x, maxX) && INSIDE_DELTA(y, minY))
		m_SideToBeDragged = UP_RIGHT_SIDE;
	else if (INSIDE_DELTA(x, minX) && INSIDE_DELTA(y, maxY))
		m_SideToBeDragged = BOTTOM_LEFT_SIDE;
	else if (INSIDE_DELTA(x, maxX) && INSIDE_DELTA(y, maxY))
		m_SideToBeDragged = BOTTOM_RIGHT_SIDE;
	else if (INSIDE_DELTA(y, minY) && x > minX && x < maxX)
		m_SideToBeDragged = UP_SIDE;
	else if (INSIDE_DELTA(y, maxY) && x > minX && x < maxX)
		m_SideToBeDragged = BOTTOM_SIDE;
	else if (INSIDE_DELTA(x, minX) && y > minY && y < maxY)
		m_SideToBeDragged = LEFT_SIDE;
	else if (INSIDE_DELTA(x, maxX) && y > minY && y < maxY)
		m_SideToBeDragged = RIGHT_SIDE;
	else if (x > 0 && x < m_SliceSize[0] && y > 0 && y < m_SliceSize[1])
		m_SideToBeDragged = NEW_BOX;
	else
		m_SideToBeDragged = NO_DRAGGING;
}

//------------------------------------------------------------------------------
void albaDICOMImporterTool::GetPlaneBounds(double *bounds)
{
	m_CropPlane->GetOutput()->GetBounds(bounds);
}

//------------------------------------------------------------------------------
void albaDICOMImporterTool::SetSliceBounds(double *bounds)
{
	m_SliceSize[0] = bounds[1] - bounds[0];
	m_SliceSize[1] = bounds[3] - bounds[2];
	m_Delta = sqrt(m_SliceSize[0] * m_SliceSize[1]) / 30.0;
	SetPlaneFromSliceSize();
}

//------------------------------------------------------------------------------
void albaDICOMImporterTool::SetPlaneFromSliceSize()
{
	m_CropPlane->SetOrigin(0.0, 0.0, 0.0);
	m_CropPlane->SetPoint1(m_SliceSize[0], 0.0, 0.0);
	m_CropPlane->SetPoint2(0.0, m_SliceSize[1], 0.0);
}

//------------------------------------------------------------------------------
void albaDICOMImporterTool::OnPush()
{
	m_CropActor->VisibilityOn();
}

//------------------------------------------------------------------------------
void albaDICOMImporterTool::OnPop()
{
	m_CropActor->VisibilityOff();
}
