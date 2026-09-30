/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputScalarMatrix
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



#include "albaVMEOutputScalarMatrix.h"
#include "albaGUI.h"

#include "albaVMEScalarMatrix.h"
#include "albaVMEItemScalarMatrix.h"
#include "albaDataPipeInterpolatorScalarMatrix.h"
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
albaCxxTypeMacro(albaVMEOutputScalarMatrix)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputScalarMatrix::albaVMEOutputScalarMatrix()
//-------------------------------------------------------------------------
{
#ifdef ALBA_USE_VTK
  vtkNEW(m_Polydata);
#endif
  m_NumberOfRows = "0";
  m_NumberOfColumns = "0";
}

//-------------------------------------------------------------------------
albaVMEOutputScalarMatrix::~albaVMEOutputScalarMatrix()
//-------------------------------------------------------------------------
{
#ifdef ALBA_USE_VTK
  vtkDEL(m_Polydata);
#endif
}

//-------------------------------------------------------------------------
albaDynamicMatrix &albaVMEOutputScalarMatrix::GetScalarData()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  albaDataPipeInterpolatorScalarMatrix *scalarInterpolator = (albaDataPipeInterpolatorScalarMatrix *)m_VME->GetDataPipe();
  scalarInterpolator->Update();
  return scalarInterpolator->GetScalarData();
}

#ifdef ALBA_USE_VTK
//-------------------------------------------------------------------------
vtkDataSet *albaVMEOutputScalarMatrix::GetVTKData()
//-------------------------------------------------------------------------
{
  UpdateVTKRepresentation();
  return m_Polydata;
}
//-------------------------------------------------------------------------
void albaVMEOutputScalarMatrix::UpdateVTKRepresentation()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  albaVMEScalarMatrix *scalar_vme = albaVMEScalarMatrix::SafeDownCast(m_VME);
  assert(scalar_vme);

  int active_scalar = scalar_vme->GetActiveScalarOnGeometry();

  albaDataPipeInterpolatorScalarMatrix *scalarInterpolator = (albaDataPipeInterpolatorScalarMatrix *)scalar_vme->GetDataPipe();
  scalarInterpolator->Update();
  if (scalarInterpolator->GetCurrentItem() != NULL)
  {
    albaDynamicMatrix scalar = scalarInterpolator->GetCurrentItem()->GetData();
    if (scalar.GetSize() != 0)
    {
      albaDynamicMatrix mat = scalarInterpolator->GetScalarData();

      int num_of_points = 0;
      int o = scalar_vme->GetScalarArrayOrientation();
      int x_coord_type = scalar_vme->GetTypeForXCoordinates();
      std::vector<double> vx;
      std::vector<double> vy;
      std::vector<double> vz;
      if (x_coord_type == albaVMEScalarMatrix::USE_SCALAR)
      {
        int sx = scalar_vme->GetScalarIdForXCoordinate();
        if (o == albaVMEScalarMatrix::ROWS)
        {
          vx = mat.GetRow(sx);
        }
        else
        {
          vx = mat.GetColumn(sx);
        }
        num_of_points = vx.size();
      }
      int y_coord_type = scalar_vme->GetTypeForYCoordinates();
      if (y_coord_type == albaVMEScalarMatrix::USE_SCALAR)
      {
        int sy = scalar_vme->GetScalarIdForYCoordinate();
        if (o == albaVMEScalarMatrix::ROWS)
        {
          vy = mat.GetRow(sy);
        }
        else
        {
          vy = mat.GetColumn(sy);
        }
        num_of_points = vy.size();
      }
      int z_coord_type = scalar_vme->GetTypeForZCoordinates();
      if (z_coord_type == albaVMEScalarMatrix::USE_SCALAR)
      {
        int sz = scalar_vme->GetScalarIdForZCoordinate();
        if (o == albaVMEScalarMatrix::ROWS)
        {
          vz = mat.GetRow(sz);
        }
        else
        {
          vz = mat.GetColumn(sz);
        }
        num_of_points = vz.size();
      }
			vtkIdType pointId[2];
      int progress_point = 0;
      double time_point = GetTimeStamp();
      double x_coord, y_coord, z_coord;
      vtkALBASmartPointer<vtkPoints> points;
      vtkALBASmartPointer<vtkCellArray> verts;
      std::vector<double> vs;
      vtkALBASmartPointer<vtkDoubleArray> scalars;
      scalars->SetNumberOfValues(num_of_points);
      scalars->SetNumberOfComponents(1);
      scalars->FillComponent(0,0.0);
      if (active_scalar > -1)
      {
        if (o == albaVMEScalarMatrix::ROWS)
        {
          active_scalar = active_scalar >= mat.GetRowsNum() ? mat.GetRowsNum() - 1 : active_scalar;
          vs = mat.GetRow(active_scalar);
        }
        else
        {
          active_scalar = active_scalar >= mat.GetColsNum() ? mat.GetColsNum() - 1 : active_scalar;
          vs = mat.GetColumn(active_scalar);
        }
        scalar_vme->SetActiveScalarOnGeometry(active_scalar);
        std::copy(vs.begin(), vs.end(), (double *)scalars->GetVoidPointer(0));
      }
      for (int p = 0; p< num_of_points; p++)
      {
        // X coordinate
        if (x_coord_type == albaVMEScalarMatrix::USE_SCALAR)
        {
          x_coord = vx[p];
        }
        else if (x_coord_type == albaVMEScalarMatrix::USE_PROGRESS_NUMBER)
        {
          x_coord = progress_point;
        }
        else
        {
          x_coord = time_point;
        }
        // Y coordinate
        if (y_coord_type == albaVMEScalarMatrix::USE_SCALAR)
        {
          y_coord = vy[p];
        }
        else if (y_coord_type == albaVMEScalarMatrix::USE_PROGRESS_NUMBER)
        {
          y_coord = progress_point;
        }
        else
        {
          y_coord = time_point;
        }
        // Z coordinate
        if (z_coord_type == albaVMEScalarMatrix::USE_SCALAR)
        {
          z_coord = vz[p];
        }
        else if (z_coord_type == albaVMEScalarMatrix::USE_PROGRESS_NUMBER)
        {
          z_coord = progress_point;
        }
        else
        {
          z_coord = time_point;
        }
        points->InsertPoint(p,x_coord,y_coord,z_coord);
        if (p>0)
        {
          pointId[0] = p-1;
          pointId[1] = p;
          verts->InsertNextCell(2,pointId);
        }
        progress_point++;
      }

      m_Polydata->SetPoints(points);
      m_Polydata->SetLines(verts);
      m_Polydata->GetPointData()->SetScalars(scalars);
      m_Polydata->Modified();
    }
  }
}
#endif

//-------------------------------------------------------------------------
albaGUI* albaVMEOutputScalarMatrix::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = albaVMEOutput::CreateGui();

  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe())
  {
    this->Update();
  }
  albaDynamicMatrix data = GetScalarData();
  m_NumberOfRows = "";
  m_NumberOfRows << data.GetRowsNum();
  m_NumberOfColumns = "";
  m_NumberOfColumns << data.GetColsNum();
  m_Gui->Label(_("Rows:"),&m_NumberOfRows);
  m_Gui->Label(_("Columns:"),&m_NumberOfColumns);
	m_Gui->Divider(); 

	return m_Gui;
}

//-------------------------------------------------------------------------
void albaVMEOutputScalarMatrix::Update()
//-------------------------------------------------------------------------
{
  albaDynamicMatrix data = GetScalarData();
  m_NumberOfRows = "";
  m_NumberOfRows << data.GetRowsNum();
  m_NumberOfColumns = "";
  m_NumberOfColumns << data.GetColsNum();
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
