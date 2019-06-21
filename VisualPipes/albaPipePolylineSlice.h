/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePolylineSlice
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipePolylineSlice_H__B
#define __albaPipePolylineSlice_H__B

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeSlice.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

class mmaMaterial;
class albaGUIMaterialButton;
class albaAxes;
class vtkActor;
class vtkALBAFixedCutter;
class vtkPlane;
class vtkPolyData;
class vtkALBAPolyDataToSinglePolyLine;
class vtkALBAToLinearTransform;
class vtkClipPolyData;
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
class vtkCaptionActor2D;
class vtkSplineFilter;

/**
  class name: albaPipePolylineSlice.
  Pipe which manage the visualization of the section of a polyline.
*/
class ALBA_EXPORT albaPipePolylineSlice : public albaPipeSlice
{
public:
  /** RTTI Macro*/
  albaTypeMacro(albaPipePolylineSlice,albaPipeSlice);

  /** Constructor. */
  albaPipePolylineSlice();

  /** Destructor. */
  virtual ~albaPipePolylineSlice ();

	/** Pipe creation */
	virtual void Create(albaSceneNode *n /*,bool use_axes = true*/); //Can't add parameters - is Virtual

  /** called if vme is selected  */
	virtual void Select(bool select);
	
	/** process events coming from gui */
	virtual void OnEvent(albaEventBase *alba_event);

  /**Return the thickness of the border*/	
	double GetThickness() { return m_Border; };

  /**Set the thickness value*/
  void SetThickness(double thickness); 

  /**Return the radius of the tube*/	
	double GetRadius() { return m_TubeRadius; };

  /**Set the radius value of the tube*/
	void SetRadius(double radius) { m_TubeRadius = radius; };

	/**Set Opacity of the actors */
	void SetOpacity(double value);

	/** Set color of the polyline */
	void SetColor(double color[3]);

  /** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
  /*virtual*/ void SetSlice(double* Origin, double* Normal);  

	/** Set the visual representation of the polyline.
	Acceptable values are 0 (POLYLINE), 1 (TUBE) or 2 (SPHERE GLYPHED).*/
	void SetRepresentation(int representation);

	/** Set the polyline representation as simple polyline.*/
	void SetRepresentationToPolyline() { SetRepresentation(LINES); };

	/** Set the polyline representation as tube.*/
	void SetRepresentationToTube() { SetRepresentation(TUBES); };

	/** Set the polyline representation as sphere glyphed polyline.*/
	void SetRepresentationToGlyph() { SetRepresentation(SPHERES); };

  /** Set spline mode of the polyline */
  void SetSplineMode(int flag){m_SplineMode = flag;};
  /** Get spline mode of the polyline */
  int GetSplineMode(){return m_SplineMode;};

  /** Set spline mode of the polyline  to ON */
  void SplineModeOn(){SetSplineMode(1);UpdateProperty();};
  /** Set spline mode of the polyline  to OFF*/
  void SplineModeOff(){SetSplineMode(0);UpdateProperty();};

  /** Set fill variable of the polyline */
  void SetFill(int flag){m_Fill = flag;};
  /** Get fill variable of the polyline */
  int GetFill(){return m_Fill;};
  
  /** Set fill variable of the polyline to ON */
  void FillOn(){SetFill(1);UpdateProperty();};
  /** Set fill variable of the polyline to OFF*/
  void FillOff(){SetFill(0);UpdateProperty();};

  /** Show actor of sliced polyline*/
  void ShowActorOn();
  /** Hide actor of sliced polyline*/
  void ShowActorOff();

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_RENDERING_DISPLAY_LIST,
    ID_CHOOSE_TEXTURE,
    ID_TEXTURE_MAPPING_MODE,
    ID_BORDER_CHANGE,
    ID_RADIUS_CHANGE,
    ID_FILL,
		ID_POLYLINE_REPRESENTATION,
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
		ID_LAST
  };

	enum POLYLINE_REPRESENTATION
	{
		LINES = 0,
		TUBES,
		SPHERES,
	};

  /** gui creation */
  virtual albaGUI *CreateGui();
  
	/** update all properties of the pipe */
  void UpdateProperty();

	/** Core of the pipe */
	////virtual void ExecutePipe();

	/** Set Tube Capping */
	void SetTubeCapping(int capping) { m_Capping = capping; };
	/** Set/Get Tube Capping */
	int GetTubeCapping() { return m_Capping; };

	/** Set Tube Resolution */
	void SetTubeResolution(double resolution);
	/** Get Tube Resolution */
	double GetTubeResolution() { return m_TubeResolution; };

	/** Set Glyph Resolution */
	void SetSphereResolution(double resolution);
	/** Get Glyph Resolution */
	double GetGlyphResolution() { return m_SphereResolution; };

  /**Set if actor is pickable */
  void SetActorPicking(int enable);

	void ShowSpheres(bool show) { m_ShowSpheres = show; };

	void SetSphereRadius(double radius) { m_SphereRadius = radius; };

protected:
  /** cap the regions*/
  vtkPolyData *RegionsCapping(vtkPolyData* inputCutters);
  /** apply a capping filter  */
  vtkPolyData *CappingFilter(vtkPolyData* inputBorder);

  vtkPolyDataMapper								*m_Mapper;
  vtkActor												*m_Actor;
  vtkPlane												*m_Plane;
  vtkALBAFixedCutter								*m_Cutter;
  vtkPolyData											*m_PolyFilteredLine;
  vtkALBAPolyDataToSinglePolyLine	*m_PolydataToPolylineFilter;
  vtkALBAToLinearTransform					*m_VTKTransform;
	vtkSphereSource									*m_Sphere;
	vtkGlyph3D											*m_Glyph;
	vtkALBATubeFilter								*m_Tube;
	vtkOutlineCornerFilter					*m_OutlineBox;
	vtkPolyDataMapper								*m_OutlineMapper;
	vtkProperty											*m_OutlineProperty;
	vtkActor												*m_OutlineActor;
	vtkAppendPolyData								*m_AppendPolyData;
	vtkAppendPolyData								*m_CappingPolyData;
	vtkSplineFilter									*m_SplineFilter;
	  
	vtkClipPolyData									*m_ClipPolyData;
	vtkClipPolyData									*m_ClipPolyDataUp;
	vtkClipPolyData									*m_ClipPolyDataDown;

	vtkPolyData											*m_PolyData;

	albaAxes													*m_Axes;
	albaGUIMaterialButton						*m_MaterialButton;

	double	m_Border;
  int			m_Fill;

	double	m_Opacity;

	int			m_ScalarVisibility;

	int			m_ScalarDim;
	int			m_Representation;
	int			m_Capping;
	int			m_SplineMode;
	int			m_ShowSpheres;

	double	m_TubeRadius;
	double	m_SphereRadius;
	double	m_TubeResolution;
	double	m_SphereResolution;

	double	m_SplineBias;
	double	m_SplineContinuty;
	double	m_SplineTension;

	/** Initialize representation, capping, radius and resolution variables.*/
	void InitializeFromTag();

	void EnableDisableGui();
};  
#endif // __albaPipePolylineSlice_H__B
