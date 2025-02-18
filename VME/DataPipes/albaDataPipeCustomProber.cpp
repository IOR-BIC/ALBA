/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeCustomProber
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


#include "albaDataPipeCustomProber.h"

#include "albaEventBase.h"
#include "albaVME.h"

#include "albaTransformFrame.h"
#include "albaAbsMatrixPipe.h"
#include "vtkALBADataPipe.h"
#include "vtkDataSet.h"
#include "vtkPolyDataNormals.h"
#include "vtkALBADistanceFilter.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaDataPipeCustomProber)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaDataPipeCustomProber::albaDataPipeCustomProber()
//------------------------------------------------------------------------------
{
  m_Surface = NULL;
  m_Volume  = NULL;

  m_DistThreshold   = -1.0;
  m_MaxDistance     = -1.0;
  m_DistanceModeType= -1;

  m_ProberMode  = -1;
  m_HighDensity = -1.0;
  m_LowDensity  = -1.0;

  vtkNEW(m_Normals);
  vtkNEW(m_Prober);
  m_Prober->SetInputConnection(m_Normals->GetOutputPort());
	m_Prober->Update();
  SetInput(m_Prober->GetOutput());
}

//------------------------------------------------------------------------------
albaDataPipeCustomProber::~albaDataPipeCustomProber()
//------------------------------------------------------------------------------
{
  vtkDEL(m_Normals);
  vtkDEL(m_Prober);
}
//------------------------------------------------------------------------------
void albaDataPipeCustomProber::SetSurface(albaVME *surface)
//------------------------------------------------------------------------------
{
  m_Surface = surface;
  if (m_Surface)
  {
    vtkDataSet *surf_data = m_Surface->GetOutput()->GetVTKData();
    m_Normals->SetInputData((vtkPolyData *)surf_data);
		m_Normals->Modified();
		m_Normals->Update();
		
		m_Prober->Modified();
		m_Prober->Update();
  }
  else
  {
    m_Normals->SetInputData(NULL);
		m_Normals->Modified();
  }
}
//------------------------------------------------------------------------------
void albaDataPipeCustomProber::SetVolume(albaVME *volume)
//------------------------------------------------------------------------------
{
  m_Volume = volume;
  if (m_Volume)
  {
    vtkDataSet *vol_data = m_Volume->GetOutput()->GetVTKData();
    m_Prober->SetSource(vol_data);
		m_Prober->Modified();
  }
  else
  {
    m_Prober->SetSource(NULL);
		m_Prober->Modified();
  }
}
//------------------------------------------------------------------------------
int albaDataPipeCustomProber::DeepCopy(albaDataPipe *pipe)
//------------------------------------------------------------------------------
{
  if (albaDataPipe::DeepCopy(pipe) == ALBA_OK)
  {
    albaDataPipeCustomProber *dpcp = albaDataPipeCustomProber::SafeDownCast(pipe);
    if (dpcp != NULL)
    {
      m_Surface = dpcp->GetSurface();
      m_Volume = dpcp->GetVolume();
      m_DistThreshold = dpcp->GetDistanceThreshold();
      m_MaxDistance = dpcp->GetMaxDistance();
      m_DistanceModeType = dpcp->GetDistanceMode();
      m_ProberMode = dpcp->GetMode();
      m_HighDensity = dpcp->GetHighDensity();
      m_LowDensity = dpcp->GetLowDensity();
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}
//------------------------------------------------------------------------------
void albaDataPipeCustomProber::PreExecute()
//------------------------------------------------------------------------------
{
  if(m_Volume && m_Surface)
  {
    m_Volume->GetOutput()->Update();
    m_Surface->GetOutput()->Update();
    vtkDataSet *vol_data = m_Volume->GetOutput()->GetVTKData();
    vtkDataSet *surf_data = m_Surface->GetOutput()->GetVTKData();
    if(vol_data && surf_data)
    {
      m_Normals->SetInputData((vtkPolyData *)surf_data);
      m_Normals->ComputePointNormalsOn();
      m_Normals->SplittingOff();
      m_Normals->Update();

      m_Prober->SetSource(vol_data);
      if(GetMode() == albaDataPipeCustomProber::DENSITY_MODE)
        m_Prober->SetFilterModeToDensity();
      else
      {
        m_Prober->SetFilterModeToDistance();
        m_Prober->SetThreshold(GetDistanceThreshold());
        m_Prober->SetMaxDistance(GetMaxDistance());
        if(GetDistanceMode() == albaDataPipeCustomProber::DISTANCE_MODE_SCALAR)
          m_Prober->SetDistanceModeToScalar();
        else
          m_Prober->SetDistanceModeToVector();
      }

      albaSmartPointer<albaTransformFrame> maps_to_volume;
      maps_to_volume->SetInput(m_VME->GetAbsMatrixPipe()->GetMatrixPointer());
      maps_to_volume->SetTargetFrame(m_Volume->GetAbsMatrixPipe()->GetMatrixPointer());

      albaMatrix tmp_matrix = maps_to_volume->GetMatrix();

      m_Prober->SetInputMatrix(tmp_matrix.GetVTKMatrix());
			m_Prober->Update();
    }
  }
  else
  {
    m_Prober->SetSource(NULL);
  }
}
//------------------------------------------------------------------------------
void albaDataPipeCustomProber::Execute()
//------------------------------------------------------------------------------
{
	m_Prober->Update();
	SetInput(m_Prober->GetOutput());
}
//-----------------------------------------------------------------------
void albaDataPipeCustomProber::SetModeToDensity()
//-----------------------------------------------------------------------
{
  this->SetMode(albaDataPipeCustomProber::DENSITY_MODE);
  Modified();
}
//-----------------------------------------------------------------------
void albaDataPipeCustomProber::SetModeToDistance()
//-----------------------------------------------------------------------
{
  this->SetMode(albaDataPipeCustomProber::DISTANCE_MODE);
  Modified();
}
//-------------------------------------------------------------------------
void albaDataPipeCustomProber::SetMode(int mode)
//-------------------------------------------------------------------------
{
  if (mode != albaDataPipeCustomProber::DENSITY_MODE && mode != albaDataPipeCustomProber::DISTANCE_MODE)
  {
    albaErrorMacro("trying to set the map mode to invalid type: allowed types are DENSITY_MODE & DISTANCE_MODE");
    return;
  }

  m_ProberMode = mode;
  this->Modified();
}
//-------------------------------------------------------------------------
int albaDataPipeCustomProber::GetMode()
//-------------------------------------------------------------------------
{
  return m_ProberMode;
}
//-------------------------------------------------------------------------
void albaDataPipeCustomProber::SetDistanceThreshold(float thr)
//-------------------------------------------------------------------------
{
  m_DistThreshold = thr;
  this->Modified();
}
//-------------------------------------------------------------------------
float albaDataPipeCustomProber::GetDistanceThreshold()
//-------------------------------------------------------------------------
{
  return m_DistThreshold;
}
//-------------------------------------------------------------------------
void albaDataPipeCustomProber::SetMaxDistance(float max_dist)
//-------------------------------------------------------------------------
{
  if (max_dist < 0)
  {
    albaErrorMacro("Bad max distance value.");
    return;
  }

  m_MaxDistance = max_dist;
  this->Modified();
}
//-------------------------------------------------------------------------
float albaDataPipeCustomProber::GetMaxDistance()
//-------------------------------------------------------------------------
{
  return m_MaxDistance;
}
//-------------------------------------------------------------------------
void albaDataPipeCustomProber::SetDistanceMode(int mode)
//-------------------------------------------------------------------------
{
  if (mode != albaDataPipeCustomProber::DISTANCE_MODE_SCALAR && mode != albaDataPipeCustomProber::DISTANCE_MODE_VECTOR)
  {
    albaErrorMacro("trying to set the map distance mode to invalid type: allowed types are DISTANCE_MODE_SCALAR & DISTANCE_MODE_VECTOR");
    return;
  }

  m_DistanceModeType = mode;
  this->Modified();
}
//-------------------------------------------------------------------------
int albaDataPipeCustomProber::GetDistanceMode()
//-------------------------------------------------------------------------
{
  return m_DistanceModeType;
}
//-------------------------------------------------------------------------
void albaDataPipeCustomProber::SetHighDensity(float high_dens)
//-------------------------------------------------------------------------
{
  m_HighDensity = high_dens;
  this->Modified();
}
//-------------------------------------------------------------------------
float albaDataPipeCustomProber::GetHighDensity()
//-------------------------------------------------------------------------
{
  return m_HighDensity;
}
//-------------------------------------------------------------------------
void albaDataPipeCustomProber::SetLowDensity(float low_dens)
//-------------------------------------------------------------------------
{
  m_LowDensity = low_dens;
  this->Modified();
}
//-------------------------------------------------------------------------
float albaDataPipeCustomProber::GetLowDensity()
//-------------------------------------------------------------------------
{
  return m_LowDensity;
}
