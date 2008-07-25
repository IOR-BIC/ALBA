/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputVolume.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:06:00 $
  Version:   $Revision: 1.15 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafVMEOutputVolume.h"
#include "mafVMEOutputVTK.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafDataPipe.h"
#include "mafGUI.h"
#include "mmaVolumeMaterial.h"

#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkUnstructuredGrid.h"

#include <assert.h>

#define NULL_STRING_DATA "--"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputVolume)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputVolume::mafVMEOutputVolume()
//-------------------------------------------------------------------------
{
  m_Material = NULL;
}

//-------------------------------------------------------------------------
mafVMEOutputVolume::~mafVMEOutputVolume()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkImageData *mafVMEOutputVolume::GetStructuredData()
//-------------------------------------------------------------------------
{
  return vtkImageData::SafeDownCast(GetVTKData());
}

//-------------------------------------------------------------------------
vtkRectilinearGrid *mafVMEOutputVolume::GetRectilinearData()
//-------------------------------------------------------------------------
{
  return vtkRectilinearGrid::SafeDownCast(GetVTKData());
}
//-------------------------------------------------------------------------
vtkUnstructuredGrid *mafVMEOutputVolume::GetUnstructuredData()
//-------------------------------------------------------------------------
{
  return vtkUnstructuredGrid::SafeDownCast(GetVTKData());
}
//-------------------------------------------------------------------------
mmaVolumeMaterial *mafVMEOutputVolume::GetMaterial()
//-------------------------------------------------------------------------
{
  // if the VME set the material directly in the output return it
  if (m_Material)
    return  m_Material;

  // search for a material attribute in the VME connected to this output
  return GetVME() ? mmaVolumeMaterial::SafeDownCast(GetVME()->GetAttribute("VolumeMaterialAttributes")) : NULL;
}
//-------------------------------------------------------------------------
void mafVMEOutputVolume::SetMaterial(mmaVolumeMaterial *material)
//-------------------------------------------------------------------------
{
  m_Material = material;
}

//-------------------------------------------------------------------------
mafGUI* mafVMEOutputVolume::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();

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

  m_Gui->Label(_("vtk type: "), &m_VtkDataType);
  m_Gui->Label(_(" bounds: "),true);
  m_Gui->Label(&m_VolumeBounds[0]);
  m_Gui->Label(&m_VolumeBounds[1]);
  m_Gui->Label(&m_VolumeBounds[2]);
  m_Gui->Label(_("scalar range:"),true);
  m_Gui->Label(&m_ScaralRangeString);
  m_Gui->Divider();
	return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputVolume::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();
  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe()->GetVTKData())
  {
    m_VtkDataType = "";
    m_VtkDataType << this->GetVTKData()->GetClassName();
    double b[6];
    m_VME->GetOutput()->GetVMELocalBounds(b);
    m_VolumeBounds[0] = "";
    m_VolumeBounds[0] << " xmin: " << wxString::Format("%g",RoundValue(b[0])).c_str() << "   xmax: " << wxString::Format("%g",RoundValue(b[1])).c_str();
    m_VolumeBounds[1] = "";
    m_VolumeBounds[1] << " ymin: " << wxString::Format("%g",RoundValue(b[2])).c_str() << "   ymax: " << wxString::Format("%g",RoundValue(b[3])).c_str();
    m_VolumeBounds[2] = "";
    m_VolumeBounds[2] << " zmin: " << wxString::Format("%g",RoundValue(b[4])).c_str() << "   zmax: " << wxString::Format("%g",RoundValue(b[5])).c_str();
    double srange[2];
    this->GetVTKData()->Update();
    this->GetVTKData()->GetScalarRange(srange);
    m_ScaralRangeString = wxString::Format("min: %6.2f max: %6.2f", srange[0], srange[1]);;
    //m_ScaralRangeString << " min: " << srange[0] << "    max: " << srange[1];
  }
  
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
