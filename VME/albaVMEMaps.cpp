/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMaps
 Authors: Eleonora Mambrini
 
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

#include "albaVMEMaps.h"

#include "albaDataPipeCustom.h"
#include "albaPipesurface.h"
#include "albaGUI.h"
#include "albaTransform.h"
#include "albaVMEOutputSurface.h"
#include "albaStorageElement.h"
#include "albaGUILutPreset.h"
#include "albaVMEVolume.h"
#include "albaVMESurface.h"
#include "mmaMaterial.h"

#include "vtkALBADistanceFilter.h"
#include "vtkPolyData.h"
#include "vtkFloatArray.h"
#include "vtkPolyDataNormals.h"
#include "vtkALBADataPipe.h"
#include "vtkColorTransferFunction.h"
#include "vtkSphereSource.h"
#include "vtkPointData.h"
#include "vtkLookuptable.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCellArray.h"
#include "vtkALBASmartPointer.h"

#include <assert.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEMaps)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEMaps::albaVMEMaps()
//-------------------------------------------------------------------------
{
  m_Volume          = NULL;
  m_Normals         = NULL;
  m_Volume          = NULL;
  m_PolyData        = NULL;
  m_Table           = NULL;

  albaNEW(m_Transform);
  vtkNEW(m_Normals);
  vtkNEW(m_DistanceFilter);
  vtkNEW(m_PolyData);

  albaVMEOutputSurface *output = albaVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);
  
  DependsOnLinkedNodeOn();

  albaDataPipeCustom *dpipe = albaDataPipeCustom::New();
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
albaVMEMaps::~albaVMEMaps()
//-------------------------------------------------------------------------
{
  vtkDEL(m_Normals);
  vtkDEL(m_DistanceFilter);
  
  albaDEL(m_Transform);
  vtkDEL(m_PolyData);

  if(m_Table)
    vtkDEL(m_Table);

  SetOutput(NULL);
}

//-------------------------------------------------------------------------
int albaVMEMaps::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMEMaps *maps = albaVMEMaps::SafeDownCast(a);

    m_Transform->SetMatrix(maps->m_Transform->GetMatrix());
    albaDataPipeCustom *dpipe = albaDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetDependOnAbsPose(true);
      dpipe->GetVTKDataPipe()->SetNthInput(0, m_PolyData);
    }
    m_MappedName      = maps->m_MappedName;
    GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;

    m_FirstThreshold          = maps->GetFirstThreshold();
    m_SecondThreshold         = maps->GetSecondThreshold();
    m_MaxDistance             = maps->GetMaxDistance();

    if(maps->GetSourceVMELink())
      SetSourceVMELink(maps->GetSourceVMELink());
    if(maps->GetMappedVMELink())
      SetMappedVMELink(maps->GetMappedVMELink());

    return ALBA_OK;
  }  
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
bool albaVMEMaps::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    return ret = (m_Transform->GetMatrix()  ==  ((albaVMEMaps *)vme)->m_Transform->GetMatrix() &&
                  //m_Volume                  ==  ((albaVMEMaps *)vme)->GetVolume() &&
                  m_FirstThreshold          ==  ((albaVMEMaps *)vme)->GetFirstThreshold() &&
                  m_SecondThreshold         ==  ((albaVMEMaps *)vme)->GetSecondThreshold() &&
                  m_MaxDistance             ==  ((albaVMEMaps *)vme)->GetMaxDistance() &&
                  m_MappedName              ==  ((albaVMEMaps *)vme)->m_MappedName
                 );
  }
  return ret;
}

//-------------------------------------------------------------------------
albaGUI* albaVMEMaps::CreateGui()
//-------------------------------------------------------------------------
{
  //return Superclass::CreateGui();
  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
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
void albaVMEMaps::OnEvent(albaEventBase *alba_event)
//-------------------------------------------------------------------------
{
  //Superclass::OnEvent(alba_event);
  // events to be sent up or down in the tree are simply forwarded
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case ID_DENSITY_DISTANCE:
      {
        SetDensityDistance(m_DensityDistance);
      }
      break;
    default:
      Superclass::OnEvent(alba_event);
    }
  }
  else
  {
    Superclass::OnEvent(alba_event);
  }
}
//-------------------------------------------------------------------------
albaVMEOutputSurface *albaVMEMaps::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputSurface::New()); // create the output
  }
  return albaVMEOutputSurface::SafeDownCast(m_Output);
}

 //-------------------------------------------------------------------------
 void albaVMEMaps::SetMatrix(const albaMatrix &mat)
 //-------------------------------------------------------------------------
 {
   m_Transform->SetMatrix(mat);
   Modified();
 }

//-------------------------------------------------------------------------
void albaVMEMaps::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-----------------------------------------------------------------------
void albaVMEMaps::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
}

//-------------------------------------------------------------------------
void albaVMEMaps::InternalPreUpdate()
//-------------------------------------------------------------------------
{
	UpdateFilter();

  albaVME *vol = GetMappedVMELink();
  m_MappedName = vol ? vol->GetName() : _("none");
}


//-------------------------------------------------------------------------
void albaVMEMaps::InternalUpdate()
//-------------------------------------------------------------------------
{

}

//-------------------------------------------------------------------------
void albaVMEMaps::SetVolume(albaVMEVolume *volume)
//-------------------------------------------------------------------------
{
  m_Volume = volume;
	SetSourceVMELink(volume);
  if(m_Gui && m_Volume)
    m_Gui->Enable(ID_DENSITY_DISTANCE, true);
}

//-------------------------------------------------------------------------
albaVMEVolume *albaVMEMaps::GetVolume()
//-------------------------------------------------------------------------
{
  if(m_Volume == NULL)
  {
    m_Volume = albaVMEVolume::SafeDownCast(GetSourceVMELink());
  }
  return m_Volume;
}

//-----------------------------------------------------------------------
int albaVMEMaps::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//-----------------------------------------------------------------------
int albaVMEMaps::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==ALBA_OK)
  {
    albaMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==ALBA_OK)
    {
      m_Transform->SetMatrix(matrix);

      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
mmaMaterial *albaVMEMaps::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    lutPreset(0, material->m_ColorLut);
    //material->m_ColorLut = CreateTable();
    material->UpdateFromLut();
    material->UpdateProp();
  }
  return material;
}


//-------------------------------------------------------------------------
void albaVMEMaps::SetDensityDistance(int densityDistance)
//-------------------------------------------------------------------------
{
  m_DensityDistance = densityDistance;
  GetOutput()->Update();
  UpdateFilter();
}

//-------------------------------------------------------------------------
void albaVMEMaps::SetFirstThreshold(int firstThreshold)
//-------------------------------------------------------------------------
{
  m_FirstThreshold = firstThreshold;
  UpdateFilter();
  //InternalPreUpdate();
  //InternalUpdate();
}

//-------------------------------------------------------------------------
void albaVMEMaps::SetSecondThreshold(int secondThreshold)
//-------------------------------------------------------------------------
{
  m_SecondThreshold = secondThreshold;
  UpdateFilter();
  //InternalPreUpdate();
  //InternalUpdate();
}

//-------------------------------------------------------------------------
void albaVMEMaps::SetMaxDistance(int maxDistance)
//-------------------------------------------------------------------------
{
  m_MaxDistance = maxDistance;
  UpdateFilter();
  //InternalPreUpdate();
  //InternalUpdate();

}

//-----------------------------------------------------------------------
albaVME *albaVMEMaps::GetMappedVMELink()
//-----------------------------------------------------------------------
{
  return GetLink("MappedVME");
}
//-----------------------------------------------------------------------
void albaVMEMaps::SetMappedVMELink(albaVME *node)
//-----------------------------------------------------------------------
{
  SetLink("MappedVME", node);
	UpdateFilter();
}

//-----------------------------------------------------------------------
albaVME *albaVMEMaps::GetSourceVMELink()
//-----------------------------------------------------------------------
{
  return GetLink("SourceVME");
}

//-----------------------------------------------------------------------
void albaVMEMaps::SetSourceVMELink(albaVME *node)
//-----------------------------------------------------------------------
{
  SetLink("SourceVME", node);
  Modified();
}

//-----------------------------------------------------------------------
vtkLookupTable *albaVMEMaps::CreateTable()
//-----------------------------------------------------------------------
{
  int m_NumSections =3;
  wxColour m_LowColour, m_HiColour, m_MidColour, m_MidColour1, m_MidColour2;

  vtkNEW(m_Table);

  m_LowColour.Set(255,0,0);
  m_MidColour.Set(0,255,0);
  m_HiColour.Set(0,0,255);
  m_MidColour1.Set(255,255,0);
  m_MidColour2.Set(0,255,255);

  m_Table->SetNumberOfTableValues(m_NumSections);

  if(m_DensityDistance==0)
  {
    int i;
    for (i=-4*m_MaxDistance;i<-m_MaxDistance;i++)
      m_Table->SetTableValue(i,m_LowColour.Red()/255.0, m_LowColour.Green()/255.0,	m_LowColour.Blue()/255.0);
    for (i=-m_MaxDistance;i<m_MaxDistance;i++)
      m_Table->SetTableValue(i,1.0,1.0,1.0);
    for (i=m_MaxDistance;i<=4*m_MaxDistance;i++)
      m_Table->SetTableValue(i,m_HiColour.Red()/255.0, m_HiColour.Green()/255.0,	m_HiColour.Blue()/255.0);
  }

  else if(m_DensityDistance==1)
  {

    double range[2];
    m_Volume->GetOutput()->GetVTKData()->GetScalarRange(range);

    m_Table->SetTableValue(range[0],m_LowColour.Red()/255.0, m_LowColour.Green()/255.0,	m_LowColour.Blue()/255.0);
    m_Table->SetTableValue(m_SecondThreshold,m_MidColour1.Red()/255.0, m_MidColour1.Green()/255.0,	m_MidColour1.Blue()/255.0);
    m_Table->SetTableValue(m_FirstThreshold,m_MidColour2.Red()/255.0, m_MidColour2.Green()/255.0,	m_MidColour2.Blue()/255.0);
    m_Table->SetTableValue(range[1],m_HiColour.Red()/255.0, m_HiColour.Green()/255.0,	m_HiColour.Blue()/255.0);
  }

  m_Table->Build();

  return m_Table;

}

//-----------------------------------------------------------------------
void albaVMEMaps::GetScalarRange(double range[2])
//-----------------------------------------------------------------------
{
  range[0] = 0;
  range[1] = 1;
  if(m_PolyData->GetPointData() && m_PolyData->GetPointData()->GetScalars())
    m_PolyData->GetPointData()->GetScalars()->GetRange(range);

}

//-----------------------------------------------------------------------
void albaVMEMaps::UpdateFilter()
//-----------------------------------------------------------------------
{

  albaVME *vme = GetMappedVMELink();
  if(!vme)
    return;
  vtkPolyData *data = (vtkPolyData *)vme->GetOutput()->GetVTKData();

  vme->Update();

  m_Normals->SetInputData(data);
  m_Normals->ComputePointNormalsOn();
  m_Normals->SplittingOff();
  m_Normals->Update();

  if(m_Volume==NULL)
  {
    SetVolume(albaVMEVolume::SafeDownCast(GetSourceVMELink()));
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

    vtkDataSet *datasetvol = m_Volume->GetOutput()->GetVTKData();
    m_DistanceFilter->SetDistanceModeToScalar();
    m_DistanceFilter->SetSource(datasetvol);
    m_DistanceFilter->SetInputConnection(m_Normals->GetOutputPort());
    m_DistanceFilter->SetMaxDistance(m_MaxDistance);
    m_DistanceFilter->SetThreshold(m_FirstThreshold);
    m_DistanceFilter->SetInputMatrix(vme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
    m_DistanceFilter->Update(); 

    vtkPolyData *polyout;
    vtkALBASmartPointer<vtkFloatArray> scalars;

    if(polyout = m_DistanceFilter->GetPolyDataOutput())
    {

      scalars->DeepCopy(polyout->GetPointData()->GetScalars());

      scalars->SetName("Distance_density");
      scalars->Modified();

      m_PolyData->DeepCopy(data);
      m_PolyData->GetPointData()->AddArray(scalars);
      m_PolyData->GetPointData()->SetActiveScalars("Distance_density");

      m_PolyData->Modified();
    }
  }
}
