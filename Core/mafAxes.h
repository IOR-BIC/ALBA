/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafAxes.h,v $
Language:  C++
Date:      $Date: 2011-10-25 20:36:07 $
Version:   $Revision: 1.1.22.5 $
Authors:   Silvano Imboden , Stefano perticoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafAxes_h
#define __mafAxes_h

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class vtkRenderer;
class vtkAxes;
class vtkPolyDataMapper2D;
class vtkCoordinate;
class vtkActor2D;
class vtkLookupTable;
class vtkMatrix4x4;
class vtkPolyData;
class vtkMAFAnnotatedCubeActor;
class vtkMAFOrientationMarkerWidget;
class vtkMAFGlobalAxesHeadActor;

//----------------------------------------------------------------------------
class MAF_EXPORT mafAxes
	//----------------------------------------------------------------------------
	/**
	Create 3D axes representation and insert them in the passed renderer: axes can be global (World Coordinates
	reference system )or local ( VME reference system ).
	
	Axes representation can be:
	TRIAD: Common 3D refsys featuring 3 orthogonal axes
	CUBE: A rotating cube featuring orientation feedback letters on cube faces
    HEAD: A rotating head

	Some API use cases follows.

	mafAxes(renderer, NULL, TRIAD); => Create a global axis triad 2D actor in the passed renderer
	mafAxes(renderer, vme, TRIAD); => Create a vme local axis triad 2D actor in the passed renderer
	
	mafAxes(renderer, NULL, CUBE); => Create a 3D global axes cube on a new renderer and on a superimposed layer
	//mafAxes(renderer, vme, CUBE); => BEWARE !!! NOT SUPPORTED !!!
	
	mafAxes(renderer, NULL, HEAD); => Create a 3D global axes head on a new renderer and on a superimposed layer
	mafAxes(renderer, vme, HEAD); => Create a 3D local axes head on a new renderer and on a superimposed layer
	
	If a vme is provided, the axes will represent 
	the local vme-reference system.

	If vme is NULL, the axes will represent the global
	reference system

	@sa mafAxesTest for example usage

	@todo API improvements needed, see mafAxesTest for api improvement use cases
	*/
{
public:

	enum AXIS_TYPE_ENUM
	{
		TRIAD = 0, // default: 2D actor representing 3 small axes attached to the main render window camera
		CUBE, // 3D actor representing a rotating cube with anatomical directions annotations attached to the main render window camera
		HEAD, // 3D actor representing a rotating head attached to the main render window camera
		NUMBER_OF_AXES_TYPES,
	};

	/** Constructor note: given renderer ren cannot be NULL */
	mafAxes(vtkRenderer *ren, mafVME* vme = NULL, int axesType = TRIAD);
	virtual ~mafAxes();

	/** Set the axes visibility. */
	void     SetVisibility(bool show);

	/** Force the position of the Axes, if omitted the ABS vme matrix is used instead. 
	Works only if a vme was specified in the constructor*/
	void     SetPose( vtkMatrix4x4 *abs_pose_matrix = NULL);

protected:

	mafVME      *m_Vme;
	vtkRenderer *m_Renderer;
	vtkAxes                *m_TriadAxes;
	vtkPolyDataMapper2D		 *m_AxesMapper2D;
	vtkCoordinate 				 *m_Coord;
	vtkActor2D						 *m_AxesActor2D;
	vtkLookupTable				 *m_AxesLUT;
	int m_AxesType;
	vtkMAFOrientationMarkerWidget* m_OrientationMarkerWidget ;
	vtkMAFAnnotatedCubeActor* m_AnnotatedCubeActor;
	vtkMAFGlobalAxesHeadActor *m_GlobalAxesHeadActor ;

	/** friend test class */
	friend class mafAxesTest;
};
#endif
