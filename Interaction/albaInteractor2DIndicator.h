/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DIndicator.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaInteractor2DIndicator_h
#define __albaInteractor2DIndicator_h

#include "albaInteractorPER.h"
#include "albaEvent.h"
#include "vtkALBATextActorMeter.h"

//----------------------------------------------------------------------------
// forward refs
//----------------------------------------------------------------------------
class albaRWI;
class albaRWIBase;
class albaDeviceButtonsPadMouse;

class vtkLineSource;
class vtkCoordinate;
class vtkPolyDataMapper2D;
class vtkActor2D;
class vtkRenderWindow;
class vtkRenderer;
class vtkXYPlotActor;
class vtkTextActor;
class vtkConeSource;



/**  Class implementing an interactor for creating 2D measure tool.
This interactor build a line from picked points on VME and draw a 2D line on the renderwindow.
If to the interactor is activated the flag to probe the scalar values of the VME then it generate also 
the histogram of the probed points on VME. This interactor works in renderwindow with ParallelProjectionOn.

@comments
- Modified to work on multiple render windows while
maintaining measure persistence at button up
*/
//----------------------------------------------------------------------------
class ALBA_EXPORT albaInteractor2DIndicator : public albaInteractorPER
	//----------------------------------------------------------------------------
{
public:
	albaTypeMacro(albaInteractor2DIndicator, albaInteractorPER);

	enum METER_2D_MODALITY
	{
		INDICATOR_ARROW = 0,

    ID_RESULT_INDICATOR,
		
	};

	
	/**
	Remove the meters from the render window*/
	void RemoveMeter();

	
	
  /** 
	Undo the last measure*/
	void UndoMeasure();

  
  /** 
	Control if disabling various buttons*/
	bool IsDisableUndoAndOkCancel();
  
  /**
	Get the boolean value of end measure*/
	bool GetRegisterMeasure(){return m_RegisterMeasure;}

	/**
	Set the label*/
	void SetLabel(albaString label);

	/**
	Get the label*/
	albaString GetLabel() { if(m_MeterVector.size()) 
													 return albaString(m_MeterVector[m_MeterVector.size()-1]->GetText());
												 else 
													 return albaString("");
											 }
  /**
  Return the dimension of Measure Vector
  */
  int SizeMeasureVector(){ return m_MeterVector.size(); }

  /** Show On/Off only last measure */
  void ShowOnlyLastMeasure(bool show);

  /** Show On/Off all measures */
  void ShowAllMeasures(bool show);

  /** return the current rwi */
  albaRWIBase *GetCurrentRwi();

protected:
	albaInteractor2DIndicator();
	virtual ~albaInteractor2DIndicator();

	virtual void OnLeftButtonDown   (albaEventInteraction *e);
	virtual void OnLeftButtonUp     (albaEventInteraction *e);
	virtual void OnMiddleButtonDown (albaEventInteraction *e);
	virtual void OnMiddleButtonUp   (albaEventInteraction *e);
	virtual void OnRightButtonDown  (albaEventInteraction *e);
	virtual void OnRightButtonUp    (albaEventInteraction *e);
	virtual void OnButtonDown       (albaEventInteraction *e);
	virtual void OnMove             (albaEventInteraction *e);
	virtual void OnButtonUp         (albaEventInteraction *e); 
	virtual void OnChar             (albaEventInteraction *e) {};

	/** 
	Draw the measure tool according to the measure type selected.*/
	void DrawMeasureTool(double x, double y);
  
	
  //persistent measure BEGIN
  std::vector<vtkActor2D *> m_LineActorVector1;
  std::vector<vtkPolyDataMapper2D *> m_LineMapperVector1;
  std::vector<vtkLineSource *> m_LineSourceVector1;

	std::vector<vtkActor2D *> m_ConeActorVector;
	std::vector<vtkPolyDataMapper2D *> m_ConeMapperVector;
	std::vector<vtkConeSource *> m_ConeSourceVector;

  std::vector<vtkActor2D *> m_LineActorVector2;
  std::vector<vtkPolyDataMapper2D *> m_LineMapperVector2;
  std::vector<vtkLineSource *> m_LineSourceVector2;

  std::vector<vtkALBATextActorMeter *> m_MeterVector;
  

  std::vector<vtkRenderer *> m_RendererVector;
  //persistent measure END
  
  vtkLineSource       *m_ProbingLine;
	
  vtkLineSource       *m_Line; ///< First line of the measure tool
	vtkCoordinate       *m_Coordinate;
	vtkPolyDataMapper2D *m_LineMapper;
	vtkActor2D          *m_LineActor;

  vtkLineSource       *m_Line2; ///< Second line of the measure tool (used for angle measures)
	vtkPolyDataMapper2D *m_LineMapper2;
	vtkActor2D          *m_LineActor2;
	vtkRenderer         *m_LastRenderer; ///< Renderer used for the first line
	vtkRenderer         *m_CurrentRenderer;
  vtkRenderer         *m_PreviousRenderer;
  
  albaRWIBase *m_CurrentRwi;
	
	bool m_DraggingLine;
	bool m_DraggingLeft;
	bool m_DraggingMouse;
	bool m_EndMeasure;
	bool m_ParallelView;
	
  bool m_Clockwise;
  bool m_DisableUndoAndOkCancel;
  bool m_RegisterMeasure;
	

	albaDeviceButtonsPadMouse *m_Mouse;

	double  m_PickedPoint[3]; ///< 3D position of the picked point; used to assign position to the m_ProbingLine
	albaVME *m_ProbedVME; ///< VME probed by the m_ProbingLine
  
private:

	albaInteractor2DIndicator(const albaInteractor2DIndicator&);   // Not implemented.
	void operator=(const albaInteractor2DIndicator&);  // Not implemented.

	friend class albaInteractor2DIndicatorTest;
};
#endif

