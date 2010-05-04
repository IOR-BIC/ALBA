/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medAttributeSegmentationVolume.h,v $
Language:  C++
Date:      $Date: 2010-05-04 15:54:58 $
Version:   $Revision: 1.1.2.3 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2010
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __medAttributeSegmentationVolume_H__
#define __medAttributeSegmentationVolume_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafAttribute.h"
#include <vector>

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// medAttributeSegmentationVolume:
//----------------------------------------------------------------------------
/**  */  
class MAF_EXPORT medAttributeSegmentationVolume : public mafAttribute
{
public:
  medAttributeSegmentationVolume();
  virtual ~medAttributeSegmentationVolume();

  mafTypeMacro(medAttributeSegmentationVolume, mafAttribute);

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

  /** Set the value to use during a global threshold */
  void SetAutomaticSegmentationGlobalThreshold(double threshold){m_AutomaticSegmentationGlobalThreshold = threshold;};

  /** Return the value to use during a global threshold */
  double GetAutomaticSegmentationGlobalThreshold(){return m_AutomaticSegmentationGlobalThreshold;};

  /** Add a new range with the threshold value */
  int AddRange(int startSlice,int endSlice,double threshold);

  /** Return the value of the range of the position index */
  int GetRange(int index,int &startSlice, int &endSlice, double &threshold);

  /** Update the value of the range of the position index - return MAF_ERROR if the index isn't correct*/
  int UpdateRange(int index,int &startSlice, int &endSlice, double &threshold);

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

protected:
  /*virtual*/ int InternalStore(mafStorageElement *parent);
  /*virtual*/ int InternalRestore(mafStorageElement *node);

  //Stuff for automatic threshold
  int m_AutomaticSegmentationThresholdModality;
  double m_AutomaticSegmentationGlobalThreshold;
  std::vector<int*> m_AutomaticSegmentationRanges;
  std::vector<double> m_AutomaticSegmentationThresholds;
  //////////////////////////////////////////////////////////////////////////
  
  //Stuff for region growing
  double m_RegionGrowingUpperThreshold;
  double m_RegionGrowingLowerThreshold;
  std::vector<int *> m_RegionGrowingSeeds;
  //////////////////////////////////////////////////////////////////////////

};
#endif
