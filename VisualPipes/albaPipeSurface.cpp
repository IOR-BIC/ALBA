/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurface
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaPipeSurface.h"
#include "albaVMEOutputSurface.h"
#include "albaVME.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeSurface::albaPipeSurface()
:albaPipeGenericPolydata()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
albaPipeSurface::~albaPipeSurface()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
vtkPolyData *albaPipeSurface::GetInputAsPolyData()
{
	if (!m_InputAsPolydata)
	{
		assert(m_Vme->GetOutput()->IsALBAType(albaVMEOutputSurface));
		albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
		assert(surface_output);
		surface_output->Update();
		m_InputAsPolydata = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
		assert(m_InputAsPolydata);
	}

	return m_InputAsPolydata;
}
