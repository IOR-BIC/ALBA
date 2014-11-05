/*=========================================================================

 Program: MAF2Medical
 Module: medVMESegmentationVolume
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medVMESegmentationVolume.h"
#include "mafGUI.h"
#include "mmaVolumeMaterial.h"

#include "mafTransform.h"
#include "mafStorageElement.h"
#include "medDataPipeCustomSegmentationVolume.h"
#include "mafVMEOutputVolume.h"
#include "mafMatrix.h"
#include "medAttributeSegmentationVolume.h"


#include "vtkMAFSmartPointer.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMESegmentationVolume)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
medVMESegmentationVolume::medVMESegmentationVolume()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputVolume *output = mafVMEOutputVolume::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  //DependsOnLinkedNodeOn();

  m_VolumeAttribute = NULL;

  // attach a data pipe which creates a bridge between VTK and MAF
  m_SegmentingDataPipe = medDataPipeCustomSegmentationVolume::New();
  SetDataPipe(m_SegmentingDataPipe);
}

//-------------------------------------------------------------------------
medVMESegmentationVolume::~medVMESegmentationVolume()
//-------------------------------------------------------------------------
{
  // these links are children, thus it's not our responsibility to
  // destroy them, it's part of the vtkTree one's
  mafDEL(m_Transform);
  SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaVolumeMaterial *medVMESegmentationVolume::GetMaterial()
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
medAttributeSegmentationVolume *medVMESegmentationVolume::GetVolumeAttribute()
//-------------------------------------------------------------------------
{
  if (this->GetAttribute("SegmentationVolumeData") != NULL)
  {
    m_VolumeAttribute = medAttributeSegmentationVolume::SafeDownCast(this->GetAttribute("SegmentationVolumeData"));
  }
  else
  {
    m_VolumeAttribute = medAttributeSegmentationVolume::New();
    this->SetAttribute("SegmentationVolumeData",m_VolumeAttribute);

    m_VolumeAttribute = medAttributeSegmentationVolume::SafeDownCast(this->GetAttribute("SegmentationVolumeData"));
  }

  return m_VolumeAttribute;

}
//-------------------------------------------------------------------------
int medVMESegmentationVolume::InternalInitialize()
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
void medVMESegmentationVolume::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink());
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
int medVMESegmentationVolume::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    ((medVMESegmentationVolume *)a)->SetVolumeLink(this->GetVolumeLink());
    ((medVMESegmentationVolume *)a)->SetManualVolumeMask(this->GetManualVolumeMask());
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool medVMESegmentationVolume::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = true;
    if (((medVMESegmentationVolume *)vme)->GetVolumeLink() != this->GetVolumeLink())
    {
      ret = false;
    }
    if (((medVMESegmentationVolume *)vme)->GetManualVolumeMask() != this->GetManualVolumeMask())
    {
      ret = false;
    }
    
  }
  return ret;
}

//-------------------------------------------------------------------------
void medVMESegmentationVolume::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void medVMESegmentationVolume::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-------------------------------------------------------------------------
bool medVMESegmentationVolume::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool medVMESegmentationVolume::IsDataAvailable()
//-------------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink());
  return (vol && vol->IsDataAvailable());
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::InternalStore(mafStorageElement *parent)
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
int medVMESegmentationVolume::InternalRestore(mafStorageElement *node)
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
mafGUI* medVMESegmentationVolume::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->Divider();
  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink());
  m_VolumeName = vol ? vol->GetName() : _("none");
  m_Gui->Button(ID_VOLUME_LINK,&m_VolumeName,_("Volume"), _("Select the volume to be segmented"));

  return m_Gui;
}
//-------------------------------------------------------------------------
void medVMESegmentationVolume::OnEvent(mafEventBase *maf_event)
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
        e->SetArg((long)&medVMESegmentationVolume::VolumeAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
        if (n != NULL)
        {
          SetVolumeLink(n);
          m_VolumeName = n->GetName();
          m_Gui->Update();
        }
      }
      break;
    default:
      mafNode::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
int medVMESegmentationVolume::AddRange(int startSlice,int endSlice,double threshold, double upperThreshold)
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
int medVMESegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold, double &upperThreshold)
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetRange(index,startSlice,endSlice,threshold, upperThreshold );
}

//-----------------------------------------------------------------------
int medVMESegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold)
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetRange(index,startSlice,endSlice,threshold );
}


//-----------------------------------------------------------------------
bool medVMESegmentationVolume::CheckNumberOfThresholds()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->CheckNumberOfThresholds();
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::UpdateRange(int index,int startSlice, int endSlice, double threshold, double upperThershold)
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
int medVMESegmentationVolume::RemoveAllRanges()
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
int medVMESegmentationVolume::RemoveAllSeeds()
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
int medVMESegmentationVolume::DeleteSeed(int index)
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
int medVMESegmentationVolume::DeleteRange(int index)
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
int medVMESegmentationVolume::SetVolumeLink(mafNode *volume)
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
void medVMESegmentationVolume::SetManualVolumeMask(mafNode *volume)
//-----------------------------------------------------------------------
{
  SetLink("ManualVolumeMask", volume);
  m_SegmentingDataPipe->SetManualVolumeMask(volume);
  Modified();
}
//-----------------------------------------------------------------------
mafNode *medVMESegmentationVolume::GetManualVolumeMask()
//-----------------------------------------------------------------------
{
  return GetLink("ManualVolumeMask");
}
//-----------------------------------------------------------------------
void medVMESegmentationVolume::SetRefinementVolumeMask(mafNode *volume)
//-----------------------------------------------------------------------
{
  SetLink("RefinementVolumeMask", volume);
  m_SegmentingDataPipe->SetRefinementVolumeMask(volume);
  Modified();
}
//-----------------------------------------------------------------------
mafNode *medVMESegmentationVolume::GetRefinementVolumeMask()
//-----------------------------------------------------------------------
{
  return GetLink("RefinementVolumeMask");
}
//-----------------------------------------------------------------------
vtkDataSet *medVMESegmentationVolume::GetAutomaticOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetAutomaticOutput();
}
//-----------------------------------------------------------------------
vtkDataSet *medVMESegmentationVolume::GetRegionGrowingOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetRegionGrowingOutput();
}
//-----------------------------------------------------------------------
vtkDataSet *medVMESegmentationVolume::GetRefinementOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetRefinementOutput();
}
//-----------------------------------------------------------------------
vtkDataSet *medVMESegmentationVolume::GetManualOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetManualOutput();
}
//-----------------------------------------------------------------------
mafNode *medVMESegmentationVolume::GetVolumeLink()
//-----------------------------------------------------------------------
{
  return GetLink("Volume");
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::AddSeed(int seed[3])
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
int medVMESegmentationVolume::GetSeed(int index,int seed[3])
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetSeed(index,seed);
}
//-------------------------------------------------------------------------
double medVMESegmentationVolume::GetRegionGrowingLowerThreshold()
//-------------------------------------------------------------------------
{
  return m_VolumeAttribute->GetRegionGrowingLowerThreshold();
}
//-------------------------------------------------------------------------
double medVMESegmentationVolume::GetRegionGrowingUpperThreshold()
//-------------------------------------------------------------------------
{
  return m_VolumeAttribute->GetRegionGrowingUpperThreshold();
}
//-------------------------------------------------------------------------
int medVMESegmentationVolume::GetNumberOfRanges()
//-------------------------------------------------------------------------
{
  return m_VolumeAttribute->GetNumberOfRanges();
}
//-------------------------------------------------------------------------
int medVMESegmentationVolume::GetNumberOfSeeds()
//-------------------------------------------------------------------------
{
  return m_VolumeAttribute->GetNumberOfSeeds();
}
//-------------------------------------------------------------------------
void medVMESegmentationVolume::SetRegionGrowingLowerThreshold(double value)
//-------------------------------------------------------------------------
{
  if (m_VolumeAttribute && m_SegmentingDataPipe)
  {
	  m_VolumeAttribute->SetRegionGrowingLowerThreshold(value);
	  m_SegmentingDataPipe->SetRegionGrowingLowerThreshold(value);
  }
}
//-------------------------------------------------------------------------
void medVMESegmentationVolume::SetRegionGrowingUpperThreshold(double value)
//-------------------------------------------------------------------------
{
  if (m_VolumeAttribute && m_SegmentingDataPipe)
  {
    m_VolumeAttribute->SetRegionGrowingUpperThreshold(value);
    m_SegmentingDataPipe->SetRegionGrowingUpperThreshold(value);
  }
}
//-----------------------------------------------------------------------
void medVMESegmentationVolume::SetAutomaticSegmentationThresholdModality(int modality)
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
int medVMESegmentationVolume::GetAutomaticSegmentationThresholdModality()
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetAutomaticSegmentationThresholdModality();
}

//-----------------------------------------------------------------------
void medVMESegmentationVolume::SetDoubleThresholdModality(int modality)
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
int medVMESegmentationVolume::GetDoubleThresholdModality()
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetDoubleThresholdModality();
}
//-----------------------------------------------------------------------
void medVMESegmentationVolume::SetAutomaticSegmentationGlobalThreshold(double lowerThreshold, double uppperThreshold)
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
double medVMESegmentationVolume::GetAutomaticSegmentationGlobalThreshold()
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetAutomaticSegmentationGlobalThreshold();
}

//-----------------------------------------------------------------------
double medVMESegmentationVolume::GetAutomaticSegmentationGlobalUpperThreshold()
//-----------------------------------------------------------------------
{
  return m_VolumeAttribute->GetAutomaticSegmentationGlobalUpperThreshold();
}

//------------------------------------------------------------------------
void medVMESegmentationVolume::SetRegionGrowingSliceRange(int startSlice, int endSlice)
//------------------------------------------------------------------------
{
  m_SegmentingDataPipe->SetRegionGrowingSliceRange(startSlice, endSlice);
}

//------------------------------------------------------------------------
int medVMESegmentationVolume::GetRegionGrowingSliceRange(int &startSlice, int &endSlice)
//------------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetRegionGrowingSliceRange(startSlice, endSlice);
}

//-------------------------------------------------------------------------
mafVMEOutput *medVMESegmentationVolume::GetOutput()
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
char** medVMESegmentationVolume::GetIcon() 
//-------------------------------------------------------------------------
{
#include "mafVMEVolume.xpm"
  return mafVMEVolume_xpm;
}
