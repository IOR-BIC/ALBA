/*=========================================================================

 Program: MAF2
 Module: mafInteractor2DIndicator
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafInteractor2DIndicator_h
#define __mafInteractor2DIndicator_h

#include "mafInteractorPER.h"
#include "mafEvent.h"
#include "vtkMAFTextActorMeter.h"

//----------------------------------------------------------------------------
// forward refs
//----------------------------------------------------------------------------
class mafRWI;
class mafRWIBase;
class mafDeviceButtonsPadMouse;

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
class MAF_EXPORT mafInteractor2DIndicator : public mafInteractorPER
	//----------------------------------------------------------------------------
{
public:
	mafTypeMacro(mafInteractor2DIndicator, mafInteractorPER);

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
  int SizeMeasureVector(){ return m_MeterVector.size(); }

  /** Show On/Off only last measure */
  void ShowOnlyLastMeasure(bool show);

  /** Show On/Off all measures */
  void ShowAllMeasures(bool show);

  /** return the current rwi */
  mafRWIBase *GetCurrentRwi();

protected:
	mafInteractor2DIndicator();
	virtual ~mafInteractor2DIndicator();

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

  std::vector<vtkMAFTextActorMeter *> m_MeterVector;
  

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
  
  mafRWIBase *m_CurrentRwi;
	
	bool m_DraggingLine;
	bool m_DraggingLeft;
	bool m_DraggingMouse;
	bool m_EndMeasure;
	bool m_ParallelView;
	
  bool m_Clockwise;
  bool m_DisableUndoAndOkCancel;
  bool m_RegisterMeasure;
	

	mafDeviceButtonsPadMouse *m_Mouse;

	double  m_PickedPoint[3]; ///< 3D position of the picked point; used to assign position to the m_ProbingLine
	mafVME *m_ProbedVME; ///< VME probed by the m_ProbingLine
  
private:
	mafInteractor2DIndicator(const mafInteractor2DIndicator&);   // Not implemented.
	void operator=(const mafInteractor2DIndicator&);  // Not implemented.
};
#endif

