/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAxes
 Authors: Silvano Imboden , Stefano perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaAxes_h
#define __albaAxes_h

//----------------------------------------------------------------------------
// Forward declarations :
//----------------------------------------------------------------------------
class albaVME;
class vtkRenderer;
class vtkAxes;
class vtkPolyDataMapper2D;
class vtkCoordinate;
class vtkActor2D;
class vtkLookupTable;
class vtkMatrix4x4;
class vtkPolyData;
class vtkALBAAnnotatedCubeActor;
class vtkALBAOrientationMarkerWidget;
class vtkALBAGlobalAxesPolydataActor;

//----------------------------------------------------------------------------
class ALBA_EXPORT albaAxes
	/**
	Create 3D axes representation and insert them in the passed renderer: axes can be global (World Coordinates
	reference system )or local ( VME reference system ).
	
	Axes representation can be:
	TRIAD: Common 3D refsys featuring 3 orthogonal axes
	CUBE: A rotating cube featuring orientation feedback letters on cube faces
  HEAD: A rotating head
	BODY: A rotating body

	Some API use cases follows.

	albaAxes(renderer, NULL, TRIAD); => Create a global axis triad 2D actor in the passed renderer
	albaAxes(renderer, vme, TRIAD); => Create a vme local axis triad 2D actor in the passed renderer
	
	albaAxes(renderer, NULL, CUBE); => Create a 3D global axes cube on a new renderer and on a superimposed layer
	//albaAxes(renderer, vme, CUBE); => BEWARE !!! NOT SUPPORTED !!!
	
	albaAxes(renderer, NULL, HEAD); => Create a 3D global axes head on a new renderer and on a superimposed layer
	albaAxes(renderer, vme, HEAD); => Create a 3D local axes head on a new renderer and on a superimposed layer

	albaAxes(renderer, NULL, BODY); => Create a 3D global axes body on a new renderer and on a superimposed layer
	albaAxes(renderer, vme, BODY); => Create a 3D local axes body on a new renderer and on a superimposed layer

	If a vme is provided, the axes will represent 
	the local vme-reference system.

	If vme is NULL, the axes will represent the global
	reference system

	@sa albaAxesTest for example usage

	@todo API improvements needed, see albaAxesTest for api improvement use cases
	*/
{
public:

	enum AXIS_TYPE_ENUM
	{
		TRIAD = 0, // default: 2D actor representing 3 small axes attached to the main render window camera
		CUBE, // 3D actor representing a rotating cube with anatomical directions annotations attached to the main render window camera
		HEAD, // 3D actor representing a rotating head attached to the main render window camera
		BODY, // 3D actor representing a rotating body attached to the main render window camera
		NUMBER_OF_AXES_TYPES,
	};

	/** Constructor note: given renderer ren cannot be NULL */
	albaAxes(vtkRenderer *ren, albaVME* vme = NULL, int axesType = TRIAD);
	virtual ~albaAxes();

	/** Set the axes visibility. */
	void     SetVisibility(bool show);

	/** Force the position of the Axes, if omitted the ABS vme matrix is used instead. 
	Works only if a vme was specified in the constructor*/
	void     SetPose( vtkMatrix4x4 *abs_pose_matrix = NULL);

protected:

	albaVME								*m_Vme;
	vtkRenderer						*m_Renderer;
	vtkAxes               *m_TriadAxes;
	vtkPolyDataMapper2D		*m_AxesMapper2D;
	vtkCoordinate 				*m_Coord;
	vtkActor2D						*m_AxesActor2D;
	vtkLookupTable				*m_AxesLUT;

	int m_AxesType;
	vtkALBAOrientationMarkerWidget	*m_OrientationMarkerWidget ;
	vtkALBAAnnotatedCubeActor				*m_AnnotatedCubeActor;
	vtkALBAGlobalAxesPolydataActor	*m_GlobalAxesPolydataActor ;

	/** friend test class */
	friend class albaAxesTest;
};
#endif
