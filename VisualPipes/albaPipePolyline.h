/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePolyline
 Authors: Matteo Giacomoni - Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipePolyline_H__
#define __albaPipePolyline_H__

#include "albaPipeWithScalar.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkALBATubeFilter;
class vtkGlyph3D;
class vtkSphereSource;
class vtkAppendPolyData;
class vtkColorTransferFunction;
class vtkPolyData;
class mmaMaterial;
class albaGUIMaterialButton;
class vtkCaptionActor2D;
class vtkSplineFilter;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,vtkCaptionActor2D*);
#endif

/**
  Class Name: albaPipePolyline
  Visualize Polyline inside a VTK Render Window with several modality:
  - line
  - glyph connected
  - glyph unconnected
  - tube
  It can be set tube and glyph dimension and resolution.
  It is also possible to create a spline of the polyline or visualize polyline resulted by a
  deformation of the original one.
*/
class ALBA_EXPORT albaPipePolyline : public albaPipeWithScalar
{
public:
  /** RTTI Macro. */
	albaTypeMacro(albaPipePolyline, albaPipeWithScalar);

  /** constructor.*/
	albaPipePolyline();
  /** destructor. */
	virtual     ~albaPipePolyline ();

	/** process events coming from gui */
	virtual void OnEvent(albaEventBase *alba_event);

  /** create the pipeline. */
	virtual void Create(albaSceneNode *n);
  /** when vme is selected, it can be catch the event in select function. */
	virtual void Select(bool select); 

	/** Set the visual representation of the polyline.
	Acceptable values are 0 (POLYLINE), 1 (TUBE) or 2 (SPHERE GLYPHED).*/
	void SetRepresentation(int representation);

	/** Set the polyline representation as simple polyline.*/
	void SetRepresentationToPolyline() {SetRepresentation(LINES);};

	/** Set the polyline representation as tube.*/
	void SetRepresentationToTube() {SetRepresentation(TUBES);};

	/** Set the polyline representation as sphere glyphed polyline.*/
	void SetRepresentationToGlyph() {SetRepresentation(SPHERES);};

	/** Set The Radius */
	void SetRadius(double radius);

	/** Set color of the polyline */
	void SetColor(double color[3]);

  /** Set spline mode of the polyline */
  void SetSplineMode(int flag){m_SplineMode = flag;};
  /** Get spline mode of the polyline */
  int GetSplineMode(){return m_SplineMode;};

  /** Enable spline mode */
  void SplineModeOn(){SetSplineMode(1);UpdateProperty();};
  /** Disable spline mode */
  void SplineModeOff(){SetSplineMode(0);UpdateProperty();};
		
  /**Set Opacity of the actors */
  void SetOpacity(double value);

	void SetShowSphere(bool show);

  /**Set if actor is pickable */
  void SetActorPicking(int enable);

  /** Set Tube Capping */
  void SetTubeCapping(int capping){m_Capping = capping;};
  /** Set/Get Tube Capping */
  int GetTubeCapping(){return m_Capping;};

  /** Set Tube Resolution */
  void SetTubeResolution(double resolution){m_TubeResolution = resolution;};
  /** Get Tube Resolution */
  double GetTubeResolution(){return m_TubeResolution;};

  /** Set Glyph Resolution */
  void SetGlyphResolution(double resolution){m_SphereResolution = resolution;};
  /** Get Glyph Resolution */
  double GetGlyphResolution(){return m_SphereResolution;};

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false);

	/** Core of the pipe */
	virtual void ExecutePipe();


	/** IDs for the GUI */
	enum PIPE_POLYLINE_WIDGET_ID
	{
		ID_POLYLINE_REPRESENTATION = Superclass::ID_LAST,
		ID_TUBE_RADIUS,
		ID_TUBE_RESOLUTION,
		ID_TUBE_CAPPING,
		ID_SPHERE_RADIUS,
		ID_SPHERE_RESOLUTION,
		ID_SCALAR_DIMENSION,
		ID_SCALAR,
		ID_SHOW_SPHERES,
		ID_SPLINE,
		ID_SPLINE_PARAMETERS,
		ID_CREATE_VME,
		ID_LAST
	};

	enum POLYLINE_REPRESENTATION
	{
		LINES = 0,
		TUBES,
		SPHERES,
	};

protected:
	vtkSphereSource        *m_Sphere;
	vtkGlyph3D             *m_Glyph;
	vtkALBATubeFilter       *m_Tube;
	vtkOutlineCornerFilter *m_OutlineBox;
	vtkPolyDataMapper      *m_OutlineMapper;
	vtkProperty            *m_OutlineProperty;
	vtkActor               *m_OutlineActor;
	vtkAppendPolyData			 *m_AppendPolyData;
	vtkSplineFilter				 *m_SplineFilter;
	albaGUIMaterialButton   *m_MaterialButton;
  
	int m_ScalarDim;
	int m_Representation;
	int m_Capping;
  int m_SplineMode;
	int m_ShowSpheres;
	double m_TubeRadius;
	double m_SphereRadius;
	double m_TubeResolution;
	double m_SphereResolution;
	double m_SplineBias;
	double m_SplineContinuty;
	double m_SplineTension;

  double m_Opacity;

	/** Initialize representation, capping, radius and resolution variables.*/
	void InitializeFromTag();
		 
	/** create gui */
	virtual albaGUI  *CreateGui();

	void EnableDisableGui();
private:
	void CreateVMEFromVisualization();
};
#endif // __albaPipePolyline_H__
