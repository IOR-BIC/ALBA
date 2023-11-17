/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputPolyline
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

#include "albaVMEOutputPolyline.h"
#include "albaVME.h"
#include "albaDataPipe.h"
#include "albaGUI.h"
#include "albaIndent.h"
#include "mmaMaterial.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkMath.h"
#include <assert.h>

#define NULL_STRING_DATA "--"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputPolyline)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputPolyline::albaVMEOutputPolyline()
//-------------------------------------------------------------------------
{
  m_Material = NULL;
  m_NumberOfPoints = "0";
}

//-------------------------------------------------------------------------
albaVMEOutputPolyline::~albaVMEOutputPolyline()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkPolyData *albaVMEOutputPolyline::GetPolylineData()
//-------------------------------------------------------------------------
{
  return (vtkPolyData *)GetVTKData();
}
//-------------------------------------------------------------------------
void albaVMEOutputPolyline::SetMaterial(mmaMaterial *material)
//-------------------------------------------------------------------------
{
  m_Material = material;
}
//-------------------------------------------------------------------------
albaGUI* albaVMEOutputPolyline::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = albaVMEOutput::CreateGui();

  wxBusyCursor wait;
  
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
  
  //m_Length = albaString(albaString::Format(_("%.2f"),CalculateLength()));
  m_Gui->Label(_("Length:"), &m_Length ,true);

  m_NumberOfPoints = albaString(((vtkPolyData *)m_VME->GetOutput()->GetVTKData())->GetNumberOfPoints());
  m_Gui->Label(_("Points:"), &m_NumberOfPoints ,true);

  return m_Gui;
}
//-------------------------------------------------------------------------
mmaMaterial *albaVMEOutputPolyline::GetMaterial()
//-------------------------------------------------------------------------
{
  // if the VME set the material directly in the output return it
  if (m_Material)
    return  m_Material;

  // search for a material attribute in the VME connected to this output
  return GetVME() ? mmaMaterial::SafeDownCast(GetVME()->GetAttribute("MaterialAttributes")) : NULL;
}
//-------------------------------------------------------------------------
void albaVMEOutputPolyline::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();
  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe()->GetVTKData())
  {
  	m_Length = albaString(albaString::Format(_("%.2f"),CalculateLength()));  
    m_NumberOfPoints = albaString(((vtkPolyData *)m_VME->GetOutput()->GetVTKData())->GetNumberOfPoints());
  }
  
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
double albaVMEOutputPolyline::CalculateLength()
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
