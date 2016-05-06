/*=========================================================================

 Program: MAF2
 Module: mafVMESegmentationVolume
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
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

#include "mafVMESegmentationVolume.h"
#include "mafGUI.h"
#include "mmaVolumeMaterial.h"

#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafDataPipeCustomSegmentationVolume.h"
#include "mafVMEOutputVolume.h"
#include "mafMatrix.h"
#include "mafAttributeSegmentationVolume.h"


#include "vtkMAFSmartPointer.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMESegmentationVolume)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMESegmentationVolume::mafVMESegmentationVolume()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputVolume *output = mafVMEOutputVolume::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  //DependsOnLinkedNodeOn();

  m_VolumeAttribute = NULL;

  // attach a data pipe which creates a bridge between VTK and MAF
  m_SegmentingDataPipe = mafDataPipeCustomSegmentationVolume::New();
  SetDataPipe(m_SegmentingDataPipe);
}

//-------------------------------------------------------------------------
mafVMESegmentationVolume::~mafVMESegmentationVolume()
//-------------------------------------------------------------------------
{
  // these links are children, thus it's not our responsibility to
  // destroy them, it's part of the vtkTree one's
  mafDEL(m_Transform);
  SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaVolumeMaterial *mafVMESegmentationVolume::GetMaterial()
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
mafAttributeSegmentationVolume *mafVMESegmentationVolume::GetVolumeAttribute()
//-------------------------------------------------------------------------
{
  if (this->GetAttribute("SegmentationVolumeData") != NULL)
  {
    m_VolumeAttribute = mafAttributeSegmentationVolume::SafeDownCast(this->GetAttribute("SegmentationVolumeData"));
  }
  else
  {
    m_VolumeAttribute = mafAttributeSegmentationVolume::New();
    this->SetAttribute("SegmentationVolumeData",m_VolumeAttribute);

    m_VolumeAttribute = mafAttributeSegmentationVolume::SafeDownCast(this->GetAttribute("SegmentationVolumeData"));
  }

  return m_VolumeAttribute;

}
//-------------------------------------------------------------------------
int mafVMESegmentationVolume::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();
    
    m_VolumeAttribute = GetVolumeAttribute();
    
    // attach a data pipe which creates a bridge between VTK and MAF
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

    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mafVMESegmentationVolume::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  mafVME *vol = GetVolumeLink();
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
int mafVMESegmentationVolume::DeepCopy(mafVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    ((mafVMESegmentationVolume *)a)->SetVolumeLink(this->GetVolumeLink());
    ((mafVMESegmentationVolume *)a)->SetManualVolumeMask(this->GetManualVolumeMask());
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMESegmentationVolume::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = true;
    if (((mafVMESegmentationVolume *)vme)->GetVolumeLink() != this->GetVolumeLink())
    {
      ret = false;
    }
    if (((mafVMESegmentationVolume *)vme)->GetManualVolumeMask() != this->GetManualVolumeMask())
    {
      ret = false;
    }
    
  }
  return ret;
}

//-------------------------------------------------------------------------
void mafVMESegmentationVolume::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void mafVMESegmentationVolume::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-------------------------------------------------------------------------
bool mafVMESegmentationVolume::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool mafVMESegmentationVolume::IsDataAvailable()
//-------------------------------------------------------------------------
{
  mafVME *vol = GetVolumeLink();
  return (vol && vol->IsDataAvailable());
}
//-----------------------------------------------------------------------
int mafVMESegmentationVolume::InternalStore(mafStorageElement *parent)
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
int mafVMESegmentationVolume::InternalRestore(mafStorageElement *node)
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
mafGUI* mafVMESegmentationVolume::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->Divider();
  mafVME *vol = GetVolumeLink();
  m_VolumeName = vol ? vol->GetName() : _("none");
  m_Gui->Button(ID_VOLUME_LINK,&m_VolumeName,_("Volume"), _("Select the volume to be segmented"));

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMESegmentationVolume::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_VOLUME_LINK:
      {
        mafString title = _("Choose volume vme");
        e->SetId(VME_CHOOSE);
        e->SetArg((long)&mafVMESegmentationVolume::VolumeAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafVME *n = e->GetVme();
        if (n != NULL)
        {
          SetVolumeLink(n);
          m_VolumeName = n->GetName();
          m_Gui->Update();
        }
      }
      break;
    default:
      mafVME::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
int mafVMESegmentationVolume::AddRange(int startSlice,int endSlice,double threshold, double upperThreshold)
//-------------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->AddRange(startSlice,endSlice,threshold, upperThreshold);
  if (result == MAF_OK)
  {
    result = m_VolumeAttribute->AddRange(startSlice,endSlice,threshold, upperThreshold);
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }
  
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMESegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold, double &upperThreshold)
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetRange(index,startSlice,endSlice,threshold, upperThreshold );
}

//-----------------------------------------------------------------------
int mafVMESegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold)
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetRange(index,startSlice,endSlice,threshold );
}


//-----------------------------------------------------------------------
bool mafVMESegmentationVolume::CheckNumberOfThresholds()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->CheckNumberOfThresholds();
}
//-----------------------------------------------------------------------
int mafVMESegmentationVolume::UpdateRange(int index,int startSlice, int endSlice, double threshold, double upperThershold)
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->UpdateRange(index,startSlice,endSlice,threshold, upperThershold);
  if (result == MAF_OK)
  {
    result = m_VolumeAttribute->UpdateRange(index,startSlice,endSlice,threshold, upperThershold);
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }

  return MAF_ERROR;  
}
//-----------------------------------------------------------------------
int mafVMESegmentationVolume::RemoveAllRanges()
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->RemoveAllRanges();
  if (result == MAF_OK)
  {
    result = m_VolumeAttribute->RemoveAllRanges();
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMESegmentationVolume::RemoveAllSeeds()
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->RemoveAllSeeds();
  if (result == MAF_OK)
  {
    result = m_VolumeAttribute->RemoveAllSeeds();
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMESegmentationVolume::DeleteSeed(int index)
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->DeleteSeed(index);
  if (result == MAF_OK)
  {
    result = m_VolumeAttribute->DeleteSeed(index);
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMESegmentationVolume::DeleteRange(int index)
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->DeleteRange(index);
  if (result == MAF_OK)
  {
    result = m_VolumeAttribute->DeleteRange(index);
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }
  
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMESegmentationVolume::SetVolumeLink(mafVME *volume)
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->SetVolume(volume);

  if (result == MAF_ERROR)
  {
    return MAF_ERROR;
  }

  SetLink("Volume", volume);
  Modified();

  return MAF_OK;
}
//-----------------------------------------------------------------------
void mafVMESegmentationVolume::SetManualVolumeMask(mafVME *volume)
//-----------------------------------------------------------------------
{
  SetLink("ManualVolumeMask", volume);
  m_SegmentingDataPipe->SetManualVolumeMask(volume);
  Modified();
}
//-----------------------------------------------------------------------
mafVME *mafVMESegmentationVolume::GetManualVolumeMask()
//-----------------------------------------------------------------------
{
  return GetLink("ManualVolumeMask");
}
//-----------------------------------------------------------------------
void mafVMESegmentationVolume::SetRefinementVolumeMask(mafVME *volume)
//-----------------------------------------------------------------------
{
  SetLink("RefinementVolumeMask", volume);
  m_SegmentingDataPipe->SetRefinementVolumeMask(volume);
  Modified();
}
//-----------------------------------------------------------------------
mafVME *mafVMESegmentationVolume::GetRefinementVolumeMask()
//-----------------------------------------------------------------------
{
  return GetLink("RefinementVolumeMask");
}
//-----------------------------------------------------------------------
vtkDataSet *mafVMESegmentationVolume::GetAutomaticOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetAutomaticOutput();
}
//-----------------------------------------------------------------------
vtkDataSet *mafVMESegmentationVolume::GetRegionGrowingOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetRegionGrowingOutput();
}
//-----------------------------------------------------------------------
vtkDataSet *mafVMESegmentationVolume::GetRefinementOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetRefinementOutput();
}
//-----------------------------------------------------------------------
vtkDataSet *mafVMESegmentationVolume::GetManualOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetManualOutput();
}
//-----------------------------------------------------------------------
mafVME *mafVMESegmentationVolume::GetVolumeLink()
//-----------------------------------------------------------------------
{
  return GetLink("Volume");
}
//-----------------------------------------------------------------------
int mafVMESegmentationVolume::AddSeed(int seed[3])
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->AddSeed(seed);
  if (result == MAF_OK)
  {
    result = m_VolumeAttribute->AddSeed(seed);
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMESegmentationVolume::GetSeed(int index,int seed[3])
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetSeed(index,seed);
}
//-------------------------------------------------------------------------
double mafVMESegmentationVolume::GetRegionGrowingLowerThreshold()
//-------------------------------------------------------------------------
{
  return m_VolumeAttribute->GetRegionGrowingLowerThreshold();
}
//-------------------------------------------------------------------------
double mafVMESegmentationVolume::GetRegionGrowingUpperThreshold()
//-------------------------------------------------------------------------
{
  return m_VolumeAttribute->GetRegionGrowingUpperThreshold();
}
//-------------------------------------------------------------------------
int mafVMESegmentationVolume::GetNumberOfRanges()
//-------------------------------------------------------------------------
{
  return m_VolumeAttribute->GetNumberOfRanges();
}
//-------------------------------------------------------------------------
int mafVMESegmentationVolume::GetNumberOfSeeds()
//-------------------------------------------------------------------------
{
  return m_VolumeAttribute->GetNumberOfSeeds();
}
//-------------------------------------------------------------------------
void mafVMESegmentationVolume::SetRegionGrowingLowerThreshold(double value)
//-------------------------------------------------------------------------
{
  if (m_VolumeAttribute && m_SegmentingDataPipe)
  {
	  m_VolumeAttribute->SetRegionGrowingLowerThreshold(value);
	  m_SegmentingDataPipe->SetRegionGrowingLowerThreshold(value);
  }
}
//-------------------------------------------------------------------------
void mafVMESegmentationVolume::SetRegionGrowingUpperThreshold(double value)
//-------------------------------------------------------------------------
{
  if (m_VolumeAttribute && m_SegmentingDataPipe)
  {
    m_VolumeAttribute->SetRegionGrowingUpperThreshold(value);
    m_SegmentingDataPipe->SetRegionGrowingUpperThreshold(value);
  }
}
//-----------------------------------------------------------------------
void mafVMESegmentationVolume::SetAutomaticSegmentationThresholdModality(int modality)
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
int mafVMESegmentationVolume::GetAutomaticSegmentationThresholdModality()
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetAutomaticSegmentationThresholdModality();
}

//-----------------------------------------------------------------------
void mafVMESegmentationVolume::SetDoubleThresholdModality(int modality)
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
int mafVMESegmentationVolume::GetDoubleThresholdModality()
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetDoubleThresholdModality();
}
//-----------------------------------------------------------------------
void mafVMESegmentationVolume::SetAutomaticSegmentationGlobalThreshold(double lowerThreshold, double uppperThreshold)
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
double mafVMESegmentationVolume::GetAutomaticSegmentationGlobalThreshold()
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetAutomaticSegmentationGlobalThreshold();
}

//-----------------------------------------------------------------------
double mafVMESegmentationVolume::GetAutomaticSegmentationGlobalUpperThreshold()
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetAutomaticSegmentationGlobalUpperThreshold();
}

//------------------------------------------------------------------------
void mafVMESegmentationVolume::SetRegionGrowingSliceRange(int startSlice, int endSlice)
//------------------------------------------------------------------------
{
  m_SegmentingDataPipe->SetRegionGrowingSliceRange(startSlice, endSlice);
}

//------------------------------------------------------------------------
int mafVMESegmentationVolume::GetRegionGrowingSliceRange(int &startSlice, int &endSlice)
//------------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetRegionGrowingSliceRange(startSlice, endSlice);
}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMESegmentationVolume::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputVolume::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
char** mafVMESegmentationVolume::GetIcon() 
//-------------------------------------------------------------------------
{
#include "mafVMEVolume.xpm"
  return mafVMEVolume_xpm;
}
