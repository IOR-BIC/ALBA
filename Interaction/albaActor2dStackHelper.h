/*=========================================================================
Program:  ALBA (Agile Library for Biomedical Applications)
Module:    albaActor2dStackHelper.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaActor2dStackHelper_h
#define __albaActor2dStackHelper_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaObject.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkPolyDataSource;
class vtkPolyDataMapper2D;
class vtkActor2D;
class vtkRenderer;
class vtkProperty2D;

struct Color
{
	double R;
	double G;
	double B;
	double Alpha;
};
	
class ALBA_EXPORT albaActor2dStackHelper
{
public:
	albaActor2dStackHelper();
	albaActor2dStackHelper(vtkPolyDataSource *source, vtkRenderer *renderer);
	virtual ~albaActor2dStackHelper();

	/** Returns the PolyDataSource*/
	vtkPolyDataSource * GetSource() const { return m_Source; }

	/** Returns the Mapper 2D*/
	vtkPolyDataMapper2D * GetMapper() const { return m_Mapper; }

	/** Returns the Actor 2D*/
	vtkActor2D * GetActor() const { return m_Actor; }

	/** Returns the Actor property*/
	vtkProperty2D* GetProperty();

	/** Sets color and opacity to the actor */
	void SetColor(Color col);

	/** Sets actor visibility*/
	void SetVisibility(bool show);

	vtkRenderer * GetRenderer() const { return m_Renderer; }
	void SetRenderer(vtkRenderer * val) { m_Renderer = val; }

protected:
	vtkPolyDataSource		*m_Source;
	vtkPolyDataMapper2D	*m_Mapper;
	vtkActor2D					*m_Actor;
	vtkRenderer					*m_Renderer;
};

#endif