/*=========================================================================

 Program: MAF2
 Module: mafPipeSurface
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafPipeSurface.h"
#include "mafVMEOutputSurface.h"
#include "mafVME.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeSurface::mafPipeSurface()
:mafPipeGenericPolydata()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
mafPipeSurface::~mafPipeSurface()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
vtkPolyData *mafPipeSurface::GetInputAsPolyData()
{
	if (!m_InputAsPolydata)
	{
		assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputSurface));
		mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
		assert(surface_output);
		surface_output->Update();
		m_InputAsPolydata = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
		assert(m_InputAsPolydata);
	}

	return m_InputAsPolydata;
}
