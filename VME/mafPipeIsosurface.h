/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeIsosurface.h,v $
Language:  C++
Date:      $Date: 2006-11-10 11:45:18 $
Version:   $Revision: 1.7 $
Authors:   Alexander Savenko  -  Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeIsosurface_H__
#define __mafPipeIsosurface_H__

#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkVolume;
class vtkContourVolumeMapper;
class vtkPolyDataMapper;
class vtkOutlineCornerFilter;
class mafVME;
class mmgFloatSlider;
class mafEventBase;
class mafVMESurface;

//----------------------------------------------------------------------------
// mafPipeIsosurface :
//----------------------------------------------------------------------------
class mafPipeIsosurface : public mafPipe 
{
public:
	mafTypeMacro(mafPipeIsosurface, mafPipe);

	mafPipeIsosurface();
	virtual  ~mafPipeIsosurface();

	/** process events coming from gui */
	virtual void OnEvent(mafEventBase *maf_event);

	virtual void Create(mafSceneNode *n);
	virtual void Select(bool select);

	/** Set the contour value for contour mapper, return false if contour mapper is NULL, otherwise return true. */
	bool   SetContourValue(float value);

	/** return the contour value. */
	float  GetContourValue();

	/** IDs for the GUI */
	enum PIPE_ISOSURFACE_WIDGET_ID
	{
		ID_CONTOUR_VALUE = Superclass::ID_LAST,
		ID_GENERATE_ISOSURFACE,
		ID_ALPHA_VALUE,
		ID_CAMERA_FRONT,
		ID_CAMERA_BACK,
		ID_CAMERA_LEFT,
		ID_CAMERA_RIGHT,
		ID_CAMERA_TOP,
		ID_CAMERA_BOTTOM,
		ID_LAST
	};

protected:
	virtual mmgGui  *CreateGui();

	mafVMESurface            *m_IsosurfaceVme;

	vtkContourVolumeMapper   *m_ContourMapper; 
	vtkVolume                *m_Volume;

	vtkOutlineCornerFilter   *m_OutlineBox;
	vtkPolyDataMapper        *m_OutlineMapper;
	vtkActor                 *m_OutlineActor;

	mmgFloatSlider  *m_ContourSlider;
	mmgFloatSlider  *m_AlphaSlider;
	double m_ContourValue;
	double m_AlphaValue;
};  
#endif // __mafPipeIsosurface_H__
