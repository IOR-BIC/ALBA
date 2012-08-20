/*=========================================================================

 Program: MAF2Medical
 Module: medAttributeSegmentationVolume
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

#include "medAttributeSegmentationVolume.h"
#include "medDecl.h"
#include "mafEvent.h"

#include "mafStorageElement.h"
#include "mafIndent.h"
#include "medVMESegmentationVolume.h"

using namespace std;

//----------------------------------------------------------------------------
mafCxxTypeMacro(medAttributeSegmentationVolume)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medAttributeSegmentationVolume::medAttributeSegmentationVolume()
//----------------------------------------------------------------------------
{  
  m_Name = "SegmentationVolumeData";

  m_AutomaticSegmentationThresholdModality = medVMESegmentationVolume::GLOBAL;
  m_AutomaticSegmentationGlobalThreshold = 0.0;
  m_AutomaticSegmentationGlobalUpperThreshold = 0.0;

  m_UseDoubleThreshold=0;

  m_RegionGrowingLowerThreshold = m_RegionGrowingUpperThreshold = 0.0;
}
//----------------------------------------------------------------------------
medAttributeSegmentationVolume::~medAttributeSegmentationVolume()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    //delete ranges before vector clear
    delete []m_AutomaticSegmentationRanges[i];
  }
  m_AutomaticSegmentationRanges.clear();

  for (int i=0;i<m_RegionGrowingSeeds.size();i++)
  {
    //delete seed before vector clear
    delete []m_RegionGrowingSeeds[i];
  }
  m_RegionGrowingSeeds.clear();
}
//-------------------------------------------------------------------------
void medAttributeSegmentationVolume::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{ 
  if (!a->IsA("medAttributeSegmentationVolume"))
  {
  	return;
  }

  Superclass::DeepCopy(a);

  //////////////////////////////////////////////////////////////////////////
 	m_AutomaticSegmentationThresholdModality = ((medAttributeSegmentationVolume*)a)->GetAutomaticSegmentationThresholdModality();
  m_UseDoubleThreshold= ((medAttributeSegmentationVolume*)a)->GetDoubleThresholdModality();
  m_AutomaticSegmentationGlobalThreshold = ((medAttributeSegmentationVolume*)a)->GetAutomaticSegmentationGlobalThreshold();
  m_AutomaticSegmentationGlobalUpperThreshold = ((medAttributeSegmentationVolume*)a)->GetAutomaticSegmentationGlobalUpperThreshold();
  for (int i=0;i<((medAttributeSegmentationVolume*)a)->GetNumberOfRanges();i++)
  {
    int startSlice,endSlice;
    double threshold;
    ((medAttributeSegmentationVolume*)a)->GetRange(i,startSlice,endSlice,threshold);
    this->AddRange(startSlice,endSlice,threshold);
  }
  //////////////////////////////////////////////////////////////////////////
  m_RegionGrowingLowerThreshold = ((medAttributeSegmentationVolume*)a)->GetRegionGrowingLowerThreshold();
  m_RegionGrowingUpperThreshold = ((medAttributeSegmentationVolume*)a)->GetRegionGrowingUpperThreshold();
  for (int i=0;i<((medAttributeSegmentationVolume*)a)->GetNumberOfSeeds();i++)
  {
    int seed[3];
    ((medAttributeSegmentationVolume*)a)->GetSeed(i,seed);
    this->AddSeed(seed);
  }
  //////////////////////////////////////////////////////////////////////////
}
//----------------------------------------------------------------------------
bool medAttributeSegmentationVolume::Equals(const mafAttribute *a)
//----------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
    if ( (m_AutomaticSegmentationThresholdModality != ((medAttributeSegmentationVolume*)a)->GetAutomaticSegmentationThresholdModality()) ||
         (m_UseDoubleThreshold != ((medAttributeSegmentationVolume*)a)->GetDoubleThresholdModality()) ||
         (m_AutomaticSegmentationGlobalThreshold != ((medAttributeSegmentationVolume*)a)->GetAutomaticSegmentationGlobalThreshold()) ||
         (m_AutomaticSegmentationGlobalUpperThreshold != ((medAttributeSegmentationVolume*)a)->GetAutomaticSegmentationGlobalUpperThreshold() && m_UseDoubleThreshold) ||
         (m_AutomaticSegmentationRanges.size() != ((medAttributeSegmentationVolume*)a)->GetNumberOfRanges()) ||
         (m_RegionGrowingLowerThreshold != ((medAttributeSegmentationVolume*)a)->GetRegionGrowingLowerThreshold()) ||
         (m_RegionGrowingUpperThreshold != ((medAttributeSegmentationVolume*)a)->GetRegionGrowingUpperThreshold()) ||
         (m_RegionGrowingSeeds.size() != ((medAttributeSegmentationVolume*)a)->GetNumberOfSeeds()) )
    {
      return false;
    }

    for (int i=0;i<((medAttributeSegmentationVolume*)a)->GetNumberOfRanges();i++)
    {
      int startSlice,endSlice;
      double threshold;
      ((medAttributeSegmentationVolume*)a)->GetRange(i,startSlice,endSlice,threshold);
      
      if (startSlice != m_AutomaticSegmentationRanges[i][0] || endSlice!= m_AutomaticSegmentationRanges[i][1] || threshold != m_AutomaticSegmentationThresholds[i])
        return false;
    }

    
    for (int i=0;i<((medAttributeSegmentationVolume*)a)->GetNumberOfSeeds();i++)
    {
      int seed[3];
      ((medAttributeSegmentationVolume*)a)->GetSeed(i,seed);

      if (seed[0] != m_RegionGrowingSeeds[i][0] || seed[1]!= m_RegionGrowingSeeds[i][1] || seed[2] != m_RegionGrowingSeeds[i][2])
        return false;
    }

    return true;
  }
  return false;
}

//-----------------------------------------------------------------------
int medAttributeSegmentationVolume::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    //////////////////////////////////////////////////////////////////////////
    mafString value = "AUTOMATIC_SEGMENTATION_THRESHOLD_MODALITY";
    int valueInt = m_AutomaticSegmentationThresholdModality;
    parent->StoreInteger(value,valueInt);
    //////////////////////////////////////////////////////////////////////////
    value = "USE_DOUBLE_THRESHOLD";
    valueInt = m_UseDoubleThreshold;
    parent->StoreInteger(value,valueInt);
    //////////////////////////////////////////////////////////////////////////
    value = "AUTOMATIC_SEGMENTATION_GLOBAL_THRESHOLD";
    double valueDouble = m_AutomaticSegmentationGlobalThreshold;
    parent->StoreDouble(value,valueDouble);
    //////////////////////////////////////////////////////////////////////////
    value = "AUTOMATIC_SEGMENTATION_GLOBAL_UPPER_THRESHOLD";
    valueDouble = m_AutomaticSegmentationGlobalUpperThreshold;
    parent->StoreDouble(value,valueDouble);
    //////////////////////////////////////////////////////////////////////////
    value = "NUM_OF_RANGES";
    parent->StoreInteger(value,m_AutomaticSegmentationRanges.size());
    for (int i=0;i<m_AutomaticSegmentationRanges.size();i++)
    {
      value = "RANGE_";
      value<<i;
      parent->StoreVectorN(value,m_AutomaticSegmentationRanges[i],2);
      value = "THRESHOLD_";
      value<<i;
      parent->StoreDouble(value,m_AutomaticSegmentationThresholds[i]);
      value = "UPPER_THRESHOLD_";
      value<<i;
      parent->StoreDouble(value,m_AutomaticSegmentationUpperThresholds[i]);

    }
    //////////////////////////////////////////////////////////////////////////
    value = "REGION_GROWING_UPPER_THRESHOLD";
    valueDouble = m_RegionGrowingUpperThreshold;
    parent->StoreDouble(value,valueDouble);
    //////////////////////////////////////////////////////////////////////////
    value = "REGION_GROWING_LOWER_THRESHOLD";
    valueDouble = m_RegionGrowingLowerThreshold;
    parent->StoreDouble(value,valueDouble);
    //////////////////////////////////////////////////////////////////////////
    value = "NUM_OF_SEEDS";
    parent->StoreInteger(value,m_RegionGrowingSeeds.size());
    for (int i=0;i<m_RegionGrowingSeeds.size();i++)
    {
      value = "SEED_";
      value<<i;
      parent->StoreVectorN(value,m_RegionGrowingSeeds[i],3);
    }
    //////////////////////////////////////////////////////////////////////////

    return MAF_OK;
  }
  return MAF_ERROR;
}
//----------------------------------------------------------------------------
int medAttributeSegmentationVolume::InternalRestore(mafStorageElement *node)
//----------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == MAF_OK)
  {
    //////////////////////////////////////////////////////////////////////////
    mafString value = "AUTOMATIC_SEGMENTATION_THRESHOLD_MODALITY";
    node->RestoreInteger(value,m_AutomaticSegmentationThresholdModality);
    
    value = "USE_DOUBLE_THRESHOLD";
    if(node->RestoreInteger(value,m_UseDoubleThreshold) == MAF_ERROR)
    {
      mafLogMessage("Old file version loaded  for retro-compatility please save it again");
      m_UseDoubleThreshold=0;
    }
    
    //////////////////////////////////////////////////////////////////////////
    value = "AUTOMATIC_SEGMENTATION_GLOBAL_THRESHOLD";
    node->RestoreDouble(value,m_AutomaticSegmentationGlobalThreshold);
    //////////////////////////////////////////////////////////////////////////
    if (m_UseDoubleThreshold)
    {
      value = "AUTOMATIC_SEGMENTATION_GLOBAL_UPPER_THRESHOLD";
      node->RestoreDouble(value,m_AutomaticSegmentationGlobalUpperThreshold);
    }
    //////////////////////////////////////////////////////////////////////////
    int numOfRanges;
    value = "NUM_OF_RANGES";
    node->RestoreInteger(value,numOfRanges);
    for (int i=0;i<numOfRanges;i++)
    {
      int *range = new int[2];
      double threshold,upperThreshold;
      value = "RANGE_";
      value<<i;
      node->RestoreVectorN(value,range,2);
      m_AutomaticSegmentationRanges.push_back(range);
      value = "THRESHOLD_";
      value<<i;
      node->RestoreDouble(value,threshold);
      //do not load upper threshold m_UseDoubleThreshold is false 
      //in this mode retro-compatibility is guaranteed
      if (m_UseDoubleThreshold)
      {
        value = "UPPER_THRESHOLD_";
        value<<i;
        node->RestoreDouble(value,upperThreshold);
      }
      else upperThreshold=0;

      m_AutomaticSegmentationThresholds.push_back(threshold);
    }
    //////////////////////////////////////////////////////////////////////////
    value = "REGION_GROWING_UPPER_THRESHOLD";
    node->RestoreDouble(value,m_RegionGrowingUpperThreshold);
    //////////////////////////////////////////////////////////////////////////
    value = "REGION_GROWING_LOWER_THRESHOLD";
    node->RestoreDouble(value,m_RegionGrowingLowerThreshold);
    //////////////////////////////////////////////////////////////////////////
    int numOfSeeds;
    value = "NUM_OF_SEEDS";
    node->RestoreInteger(value,numOfSeeds);
    for (int i=0;i<numOfSeeds;i++)
    {
      int *seed = new int[3];
      value = "SEED_";
      value<<i;
      node->RestoreVectorN(value,seed,3);
      m_RegionGrowingSeeds.push_back(seed);
    }
    //////////////////////////////////////////////////////////////////////////

    return MAF_OK;
  }
  return MAF_ERROR;
}
//----------------------------------------------------------------------------
int medAttributeSegmentationVolume::AddRange(int startSlice,int endSlice,double threshold, double upperThreshold)
//----------------------------------------------------------------------------
{
  //on addrange you push back threshold in m_AutomaticSegmentationThresholds and m_AutomaticSegmentationUpperThresholds
  //if you are in single threshold modality m_AutomaticSegmentationUpperThresholds will be ignored
  int *range = new int[2];
  range[0] = startSlice;
  range[1] = endSlice;
  m_AutomaticSegmentationRanges.push_back(range);
  m_AutomaticSegmentationThresholds.push_back(threshold);
  m_AutomaticSegmentationUpperThresholds.push_back(upperThreshold);

  return MAF_OK;
}
//----------------------------------------------------------------------------
int medAttributeSegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold)
//----------------------------------------------------------------------------
{
  //use this function in single threshold mode
  int nRanges = m_AutomaticSegmentationRanges.size();
  if (index<0 || index>(nRanges-1))
  {
    return MAF_ERROR;
  }

  startSlice = m_AutomaticSegmentationRanges[index][0];
  endSlice = m_AutomaticSegmentationRanges[index][1];
  threshold = m_AutomaticSegmentationThresholds[index];

  return MAF_OK;
}
//----------------------------------------------------------------------------
int medAttributeSegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold, double &upperThreshold)
//----------------------------------------------------------------------------
{
  //use this function in double threshold mode
  int nRanges = m_AutomaticSegmentationRanges.size();
  if (index<0 || index>(nRanges-1))
  {
    return MAF_ERROR;
  }

  startSlice = m_AutomaticSegmentationRanges[index][0];
  endSlice = m_AutomaticSegmentationRanges[index][1];
  threshold = m_AutomaticSegmentationThresholds[index];
  upperThreshold = m_AutomaticSegmentationUpperThresholds[index];

  return MAF_OK;
}
//----------------------------------------------------------------------------
int medAttributeSegmentationVolume::UpdateRange(int index,int startSlice, int endSlice, double threshold, double upperThreshold)
//----------------------------------------------------------------------------
{
  //on addrange you push back threshold in m_AutomaticSegmentationThresholds and m_AutomaticSegmentationUpperThresholds
  //if you are in single threshold modality m_AutomaticSegmentationUpperThresholds will be ignored
  int nRanges = m_AutomaticSegmentationRanges.size();
  if (index<0 || index>(nRanges-1))
  {
    return MAF_ERROR;
  }
  m_AutomaticSegmentationRanges[index][0] = startSlice;
  m_AutomaticSegmentationRanges[index][1] = endSlice;
  m_AutomaticSegmentationThresholds[index] = threshold;
  m_AutomaticSegmentationUpperThresholds[index] = upperThreshold;

  return MAF_OK;
}

//----------------------------------------------------------------------------
int medAttributeSegmentationVolume::RemoveAllRanges()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    //delete ranges before vector clear
    delete []m_AutomaticSegmentationRanges[i];
  }
  m_AutomaticSegmentationRanges.clear();
  m_AutomaticSegmentationThresholds.clear();
  m_AutomaticSegmentationUpperThresholds.clear();

  return MAF_OK;
}
//----------------------------------------------------------------------------
int medAttributeSegmentationVolume::RemoveAllSeeds()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_RegionGrowingSeeds.size();i++)
  {
    //delete seeds before vector clean
    delete []m_RegionGrowingSeeds[i];
  }
  m_RegionGrowingSeeds.clear();

  return MAF_OK;
}
//----------------------------------------------------------------------------
int medAttributeSegmentationVolume::DeleteSeed(int index)
//----------------------------------------------------------------------------
{
  int nSeeds = m_RegionGrowingSeeds.size();
  if (index<0 || index>(nSeeds-1))
  {
    return MAF_ERROR;
  }

  for (int i=0,j=0;i<m_RegionGrowingSeeds.size();i++)
  {
    //all seeds from index will be shifted left of one position
    if (i != index)
    {
      m_RegionGrowingSeeds[j][0] = m_RegionGrowingSeeds[i][0];
      m_RegionGrowingSeeds[j][1] = m_RegionGrowingSeeds[i][1];
      m_RegionGrowingSeeds[j][2] = m_RegionGrowingSeeds[i][2];
      j++;
    }
  }

  //delete last seed
  delete []m_RegionGrowingSeeds[m_RegionGrowingSeeds.size()-1];
  m_RegionGrowingSeeds.pop_back();

  return MAF_OK;
}
//----------------------------------------------------------------------------
int medAttributeSegmentationVolume::DeleteRange(int index)
//----------------------------------------------------------------------------
{
  int nRanges = m_AutomaticSegmentationRanges.size();
  if (index<0 || index>(nRanges-1))
  {
    return MAF_ERROR;
  }

  for (int i=0,j=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    //all ranges from index will be shifted left of one position
    if (i != index)
    {
      m_AutomaticSegmentationRanges[j][0] = m_AutomaticSegmentationRanges[i][0];
      m_AutomaticSegmentationRanges[j][1] = m_AutomaticSegmentationRanges[i][1];
      m_AutomaticSegmentationThresholds[j] = m_AutomaticSegmentationThresholds[i];
      m_AutomaticSegmentationUpperThresholds[j] = m_AutomaticSegmentationUpperThresholds[i];
      j++;
    }
  }

  //delete last range
  delete []m_AutomaticSegmentationRanges[m_AutomaticSegmentationRanges.size()-1];
  m_AutomaticSegmentationRanges.pop_back();
  m_AutomaticSegmentationThresholds.pop_back();
  m_AutomaticSegmentationUpperThresholds.pop_back();

  return MAF_OK;
}
//----------------------------------------------------------------------------
int medAttributeSegmentationVolume::AddSeed(int seed[3])
//----------------------------------------------------------------------------
{
  int *newSeed = new int[3];
  newSeed[0] = seed[0];
  newSeed[1] = seed[1];
  newSeed[2] = seed[2];
  m_RegionGrowingSeeds.push_back(newSeed);

  return MAF_OK;
}
//----------------------------------------------------------------------------
int medAttributeSegmentationVolume::GetSeed(int index,int seed[3])
//----------------------------------------------------------------------------
{
  int nSeeds = m_RegionGrowingSeeds.size();
  if (index<0 || index>(nSeeds-1))
  {
    return MAF_ERROR;
  }
  seed[0] = m_RegionGrowingSeeds[index][0];
  seed[1] = m_RegionGrowingSeeds[index][1];
  seed[2] = m_RegionGrowingSeeds[index][2];

  return MAF_OK;
}
//-----------------------------------------------------------------------
void medAttributeSegmentationVolume::Print(std::ostream& os, const int tabs) const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
  
  //////////////////////////////////////////////////////////////////////////
  os << indent << "SegmentationVolumeData:"<<std::endl;
  if (m_AutomaticSegmentationThresholdModality == medVMESegmentationVolume::GLOBAL)
  {
    os << indent << indent << "AutomaticSegmentationThresholdModality: GLOBAL"<<std::endl;
  }
  else
  {
    os << indent << indent << "AutomaticSegmentationThresholdModality: RANGE"<<std::endl;
  }
  //////////////////////////////////////////////////////////////////////////
  if (m_UseDoubleThreshold)
  {
    os << indent << indent << "Double threshold active"<<std::endl;
  }
  else
  {
    os << indent << indent << "Single threshold active"<<std::endl;
  }
  //////////////////////////////////////////////////////////////////////////
  os << indent << indent << "Global Threshold: "<<m_AutomaticSegmentationGlobalThreshold<<std::endl;
  //////////////////////////////////////////////////////////////////////////
  for (int i=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    os << indent << indent << "Range " << i+1 << "°:";
    os << indent << indent << indent << "Start Slice " << m_AutomaticSegmentationRanges[i][0]<<std::endl;
    os << indent << indent << indent << "End Slice " << m_AutomaticSegmentationRanges[i][1]<<std::endl;
    //show the second threshold only if m_UseDoubleThreshold 
    if (m_UseDoubleThreshold)
    {
      os << indent << indent << indent << "Threshold " << m_AutomaticSegmentationThresholds[i]<<std::endl;
    }
  }
  //////////////////////////////////////////////////////////////////////////
  os << indent << indent << "Region Growing Upper Threshold: "<<m_RegionGrowingUpperThreshold<<std::endl;
  os << indent << indent << "Region Growing Lower Threshold: "<<m_RegionGrowingLowerThreshold<<std::endl;
  //////////////////////////////////////////////////////////////////////////
  for (int i=0;i<m_RegionGrowingSeeds.size();i++)
  {
    os << indent << indent << "Seed " << i+1 << "°:";
    os << indent << indent << indent << "x " << m_RegionGrowingSeeds[i][0]<<std::endl;
    os << indent << indent << indent << "y " << m_RegionGrowingSeeds[i][1]<<std::endl;
    os << indent << indent << indent << "z " << m_RegionGrowingSeeds[i][2]<<std::endl;
  }
  //////////////////////////////////////////////////////////////////////////
}
