/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputScalar
 Authors: Paolo Quadrani
 
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



#include "albaVMEOutputScalar.h"
#include "albaGUI.h"

#include "albaVMEScalar.h"
#include "albaScalarVector.h"
#include "albaIndent.h"

#ifdef ALBA_USE_VTK
#include "vtkALBASmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#endif

#include <assert.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputScalar)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputScalar::albaVMEOutputScalar()
//-------------------------------------------------------------------------
{
#ifdef ALBA_USE_VTK
  vtkNEW(m_Polydata);
#endif

  m_ScalarDataString = "";
}

//-------------------------------------------------------------------------
albaVMEOutputScalar::~albaVMEOutputScalar()
//-------------------------------------------------------------------------
{
#ifdef ALBA_USE_VTK
  vtkDEL(m_Polydata);
#endif
}

//-------------------------------------------------------------------------
double albaVMEOutputScalar::GetScalarData()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  albaScalarVector *scalarVector = ((albaVMEScalar *)m_VME)->GetScalarVector();
  return scalarVector->GetItemBefore(GetTimeStamp());
  //return scalarVector->GetNearestScalar(GetTimeStamp());  // Also this method could be used; depend on interpolation
                                                            // we want to use.
}

#ifdef ALBA_USE_VTK
//-------------------------------------------------------------------------
vtkDataSet *albaVMEOutputScalar::GetVTKData()
//-------------------------------------------------------------------------
{
  UpdateVTKRepresentation();
  return m_Polydata;
}
//-------------------------------------------------------------------------
void albaVMEOutputScalar::UpdateVTKRepresentation()
//-------------------------------------------------------------------------
{
  double data = GetScalarData();

  int pointId[2];
  double time_point = GetTimeStamp();
  vtkALBASmartPointer<vtkPoints> points;
  vtkALBASmartPointer<vtkCellArray> verts;
  vtkALBASmartPointer<vtkDoubleArray> scalars;
  scalars->SetNumberOfValues(1);
  scalars->SetNumberOfComponents(1);
  scalars->FillComponent(0,data);
  points->InsertPoint(0, 0.0, 0.0, 0.0);
  pointId[0] = 0;
  pointId[1] = 0;
  verts->InsertNextCell(2,pointId);

  m_Polydata->SetPoints(points);
  m_Polydata->SetLines(verts);
  m_Polydata->GetPointData()->SetScalars(scalars);
  m_Polydata->Modified();
}
#endif

//-------------------------------------------------------------------------
albaGUI* albaVMEOutputScalar::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = albaVMEOutput::CreateGui();

  if (m_VME)
  {
    this->Update();
  }
  /*double data = GetScalarData();
  m_ScalarDataString = "";
  m_ScalarDataString << data;*/
  m_Gui->Label(_("data: "),&m_ScalarDataString);
	m_Gui->Divider(); 
	return m_Gui;
}

//-------------------------------------------------------------------------
void albaVMEOutputScalar::Update()
//-------------------------------------------------------------------------
{
  double data = GetScalarData();
  m_ScalarDataString = "";
  m_ScalarDataString << data;
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
