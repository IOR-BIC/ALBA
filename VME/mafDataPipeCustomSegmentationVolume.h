/*=========================================================================

 Program: MAF2
 Module: mafDataPipeCustomSegmentationVolume
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafDataPipeCustomSegmentationVolume_h
#define __mafDataPipeCustomSegmentationVolume_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafDataPipeCustom.h"
#include <vector>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class vtkRectilinearGrid;
class vtkStructuredPoints;

/** */
class MAF_EXPORT mafDataPipeCustomSegmentationVolume : public mafDataPipeCustom
{
public:
  mafTypeMacro(mafDataPipeCustomSegmentationVolume,mafDataPipe);

  /** Make a copy of itself.*/
  int DeepCopy(mafDataPipe *pipe);

  /** Set the volume to segment*/
  int SetVolume(mafNode *volume);

  /** Return the segmented volume*/
  mafNode *GetVolume() {return m_Volume;};

  /** Set the threshold modality for automatic segmentation: GLOBAL or RANGE */
  void SetAutomaticSegmentationThresholdModality(int modality);

  /** Return the threshold modality for automatic segmentation: GLOBAL or RANGE */
  int GetAutomaticSegmentationThresholdModality(){return m_AutomaticSegmentationThresholdModality;};

  /** Set Threshold filtering to double Threshold if modality is true*/
  void SetDoubleThresholdModality(int modality);

  /** Return Threshold filtering modality, true if double Threshold is enabled */
  int GetDoubleThresholdModality(){return m_AutomaticSegmentationThresholdModality;};

  /** Add a new range with the threshold value */
  int AddRange(int startSlice,int endSlice,double threshold, double upperThershold=0);

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

  /** Set the value to use during a global threshold */
  void SetAutomaticSegmentationGlobalThreshold(double threshold, double upperThreshold=0);

  /** Return the value to use during a global threshold */
  double GetAutomaticSegmentationGlobalThreshold(){return m_AutomaticSegmentationGlobalThreshold;};

  /** Return the value to use during a global threshold */
  double GetAutomaticSegmentationGlobalUpperThreshold(){ return m_AutomaticSegmentationGlobalUpperThreshold; };

  /** Check if all thresholds exist */
  bool CheckNumberOfThresholds();

  /** Set the volume mask for the manual segmentation */
  void SetManualVolumeMask(mafNode *volume);

  /** Get the volume mask for the manual segmentation */
  mafNode *GetManualVolumeMask(){return m_ManualVolumeMask;};

  /** Set the volume mask for the refinement segmentation */
  void SetRefinementVolumeMask(mafNode *volume);

  /** Get the volume mask for the refinement segmentation */
  mafNode *GetRefinementVolumeMask();

  /** Return the vtkDataSet of the automatic segmentation */
  vtkDataSet *GetAutomaticOutput();

  /** Return the vtkDataSet of the manual segmentation */
  vtkDataSet *GetManualOutput();

  /** Return the vtkDataSet of the refinement segmentation */
  vtkDataSet *GetRefinementOutput();

  /** Return the vtkDataSet of the region growing segmentation */
  vtkDataSet *GetRegionGrowingOutput();

  /** Set the region growing upper threshold */
  void SetRegionGrowingUpperThreshold(double value);

  /** Set the region growing lower threshold */
  void SetRegionGrowingLowerThreshold(double value);

  /** Return the region growing upper threshold */
  double GetRegionGrowingUpperThreshold(){return m_RegionGrowingUpperThreshold;};

  /** Return the region growing lower threshold */
  double GetRegionGrowingLowerThreshold(){return m_RegionGrowingLowerThreshold;};

  /** Set the range of action of Region Growing algorithm */
  void SetRegionGrowingSliceRange(int startSlice, int endSlice);

  /** Return the range of action of Region Growing algorithm */
  int GetRegionGrowingSliceRange(int &startSlice, int &endSlice);

  /** Return the seed of the position index */
  int GetSeed(int index,int seed[3]);

  /** Add a new seed */
  int AddSeed(int seed[3]);

  /** Delete the seed of the position index */
  int DeleteSeed(int index);

  /** Remove all seeds */
  int RemoveAllSeeds();

  /** Return the number of seeds stored */
  int GetNumberOfSeeds(){return m_RegionGrowingSeeds.size();};

protected:
  /** constructor. */
  mafDataPipeCustomSegmentationVolume();
  /** destructor. */
  virtual ~mafDataPipeCustomSegmentationVolume();

  /** Check that the range has right values */
  int AutomaticCheckRange(int startSlice,int endSlice,int indexToExclude = -1);

  /** function called before of data pipe execution */
  /*virtual*/ void PreExecute();

  /** Apply the automatic segmentation */
  void ApplyAutomaticSegmentation();

  /** Apply the manual segmentation */
  void ApplyManualSegmentation();

  /** Apply the refinement segmentation */
  void ApplyRefinementSegmentation();

  /** Apply the region growing segmentation */
  void ApplyRegionGrowingSegmentation();

  /** function called to updated the data pipe output */
  /*virtual*/ void Execute();

  vtkRectilinearGrid *m_RG;
  vtkStructuredPoints *m_SP;

  vtkRectilinearGrid *m_AutomaticRG;
  vtkStructuredPoints *m_AutomaticSP;
  vtkRectilinearGrid *m_ManualRG;
  vtkStructuredPoints *m_ManualSP;
  vtkRectilinearGrid *m_RefinementRG;
  vtkStructuredPoints *m_RefinementSP;
  vtkRectilinearGrid *m_RegionGrowingRG;
  vtkStructuredPoints *m_RegionGrowingSP;

  bool m_ChangedManualData;
  bool m_ChangedAutomaticData;
  bool m_ChangedRefinementData;
  bool m_ChangedRegionGrowingData;

  mafNode *m_Volume;
  mafNode *m_ManualVolumeMask;
  mafNode *m_RefinementVolumeMask;

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
  int m_RegionGrowingStartSlice;
  int m_RegionGrowingEndSlice;
  //////////////////////////////////////////////////////////////////////////


private:
  /** Not implemented */
  mafDataPipeCustomSegmentationVolume(const mafDataPipeCustomSegmentationVolume&); 
  /** Operator = Not implemented*/
  void operator=(const mafDataPipeCustomSegmentationVolume&); 
};
#endif /* __mafDataPipeCustomSegmentationVolume_h */
