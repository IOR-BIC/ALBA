/*=========================================================================

 Program: MAF2
 Module: mafDataPipeCustomProber
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafDataPipeCustomProber.h"

#include "mafEventBase.h"
#include "mafVME.h"

#include "mafTransformFrame.h"
#include "mafAbsMatrixPipe.h"
#include "vtkMAFDataPipe.h"
#include "vtkDataSet.h"
#include "vtkPolyDataNormals.h"
#include "vtkMAFDistanceFilter.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDataPipeCustomProber)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDataPipeCustomProber::mafDataPipeCustomProber()
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
mafDataPipeCustomProber::~mafDataPipeCustomProber()
//------------------------------------------------------------------------------
{
  vtkDEL(m_Normals);
  vtkDEL(m_Prober);
}
//------------------------------------------------------------------------------
void mafDataPipeCustomProber::SetSurface(mafVME *surface)
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
void mafDataPipeCustomProber::SetVolume(mafVME *volume)
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
int mafDataPipeCustomProber::DeepCopy(mafDataPipe *pipe)
//------------------------------------------------------------------------------
{
  if (mafDataPipe::DeepCopy(pipe) == MAF_OK)
  {
    mafDataPipeCustomProber *dpcp = mafDataPipeCustomProber::SafeDownCast(pipe);
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
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
void mafDataPipeCustomProber::PreExecute()
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
      if(GetMode() == mafDataPipeCustomProber::DENSITY_MODE)
        m_Prober->SetFilterModeToDensity();
      else
      {
        m_Prober->SetFilterModeToDistance();
        m_Prober->SetThreshold(GetDistanceThreshold());
        m_Prober->SetMaxDistance(GetMaxDistance());
        if(GetDistanceMode() == mafDataPipeCustomProber::DISTANCE_MODE_SCALAR)
          m_Prober->SetDistanceModeToScalar();
        else
          m_Prober->SetDistanceModeToVector();
      }

      mafSmartPointer<mafTransformFrame> maps_to_volume;
      maps_to_volume->SetInput(m_VME->GetAbsMatrixPipe()->GetMatrixPointer());
      maps_to_volume->SetTargetFrame(m_Volume->GetAbsMatrixPipe()->GetMatrixPointer());

      mafMatrix tmp_matrix = maps_to_volume->GetMatrix();

      m_Prober->SetInputMatrix(tmp_matrix.GetVTKMatrix());
    }
  }
  else
  {
    m_Prober->SetSource(NULL);
  }
}
//------------------------------------------------------------------------------
void mafDataPipeCustomProber::Execute()
//------------------------------------------------------------------------------
{
	m_Prober->Update();
	SetInput(m_Prober->GetOutput());
}
//-----------------------------------------------------------------------
void mafDataPipeCustomProber::SetModeToDensity()
//-----------------------------------------------------------------------
{
  this->SetMode(mafDataPipeCustomProber::DENSITY_MODE);
  Modified();
}
//-----------------------------------------------------------------------
void mafDataPipeCustomProber::SetModeToDistance()
//-----------------------------------------------------------------------
{
  this->SetMode(mafDataPipeCustomProber::DISTANCE_MODE);
  Modified();
}
//-------------------------------------------------------------------------
void mafDataPipeCustomProber::SetMode(int mode)
//-------------------------------------------------------------------------
{
  if (mode != mafDataPipeCustomProber::DENSITY_MODE && mode != mafDataPipeCustomProber::DISTANCE_MODE)
  {
    mafErrorMacro("trying to set the map mode to invalid type: allowed types are DENSITY_MODE & DISTANCE_MODE");
    return;
  }

  m_ProberMode = mode;
  this->Modified();
}
//-------------------------------------------------------------------------
int mafDataPipeCustomProber::GetMode()
//-------------------------------------------------------------------------
{
  return m_ProberMode;
}
//-------------------------------------------------------------------------
void mafDataPipeCustomProber::SetDistanceThreshold(float thr)
//-------------------------------------------------------------------------
{
  m_DistThreshold = thr;
  this->Modified();
}
//-------------------------------------------------------------------------
float mafDataPipeCustomProber::GetDistanceThreshold()
//-------------------------------------------------------------------------
{
  return m_DistThreshold;
}
//-------------------------------------------------------------------------
void mafDataPipeCustomProber::SetMaxDistance(float max_dist)
//-------------------------------------------------------------------------
{
  if (max_dist < 0)
  {
    mafErrorMacro("Bad max distance value.");
    return;
  }

  m_MaxDistance = max_dist;
  this->Modified();
}
//-------------------------------------------------------------------------
float mafDataPipeCustomProber::GetMaxDistance()
//-------------------------------------------------------------------------
{
  return m_MaxDistance;
}
//-------------------------------------------------------------------------
void mafDataPipeCustomProber::SetDistanceMode(int mode)
//-------------------------------------------------------------------------
{
  if (mode != mafDataPipeCustomProber::DISTANCE_MODE_SCALAR && mode != mafDataPipeCustomProber::DISTANCE_MODE_VECTOR)
  {
    mafErrorMacro("trying to set the map distance mode to invalid type: allowed types are DISTANCE_MODE_SCALAR & DISTANCE_MODE_VECTOR");
    return;
  }

  m_DistanceModeType = mode;
  this->Modified();
}
//-------------------------------------------------------------------------
int mafDataPipeCustomProber::GetDistanceMode()
//-------------------------------------------------------------------------
{
  return m_DistanceModeType;
}
//-------------------------------------------------------------------------
void mafDataPipeCustomProber::SetHighDensity(float high_dens)
//-------------------------------------------------------------------------
{
  m_HighDensity = high_dens;
  this->Modified();
}
//-------------------------------------------------------------------------
float mafDataPipeCustomProber::GetHighDensity()
//-------------------------------------------------------------------------
{
  return m_HighDensity;
}
//-------------------------------------------------------------------------
void mafDataPipeCustomProber::SetLowDensity(float low_dens)
//-------------------------------------------------------------------------
{
  m_LowDensity = low_dens;
  this->Modified();
}
//-------------------------------------------------------------------------
float mafDataPipeCustomProber::GetLowDensity()
//-------------------------------------------------------------------------
{
  return m_LowDensity;
}
