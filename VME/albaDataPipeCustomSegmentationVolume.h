/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeCustomSegmentationVolume
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaDataPipeCustomSegmentationVolume_h
#define __albaDataPipeCustomSegmentationVolume_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaDataPipeCustom.h"
#include <vector>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class vtkRectilinearGrid;
class vtkImageData;
class albaVME;
/** */
class ALBA_EXPORT albaDataPipeCustomSegmentationVolume : public albaDataPipeCustom
{
public:
  albaTypeMacro(albaDataPipeCustomSegmentationVolume,albaDataPipe);

  /** Make a copy of itself.*/
  int DeepCopy(albaDataPipe *pipe);

  /** Set the volume to segment*/
  int SetVolume(albaVME *volume);

  /** Return the segmented volume*/
	albaVME *GetVolume() {return m_Volume;};

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

  /** Update the value of the range of the position index - return ALBA_ERROR if the index isn't correct*/
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
  void SetManualVolumeMask(albaVME *volume);

  /** Get the volume mask for the manual segmentation */
  albaVME *GetManualVolumeMask(){return m_ManualVolumeMask;};

  /** Set the volume mask for the refinement segmentation */
  void SetRefinementVolumeMask(albaVME *volume);

  /** Get the volume mask for the refinement segmentation */
  albaVME *GetRefinementVolumeMask();

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
  albaDataPipeCustomSegmentationVolume();
  /** destructor. */
  virtual ~albaDataPipeCustomSegmentationVolume();

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
  vtkImageData *m_SP;

  vtkRectilinearGrid *m_AutomaticRG;
  vtkImageData *m_AutomaticSP;
  vtkRectilinearGrid *m_ManualRG;
  vtkImageData *m_ManualSP;
  vtkRectilinearGrid *m_RefinementRG;
  vtkImageData *m_RefinementSP;
  vtkRectilinearGrid *m_RegionGrowingRG;
  vtkImageData *m_RegionGrowingSP;

  bool m_ChangedManualData;
  bool m_ChangedAutomaticData;
  bool m_ChangedRefinementData;
  bool m_ChangedRegionGrowingData;

	albaVME *m_Volume;
	albaVME *m_ManualVolumeMask;
	albaVME *m_RefinementVolumeMask;

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
  albaDataPipeCustomSegmentationVolume(const albaDataPipeCustomSegmentationVolume&); 
  /** Operator = Not implemented*/
  void operator=(const albaDataPipeCustomSegmentationVolume&); 
};
#endif /* __albaDataPipeCustomSegmentationVolume_h */
