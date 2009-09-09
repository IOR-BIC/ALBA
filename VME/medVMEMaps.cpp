/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEMaps.cpp,v $
Language:  C++
Date:      $Date: 2009-09-09 15:33:02 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
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

#include "medVMEMaps.h"

#include "mafDataPipeCustom.h"
#include "mafPipesurface.h"
#include "mafGUI.h"
#include "mafTransform.h"
#include "mafVMEOutputSurface.h"
#include "mafStorageElement.h"
#include "mafGUILutPreset.h"

#include "mmaMaterial.h"

#include "vtkMAFDistanceFilter.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkMAFDataPipe.h"
#include "vtkColorTransferFunction.h"
#include "vtkCubeSource.h"
#include "vtkPointData.h"
#include "vtkLookuptable.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMEMaps)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
medVMEMaps::medVMEMaps()
//-------------------------------------------------------------------------
{
  m_Volume    = NULL;
  m_Normals         = NULL;
  m_Volume          = NULL;
  m_PolyData        = NULL;

  mafNEW(m_Transform);
  vtkNEW(m_Normals);
  vtkNEW(m_DistanceFilter);
  
  mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
  dpipe->SetInput(m_Normals->GetOutput());
 
  GetMaterial()->SetMaterialTexture(GetSurfaceOutput()->GetTexture());
  GetMaterial()->m_TextureMappingMode = mmaMaterial::PLANE_MAPPING;
  GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;

  m_DensityDistance = 0;
  m_FirstThreshold = 700;
  m_SecondThreshold = 300;
  m_MaxDistance = 2;
}

//-------------------------------------------------------------------------
medVMEMaps::~medVMEMaps()
//-------------------------------------------------------------------------
{
  mafDEL(m_DistanceFilter);
  mafDEL(m_Transform);

  vtkDEL(m_Normals);

  SetOutput(NULL);
}

//-------------------------------------------------------------------------
int medVMEMaps::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool medVMEMaps::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    //return ret = 
  }
  return ret;
}

//-------------------------------------------------------------------------
mafGUI* medVMEMaps::CreateGui()
//-------------------------------------------------------------------------
{
  //return Superclass::CreateGui();
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();

  wxString m_Choices[2];
  m_Choices[0]="Distance";
  m_Choices[1]="Density";
  m_Gui->Radio(ID_DENSITY_DISTANCE,"",&m_DensityDistance,2,m_Choices);

  if(!m_Volume)
  {
    m_Gui->Enable(ID_DENSITY_DISTANCE,false);
    /*m_Gui->Enable(ID_FIRST_THRESHOLD,false);
    m_Gui->Enable(ID_SECOND_THRESHOLD,false);
    m_Gui->Enable(ID_MAX_DISTANCE,false);
    m_Gui->Enable(ID_NUM_SECTIONS,false);
    m_Gui->Enable(ID_BAR_TIPOLOGY,false);*/
  }
  else
  {
    //m_Gui->Enable(ID_SECOND_THRESHOLD,false);
    //m_Gui->Enable(ID_BAR_TIPOLOGY,false);
  }

  m_Gui->Divider();

  InternalUpdate();
 
  return m_Gui;
}
//-------------------------------------------------------------------------
void medVMEMaps::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  //Superclass::OnEvent(maf_event);
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_DENSITY_DISTANCE:
      {
        UpdateMaps();
      }
      break;
    default:
      Superclass::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
mafVMEOutputSurface *medVMEMaps::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputSurface *)GetOutput();
}

//-------------------------------------------------------------------------
void medVMEMaps::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void medVMEMaps::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-----------------------------------------------------------------------
void medVMEMaps::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
}

//-------------------------------------------------------------------------
void medVMEMaps::InternalPreUpdate()
//-------------------------------------------------------------------------
{

  mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());    

  vtkNEW(m_Normals);
  vtkNEW(m_DistanceFilter);

  GetSurfaceOutput()->Update();

  vtkPolyData *data = vtkPolyData::SafeDownCast(GetSurfaceOutput()->GetVTKData());
  if(!data)
    return;

  m_Normals->SetInput(data);
  m_Normals->ComputePointNormalsOn();
  m_Normals->SplittingOff();
  m_Normals->Update();

  if (m_Volume)
  {
    if(m_DensityDistance == 0)
    {
      m_DistanceFilter->SetFilterModeToDistance();
    }
    if(m_DensityDistance == 1)
    {
      m_DistanceFilter->SetFilterModeToDensity();
    }

    m_DistanceFilter->SetDistanceModeToScalar();
    m_DistanceFilter->SetSource(((mafVME*)m_Volume)->GetOutput()->GetVTKData());
    m_DistanceFilter->SetInput((vtkDataSet::SafeDownCast(m_Normals->GetOutput())));
    m_DistanceFilter->SetMaxDistance(m_MaxDistance);
    m_DistanceFilter->SetThreshold(m_FirstThreshold);
    m_DistanceFilter->SetInputMatrix(GetSurfaceOutput()->GetAbsMatrix()->GetVTKMatrix());
    m_DistanceFilter->Update();

    vtkDataSet *dataset;
    vtkDataArray *scalars; 

    dataset = m_DistanceFilter->GetOutput();
    scalars = dataset->GetPointData()->GetScalars();
    GetSurfaceOutput()->GetVTKData()->GetPointData()->SetActiveScalars("Distance_density");
    GetSurfaceOutput()->GetVTKData()->GetPointData()->SetScalars(scalars);

    dpipe->SetInput((vtkPolyData*)m_DistanceFilter->GetOutput());

  }
  else
  {
    dpipe->SetInput(m_Normals->GetOutput());
  }
  
}

//-------------------------------------------------------------------------
void medVMEMaps::InternalUpdate()
//-------------------------------------------------------------------------
{
  if (vtkDataSet *vtkdata=GetOutput()->GetVTKData())
  {
    vtkDataArray *scalars = vtkdata->GetPointData()->GetScalars();
    if (scalars == NULL)
    {
      return;
    }

    double range[2];
    vtkdata->GetScalarRange(range);
    GetMaterial()->m_ColorLut->SetRange(range);
  }
  m_DistanceFilter->Update();

  mafEvent ev(this, CAMERA_UPDATE);
  this->ForwardUpEvent(&ev);
}

//-------------------------------------------------------------------------
void medVMEMaps::SetVolume(mafVMEVolume *volume)
//-------------------------------------------------------------------------
{
  m_Volume = volume;
  if(m_Gui && m_Volume)
    m_Gui->Enable(ID_DENSITY_DISTANCE, true);
}

//-------------------------------------------------------------------------
mafVMEVolume *medVMEMaps::GetVolume()
//-------------------------------------------------------------------------
{
  return m_Volume;
}

//-------------------------------------------------------------------------
void medVMEMaps::SetVTKPolyData(vtkPolyData *data)
//-------------------------------------------------------------------------
{
  m_PolyData = data;
  SetData(data, 0.0);
}

//-------------------------------------------------------------------------
vtkDataSet *medVMEMaps::GetVTKPolyData()
//-------------------------------------------------------------------------
{
  return GetOutput()->GetVTKData();
}

//-------------------------------------------------------------------------
void medVMEMaps::Selected(bool sel)
//-------------------------------------------------------------------------
{
  mafEvent ev(this,CAMERA_UPDATE);
  this->ForwardUpEvent(&ev);
}

//-------------------------------------------------------------------------
void medVMEMaps::UpdateMaps()
//-------------------------------------------------------------------------
{
  if(m_DistanceFilter)
  {
    m_DistanceFilter->SetInputMatrix(GetSurfaceOutput()->GetAbsMatrix()->GetVTKMatrix());

    if(m_DensityDistance == 0)
    {
      m_DistanceFilter->SetFilterModeToDistance();
    }
    else
    {
      m_DistanceFilter->SetFilterModeToDensity();
    }
  }
  m_DistanceFilter->Update();

  vtkDataSet *dataset = m_DistanceFilter->GetOutput();
  vtkDataArray *scalars = dataset->GetPointData()->GetScalars();

  GetSurfaceOutput()->GetVTKData()->GetPointData()->SetActiveScalars("Distance_density");
  GetSurfaceOutput()->GetVTKData()->GetPointData()->SetScalars(scalars);

  mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
  dpipe->SetInput((vtkPolyData*)m_DistanceFilter->GetOutput());

  double range[2];
  scalars->GetRange(range);
  GetMaterial()->m_ColorLut->SetRange(range);
  Update();
  mafEvent ev(this,CAMERA_UPDATE);
}

//-----------------------------------------------------------------------
int medVMEMaps::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int medVMEMaps::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);

      return MAF_OK;
    }
  }

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
mmaMaterial *medVMEMaps::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    //GetSurfaceOutput()->SetMaterial(material);
  }
  return material;
}