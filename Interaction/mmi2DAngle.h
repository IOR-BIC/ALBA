/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmi2DAngle.h,v $
Language:  C++
Date:      $Date: 2007-08-27 12:55:56 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmi2DAngle_h
#define __mmi2DAngle_h

#include "mmiPER.h"
#include "mafEvent.h"
#include "vtkTextActorMeter.h"

//----------------------------------------------------------------------------
// forward refs
//----------------------------------------------------------------------------
class mafRWI;
class mmdMouse;
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
class mmi2DAngle : public mmiPER
	//----------------------------------------------------------------------------
{
public:
	mafTypeMacro(mmi2DAngle, mmiPER);

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
	void SetLabel(mafString label);

	/**
	Get the label*/
	mafString GetLabel() { if(m_MeterVector.size()) 
													 return mafString(m_MeterVector[m_MeterVector.size()-1]->GetText());
												 else 
													 return mafString("");
												}

  /**
  Return the dimension of Measure Vector
  */
  int SizeMeasureVector(){ return m_Measure.size(); }
protected:
	mmi2DAngle();
	virtual ~mmi2DAngle();

	virtual void OnLeftButtonDown   (mafEventInteraction *e);
	virtual void OnLeftButtonUp     (mafEventInteraction *e);
	virtual void OnMiddleButtonDown (mafEventInteraction *e);
	virtual void OnMiddleButtonUp   (mafEventInteraction *e);
	virtual void OnRightButtonDown  (mafEventInteraction *e);
	virtual void OnRightButtonUp    (mafEventInteraction *e);
	virtual void OnButtonDown       (mafEventInteraction *e);
	virtual void OnMove             (mafEventInteraction *e);
	virtual void OnButtonUp         (mafEventInteraction *e); 
	virtual void OnChar             (mafEventInteraction *e) {};

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

  std::vector<vtkTextActorMeter *> m_MeterVector;
  

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


	
	bool m_DraggingLine;
	bool m_DraggingLeft;
	bool m_DraggingMouse;
	bool m_EndMeasure;
	bool m_ParallelView;
	int  m_MeasureType;
  bool m_Clockwise;
  bool m_DisableUndoAndOkCancel;
	bool m_RegisterMeasure;
	mmdMouse *m_Mouse;

  std::vector<int> m_FlagMeasureType;
  std::vector<double> m_Measure;
  
  

  double  m_Angle;
  double m_AbsoluteAngle ;
	double  m_PickedPoint[3]; ///< 3D position of the picked point; used to assign position to the m_ProbingLine
	mafVME *m_ProbedVME; ///< VME probed by the m_ProbingLine
  
private:
	mmi2DAngle(const mmi2DAngle&);   // Not implemented.
	void operator=(const mmi2DAngle&);  // Not implemented.
};
#endif

