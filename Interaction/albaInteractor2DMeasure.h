/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaInteractor2DMeasure_h
#define __albaInteractor2DMeasure_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
/*#include "appGeometry.h"*/

#include "albaEvent.h"
#include "albaInteractorPER.h"
#include "albaActor2dStackHelper.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;
class albaRWI;

class vtkALBATextActorMeter;
class vtkActor2D;
class vtkCoordinate;
class vtkLineSource;
class vtkPointSource;
class vtkPolyDataMapper2D;
class vtkRenderWindow;
class vtkRenderer;
class vtkRendererCollection;
class vtkTextActor;
class vtkXYPlotActor;

#define TEXT_W_SHIFT 10
#define TEXT_H_SHIFT 5
#define POINT_UPDATE_DISTANCE 4
#define POINT_UPDATE_DISTANCE_2 (POINT_UPDATE_DISTANCE * POINT_UPDATE_DISTANCE)

// Class Name: albaInteractor2DMeasure
class ALBA_EXPORT albaInteractor2DMeasure : public albaInteractorPER
{
public:

	albaTypeMacro(albaInteractor2DMeasure, albaInteractorPER);

	enum MEASURE_INTERACTIONS
	{
		ID_MEASURE_STARTED = MINID,
		ID_MEASURE_FINISHED,
		ID_MEASURE_ADDED,
		ID_MEASURE_CHANGED,
		ID_MEASURE_MOVED,
		ID_MEASURE_SELECTED,
		ID_MEASURE_RCLICK,
	};

	enum MEASURE_ACTIONS
	{
		ACTION_NONE = 0,
		ACTION_ADD_MEASURE,
		ACTION_EDIT_MEASURE,
		ACTION_MOVE_MEASURE,
	};

	enum LINE_POINT
	{
		NO_POINT = 0,
		POINT_1,
		POINT_2,
		POINT_3,
		POINT_4,
	};

	virtual void OnEvent(albaEventBase *event);
	
	/// MEASURE
	/** Add Measure*/
	virtual void AddMeasure(double *point1, double *point2 = NULL);
	/** Delete the Measure*/
	virtual void RemoveMeasure(int index);
	/** Delete all Measures*/
	virtual void RemoveAllMeasures();
	/** Select a Measure*/
	virtual void SelectMeasure(int index);
	/** Disable Measure*/
	virtual void ActivateMeasure(int index = -1 /*-1=All*/, bool activate = true);
	
	/** Enable/Disable Measure Interactor*/
	void Enable(bool enable = true) { m_IsEnabled = enable; SelectMeasure(-1); };

	/** Enable/Disable Add Mode*/
	void EnableAddMeasure(bool move = true) { m_AddMeasureEnable = move; };
	/** Enable/Disable Editing Mode*/
	void EnableEditMeasure(bool edit = true) { m_EditMeasureEnable = edit; };
	/** Enable/Disable Moving Mode*/
	void EnableMoveMeasure(bool move = true) { m_MoveMeasureEnable = move; };
		
	/** Update All Actors*/
	virtual void Update(int index = -1 /*Update All*/) {};

	/// LOAD/SAVE
	virtual bool Load(albaVME *input, wxString tag) { return true; };
	virtual bool Save(albaVME *input, wxString tag) { return true; };

	/// GET
	/** Get Measure Text Value*/
	albaString GetMeasureText(int index);	
	/** Get Measure Extra Label*/
	albaString GetMeasureLabel(int index);
	/** Get a string type of measure*/
	albaString GetMeasureType() { return m_MeasureTypeText; };
	
	/** Get Max Measures*/
	int GetMaxMeasures(){ return m_MaxMeasures; };
	/** Get Number of Measures*/
	int GetMeasureCount() {	return m_Measure2DVector.size(); };
	/** Returns the last Edited Measure index*/
	int GetLastEditedMeasureIndex() { return m_LastEditing; }
	/** Returns the Current Measure Selected index*/
	int GetSelectedMeasureIndex() { return m_LastSelection; }
	/** Measure Interactor Is Enabled */
	bool IsEnabled() { return m_IsEnabled; };
		
	double GetPointSize() { return m_PointSize; };
	double GetLineWidth() { return m_LineWidth; };
	
	/// SET
	/** Set Max Measures*/
	void SetMaxMeasures(int maxMeasures) { m_MaxMeasures = maxMeasures; };
	/** Set Measure Extra Label*/
	void SetMeasureLabel(int index, albaString text);

	/** Set Color Default*/
	void SetColorDefault(double r, double g, double b, double a = 1.0) { iSetColor(COLOR_DEFAULT, r, g, b, a); };
	/** Set Color Selection*/
	void SetColorSelection(double r, double g, double b, double a = 1.0) { iSetColor(COLOR_SELECTION, r, g, b, a); };
	/** Set Color Edit*/
	void SetColorEdit(double r, double g, double b, double a = 1.0) { iSetColor(COLOR_EDIT, r, g, b, a); };
	/** Set Color For Disable Measure*/
	void SetColorDisable(double r, double g, double b, double a = 1.0) { iSetColor(COLOR_DISABLE, r, g, b, a); };
	/** Set Color For Disable Measure*/
	void SetColorText(double r, double g, double b, double a = 1.0) { iSetColor(COLOR_TEXT, r, g, b, a); };

	virtual void SetPointSize(double size) {};
	virtual void SetLineWidth(double width) {};

	virtual void Show(bool show) {};

	/** Show/Hide Text Labels*/
	void ShowText(bool show);

	/** Set Renderer by View needed*/
	void SetRendererByView(albaView * view);

	/* Call Rendering and Camera Update */
	void Render();

	/** Returns current measure index */
	int GetCurreasureIdx() const { return m_CurrMeasure; }

protected:

	struct Measure2D
	{
		bool Active;
		albaString MeasureType;
		albaString Text;
		albaString Label;
		//std::vector<double[3]> Points;
	};

	// Measure Vector
	std::vector<Measure2D> m_Measure2DVector;
	int m_MaxMeasures;

	enum AXIS { X, Y, Z, };
	enum Colors {	COLOR_DEFAULT, COLOR_EDIT, COLOR_SELECTION, COLOR_DISABLE, COLOR_TEXT };
	
	albaInteractor2DMeasure();
	virtual ~albaInteractor2DMeasure();

	void InitRenderer(albaEventInteraction *e);

	/// Mouse Events
	virtual void OnLeftButtonDown(albaEventInteraction *e);
	virtual void OnLeftButtonUp(albaEventInteraction *e);
	virtual void OnRightButtonUp(albaEventInteraction *e);
	virtual void OnMove(albaEventInteraction *e);
		
	/** Draw New Measure*/
	virtual void DrawNewMeasure(double * wp) {};
	/** Move Measure*/
	virtual void MoveMeasure(int index, double *pointCoord) {};
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point) {};
	/** Find Measure Point and Set Action*/
	virtual void FindAndHighlight(double *pointCoord) {};

	/// Update
	virtual void UpdateEditActors(double * point1, double * point2 = NULL) {};
	virtual void UpdatePointActor(double * point) {};
	virtual void UpdateTextActor(int index, double * point);

	virtual void ShowEditLineActors() {};
	virtual void HideEditActors() {};

	/** Set Measure Action (None, Add, Edit, Move) and Update mouse Cursor */
	void SetAction(MEASURE_ACTIONS action);

	void SetColor(vtkActor2D *actor, Color *color);

	/** Get current Measure */
	MEASURE_ACTIONS GetAction() { return m_Action; }

	void iSetColor(int color, double r, double g, double b, double a);

	/// Utilities
	bool IsInBound(double *pos);
	void ScreenToWorld(double screen[2], double world[3]);
	void WorldToScreen(double world[3], double screen[2]);


	//vtkPointSource produces a random-distributed pointCloud, use this method to obtain a fixed position single point output
	vtkPointSource *GetNewPointSource();

	albaDeviceButtonsPadMouse	*m_Mouse;
	vtkRenderer								*m_Renderer;
	albaView									*m_View;
	vtkCoordinate							*m_Coordinate;

	// Text Actor Vector
	std::vector<vtkALBATextActorMeter *> m_TextActorVector;
	
 	albaString m_MeasureTypeText;

	int m_AltPressed;
	int m_ShiftPressed;

	Color m_Colors[5];

	int m_CurrMeasure;
	int m_CurrPoint;

	double m_OldLineP1[3];
	double m_OldLineP2[3];

	double *m_Bounds;
	bool m_IsInBound;
	bool m_ButtonDownInside;

	double m_StartMousePosition[3];
	
	bool m_EndMeasure;
	bool m_ParallelView;
	double m_ParallelScale_OnStart;

	long m_AddMeasurePhase_Counter;
	bool m_ActorAdded;

	double m_MeasureValue;
	int m_LastSelection;
	int m_LastEditing;

	bool m_IsEnabled;
	bool m_MovingMeasure;
	bool m_AddMeasureEnable;
	bool m_EditMeasureEnable;
	bool m_MoveMeasureEnable;

	bool m_ShowText;
	bool m_ShowPoint;

	double m_PointSize;
	double m_LineWidth;
	int m_TextSide;

private:

	MEASURE_ACTIONS m_Action; // Measure Action
	
	albaInteractor2DMeasure(const albaInteractor2DMeasure&);   // Not implemented.
	void operator=(const albaInteractor2DMeasure&);  // Not implemented.
	friend class albaInteractor2DMeasureTest;
};


class appGeometry
{
public:

	/// Points Utils

	static bool Equal(double *point1, double *point2);

	static double * GetMidPoint(double *point1, double *point2);

	static void RotatePoint(double *point, double *origin, double angle);

	static double DistanceBetweenPoints(double *point1, double *point2);

	static double CalculateAngle(double point1[3], double point2[3], double origin[3]);

	/// Lines Utils

	static bool FindIntersectionLines(double(&point)[3], double *line1Point1, double *line1Point2, double *line2Point1, double *line2Point2);
	static int IntersectLineLine(double *l1p1, double *l1p2, double *l2p1, double *l2p2, double &perc);

	static double GetPointToLineDistance(double *point, double *linePoint1, double *linePoint2);
	static float DistancePointToLine(double *point, double *lineP1, double *lineP2);

	static void GetParallelLine(double(&point1)[3], double(&point2)[3], double *linePoint1, double *linePoint2, double distance);

	static bool FindPointOnLine(double(&point)[3], double *linePoint1, double *linePoint2, double distance);

	//Returns 1 if the point is up, -1 if is down and 0 if the point is in the line
	static int PointUpDownLine(double *point, double *lp1, double *lp2);

protected:

	enum { X, Y, Z, };
};
#endif