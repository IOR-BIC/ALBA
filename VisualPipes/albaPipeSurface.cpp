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
#include "vtkTrivialProducer.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeSurface);

//----------------------------------------------------------------------------
albaPipeSurface::albaPipeSurface():albaPipeGenericPolydata()
{
}

//----------------------------------------------------------------------------
albaPipeSurface::~albaPipeSurface()
{
	vtkDEL(m_TrivialProd);
}

//----------------------------------------------------------------------------
vtkAlgorithmOutput* albaPipeSurface::GetPolyDataOutputPort()
{
	if (!m_PolydataConnection)
	{
		assert(m_Vme->GetOutput()->IsALBAType(albaVMEOutputSurface));
		albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
		assert(surface_output);
		surface_output->Update();
		vtkNEW(m_TrivialProd);
		m_TrivialProd->SetOutput(surface_output->GetVTKData());
		m_PolydataConnection = m_TrivialProd->GetOutputPort();
	}

	return m_PolydataConnection;
}
