/*=========================================================================

 Program: MAF2
 Module: mafAttributeSegmentationVolume
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafAttributeSegmentationVolume_H__
#define __mafAttributeSegmentationVolume_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafAttribute.h"
#include <vector>

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafAttributeSegmentationVolume:
//----------------------------------------------------------------------------
/**  */  
class MAF_EXPORT mafAttributeSegmentationVolume : public mafAttribute
{
public:
  /** constructor. */
  mafAttributeSegmentationVolume();
  /** destructor. */
  virtual ~mafAttributeSegmentationVolume();

  mafTypeMacro(mafAttributeSegmentationVolume, mafAttribute);

  /** Print a dump of this object */
  /*virtual*/ void Print(std::ostream& os, const int tabs=0) const;

  /** Copy the contents of another Meter attribute into this one. */
  /*virtual*/ void DeepCopy(const mafAttribute *a);

  /** Compare with another Meter attribute. */
  /*virtual*/ bool Equals(const mafAttribute *a);


  /** Set the threshold modality for automatic segmentation: GLOBAL or RANGE */
  void SetAutomaticSegmentationThresholdModality(int modality){m_AutomaticSegmentationThresholdModality = modality;};

  /** Return the threshold modality for automatic segmentation: GLOBAL or RANGE */
  int GetAutomaticSegmentationThresholdModality(){return m_AutomaticSegmentationThresholdModality;};

  /** Set Threshold filtering to double Threshold if modality is true */
  void SetDoubleThresholdModality(int modality){m_UseDoubleThreshold = modality;};

  /** Return Threshold filtering modality, true if double Threshold is enabled, on modality changes all ranges will be remove */
  int GetDoubleThresholdModality(){return m_UseDoubleThreshold;};

  /** Set the value to use during a global threshold */
  void SetAutomaticSegmentationGlobalThreshold(double lowerThreshold, double uppperThreshold=0){m_AutomaticSegmentationGlobalThreshold = lowerThreshold; m_AutomaticSegmentationGlobalUpperThreshold = uppperThreshold;};

  /** Return the value to use during a global threshold */
  double GetAutomaticSegmentationGlobalThreshold(){ return m_AutomaticSegmentationGlobalThreshold; };

  /** Return the value to use during a global threshold */
  double GetAutomaticSegmentationGlobalUpperThreshold(){ return m_AutomaticSegmentationGlobalUpperThreshold; };

  /** Add a new range with the threshold value */
  int AddRange(int startSlice,int endSlice,double threshold, double upperThreshold=0);

  /** Return the value of the range of the position index */
  int GetRange(int index,int &startSlice, int &endSlice, double &threshold);

  /** Return the value of the range of the position index */
  int GetRange(int index,int &startSlice, int &endSlice, double &threshold, double &upperThreshold);


  /** Update the value of the range of the position index - return MAF_ERROR if the index isn't correct*/
  int UpdateRange(int index,int startSlice, int endSlice, double threshold, double upperThershold=0);

  /** Delete the range of the position index */
  int DeleteRange(int index);

  /** Remove all ranges */
  int RemoveAllRanges();

  /** Return the number of ranges stored */
  int GetNumberOfRanges(){return m_AutomaticSegmentationRanges.size();};

  /** Return the region growing upper threshold */
  double GetRegionGrowingUpperThreshold(){return m_RegionGrowingUpperThreshold;};

  /** Return the region growing lower threshold */
  double GetRegionGrowingLowerThreshold(){return m_RegionGrowingLowerThreshold;};

  /** Set the region growing upper threshold */
  void SetRegionGrowingUpperThreshold(double value){m_RegionGrowingUpperThreshold = value;};

  /** Set the region growing lower threshold */
  void SetRegionGrowingLowerThreshold(double value){m_RegionGrowingLowerThreshold = value;};

  /** Return the seed of the position index */
  int GetSeed(int index,int seed[3]);

  /** Add a new seed */
  int AddSeed(int seed[3]);

  /** Delete the seed of the position index */
  int DeleteSeed(int index);

  /** Return the number of seeds stored */
  int GetNumberOfSeeds(){return m_RegionGrowingSeeds.size();};

  /** Remove all seeds */
  int RemoveAllSeeds();

protected:
  //** Auto storing function */  
  /*virtual*/ int InternalStore(mafStorageElement *parent);
  //** Auto restoring function */  
  /*virtual*/ int InternalRestore(mafStorageElement *node);

  //Stuff for automatic threshold
  int m_UseDoubleThreshold;
  int m_AutomaticSegmentationThresholdModality;
  double m_AutomaticSegmentationGlobalThreshold;
  double m_AutomaticSegmentationGlobalUpperThreshold;
  std::vector<int*> m_AutomaticSegmentationRanges;
  std::vector<double> m_AutomaticSegmentationThresholds;
  std::vector<double> m_AutomaticSegmentationUpperThresholds;
  //////////////////////////////////////////////////////////////////////////
  
  //Stuff for region growing
  double m_RegionGrowingUpperThreshold;
  double m_RegionGrowingLowerThreshold;
  std::vector<int *> m_RegionGrowingSeeds;
  //////////////////////////////////////////////////////////////////////////

};
#endif
