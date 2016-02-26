/*=========================================================================

 Program: MAF2
 Module: mafPipePolyline
 Authors: Matteo Giacomoni - Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipePolyline_H__
#define __mafPipePolyline_H__

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkMAFTubeFilter;
class vtkGlyph3D;
class vtkSphereSource;
class vtkAppendPolyData;
class vtkColorTransferFunction;
class vtkPolyData;
class mmaMaterial;
class mafGUIMaterialButton;
class vtkCaptionActor2D;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,vtkCaptionActor2D*);
#endif

/**
  Class Name: mafPipePolyline
  Visualize Polyline inside a VTK Render Window with several modality:
  - line
  - glyph connected
  - glyph unconnected
  - tube
  It can be set tube and glyph dimension and resolution.
  It is also possible to create a spline of the polyline or visualize polyline resulted by a
  deformation of the original one.
*/
class MAF_EXPORT mafPipePolyline : public mafPipe
{
public:
  /** RTTI Macro. */
	mafTypeMacro(mafPipePolyline,mafPipe);

  /** constructor.*/
	mafPipePolyline();
  /** destructor. */
	virtual     ~mafPipePolyline ();

	/** process events coming from gui */
	virtual void OnEvent(mafEventBase *maf_event);

  /** create the pipeline. */
	virtual void Create(mafSceneNode *n);
  /** when vme is selected, it can be catch the event in select funztion. */
	virtual void Select(bool select); 

	/** Set the visual representation of the polyline.
	Acceptable values are 0 (POLYLINE), 1 (TUBE) or 2 (SPHERE GLYPHED).*/
	void SetRepresentation(int representation);

	/** Set the polyline representation as simple polyline.*/
	void SetRepresentationToPolyline() {SetRepresentation(POLYLINE);};

	/** Set the polyline representation as tube.*/
	void SetRepresentationToTube() {SetRepresentation(TUBE);};

	/** Set the polyline representation as sphere glyphed polyline.*/
	void SetRepresentationToGlyph() {SetRepresentation(GLYPH);};

	/** Set the polyline representation as sphere glyphed polyline.*/
	void SetRepresentationToUnconnectedGlyph() {SetRepresentation(GLYPH_UNCONNECTED);};

	/** Set The Radius */
	void SetRadius(double radius);

	/** Set color of the polyline */
	void SetColor(double color[3]);

	/** Set color of the polyline */
	void SetMapperScalarRange(double range[2]);

  /* Set lookup table color range*/
	void SetLookupTableColorRange(double range[2], double colorMin[3], double colorMax[3]);


  /** Set spline mode of the polyline */
  void SetSplineMode(int flag){m_SplineMode = flag;};
  /** Get spline mode of the polyline */
  int GetSplineMode(){return m_SplineMode;};

  /** Enable spline mode */
  void SplineModeOn(){SetSplineMode(1);UpdateProperty();};
  /** Disable spline mode */
  void SplineModeOff(){SetSplineMode(0);UpdateProperty();};

	/** Calculate the cardinal spline over original polyline*/
  vtkPolyData *SplineProcess(vtkPolyData *polyData);
	
  /** Set Border Distance*/
  void SetDistanceBorder(double value){m_DistanceBorder = value;};
  /** Get Border Distance*/
  double GetDistanceBorder(){return m_DistanceBorder;};

  /** Set the half number of borders. Actually it will be created a number of border equal  to halfNumberOfBorders,
      in order to have 2n+1 number of splines in which the middle one is the original. */
  void SetHalfNumberOfBorders(int halfNumberOfBorders)
  {
    if(halfNumberOfBorders >= 0)
    {
      m_HalfNumberOfBorders = halfNumberOfBorders;
    }
  };

  /** Modify data in order to create a up & down border */
  vtkPolyData *BorderCreation();

  /**Set Opacity of the actors */
  void SetOpacity(double value);

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

  /** Show/Hide Text Identify Border */
  void SetTextIdentifierBorderVisibility(int visibility){m_TextIdentifierBorderVisibility = visibility;};

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false);

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
    ID_SPLINE,
    ID_DISTANCE_BORDER,
		ID_LAST
	};

	enum POLYLINE_REPRESENTATION
	{
		POLYLINE = 0,
		TUBE,
		GLYPH,
		GLYPH_UNCONNECTED,
	};

protected:
	vtkSphereSource        *m_Sphere;
	vtkGlyph3D             *m_Glyph;
	vtkMAFTubeFilter       *m_Tube;
	vtkPolyDataMapper	     *m_Mapper;
	vtkActor               *m_Actor;
	vtkOutlineCornerFilter *m_OutlineBox;
	vtkPolyDataMapper      *m_OutlineMapper;
	vtkProperty            *m_OutlineProperty;
	vtkActor               *m_OutlineActor;

  vtkAppendPolyData      *m_BorderData;
  
  vtkPolyDataMapper      *m_BorderMapper;
  vtkProperty            *m_BorderProperty;
  vtkActor               *m_BorderActor;
  
	vtkColorTransferFunction *m_Table;
  mmaMaterial              *m_PolylineMaterial;
	vtkPolyData              *m_PolyFilteredLine;
  mafGUIMaterialButton        *m_MaterialButton;
  std::vector<vtkCaptionActor2D *> m_CaptionActorList;
  
	wxString *m_ScalarsName;
  
	int m_Scalar;
	int m_ScalarDim;
	int m_Representation;
	int m_Capping;
  int m_SplineMode;
  int m_HalfNumberOfBorders;
  int m_TextIdentifierBorderVisibility;
	double m_TubeRadius;
	double m_SphereRadius;
	double m_TubeResolution;
	double m_SphereResolution;
  double m_SplineCoefficient; 
  double m_DistanceBorder;
  double m_Opacity;

	/** Initialize representation, capping, radius and resolution variables.*/
	void InitializeFromTag();

  /** Update data structures. */
	void UpdateData();

  /** Update attributes of the vme */
	void UpdateScalars();

  /** update specific scalar array*/
  void UpdatePipeFromScalars();
  /** create/edit caption actor of the borders*/
  void SetCaptionActorBorder(int index, double position[3]);
  /** destroy all caption actors*/
  void DeleteCaptionActorList();

	/** create gui */
	virtual mafGUI  *CreateGui();
};  
#endif // __mafPipePolyline_H__
