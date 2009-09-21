/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEMaps.cpp,v $
Language:  C++
Date:      $Date: 2009-09-21 15:48:09 $
Version:   $Revision: 1.1.2.2 $
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
#include "mafEventSource.h"

#include "mmaMaterial.h"

#include "vtkMAFDistanceFilter.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkMAFDataPipe.h"
#include "vtkColorTransferFunction.h"
#include "vtkSphereSource.h"
#include "vtkPointData.h"
#include "vtkLookuptable.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCellArray.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMEMaps)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
medVMEMaps::medVMEMaps()
//-------------------------------------------------------------------------
{
  m_Volume          = NULL;
  m_Normals         = NULL;
  m_Volume          = NULL;
  m_PolyData        = NULL;

  mafNEW(m_Transform);
  vtkNEW(m_Normals);
  vtkNEW(m_DistanceFilter);
  vtkNEW(m_PolyData);

  mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);
  
  //DependsOnLinkedNodeOn();

  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
  dpipe->GetVTKDataPipe()->SetNthInput(0, m_PolyData);

  //GetSurfaceOutput()->SetTexture((vtkImageData *)((mafDataPipeCustom *)GetDataPipe())->GetVTKDataPipe()->GetOutput(1));
  //GetMaterial()->SetMaterialTexture(GetSurfaceOutput()->GetTexture());
  //GetMaterial()->m_TextureMappingMode = mmaMaterial::PLANE_MAPPING;
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
  vtkDEL(m_PolyData);

  SetOutput(NULL);
}

//-------------------------------------------------------------------------
int medVMEMaps::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    medVMEMaps *maps = medVMEMaps::SafeDownCast(a);

    m_Transform->SetMatrix(maps->m_Transform->GetMatrix());
    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetDependOnAbsPose(true);
      dpipe->GetVTKDataPipe()->SetNthInput(0, m_PolyData);
    }
    GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;

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
    return ret = (m_Transform->GetMatrix()  ==  ((medVMEMaps *)vme)->m_Transform->GetMatrix() &&
                  m_Volume                  ==  ((medVMEMaps *)vme)->GetVolume() &&
                  m_FirstThreshold          ==  ((medVMEMaps *)vme)->GetFirstThreshold() &&
                  m_SecondThreshold         ==  ((medVMEMaps *)vme)->GetSecondThreshold() &&
                  m_MaxDistance             ==  ((medVMEMaps *)vme)->GetMaxDistance() &&
                  m_PolyData                ==  ((medVMEMaps *)vme)->GetPolyData()
                 );
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
        SetDensityDistance(m_DensityDistance);
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
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputSurface::New()); // create the output
  }
  return mafVMEOutputSurface::SafeDownCast(m_Output);
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
  if(m_PolyData==NULL)
    return;
  m_PolyData->Update();

  m_Normals->SetInput(m_PolyData);
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

    assert(scalars);

    m_PolyData->GetPointData()->SetActiveScalars("Distance_density");
    m_PolyData->GetPointData()->SetScalars(scalars);

    m_PolyData->Modified();

  }
  
}

//-------------------------------------------------------------------------
void medVMEMaps::InternalUpdate()
//-------------------------------------------------------------------------
{
  //GetSurfaceOutput()->GetVTKData()->GetPointData()->SetScalars(scalars);
  if (m_PolyData)
  {
    m_PolyData->Update();
    /*vtkDataArray *scalars = m_PolyData->GetPointData()->GetScalars();
    if (scalars == NULL)
    {
      return;
    }
    double range[2];
    m_PolyData->GetScalarRange(range);
    GetMaterial()->m_ColorLut->SetRange(range);
    GetMaterial()->UpdateProp();
    GetMaterial()->UpdateFromLut();*/
  }

  //m_DistanceFilter->Update();

  //mafEvent ev(this, CAMERA_UPDATE);
  //this->ForwardUpEvent(&ev);

  /*if (vtkDataSet *vtkdata=vol->GetOutput()->GetVTKData())
  {
    m_PSlicer->Update();
    m_ISlicer->Update();

    vtkDataArray *scalars = vtkdata->GetPointData()->GetScalars();
    if (scalars == NULL)
    {
      return;
    }

    vtkImageData *texture = m_PSlicer->GetTexture();

    GetMaterial()->SetMaterialTexture(texture);
    texture->GetScalarRange(GetMaterial()->m_TableRange);
    GetMaterial()->UpdateProp();
  }*/

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
vtkPolyData *medVMEMaps::GetPolyData()
//-------------------------------------------------------------------------
{
  return m_PolyData;
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
    lutPreset(14,GetMaterial()->m_ColorLut);
    if (m_Output)
    {
      ((mafVMEOutputSurface *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}

//-------------------------------------------------------------------------
int medVMEMaps::SetData(vtkPolyData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);

  if(polydata)
  {
    polydata->Update();
    if (polydata->GetPoints() && polydata->GetVerts()->GetNumberOfCells()==0 && \
      (polydata->GetPolys()->GetNumberOfCells() > 0 || polydata->GetStrips()->GetNumberOfCells() > 0) && \
      polydata->GetLines()->GetNumberOfCells() == 0)
    {
      int res = Superclass::SetData(polydata,t,mode);
      if(m_PolyData==NULL)
        vtkNEW(m_PolyData);
      m_PolyData->DeepCopy(polydata);
      return res;
    }
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int medVMEMaps::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);

  if (polydata) polydata->Update();

  if (polydata && polydata->GetPoints() && polydata->GetVerts()->GetNumberOfCells()==0 && \
    (polydata->GetPolys()->GetNumberOfCells() > 0 || polydata->GetStrips()->GetNumberOfCells() > 0) && \
    polydata->GetLines()->GetNumberOfCells() == 0)
  {
    return Superclass::SetData(data,t,mode);
  }

  mafErrorMacro("Trying to set the wrong type of data inside a VME Image :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
void medVMEMaps::SetDensityDistance(int densityDistance)
//-------------------------------------------------------------------------
{
  mafEvent ev1(this, VME_SHOW, this, false);
  ForwardUpEvent(ev1);
  m_DensityDistance = densityDistance;
  mafEvent ev2(this, VME_SHOW, this, true);
  ForwardUpEvent(ev2);
}

//-------------------------------------------------------------------------
void medVMEMaps::SetFirstThreshold(int firstThreshold)
//-------------------------------------------------------------------------
{
  m_FirstThreshold = firstThreshold;
}

//-------------------------------------------------------------------------
void medVMEMaps::SetSecondThreshold(int secondThreshold)
//-------------------------------------------------------------------------
{
  m_SecondThreshold = secondThreshold;
}

//-------------------------------------------------------------------------
void medVMEMaps::SetMaxDistance(int maxDistance)
//-------------------------------------------------------------------------
{
  m_MaxDistance = maxDistance;
}