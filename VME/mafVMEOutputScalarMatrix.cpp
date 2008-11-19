/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputScalarMatrix.cpp,v $
  Language:  C++
  Date:      $Date: 2008-11-19 14:04:02 $
  Version:   $Revision: 1.3.2.1 $
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



#include "mafVMEOutputScalarMatrix.h"
#include "mafGUI.h"

#include "mafVMEScalarMatrix.h"
#include "mafVMEItemScalarMatrix.h"
#include "mafScalarMatrixInterpolator.h"
#include "mafIndent.h"

#ifdef MAF_USE_VTK
#include "vtkMAFSmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#endif

#include <vnl/vnl_vector.h>

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputScalarMatrix)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputScalarMatrix::mafVMEOutputScalarMatrix()
//-------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  vtkNEW(m_Polydata);
#endif
  m_NumberOfRows = "0";
  m_NumberOfColumns = "0";
}

//-------------------------------------------------------------------------
mafVMEOutputScalarMatrix::~mafVMEOutputScalarMatrix()
//-------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  vtkDEL(m_Polydata);
#endif
}

//-------------------------------------------------------------------------
vnl_matrix<double> &mafVMEOutputScalarMatrix::GetScalarData()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  mafScalarMatrixInterpolator *scalarInterpolator = (mafScalarMatrixInterpolator *)m_VME->GetDataPipe();
  scalarInterpolator->Update();
  return scalarInterpolator->GetScalarData();
}

#ifdef MAF_USE_VTK
//-------------------------------------------------------------------------
vtkDataSet *mafVMEOutputScalarMatrix::GetVTKData()
//-------------------------------------------------------------------------
{
  UpdateVTKRepresentation();
  return m_Polydata;
}
//-------------------------------------------------------------------------
void mafVMEOutputScalarMatrix::UpdateVTKRepresentation()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  mafVMEScalarMatrix *scalar_vme = mafVMEScalarMatrix::SafeDownCast(m_VME);
  assert(scalar_vme);

  int active_scalar = scalar_vme->GetActiveScalarOnGeometry();

  mafScalarMatrixInterpolator *scalarInterpolator = (mafScalarMatrixInterpolator *)scalar_vme->GetDataPipe();
  scalarInterpolator->Update();
  vnl_matrix<double> scalar = scalarInterpolator->GetCurrentItem()->GetData();
  if (scalar.size() != 0)
  {
    vnl_matrix<double> mat = scalarInterpolator->GetScalarData();

    int num_of_points = 0;
    int o = scalar_vme->GetScalarArrayOrientation();
    int x_coord_type = scalar_vme->GetTypeForXCoordinates();
    vnl_vector<double> vx;
    vnl_vector<double> vy;
    vnl_vector<double> vz;
    if (x_coord_type == mafVMEScalarMatrix::USE_SCALAR)
    {
      int sx = scalar_vme->GetScalarIdForXCoordinate();
      if (o == mafVMEScalarMatrix::ROWS)
      {
        vx = mat.get_row(sx);
      }
      else
      {
        vx = mat.get_column(sx);
      }
      num_of_points = vx.size();
    }
    int y_coord_type = scalar_vme->GetTypeForYCoordinates();
    if (y_coord_type == mafVMEScalarMatrix::USE_SCALAR)
    {
      int sy = scalar_vme->GetScalarIdForYCoordinate();
      if (o == mafVMEScalarMatrix::ROWS)
      {
        vy = mat.get_row(sy);
      }
      else
      {
        vy = mat.get_column(sy);
      }
      num_of_points = vy.size();
    }
    int z_coord_type = scalar_vme->GetTypeForZCoordinates();
    if (z_coord_type == mafVMEScalarMatrix::USE_SCALAR)
    {
      int sz = scalar_vme->GetScalarIdForZCoordinate();
      if (o == mafVMEScalarMatrix::ROWS)
      {
        vz = mat.get_row(sz);
      }
      else
      {
        vz = mat.get_column(sz);
      }
      num_of_points = vz.size();
    }
    int pointId[2];
    int progress_point = 0;
    double time_point = GetTimeStamp();
    double x_coord, y_coord, z_coord;
    vtkMAFSmartPointer<vtkPoints> points;
    vtkMAFSmartPointer<vtkCellArray> verts;
    vnl_vector<double> vs;
    vtkMAFSmartPointer<vtkDoubleArray> scalars;
    scalars->SetNumberOfValues(num_of_points);
    scalars->SetNumberOfComponents(1);
    scalars->FillComponent(0,0.0);
    if (active_scalar > -1)
    {
      if (o == mafVMEScalarMatrix::ROWS)
      {
        active_scalar = active_scalar >= mat.rows() ? mat.rows() - 1 : active_scalar;
        vs = mat.get_row(active_scalar);
      }
      else
      {
        active_scalar = active_scalar >= mat.columns() ? mat.columns() - 1 : active_scalar;
        vs = mat.get_column(active_scalar);
      }
      scalar_vme->SetActiveScalarOnGeometry(active_scalar);
      vs.copy_out((double *)scalars->GetVoidPointer(0));
    }
    for (int p = 0; p< num_of_points; p++)
    {
      // X coordinate
      if (x_coord_type == mafVMEScalarMatrix::USE_SCALAR)
      {
        x_coord = vx.get(p);
      }
      else if (x_coord_type == mafVMEScalarMatrix::USE_PROGRESS_NUMBER)
      {
        x_coord = progress_point;
      }
      else
      {
        x_coord = time_point;
      }
      // Y coordinate
      if (y_coord_type == mafVMEScalarMatrix::USE_SCALAR)
      {
        y_coord = vy.get(p);
      }
      else if (y_coord_type == mafVMEScalarMatrix::USE_PROGRESS_NUMBER)
      {
        y_coord = progress_point;
      }
      else
      {
        y_coord = time_point;
      }
      // Z coordinate
      if (z_coord_type == mafVMEScalarMatrix::USE_SCALAR)
      {
        z_coord = vz.get(p);
      }
      else if (z_coord_type == mafVMEScalarMatrix::USE_PROGRESS_NUMBER)
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
#endif

//-------------------------------------------------------------------------
mafGUI* mafVMEOutputScalarMatrix::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();

  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe())
  {
    this->Update();
  }
  vnl_matrix<double> data = GetScalarData();
  m_NumberOfRows = "";
  m_NumberOfRows << (int)data.rows();
  m_NumberOfColumns = "";
  m_NumberOfColumns << (int)data.columns();
  m_Gui->Label(_("rows: "),&m_NumberOfRows);
  m_Gui->Label(_("columns: "),&m_NumberOfColumns);
	m_Gui->Divider(); 
	return m_Gui;
}

//-------------------------------------------------------------------------
void mafVMEOutputScalarMatrix::Update()
//-------------------------------------------------------------------------
{
  vnl_matrix<double> data = GetScalarData();
  m_NumberOfRows = "";
  m_NumberOfRows << (int)data.rows();
  m_NumberOfColumns = "";
  m_NumberOfColumns << (int)data.columns();
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
