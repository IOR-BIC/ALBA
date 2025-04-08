/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputVolume
 Authors: Marco Petrone
 
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


#include "albaVMEOutputVolume.h"
#include "albaVMEOutputVTK.h"
#include "albaVME.h"
#include "albaVMEVolumeGray.h"
#include "albaDataPipe.h"
#include "albaGUI.h"
#include "mmaVolumeMaterial.h"

#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkUnstructuredGrid.h"

#include <assert.h>

#define NULL_STRING_DATA "--"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputVolume)

//-------------------------------------------------------------------------
albaVMEOutputVolume::albaVMEOutputVolume()
{
  m_Material = NULL;
}

//-------------------------------------------------------------------------
albaVMEOutputVolume::~albaVMEOutputVolume()
{
}

//-------------------------------------------------------------------------
vtkImageData *albaVMEOutputVolume::GetStructuredData()
{
  return vtkImageData::SafeDownCast(GetVTKData());
}

//-------------------------------------------------------------------------
vtkRectilinearGrid *albaVMEOutputVolume::GetRectilinearData()
{
  return vtkRectilinearGrid::SafeDownCast(GetVTKData());
}
//-------------------------------------------------------------------------
vtkUnstructuredGrid *albaVMEOutputVolume::GetUnstructuredData()
{
  return vtkUnstructuredGrid::SafeDownCast(GetVTKData());
}
//-------------------------------------------------------------------------
mmaVolumeMaterial *albaVMEOutputVolume::GetMaterial()
{
  // if the VME set the material directly in the output return it
  if (m_Material)
    return  m_Material;

  // search for a material attribute in the VME connected to this output
  return GetVME() ? mmaVolumeMaterial::SafeDownCast(GetVME()->GetAttribute("VolumeMaterialAttributes")) : NULL;
}
//-------------------------------------------------------------------------
void albaVMEOutputVolume::SetMaterial(mmaVolumeMaterial *material)
{
  m_Material = material;
}

//-------------------------------------------------------------------------
albaGUI* albaVMEOutputVolume::CreateGui()
{
  assert(m_Gui == NULL);
  m_Gui = albaVMEOutput::CreateGui();

  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe()->GetVTKData())
  {
    this->Update();
  }
  m_VtkDataType << NULL_STRING_DATA;

  vtkDataSet *data = this->GetVTKData();
  if (data != NULL)
  {
    m_VtkDataType.Erase(0);
    m_VtkDataType << this->GetVTKData()->GetClassName();
  }

	m_Gui->Label(_("Vtk type:"), &m_VtkDataType, true, false, false, 0.35);
  m_Gui->Label(_("Bounds:"),true);
  m_Gui->Label(&m_VolumeBounds[0]);
  m_Gui->Label(&m_VolumeBounds[1]);
  m_Gui->Label(&m_VolumeBounds[2]);
	m_Gui->Label(_("Dims:"), true);
	m_Gui->Label(&m_VolumeDims);
  m_Gui->Label(_("Scalar range:"),true);
  m_Gui->Label(&m_ScaralRangeString);
  m_Gui->Divider();

	return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEOutputVolume::Update()
{
  assert(m_VME);
  m_VME->Update();
  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe()->GetVTKData())
  {
		vtkDataSet * vtkData = GetVTKData();
    m_VtkDataType = "";
    m_VtkDataType << vtkData->GetClassName();

		double b[6];
    GetVMELocalBounds(b);
    m_VolumeBounds[0] = "";
    m_VolumeBounds[0] << " xmin: " << albaString::Format("%g",RoundValue(b[0])).ToAscii() << "   xmax: " << albaString::Format("%g",RoundValue(b[1])).ToAscii();
    m_VolumeBounds[1] = "";
    m_VolumeBounds[1] << " ymin: " << albaString::Format("%g",RoundValue(b[2])).ToAscii() << "   ymax: " << albaString::Format("%g",RoundValue(b[3])).ToAscii();
    m_VolumeBounds[2] = "";
    m_VolumeBounds[2] << " zmin: " << albaString::Format("%g",RoundValue(b[4])).ToAscii() << "   zmax: " << albaString::Format("%g",RoundValue(b[5])).ToAscii();

		int dims[3] = { 0,0,0 };
		if (vtkImageData::SafeDownCast(vtkData))
			((vtkImageData *)vtkData)->GetDimensions(dims);
		else if (vtkRectilinearGrid::SafeDownCast(vtkData))
			((vtkRectilinearGrid *)vtkData)->GetDimensions(dims);
		m_VolumeDims.Printf("[%d, %d, %d]", dims[0], dims[1], dims[2]);
		
		double srange[2];
		vtkData->Update();
		vtkData->GetScalarRange(srange);
    m_ScaralRangeString = albaString::Format("min: %6.2f max: %6.2f", srange[0], srange[1]);;
  }
  
  if (m_Gui)
    m_Gui->Update();
}
