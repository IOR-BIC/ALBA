/*=========================================================================
Program:   ALBA
Module:    albaInteractorSlider.h
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

#ifndef __albaInteractorSlider_h
#define __albaInteractorSlider_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaEvent.h"
//#include "albaGUI.h"
#include "albaInteractorPER.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;
class albaRWI;

class vtkActor2D;
class vtkCoordinate;
class vtkLineSource;
class vtkPolyDataMapper2D;
class vtkRenderWindow;
class vtkRenderer;
class vtkRendererCollection;
class vtkTextActor;
class vtkTextMapper;

#define ID_TIMER 702

// Class Name: albaInteractorSlider
class ALBA_EXPORT albaInteractorSlider : public wxFrame, public albaInteractorPER
{
public:

	enum SLIDER_EVENT_ID
	{
		ID_VALUE_CHANGED = MINID,
	};

	enum SLIDER_ORIENTATION
	{
		HORIZONTAL = 0,
		VERTICAL,
	};

	albaInteractorSlider();
	albaInteractorSlider(albaView *view, int orientation = HORIZONTAL, double value = 0.0, double minValue = -1.0, double maxValue = 1.0);
	albaInteractorSlider(albaView *view, int orientation, double value, double minValue, double maxValue, int x, int y, int lenght);

	virtual ~albaInteractorSlider();

	albaTypeMacro(albaInteractorSlider, albaInteractorPER);

	virtual void OnEvent(albaEventBase *event);

	void ShowSlider(bool show);
	void ShowText(bool show) { m_ShowText = show; };

	void SetRendererByView(albaView * view);

	void SetSteps(int steps);
	void SetRange(double min, double max);

	double GetValue() { return m_Value; };
	void SetValue(double value);

	void OnTimer(wxTimerEvent& event);

protected:

	virtual void OnLeftButtonDown(albaEventInteraction *e);
	virtual void OnLeftButtonUp(albaEventInteraction *e);
	virtual void OnMove(albaEventInteraction *e);

	void UpdatePadPosition(double pos);
	void UpdateSlider();

	double ValueToPosition(double value);
	bool IsInBound(double *pos);
	
	albaDeviceButtonsPadMouse	*m_Mouse;
	vtkRenderer								*m_Renderer;
	albaView										*m_View;
	vtkRenderWindow						*m_RenderWindow;

	vtkCoordinate							*m_Coordinate;

	bool m_ParallelView;

	// Slider Props
	int m_SliderPosition[2];
	int m_SliderLenght;
	int m_Orientation;

	int m_Bounds[4];
	bool m_IsInBound;
	bool m_CanEditLine;

	double m_Value;
	double m_MinValue;
	double m_MaxValue;

	double m_Steps;
	double m_ValStep;

	double m_OldValue;

	// Edit Line Props
	vtkLineSource							*m_EditLineSource;
	vtkPolyDataMapper2D				*m_EditLineMapper;
	vtkActor2D								*m_EditLineActor;

	double m_LineP1[3];
	double m_LineP2[3];
	
	float m_LineWidth;
	double m_EditLineColor[3];

	double m_TextColor[3];

	// Decrease-Increase Line Props
	vtkLineSource							*m_DescreaseLineSource;
	vtkPolyDataMapper2D				*m_DescreaseLineMapper;
	vtkActor2D								*m_DescreaseLineActor;

	vtkLineSource							*m_IncreaseLineSource;
	vtkPolyDataMapper2D				*m_IncreaseLineMapper;
	vtkActor2D								*m_IncreaseLineActor;

	// Pad Line Props
	vtkLineSource							*m_PadLineSource;
	vtkPolyDataMapper2D				*m_PadLineMapper;
	vtkActor2D								*m_PadLineActor;

	double m_PadLineColor[3];

	int m_PadPosition;
	int m_PadDirection;		// -1=decrease | 1=increase
	int m_PadStep;				// = EditLine_Lenght / m_Steps
	int m_PadWidth;

	bool m_ShowText;

	// Text Labels
	vtkTextMapper							*m_TextValueMapper;
	vtkTextActor							*m_TextValueActor;
	vtkTextActor							*m_TextMinActor;
	vtkTextActor							*m_TextMaxActor;

	int *m_ViewSize;

private:
	
	double m_Time;
	wxTimer m_timer;

	albaInteractorSlider(const albaInteractorSlider&);   // Not implemented.
	void operator=(const albaInteractorSlider&);  // Not implemented.

	friend class albaInteractorSliderTest;	

	/** declaring event table macro */
	DECLARE_EVENT_TABLE();
};
#endif

