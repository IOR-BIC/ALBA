/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESegmentationVolume
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
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

#include "albaVMESegmentationVolume.h"
#include "albaGUI.h"
#include "mmaVolumeMaterial.h"

#include "albaTransform.h"
#include "albaStorageElement.h"
#include "albaDataPipeCustomSegmentationVolume.h"
#include "albaVMEOutputVolume.h"
#include "albaMatrix.h"
#include "albaAttributeSegmentationVolume.h"


#include "vtkALBASmartPointer.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMESegmentationVolume)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMESegmentationVolume::albaVMESegmentationVolume()
//-------------------------------------------------------------------------
{
  albaNEW(m_Transform);
  albaVMEOutputVolume *output = albaVMEOutputVolume::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  //DependsOnLinkedNodeOn();

  m_VolumeAttribute = NULL;

  // attach a data pipe which creates a bridge between VTK and ALBA
  m_SegmentingDataPipe = albaDataPipeCustomSegmentationVolume::New();
  SetDataPipe(m_SegmentingDataPipe);
}

//-------------------------------------------------------------------------
albaVMESegmentationVolume::~albaVMESegmentationVolume()
//-------------------------------------------------------------------------
{
  // these links are children, thus it's not our responsibility to
  // destroy them, it's part of the vtkTree one's
  albaDEL(m_Transform);
  SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaVolumeMaterial *albaVMESegmentationVolume::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaVolumeMaterial *material = (mmaVolumeMaterial *)GetAttribute("VolumeMaterialAttributes");
  if (material == NULL)
  {
    material = mmaVolumeMaterial::New();
    SetAttribute("VolumeMaterialAttributes", material);
  }
  return material;
}
//-------------------------------------------------------------------------
albaAttributeSegmentationVolume *albaVMESegmentationVolume::GetVolumeAttribute()
//-------------------------------------------------------------------------
{
  if (this->GetAttribute("SegmentationVolumeData") != NULL)
  {
    m_VolumeAttribute = albaAttributeSegmentationVolume::SafeDownCast(this->GetAttribute("SegmentationVolumeData"));
  }
  else
  {
    m_VolumeAttribute = albaAttributeSegmentationVolume::New();
    this->SetAttribute("SegmentationVolumeData",m_VolumeAttribute);

    m_VolumeAttribute = albaAttributeSegmentationVolume::SafeDownCast(this->GetAttribute("SegmentationVolumeData"));
  }

  return m_VolumeAttribute;

}
//-------------------------------------------------------------------------
int albaVMESegmentationVolume::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==ALBA_OK)
  {
    // force material allocation
    GetMaterial();
    
    m_VolumeAttribute = GetVolumeAttribute();
    
    // attach a data pipe which creates a bridge between VTK and ALBA
    m_SegmentingDataPipe->SetVolume(this->GetVolumeLink());
    m_SegmentingDataPipe->SetManualVolumeMask(this->GetManualVolumeMask());
    m_SegmentingDataPipe->SetDependOnVMETime(false);
    m_SegmentingDataPipe->SetAutomaticSegmentationThresholdModality(m_VolumeAttribute->GetAutomaticSegmentationThresholdModality());
    m_SegmentingDataPipe->SetDoubleThresholdModality(m_VolumeAttribute->GetDoubleThresholdModality());
    m_SegmentingDataPipe->SetAutomaticSegmentationGlobalThreshold(m_VolumeAttribute->GetAutomaticSegmentationGlobalThreshold());
    for (int i=0;i<m_VolumeAttribute->GetNumberOfRanges();i++)
    {
      int startSlice,endSlice;
      double threshold;
      m_VolumeAttribute->GetRange(i,startSlice,endSlice,threshold);
      m_SegmentingDataPipe->AddRange(startSlice,endSlice,threshold);
    }

    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
void albaVMESegmentationVolume::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  albaVME *vol = GetVolumeLink();
  if (m_SegmentingDataPipe->GetVolume() != vol)
  {
    m_SegmentingDataPipe->SetVolume(vol);
  }

  if (m_Gui)
  {
    m_Gui->Update();
  }
}

//-------------------------------------------------------------------------
int albaVMESegmentationVolume::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    ((albaVMESegmentationVolume *)a)->SetVolumeLink(this->GetVolumeLink());
    ((albaVMESegmentationVolume *)a)->SetManualVolumeMask(this->GetManualVolumeMask());
    return ALBA_OK;
  }  
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
bool albaVMESegmentationVolume::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = true;
    if (((albaVMESegmentationVolume *)vme)->GetVolumeLink() != this->GetVolumeLink())
    {
      ret = false;
    }
    if (((albaVMESegmentationVolume *)vme)->GetManualVolumeMask() != this->GetManualVolumeMask())
    {
      ret = false;
    }
    
  }
  return ret;
}

//-------------------------------------------------------------------------
void albaVMESegmentationVolume::SetMatrix(const albaMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void albaVMESegmentationVolume::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-------------------------------------------------------------------------
bool albaVMESegmentationVolume::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool albaVMESegmentationVolume::IsDataAvailable()
//-------------------------------------------------------------------------
{
  albaVME *vol = GetVolumeLink();
  return (vol && vol->IsDataAvailable());
}
//-----------------------------------------------------------------------
int albaVMESegmentationVolume::InternalStore(albaStorageElement *parent)
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
int albaVMESegmentationVolume::InternalRestore(albaStorageElement *node)
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
albaGUI* albaVMESegmentationVolume::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->Divider();
  albaVME *vol = GetVolumeLink();
  m_VolumeName = vol ? vol->GetName() : _("none");
  m_Gui->Button(ID_VOLUME_LINK,&m_VolumeName,_("Volume"), _("Select the volume to be segmented"));

  return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMESegmentationVolume::OnEvent(albaEventBase *alba_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case ID_VOLUME_LINK:
      {
        albaString title = _("Choose volume vme");
        e->SetId(VME_CHOOSE);
        e->SetPointer(&albaVMESegmentationVolume::VolumeAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        albaVME *n = e->GetVme();
        if (n != NULL)
        {
          SetVolumeLink(n);
          m_VolumeName = n->GetName();
          m_Gui->Update();
        }
      }
      break;
    default:
      albaVME::OnEvent(alba_event);
    }
  }
  else
  {
    Superclass::OnEvent(alba_event);
  }
}
//-------------------------------------------------------------------------
int albaVMESegmentationVolume::AddRange(int startSlice,int endSlice,double threshold, double upperThreshold)
//-------------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->AddRange(startSlice,endSlice,threshold, upperThreshold);
  if (result == ALBA_OK)
  {
    result = m_VolumeAttribute->AddRange(startSlice,endSlice,threshold, upperThreshold);
    if (result == ALBA_OK)
    {
      Modified();
      return ALBA_OK;
    }
  }
  
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMESegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold, double &upperThreshold)
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetRange(index,startSlice,endSlice,threshold, upperThreshold );
}

//-----------------------------------------------------------------------
int albaVMESegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold)
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetRange(index,startSlice,endSlice,threshold );
}


//-----------------------------------------------------------------------
bool albaVMESegmentationVolume::CheckNumberOfThresholds()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->CheckNumberOfThresholds();
}
//-----------------------------------------------------------------------
int albaVMESegmentationVolume::UpdateRange(int index,int startSlice, int endSlice, double threshold, double upperThershold)
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->UpdateRange(index,startSlice,endSlice,threshold, upperThershold);
  if (result == ALBA_OK)
  {
    result = m_VolumeAttribute->UpdateRange(index,startSlice,endSlice,threshold, upperThershold);
    if (result == ALBA_OK)
    {
      Modified();
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;  
}
//-----------------------------------------------------------------------
int albaVMESegmentationVolume::RemoveAllRanges()
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->RemoveAllRanges();
  if (result == ALBA_OK)
  {
    result = m_VolumeAttribute->RemoveAllRanges();
    if (result == ALBA_OK)
    {
      Modified();
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMESegmentationVolume::RemoveAllSeeds()
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->RemoveAllSeeds();
  if (result == ALBA_OK)
  {
    result = m_VolumeAttribute->RemoveAllSeeds();
    if (result == ALBA_OK)
    {
      Modified();
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMESegmentationVolume::DeleteSeed(int index)
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->DeleteSeed(index);
  if (result == ALBA_OK)
  {
    result = m_VolumeAttribute->DeleteSeed(index);
    if (result == ALBA_OK)
    {
      Modified();
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMESegmentationVolume::DeleteRange(int index)
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->DeleteRange(index);
  if (result == ALBA_OK)
  {
    result = m_VolumeAttribute->DeleteRange(index);
    if (result == ALBA_OK)
    {
      Modified();
      return ALBA_OK;
    }
  }
  
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMESegmentationVolume::SetVolumeLink(albaVME *volume)
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->SetVolume(volume);

  if (result == ALBA_ERROR)
  {
    return ALBA_ERROR;
  }

  SetLink("Volume", volume);
  Modified();

  return ALBA_OK;
}
//-----------------------------------------------------------------------
void albaVMESegmentationVolume::SetManualVolumeMask(albaVME *volume)
//-----------------------------------------------------------------------
{
  SetLink("ManualVolumeMask", volume);
  m_SegmentingDataPipe->SetManualVolumeMask(volume);
  Modified();
}
//-----------------------------------------------------------------------
albaVME *albaVMESegmentationVolume::GetManualVolumeMask()
//-----------------------------------------------------------------------
{
  return GetLink("ManualVolumeMask");
}
//-----------------------------------------------------------------------
void albaVMESegmentationVolume::SetRefinementVolumeMask(albaVME *volume)
//-----------------------------------------------------------------------
{
  SetLink("RefinementVolumeMask", volume);
  m_SegmentingDataPipe->SetRefinementVolumeMask(volume);
  Modified();
}
//-----------------------------------------------------------------------
albaVME *albaVMESegmentationVolume::GetRefinementVolumeMask()
//-----------------------------------------------------------------------
{
  return GetLink("RefinementVolumeMask");
}
//-----------------------------------------------------------------------
vtkDataSet *albaVMESegmentationVolume::GetAutomaticOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetAutomaticOutput();
}
//-----------------------------------------------------------------------
vtkDataSet *albaVMESegmentationVolume::GetRegionGrowingOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetRegionGrowingOutput();
}
//-----------------------------------------------------------------------
vtkDataSet *albaVMESegmentationVolume::GetRefinementOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetRefinementOutput();
}
//-----------------------------------------------------------------------
vtkDataSet *albaVMESegmentationVolume::GetManualOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetManualOutput();
}
//-----------------------------------------------------------------------
albaVME *albaVMESegmentationVolume::GetVolumeLink()
//-----------------------------------------------------------------------
{
  return GetLink("Volume");
}
//-----------------------------------------------------------------------
int albaVMESegmentationVolume::AddSeed(int seed[3])
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->AddSeed(seed);
  if (result == ALBA_OK)
  {
    result = m_VolumeAttribute->AddSeed(seed);
    if (result == ALBA_OK)
    {
      Modified();
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMESegmentationVolume::GetSeed(int index,int seed[3])
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetSeed(index,seed);
}
//-------------------------------------------------------------------------
double albaVMESegmentationVolume::GetRegionGrowingLowerThreshold()
//-------------------------------------------------------------------------
{
  return m_VolumeAttribute->GetRegionGrowingLowerThreshold();
}
//-------------------------------------------------------------------------
double albaVMESegmentationVolume::GetRegionGrowingUpperThreshold()
//-------------------------------------------------------------------------
{
  return m_VolumeAttribute->GetRegionGrowingUpperThreshold();
}
//-------------------------------------------------------------------------
int albaVMESegmentationVolume::GetNumberOfRanges()
//-------------------------------------------------------------------------
{
  return m_VolumeAttribute->GetNumberOfRanges();
}
//-------------------------------------------------------------------------
int albaVMESegmentationVolume::GetNumberOfSeeds()
//-------------------------------------------------------------------------
{
  return m_VolumeAttribute->GetNumberOfSeeds();
}
//-------------------------------------------------------------------------
void albaVMESegmentationVolume::SetRegionGrowingLowerThreshold(double value)
//-------------------------------------------------------------------------
{
  if (m_VolumeAttribute && m_SegmentingDataPipe)
  {
	  m_VolumeAttribute->SetRegionGrowingLowerThreshold(value);
	  m_SegmentingDataPipe->SetRegionGrowingLowerThreshold(value);
  }
}
//-------------------------------------------------------------------------
void albaVMESegmentationVolume::SetRegionGrowingUpperThreshold(double value)
//-------------------------------------------------------------------------
{
  if (m_VolumeAttribute && m_SegmentingDataPipe)
  {
    m_VolumeAttribute->SetRegionGrowingUpperThreshold(value);
    m_SegmentingDataPipe->SetRegionGrowingUpperThreshold(value);
  }
}
//-----------------------------------------------------------------------
void albaVMESegmentationVolume::SetAutomaticSegmentationThresholdModality(int modality)
//-----------------------------------------------------------------------
{
  if (m_VolumeAttribute == NULL)// force attribute reading
  {
    GetVolumeAttribute();
  }

  m_VolumeAttribute->SetAutomaticSegmentationThresholdModality(modality);
  m_SegmentingDataPipe->SetAutomaticSegmentationThresholdModality(modality);
  Modified();
}
//-----------------------------------------------------------------------
int albaVMESegmentationVolume::GetAutomaticSegmentationThresholdModality()
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetAutomaticSegmentationThresholdModality();
}

//-----------------------------------------------------------------------
void albaVMESegmentationVolume::SetDoubleThresholdModality(int modality)
//-----------------------------------------------------------------------
{
  if (m_VolumeAttribute == NULL)// force attribute reading
  {
    GetVolumeAttribute();
  }

  m_VolumeAttribute->SetDoubleThresholdModality(modality);
  m_SegmentingDataPipe->SetDoubleThresholdModality(modality);
  Modified();
}
//-----------------------------------------------------------------------
int albaVMESegmentationVolume::GetDoubleThresholdModality()
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetDoubleThresholdModality();
}
//-----------------------------------------------------------------------
void albaVMESegmentationVolume::SetAutomaticSegmentationGlobalThreshold(double lowerThreshold, double uppperThreshold)
//-----------------------------------------------------------------------
{
  if (m_VolumeAttribute == NULL)// force attribute reading
  {
    GetVolumeAttribute();
  }

  m_VolumeAttribute->SetAutomaticSegmentationGlobalThreshold(lowerThreshold,uppperThreshold);
  m_SegmentingDataPipe->SetAutomaticSegmentationGlobalThreshold(lowerThreshold,uppperThreshold);
  Modified();
}
//-----------------------------------------------------------------------
double albaVMESegmentationVolume::GetAutomaticSegmentationGlobalThreshold()
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetAutomaticSegmentationGlobalThreshold();
}

//-----------------------------------------------------------------------
double albaVMESegmentationVolume::GetAutomaticSegmentationGlobalUpperThreshold()
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetAutomaticSegmentationGlobalUpperThreshold();
}

//------------------------------------------------------------------------
void albaVMESegmentationVolume::SetRegionGrowingSliceRange(int startSlice, int endSlice)
//------------------------------------------------------------------------
{
  m_SegmentingDataPipe->SetRegionGrowingSliceRange(startSlice, endSlice);
}

//------------------------------------------------------------------------
int albaVMESegmentationVolume::GetRegionGrowingSliceRange(int &startSlice, int &endSlice)
//------------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetRegionGrowingSliceRange(startSlice, endSlice);
}

//-------------------------------------------------------------------------
albaVMEOutput *albaVMESegmentationVolume::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputVolume::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
char** albaVMESegmentationVolume::GetIcon() 
//-------------------------------------------------------------------------
{
#include "albaVMEVolume.xpm"
  return albaVMEVolume_xpm;
}
