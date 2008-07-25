/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputPolyline.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:05:59 $
  Version:   $Revision: 1.5 $
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

#include "mafVMEOutputPolyline.h"
#include "mafVME.h"
#include "mafDataPipe.h"
#include "mafGUI.h"
#include "mafIndent.h"
#include "mmaMaterial.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"

#include "vtkMath.h"
#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputPolyline)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputPolyline::mafVMEOutputPolyline()
//-------------------------------------------------------------------------
{
  m_Material = NULL;
  m_NumberOfPoints = "0";
}

//-------------------------------------------------------------------------
mafVMEOutputPolyline::~mafVMEOutputPolyline()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkPolyData *mafVMEOutputPolyline::GetPolylineData()
//-------------------------------------------------------------------------
{
  return (vtkPolyData *)GetVTKData();
}
//-------------------------------------------------------------------------
void mafVMEOutputPolyline::SetMaterial(mmaMaterial *material)
//-------------------------------------------------------------------------
{
  m_Material = material;
}
//-------------------------------------------------------------------------
mafGUI* mafVMEOutputPolyline::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();

  wxBusyCursor wait;
  
  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe()->GetVTKData())
  {
    this->Update();
  }
  mafString vtk_data_type;
  vtk_data_type << mafString(GetVTKData()->GetClassName());
  m_Gui->Label(_("vtk type: "), vtk_data_type, true);
  
  //m_Length = mafString(wxString::Format(_("%.2f"),CalculateLength()));
  m_Gui->Label(_(" Length: "), &m_Length ,true);

  m_NumberOfPoints = mafString(((vtkPolyData *)m_VME->GetOutput()->GetVTKData())->GetNumberOfPoints());
  m_Gui->Label(_("Points: "), &m_NumberOfPoints ,true);

  return m_Gui;
}
//-------------------------------------------------------------------------
mmaMaterial *mafVMEOutputPolyline::GetMaterial()
//-------------------------------------------------------------------------
{
  // if the VME set the material directly in the output return it
  if (m_Material)
    return  m_Material;

  // search for a material attribute in the VME connected to this output
  return GetVME() ? mmaMaterial::SafeDownCast(GetVME()->GetAttribute("MaterialAttributes")) : NULL;
}
//-------------------------------------------------------------------------
void mafVMEOutputPolyline::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();
  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe()->GetVTKData())
  {
  	m_Length = mafString(wxString::Format(_("%.2f"),CalculateLength()));  
    m_NumberOfPoints = mafString(((vtkPolyData *)m_VME->GetOutput()->GetVTKData())->GetNumberOfPoints());
  }
  
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
double mafVMEOutputPolyline::CalculateLength()
//----------------------------------------------------------------------------
{
  double sum = 0;
  vtkPoints *pts = ((vtkPolyData *)m_VME->GetOutput()->GetVTKData())->GetPoints();
	if(pts == NULL) return 0.0;
  for(int i=0; i< pts->GetNumberOfPoints(); i++)
  { 
    if (i > 0)
    {
      double pos1[3], pos2[3];
      pts->GetPoint(i, pos1);
      pts->GetPoint(i-1, pos2);

      sum += sqrt(vtkMath::Distance2BetweenPoints(pos1, pos2));

    }

  }
  
  return sum;
}
