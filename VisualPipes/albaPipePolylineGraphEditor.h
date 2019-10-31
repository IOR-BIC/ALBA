/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePolylineGraphEditor
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipePolylineGraphEditor_H__
#define __albaPipePolylineGraphEditor_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeSlice.h"
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkSphereSource;
class vtkGlyph3D;
class vtkPolyDataMapper;
class vtkAppendPolyData;
class vtkActor;
class vtkLookupTable;
class vtkALBAFixedCutter;
class vtkPlane;

//----------------------------------------------------------------------------
// albaPipePolylineGraphEditor :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipePolylineGraphEditor : public albaPipe, public albaPipeSlice
{
public:
	albaTypeMacro(albaPipePolylineGraphEditor,albaPipe);

	albaPipePolylineGraphEditor();
	virtual ~albaPipePolylineGraphEditor();

	/** process events coming from gui */
	virtual void OnEvent(albaEventBase *alba_event);

	/** IDs for the GUI */
	enum PIPE_VOLUME_MIP_WIDGET_ID
	{
		ID_LUT_CHOOSER = Superclass::ID_LAST,
		ID_RESAMPLE_FACTOR,
		ID_LAST
	};

	enum MODALITY
	{
		ID_SLICE = 0,
		ID_PERSPECTIVE,
	};

	virtual void Create(albaSceneNode *n);
	void Show(bool show); 
	virtual void Select(bool select);

  /** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
  /*virtual*/ void SetSlice(double* Origin, double* Normal);  

	void SetModalitySlice();
	void SetModalityPerspective();

protected:
	virtual albaGUI  *CreateGui();

	vtkPolyDataMapper	*m_Mapper;
	vtkActor					*m_Actor;
	vtkALBAFixedCutter		*m_Cutter;
	vtkPlane				  *m_Plane;	

	int	m_Modality;

	vtkLookupTable		*m_LUT;
};
#endif // __albaPipePolylineGraphEditor_H__
