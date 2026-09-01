/*=========================================================================
Program:   ALBA
Module:    albaInteractorSlider.cpp
Language:  C++
Date:      $Date: 2019-04-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaInteractorSlider.h"
#include "albaDecl.h"

#include "albaDeviceButtonsPadMouse.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaEvent.h"
#include "albaEventInteraction.h"
#include "albaGUI.h"
#include "albaRWI.h"
#include "albaView.h"
#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkLine.h"
#include "vtkLineSource.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProbeFilter.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkTextActor.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "wx\timer.h"

#define LINE_OFFSET 10
#define LINE_WIDTH 10
#define BUTTON_WIDTH 30
#define TEXT_OFFSET 10

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractorSlider)

// EVENT_TABLE
BEGIN_EVENT_TABLE(albaInteractorSlider, wxFrame)
EVT_TIMER(ID_TIMER, albaInteractorSlider::OnTimer)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaInteractorSlider::albaInteractorSlider()
{
	m_TestMode = false;

	m_Mouse = NULL;
	m_Renderer = NULL;
	m_View = NULL;

	m_Time = 0;
	m_timer.SetOwner(this, ID_TIMER);

	m_ParallelView = false;

	m_Coordinate = vtkCoordinate::New();
	m_Coordinate->SetCoordinateSystemToDisplay();

	// Slider Props
	m_SliderPosition[0] = 100;
	m_SliderPosition[1] = 100;
	m_SliderPosition[2] = 0;
	m_SliderLenght = 600.0;

	m_IsInBound = false;
	m_CanEditLine = false;
	m_ShowText = true;

	m_Orientation = VERTICAL;
	
	// Edit Line Props
	m_EditLineColor[0] = 1.8;
	m_EditLineColor[1] = 1.8;
	m_EditLineColor[2] = 1.8;

	m_PadLineColor[0] = 0.4;
	m_PadLineColor[1] = 0.4;
	m_PadLineColor[2] = 0.4;

	m_TextColor[0] = 1.0;
	m_TextColor[1] = 1.0;
	m_TextColor[2] = 1.0;
	
	// Edit Line
	vtkNEW(m_EditLineSource);

	vtkNEW(m_EditLineMapper);
	m_EditLineMapper->SetInputConnection(m_EditLineSource->GetOutputPort());
	m_EditLineMapper->SetTransformCoordinate(m_Coordinate);

	vtkNEW(m_EditLineActor);
	m_EditLineActor->SetMapper(m_EditLineMapper);

	// Decrease Line Props
	vtkNEW(m_DescreaseLineSource);

	vtkNEW(m_DescreaseLineMapper);
	m_DescreaseLineMapper->SetInputConnection(m_DescreaseLineSource->GetOutputPort());
	m_DescreaseLineMapper->SetTransformCoordinate(m_Coordinate);

	vtkNEW(m_DescreaseLineActor);
	m_DescreaseLineActor->SetMapper(m_DescreaseLineMapper);

	// Increase Line Props
	vtkNEW(m_IncreaseLineSource);

	vtkNEW(m_IncreaseLineMapper);
	m_IncreaseLineMapper->SetInputConnection(m_IncreaseLineSource->GetOutputPort());
	m_IncreaseLineMapper->SetTransformCoordinate(m_Coordinate);

	vtkNEW(m_IncreaseLineActor);
	m_IncreaseLineActor->SetMapper(m_IncreaseLineMapper);

	// Pad Line Props
	vtkNEW(m_PadLineSource);

	vtkNEW(m_PadLineMapper);
	m_PadLineMapper->SetInputConnection(m_PadLineSource->GetOutputPort());
	m_PadLineMapper->SetTransformCoordinate(m_Coordinate);

	vtkNEW(m_PadLineActor);
	m_PadLineActor->SetMapper(m_PadLineMapper);
	m_PadLineActor->GetProperty()->SetColor(m_PadLineColor);
	m_PadLineActor->GetProperty()->SetLineWidth(LINE_WIDTH);
		
	// Text
	vtkNEW(m_TextValueMapper);
	m_TextValueMapper->GetTextProperty()->SetFontFamily(VTK_ARIAL);
	m_TextValueMapper->GetTextProperty()->SetLineOffset(0.5);
	m_TextValueMapper->GetTextProperty()->SetLineSpacing(1.5);
	m_TextValueMapper->GetTextProperty()->SetJustificationToCentered();
	m_TextValueMapper->GetTextProperty()->SetVerticalJustificationToTop();

	vtkNEW(m_TextValueActor);
	m_TextValueActor->SetMapper(m_TextValueMapper);
	m_TextValueActor->GetProperty()->SetColor(m_TextColor);

	vtkNEW(m_TextMinActor);
	m_TextMinActor->GetProperty()->SetColor(m_TextColor);

	vtkNEW(m_TextMaxActor);
	m_TextMaxActor->GetProperty()->SetColor(m_TextColor);

	SetRange(-1.0, 1.0);
	SetValue(1.0);
	SetSteps(10);
	UpdateSlider();
}
//----------------------------------------------------------------------------
albaInteractorSlider::albaInteractorSlider(albaView *view, int orientation, double value, double minValue, double maxValue) : albaInteractorSlider()
{
	SetRendererByView(view);
	
	m_Orientation = orientation;

	SetRange(minValue, maxValue);
	SetValue(value);

	UpdateSlider();
	ShowSlider(true);
	UpdatePadPosition(ValueToPosition(m_Value));
}
//----------------------------------------------------------------------------
albaInteractorSlider::albaInteractorSlider(albaView *view, int orientation, double value, double minValue, double maxValue, int x, int y, int lenght) : albaInteractorSlider()
{
	SetRendererByView(view);

	// Slider Props
	m_SliderPosition[0] = x;
	m_SliderPosition[1] = y;
	m_SliderLenght = lenght;

	m_Orientation = orientation;

	SetRange(minValue, maxValue);
	SetValue(value);

	UpdateSlider();
	ShowSlider(true);
	UpdatePadPosition(ValueToPosition(m_Value));
}

//----------------------------------------------------------------------------
albaInteractorSlider::~albaInteractorSlider()
{
	vtkDEL(m_Coordinate);

	m_Renderer->RemoveActor2D(m_EditLineActor);
	vtkDEL(m_EditLineActor);
	vtkDEL(m_EditLineSource);
	vtkDEL(m_EditLineMapper);

	m_Renderer->RemoveActor2D(m_DescreaseLineActor);
	vtkDEL(m_DescreaseLineSource);
	vtkDEL(m_DescreaseLineMapper);
	vtkDEL(m_DescreaseLineActor);

	m_Renderer->RemoveActor2D(m_IncreaseLineActor);
	vtkDEL(m_IncreaseLineSource);
	vtkDEL(m_IncreaseLineMapper);
	vtkDEL(m_IncreaseLineActor);

	m_Renderer->RemoveActor2D(m_PadLineActor);	
	vtkDEL(m_PadLineSource);
	vtkDEL(m_PadLineMapper);
	vtkDEL(m_PadLineActor);

	m_Renderer->RemoveActor2D(m_TextValueActor);
	vtkDEL(m_TextValueMapper);
	vtkDEL(m_TextValueActor);

	m_Renderer->RemoveActor2D(m_TextMinActor);
	vtkDEL(m_TextMinActor);
	
	m_Renderer->RemoveActor2D(m_TextMaxActor);
	vtkDEL(m_TextMaxActor);
}

// EVENTS
//----------------------------------------------------------------------------
void albaInteractorSlider::OnEvent(albaEventBase *event)
{
	albaID ch = event->GetChannel();

	if (ch == MCH_INPUT)
	{
		albaID id = event->GetId();

		if (id == albaDeviceButtonsPadTracker::GetTracker3DMoveId() || id == albaDeviceButtonsPadMouse::GetMouse2DMoveId())
		{
			albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);
			OnMove(e);
		}
	}

	Superclass::OnEvent(event);
}
//----------------------------------------------------------------------------
void albaInteractorSlider::OnTimer(wxTimerEvent& event)
{
	double timeStep = 1.0;
	m_Time += timeStep;
	
	m_Value += (m_ValStep*m_PadDirection);

	if (m_Value < m_MinValue) m_Value = m_MinValue;
	if (m_Value > m_MaxValue) m_Value = m_MaxValue;

	if (m_Value != m_OldValue)
	{
		m_OldValue = m_Value;
		UpdatePadPosition(ValueToPosition(m_Value));
	}

	if (m_timer.IsRunning())
	{
		m_timer.Stop();
		m_Time = 0;
		m_timer.Start(10);
		//albaLogMessage("--------- Start Timer ---------");
	}
}


// MOUSE EVENTS
//----------------------------------------------------------------------------
void albaInteractorSlider::OnLeftButtonDown(albaEventInteraction *e)
{
	Superclass::OnLeftButtonDown(e);

	if (m_ParallelView && m_IsInBound)
	{
		m_DraggingLeft = true;

		double mousePosition[3];
		e->Get2DPosition(mousePosition);

		m_PadDirection = mousePosition[m_Orientation] > m_PadPosition ? 1 : -1;
		double position = mousePosition[m_Orientation];

		if ((position < m_LineP1[m_Orientation]) || (position > m_LineP2[m_Orientation]))
		{
			m_Value += (m_ValStep*m_PadDirection);

			if (m_Value < m_MinValue) m_Value = m_MinValue;
			if (m_Value > m_MaxValue) m_Value = m_MaxValue;

			if (m_PadDirection == -1)
				m_DescreaseLineActor->GetProperty()->SetColor(10, 10, 10);
			else
				m_IncreaseLineActor->GetProperty()->SetColor(10, 10, 10);

			if (!m_timer.IsRunning())
			{
				//albaLogMessage("--------- Start Timer ---------");
				m_Time = 0;
				m_timer.Start(300);
			}
		}
		else
		{
			int bin = ((position - m_LineP1[m_Orientation] + (m_PadWidth -1 )) / m_PadStep);
			m_Value = (bin*m_ValStep) + m_MinValue;
		}

		UpdatePadPosition(ValueToPosition(m_Value));
	}
}
//----------------------------------------------------------------------------
void albaInteractorSlider::OnLeftButtonUp(albaEventInteraction *e)
{
	Superclass::OnLeftButtonUp(e);

	m_DraggingLeft = false;

	if (m_ParallelView && m_IsInBound)
	{
		m_IncreaseLineActor->GetProperty()->SetColor(100, 100, 100);
		m_DescreaseLineActor->GetProperty()->SetColor(100, 100, 100);

		m_PadLineActor->GetProperty()->SetColor(m_PadLineColor);
		m_Renderer->GetRenderWindow()->Render();

		if (m_timer.IsRunning())
		{
			//albaLogMessage("--------- Stop Timer ---------");
			m_timer.Stop();
		}
	}
}
//----------------------------------------------------------------------------
void albaInteractorSlider::OnMove(albaEventInteraction *e)
{
	Superclass::OnMove(e);

	if (m_ParallelView)
	{
		double mousePosition[3];
		e->Get2DPosition(mousePosition);

		m_IsInBound = IsInBound(mousePosition);
		double position = mousePosition[m_Orientation];

		if (m_DraggingLeft && m_IsInBound)
		{
			if (position > m_LineP1[m_Orientation] && position < m_LineP2[m_Orientation])
			{
				int bin = ((position - m_LineP1[m_Orientation] + (m_PadWidth - 1)) / m_PadStep);
				m_Value = (bin*m_ValStep) + m_MinValue;

				if (m_Value != m_OldValue)
				{
					m_OldValue = m_Value;
					UpdatePadPosition(ValueToPosition(m_Value));
				}
			}
		}
		else
		{
			if (m_timer.IsRunning())
			{
				m_timer.Stop();
				//albaLogMessage("--------- Stop Timer ---------");
			}
		}
	}
}

//----------------------------------------------------------------------------
void albaInteractorSlider::UpdatePadPosition(double pos)
{
	m_PadPosition = pos;

	double point1[3] = { m_LineP1[0], m_LineP1[1] , 0.0 };
	double point2[3] = { m_LineP1[0], m_LineP1[1] , 0.0 };

	point1[m_Orientation] = pos - m_PadWidth;
	point2[m_Orientation] = pos + m_PadWidth;

	if (point1[m_Orientation] <= m_LineP1[m_Orientation])
	{
		point1[m_Orientation] = m_LineP1[m_Orientation];
		point2[m_Orientation] = point1[m_Orientation] + (2 * m_PadWidth);

		m_PadPosition = point1[m_Orientation] + m_PadWidth;
	}

	if (point2[m_Orientation] >= m_LineP2[m_Orientation])
	{
		point2[m_Orientation] = m_LineP2[m_Orientation];
		point1[m_Orientation] = point2[m_Orientation] - (2 * m_PadWidth);

		m_PadPosition = point2[m_Orientation] - m_PadWidth;
	}
	
	m_PadLineActor->GetProperty()->SetColor(0.10, 0.10, 0.10);

	m_PadLineSource->SetPoint1(point1);
	m_PadLineSource->SetPoint2(point2);
	m_PadLineSource->Update();
	
	if (m_ShowText)
	{
		// Update Text Value
		double textPos[2];
		textPos[0] = point1[0];
		textPos[1] = point1[1];

		if (m_Orientation == HORIZONTAL)
		{
			textPos[0] = pos;
			textPos[1] += TEXT_OFFSET * 2.5;
		}
		else
		{
			textPos[0] += TEXT_OFFSET * 2.5;
			textPos[1] = pos;
		}

		m_TextValueActor->SetInput(albaString::Format(_("%.2f"), m_Value));
		m_TextValueActor->SetPosition(textPos[0], textPos[1]);
		m_TextValueActor->Modified();
	}

	m_Renderer->GetRenderWindow()->Render();

	albaEventMacro(albaEvent(this, ID_VALUE_CHANGED, m_Value));
}
//----------------------------------------------------------------------------
void albaInteractorSlider::UpdateSlider()
{
	m_LineP1[0] = m_SliderPosition[0];
	m_LineP1[1] = m_SliderPosition[1];
	m_LineP1[2] = 0;

	m_LineP1[m_Orientation] += (BUTTON_WIDTH + LINE_OFFSET);

	m_LineP2[0] = m_SliderPosition[0];
	m_LineP2[1] = m_SliderPosition[1];
	m_LineP2[2] = 0;

	m_LineP2[m_Orientation] += (m_SliderLenght - LINE_OFFSET - BUTTON_WIDTH);

	m_PadDirection = 0; // -1=decrease | 1=increase

	if (m_Steps <= 0) m_Steps = 100;

	m_PadStep = (m_LineP2[m_Orientation] - m_LineP1[m_Orientation]) / m_Steps;
	m_ValStep = (m_MaxValue - m_MinValue) / m_Steps;

	m_PadWidth = m_PadStep / 2;

	if (m_PadWidth < 3)
		m_PadWidth = 3;

	m_OldValue = 0.12345;
	m_PadPosition = ValueToPosition(m_Value);

	// Calculate Bounds
	if (m_Orientation == HORIZONTAL)
	{
		m_Bounds[0] = m_SliderPosition[0];
		m_Bounds[1] = m_SliderPosition[1] + (LINE_OFFSET * 2);
		m_Bounds[2] = m_SliderPosition[0] + m_SliderLenght;
		m_Bounds[3] = m_SliderPosition[1] - (LINE_OFFSET * 2);
	}
	else
	{
		m_Bounds[0] = m_SliderPosition[0] - (LINE_OFFSET * 2);
		m_Bounds[1] = m_SliderPosition[1] + m_SliderLenght;
		m_Bounds[2] = m_SliderPosition[0] + (LINE_OFFSET * 2);
		m_Bounds[3] = m_SliderPosition[1];
	}
}

//----------------------------------------------------------------------------
double albaInteractorSlider::ValueToPosition(double value)
{
	double deltaDistance = m_LineP2[m_Orientation] - m_LineP1[m_Orientation] - (m_PadWidth * 2);
	double deltaValue = m_MaxValue - m_MinValue;

	return (((value - m_MinValue)*deltaDistance) / deltaValue) + (m_LineP1[m_Orientation] + m_PadWidth) - m_MinValue;
}
//----------------------------------------------------------------------------
bool albaInteractorSlider::IsInBound(double *pos)
{
	m_IsInBound = false;

	if ((pos[0] > m_Bounds[0] && pos[1] < m_Bounds[1]) && // MarginLeft &  MarginUp
		(pos[0] < m_Bounds[2] && pos[1] > m_Bounds[3])) // MarginRight & MarginDown
	{
		m_IsInBound = true;

		//if (!m_DraggingLeft) // HighlightCurrentPoint ON
		{
			m_EditLineActor->GetProperty()->SetOpacity(1.0);

			if (m_PadDirection == 1)
				m_DescreaseLineActor->GetProperty()->SetColor(100, 100, 100);
			else
				m_IncreaseLineActor->GetProperty()->SetColor(100, 100, 100);

			m_PadLineActor->GetProperty()->SetColor(m_PadLineColor);
			m_PadLineActor->GetProperty()->SetOpacity(1.0);

			if (m_ShowText)
			{
				m_TextValueActor->GetProperty()->SetOpacity(1.0);
				m_TextMinActor->GetProperty()->SetOpacity(1.0);
				m_TextMaxActor->GetProperty()->SetOpacity(1.0);

				m_TextValueActor->GetProperty()->SetColor(1, 1, 1);
				m_TextMinActor->GetProperty()->SetColor(1, 1, 1);
				m_TextMaxActor->GetProperty()->SetColor(1, 1, 1);
			}

			m_Renderer->GetRenderWindow()->Render();

			m_CanEditLine = true;
		}
	}
	else // Highlight OFF
	{
		m_EditLineActor->GetProperty()->SetOpacity(0.6);

		m_DescreaseLineActor->GetProperty()->SetColor(100, 100, 100);
		m_IncreaseLineActor->GetProperty()->SetColor(100, 100, 100);

		m_PadLineActor->GetProperty()->SetColor(m_PadLineColor);
		m_PadLineActor->GetProperty()->SetOpacity(0.8);

		if (m_ShowText)
		{
			m_TextValueActor->GetProperty()->SetOpacity(0.2);
			m_TextMinActor->GetProperty()->SetOpacity(0.2);
			m_TextMaxActor->GetProperty()->SetOpacity(0.2);

			m_TextValueActor->GetProperty()->SetColor(100, 100, 100);
			m_TextMinActor->GetProperty()->SetColor(100, 100, 100);
			m_TextMaxActor->GetProperty()->SetColor(100, 100, 100);
		}

		m_Renderer->GetRenderWindow()->Render();

		m_CanEditLine = false;
	}

	return m_IsInBound;
}

// GET/SET

//----------------------------------------------------------------------------
void albaInteractorSlider::SetRendererByView(albaView *view)
{
	m_View = view;

	vtkRenderer *newRenderer = NULL;
	vtkRendererCollection *rc = view->GetRWI()->GetRenderWindow()->GetRenderers();
	
	// Searching for a frontal renderer on render collection
	if (rc)
	{ 
		rc->InitTraversal();
		vtkRenderer *ren;
		while (ren = rc->GetNextItem())
		{
			if (ren->GetLayer() == 1) // Frontal Render
			{
				newRenderer = ren;
				break;
			}
		}
	}

	m_Renderer = newRenderer;
	m_Renderer->GetRenderWindow()->Render();

	m_ViewSize = view->GetRWI()->GetRenderWindow()->GetSize();	
	m_ParallelView = m_Renderer->GetActiveCamera()->GetParallelProjection() != 0;
}
//----------------------------------------------------------------------------
void albaInteractorSlider::SetSteps(int steps)
{
	m_Steps = steps;

	UpdateSlider();

	if (m_Renderer)
		UpdatePadPosition(ValueToPosition(m_Value));
}
//----------------------------------------------------------------------------
void albaInteractorSlider::SetRange(double min, double max)
{
	m_MinValue = min;
	m_MaxValue = max;

	UpdateSlider();

	if (m_Renderer)
		UpdatePadPosition(ValueToPosition(m_Value));
}
//----------------------------------------------------------------------------
void albaInteractorSlider::SetValue(double value)
{
	m_Value = value;

	UpdateSlider();

	if (m_Renderer)
		UpdatePadPosition(ValueToPosition(m_Value));
}

//----------------------------------------------------------------------------
void albaInteractorSlider::ShowSlider(bool show)
{
	if (m_Renderer == NULL)
		return;

	if (show)
	{
		// Decrease Line
		m_Renderer->RemoveActor(m_DescreaseLineActor);

		double decreaseLineP1[3];
		double decreaseLineP2[3];

		decreaseLineP1[0] = m_SliderPosition[0];
		decreaseLineP1[1] = m_SliderPosition[1];
		decreaseLineP1[2] = 0.0;

		decreaseLineP2[0] = m_SliderPosition[0];
		decreaseLineP2[1] = m_SliderPosition[1];
		decreaseLineP2[2] = 0.0;

		decreaseLineP2[m_Orientation] += BUTTON_WIDTH;

		m_DescreaseLineSource->SetPoint1(decreaseLineP1);
		m_DescreaseLineSource->SetPoint2(decreaseLineP2);
		m_DescreaseLineActor->GetProperty()->SetColor(100, 100, 100);
		m_DescreaseLineActor->GetProperty()->SetLineWidth(LINE_WIDTH);
		m_DescreaseLineActor->GetProperty()->SetOpacity(0.5);
		m_DescreaseLineSource->Update();

		m_Renderer->AddActor2D(m_DescreaseLineActor);

		// Increase Line
		m_Renderer->RemoveActor(m_IncreaseLineActor);

		double line3P1[3];
		double line3P2[3];

		line3P1[0] = m_SliderPosition[0];
		line3P1[1] = m_SliderPosition[1];
		line3P1[2] = 0.0;

		line3P2[0] = m_SliderPosition[0];
		line3P2[1] = m_SliderPosition[1];
		line3P2[2] = 0.0;

		line3P1[m_Orientation] += (m_SliderLenght - BUTTON_WIDTH);
		line3P2[m_Orientation] += m_SliderLenght;

		m_IncreaseLineSource->SetPoint1(line3P1);
		m_IncreaseLineSource->SetPoint2(line3P2);
		m_IncreaseLineActor->GetProperty()->SetColor(100, 100, 100);
		m_IncreaseLineActor->GetProperty()->SetLineWidth(LINE_WIDTH);
		m_IncreaseLineActor->GetProperty()->SetOpacity(0.5);
		m_IncreaseLineSource->Update();

		m_Renderer->AddActor2D(m_IncreaseLineActor);

		// Edit Line
		m_Renderer->RemoveActor(m_EditLineActor);

		m_EditLineSource->SetPoint1(m_LineP1);
		m_EditLineSource->SetPoint2(m_LineP2);
		m_EditLineActor->GetProperty()->SetColor(m_EditLineColor);
		m_EditLineActor->GetProperty()->SetLineWidth(LINE_WIDTH);
		m_EditLineActor->GetProperty()->SetOpacity(0.5);
		m_EditLineSource->Update();

		m_Renderer->AddActor2D(m_EditLineActor);

		// Pad Line
		m_Renderer->RemoveActor(m_PadLineActor);
		m_Renderer->AddActor2D(m_PadLineActor);

		if (m_ShowText)
		{
			// Update Text Value
			m_Renderer->AddActor2D(m_TextValueActor);

			// Update Text Min Value
			m_Renderer->RemoveActor(m_TextMinActor);

			double textPos[2];
			textPos[0] = decreaseLineP1[0];
			textPos[1] = decreaseLineP1[1];

			if (m_Orientation == HORIZONTAL)
			{
				textPos[1] += TEXT_OFFSET;
			}
			else
			{
				textPos[0] += TEXT_OFFSET;
			}

			m_TextMinActor->SetInput(albaString::Format(_("%.2f"), m_MinValue));
			m_TextMinActor->SetPosition(textPos[0], textPos[1]);
			m_TextMinActor->Modified();

			m_Renderer->AddActor2D(m_TextMinActor);

			// Update Text Max Value
			m_Renderer->RemoveActor(m_TextMaxActor);

			textPos[0] = line3P1[0];
			textPos[1] = line3P1[1];

			if (m_Orientation == HORIZONTAL)
			{
				textPos[1] += TEXT_OFFSET;
			}
			else
			{
				textPos[0] += TEXT_OFFSET;
			}

			m_TextMaxActor->SetInput(albaString::Format(_("%.2f"), m_MaxValue));
			m_TextMaxActor->SetPosition(textPos[0], textPos[1]);
			m_TextMaxActor->Modified();

			m_Renderer->AddActor2D(m_TextMaxActor);
		}
	}
	else
	{
		m_Renderer->RemoveActor2D(m_DescreaseLineActor);
		m_Renderer->RemoveActor2D(m_IncreaseLineActor);
		m_Renderer->RemoveActor2D(m_EditLineActor);
		m_Renderer->RemoveActor(m_PadLineActor);
		m_Renderer->RemoveActor(m_TextValueActor);
		m_Renderer->RemoveActor(m_TextMinActor);
		m_Renderer->RemoveActor(m_TextMaxActor);
	}

	m_Renderer->GetRenderWindow()->Render();
}

