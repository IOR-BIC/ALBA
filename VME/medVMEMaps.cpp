/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEMaps.cpp,v $
Language:  C++
Date:      $Date: 2009-09-24 09:36:17 $
Version:   $Revision: 1.1.2.4 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "medDefines.h" 
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
#include "vtkFloatArray.h"
#include "vtkPolyDataNormals.h"
#include "vtkMAFDataPipe.h"
#include "vtkColorTransferFunction.h"
#include "vtkSphereSource.h"
#include "vtkPointData.h"
#include "vtkLookuptable.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCellArray.h"
#include "vtkMAFSmartPointer.h"

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
  
  DependsOnLinkedNodeOn();

  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
  dpipe->GetVTKDataPipe()->SetNthInput(0, m_PolyData);

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
  vtkDEL(m_DistanceFilter);
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
    m_MappedName      = maps->m_MappedName;
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
                  m_MappedName              ==  ((medVMEMaps *)vme)->m_MappedName &&
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
  mafVME *vme = mafVME::SafeDownCast(GetMappedVMELink());
  if(!vme)
    return;
  vtkPolyData *data = (vtkPolyData *)vme->GetOutput()->GetVTKData();
  data->Update();

  m_Normals->SetInput(data);
  m_Normals->ComputePointNormalsOn();
  m_Normals->SplittingOff();
  m_Normals->Update();

  if(m_Volume==NULL)
  {
    SetVolume(mafVMEVolume::SafeDownCast(GetSourceVMELink()));
  }

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

    vtkDataSet *datasetvol = ((mafVME*)m_Volume)->GetOutput()->GetVTKData();
    datasetvol->Update();
    m_DistanceFilter->SetDistanceModeToScalar();
    m_DistanceFilter->SetSource(datasetvol);
    m_DistanceFilter->SetInput((vtkDataSet::SafeDownCast(m_Normals->GetOutput())));
    m_DistanceFilter->SetMaxDistance(m_MaxDistance);
    m_DistanceFilter->SetThreshold(m_FirstThreshold);
    m_DistanceFilter->SetInputMatrix(vme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
    m_DistanceFilter->Update(); 


    vtkPolyData *polyout;
    vtkMAFSmartPointer<vtkFloatArray> scalars;

    //m_PolyData = m_DistanceFilter->GetPolyDataOutput();

    if(polyout = m_DistanceFilter->GetPolyDataOutput())
    {
      polyout->Update();

      scalars->DeepCopy(polyout->GetPointData()->GetScalars());

      scalars->SetName("Distance_density");
      scalars->Modified();

      m_PolyData->DeepCopy(data);
      m_PolyData->GetPointData()->AddArray(scalars);
      m_PolyData->GetPointData()->SetActiveScalars("Distance_density");

      m_PolyData->Modified();
      m_PolyData->Update();
    }

//     if(polyout)
//     {
//       polyout->Update();
// 
//       scalars->DeepCopy(polyout->GetPointData()->GetScalars());
// 
//       scalars->SetName("Distance_density");
//       scalars->Modified();
// 
//       vtkPolyData *polydata = vtkPolyData::SafeDownCast(this->GetOutput()->GetVTKData());
//       polydata->GetPointData()->AddArray(scalars);
//       polydata->GetPointData()->SetActiveScalars("Distance_density");
// 
//       polydata->Modified();
//       polydata->Update();
//     }
  }

  mafVME *vol = mafVME::SafeDownCast(GetMappedVMELink());
  m_MappedName = vol ? vol->GetName() : _("none");
}


//-------------------------------------------------------------------------
void medVMEMaps::InternalUpdate()
//-------------------------------------------------------------------------
{
  if (m_PolyData)
  {
    m_PolyData->Update();
  }

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
    material->UpdateFromLut();
    material->UpdateProp();
  }
  return material;
}

// //-------------------------------------------------------------------------
// int medVMEMaps::SetData(vtkPolyData *data, mafTimeStamp t, int mode)
// //-------------------------------------------------------------------------
// {
//   vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);
// 
//   if(polydata)
//   {
//     polydata->Update();
//     if (polydata->GetPoints() && polydata->GetVerts()->GetNumberOfCells()==0 && \
//       (polydata->GetPolys()->GetNumberOfCells() > 0 || polydata->GetStrips()->GetNumberOfCells() > 0) && \
//       polydata->GetLines()->GetNumberOfCells() == 0)
//     {
//       int res = Superclass::SetData(polydata,t,mode);
//       if(m_PolyData==NULL)
//         vtkNEW(m_PolyData);
//       m_PolyData->DeepCopy(polydata);
//       return res;
//     }
//   }
//   return MAF_ERROR;
// }
// 
// //-------------------------------------------------------------------------
// int medVMEMaps::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
// //-------------------------------------------------------------------------
// {
//   assert(data);
//   vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);
// 
//   if (polydata) polydata->Update();
// 
//   if (polydata && polydata->GetPoints() && polydata->GetVerts()->GetNumberOfCells()==0 && \
//     (polydata->GetPolys()->GetNumberOfCells() > 0 || polydata->GetStrips()->GetNumberOfCells() > 0) && \
//     polydata->GetLines()->GetNumberOfCells() == 0)
//   {
//     return Superclass::SetData(data,t,mode);
//   }
// 
//   mafErrorMacro("Trying to set the wrong type of data inside a VME Image :"<< (data?data->GetClassName():"NULL"));
//   return MAF_ERROR;
// }

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

//-----------------------------------------------------------------------
mafNode *medVMEMaps::GetMappedVMELink()
//-----------------------------------------------------------------------
{
  return GetLink("MappedVME");
}
//-----------------------------------------------------------------------
void medVMEMaps::SetMappedVMELink(mafNode *node)
//-----------------------------------------------------------------------
{
  SetLink("MappedVME", node);
  Modified();
}

//-----------------------------------------------------------------------
mafNode *medVMEMaps::GetSourceVMELink()
//-----------------------------------------------------------------------
{
  return GetLink("SourceVME");
}
//-----------------------------------------------------------------------
void medVMEMaps::SetSourceVMELink(mafNode *node)
//-----------------------------------------------------------------------
{
  SetLink("SourceVME", node);
  Modified();
}