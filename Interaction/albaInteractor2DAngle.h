/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DAngle.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaInteractor2DAngle_h
#define __albaInteractor2DAngle_h

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
class vtkTextActor;



/**  Class implementing an interactor for creating 2D measure tool.
This interactor build a line from picked points on VME and draw a 2D line on the renderwindow.
If to the interactor is activated the flag to probe the scalar values of the VME then it generate also 
the histogram of the probed points on VME. This interactor works in renderwindow with ParallelProjectionOn.

@comments
- Modified to work on multiple render windows while
maintaining measure persistence at button up
*/
//----------------------------------------------------------------------------
class ALBA_EXPORT albaInteractor2DAngle : public albaInteractorPER
	//----------------------------------------------------------------------------
{
public:
	albaTypeMacro(albaInteractor2DAngle, albaInteractorPER);

	enum METER_2D_MODALITY
	{
		
		ANGLE_BETWEEN_LINES = 0,
		ANGLE_BETWEEN_POINTS,
		ID_RESULT_MEASURE,
		ID_RESULT_ANGLE,
	};

	/** 
	Set the type of the measure, selectable from DISTANCE_BETWEEN_POINTS, DISTANCE_BETWEEN_LINES and ANGLE_BETWEEN_LINES.*/
	void SetMeasureType(int t);
	
	
	/** 
	Set the type of the measure to ANGLE_BETWEEN_LINES.*/
	void SetMeasureTypeToAngleBetweenLines()     {SetMeasureType(ANGLE_BETWEEN_LINES);};

	/** 
	Set the type of the measure to ANGLE_BETWEEN_POINTS.*/
	void SetMeasureTypeToAngleBetweenPoints()     {SetMeasureType(ANGLE_BETWEEN_POINTS);};

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
	Set manually the angle*/
	void SetManualAngle(double manualAngle);

	/**
	Get Last Angle */
	double GetLastAngle() {
													if(m_Measure.size()) 
														return m_Measure[m_Measure.size()-1];
													else
														return 0;
												}

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
  int SizeMeasureVector(){ return m_Measure.size(); }

  /** Show On/Off only last measure */
  void ShowOnlyLastMeasure(bool show);

  /** Show On/Off all measures */
  void ShowAllMeasures(bool show);

  /** return the current rwi */
  albaRWIBase *GetCurrentRwi();

  void SetColor(double r,double g,double b);

protected:
	albaInteractor2DAngle();
	virtual ~albaInteractor2DAngle();

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
	/** 
	Calculate the measure according to the measure type selected*/
	void CalculateMeasure();
  

  //persistent measure BEGIN
  std::vector<vtkActor2D *> m_LineActorVector1;
  std::vector<vtkPolyDataMapper2D *> m_LineMapperVector1;
  std::vector<vtkLineSource *> m_LineSourceVector1;

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
	int  m_MeasureType;
  bool m_Clockwise;
  bool m_DisableUndoAndOkCancel;
	bool m_RegisterMeasure;
	albaDeviceButtonsPadMouse *m_Mouse;

  std::vector<int> m_FlagMeasureType;
  std::vector<double> m_Measure;
  
  

  double  m_Angle;
  double m_AbsoluteAngle ;
	double  m_PickedPoint[3]; ///< 3D position of the picked point; used to assign position to the m_ProbingLine
	albaVME *m_ProbedVME; ///< VME probed by the m_ProbingLine

  double m_Color[3];
  
private:
	albaInteractor2DAngle(const albaInteractor2DAngle&);   // Not implemented.
	void operator=(const albaInteractor2DAngle&);  // Not implemented.
	friend class albaInteractor2DAngleTest;
};
#endif

