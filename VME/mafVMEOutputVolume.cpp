/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputVolume.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-30 16:31:19 $
  Version:   $Revision: 1.2 $
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
#include <wx/busyinfo.h>
#include "mafVMEOutputVTK.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafDataPipe.h"
#include "mmgGui.h"

#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputVolume)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputVolume::mafVMEOutputVolume()
//-------------------------------------------------------------------------
{
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
mmgGui* mafVMEOutputVolume::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();

  wxBusyCursor wait;

  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe()->GetVTKData())
  {
    this->Update();
    m_VME->GetOutput()->GetVMEBounds(m_VolumeBounds);
  }
  mafString vtk_data_type;
  vtk_data_type << this->GetVTKData()->GetClassName();
  m_Gui->Label("vtk type: ", vtk_data_type, true);
  m_Gui->Label(" bounds: ",true);
  mafString b;
  b << m_VolumeBounds.m_Bounds[0] << "   xmax: " << m_VolumeBounds.m_Bounds[1];
  m_Gui->Label(" xmin: ", b);
  b = "";
  b << m_VolumeBounds.m_Bounds[2] << "   ymax: " << m_VolumeBounds.m_Bounds[3];
  m_Gui->Label(" ymin: ", b);
  b = "";
  b << m_VolumeBounds.m_Bounds[4] << "   zmax: " << m_VolumeBounds.m_Bounds[5];
  m_Gui->Label(" zmin: ", b);
  if (m_VME->IsMAFType(mafVMEVolumeGray))
  {
    m_Gui->Label("scalar range:",true);
    double srange[2];
    this->GetVTKData()->Update();
    this->GetVTKData()->GetScalarRange(srange);
    mafString range_text;
    range_text << srange[0] << "    max: " << srange[1];
    m_Gui->Label(" min: ",range_text);
  }
  return m_Gui;
}
