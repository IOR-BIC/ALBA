/*=========================================================================

 Program: MAF2Medical
 Module: medPipePolylineGraphEditor
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medPipePolylineGraphEditor_H__
#define __medPipePolylineGraphEditor_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medVMEDefines.h"
#include "mafPipeSlice.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkSphereSource;
class vtkGlyph3D;
class vtkTubeFilter;
class vtkPolyDataMapper;
class vtkAppendPolyData;
class vtkActor;
class vtkLookupTable;
class vtkMAFFixedCutter;
class vtkPlane;

//----------------------------------------------------------------------------
// medPipePolylineGraphEditor :
//----------------------------------------------------------------------------
class MED_VME_EXPORT medPipePolylineGraphEditor : public mafPipeSlice
{
public:
	mafTypeMacro(medPipePolylineGraphEditor,mafPipe);

	medPipePolylineGraphEditor();
	virtual ~medPipePolylineGraphEditor();

	/** process events coming from gui */
	virtual void OnEvent(mafEventBase *maf_event);

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

	virtual void Create(mafSceneNode *n);
	void Show(bool show); 
	virtual void Select(bool select);

  /** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
  /*virtual*/ void SetSlice(double* Origin, double* Normal);  

	void SetModalitySlice();
	void SetModalityPerspective();

protected:
	virtual mafGUI  *CreateGui();

	vtkPolyDataMapper	*m_Mapper;
	vtkActor					*m_Actor;
	vtkMAFFixedCutter		*m_Cutter;
	vtkPlane				  *m_Plane;	

	int	m_Modality;

	vtkLookupTable		*m_LUT;
};
#endif // __medPipePolylineGraphEditor_H__
