/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipePolyline.h,v $
Language:  C++
Date:      $Date: 2007-06-25 09:23:47 $
Version:   $Revision: 1.8 $
Authors:   Matteo Giacomoni - Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
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
class vtkTubeFilter;
class vtkGlyph3D;
class vtkSphereSource;
class vtkAppendPolyData;
class vtkColorTransferFunction;
class vtkPolyData;
class mmaMaterial;

//----------------------------------------------------------------------------
// mafPipePolyline :
//----------------------------------------------------------------------------
class mafPipePolyline : public mafPipe
{
public:
	mafTypeMacro(mafPipePolyline,mafPipe);

	mafPipePolyline();
	virtual     ~mafPipePolyline ();

	/** process events coming from gui */
	virtual void OnEvent(mafEventBase *maf_event);

	virtual void Create(mafSceneNode *n);
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

	void SetLookupTableColorRange(double range[2], double colorMin[3], double colorMax[3]);


  /** Set spline mode of the polyline */
  void SetSplineMode(int flag){m_SplineMode = flag;};
  int GetSplineMode(){return m_SplineMode;};

  void SplineModeOn(){SetSplineMode(1);UpdateProperty();};
  void SplineModeOff(){SetSplineMode(0);UpdateProperty();};

	/** Calculate the cardinal spline over original polyline*/
  vtkPolyData *SplineProcess(vtkPolyData *polyData);

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
	vtkTubeFilter          *m_Tube;
	vtkPolyDataMapper	     *m_Mapper;
	vtkActor               *m_Actor;
	vtkOutlineCornerFilter *m_OutlineBox;
	vtkPolyDataMapper      *m_OutlineMapper;
	vtkProperty            *m_OutlineProperty;
	vtkActor               *m_OutlineActor;
	vtkAppendPolyData			 *m_Apd;;
	vtkColorTransferFunction *m_Table;
  mmaMaterial              *m_PolylineMaterial;
	vtkPolyData              *m_PolySpline;

	wxString *m_ScalarsName;

	int m_Scalar;
	int m_ScalarDim;
	int m_Representation;
	int m_Capping;
  int m_SplineMode;
	double m_TubeRadius;
	double m_SphereRadius;
	double m_TubeResolution;
	double m_SphereResolution;
  double m_SplineCoefficient;

	/** Initialize representation, capping, radius and resolution variables.*/
	void InitializeFromTag();

	void UpdateData();

	void UpdateScalars();

  void UpdatePipeFromScalars();

	/** Update visual properties*/
	void UpdateProperty(bool fromTag = false);
	virtual mmgGui  *CreateGui();
};  
#endif // __mafPipePolyline_H__
