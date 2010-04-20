/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medDataPipeCustomSegmentationVolume.h,v $
Language:  C++
Date:      $Date: 2010-04-20 16:02:27 $
Version:   $Revision: 1.1.2.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2010
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medDataPipeCustomSegmentationVolume_h
#define __medDataPipeCustomSegmentationVolume_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDataPipeCustom.h"
#include <vector>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class vtkRectilinearGrid;
class vtkStructuredPoints;

/** */
class MAF_EXPORT medDataPipeCustomSegmentationVolume : public mafDataPipeCustom
{
public:
  mafTypeMacro(medDataPipeCustomSegmentationVolume,mafDataPipe);

  /** Make a copy of itself.*/
  int DeepCopy(mafDataPipe *pipe);

  /** Set the volume to segment*/
  void SetVolume(mafNode *volume);

  /** Return the segmented volume*/
  mafNode *GetVolume() {return m_Volume;};

  /** Set the threshold modality for automatic segmentation: GLOBAL or RANGE */
  void SetAutomaticSegmentationThresholdModality(int modality);

  /** Return the threshold modality for automatic segmentation: GLOBAL or RANGE */
  int GetAutomaticSegmentationThresholdModality(){return m_AutomaticSegmentationThresholdModality;};

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

  /** Set the value to use during a global threshold */
  void SetAutomaticSegmentationGlobalThreshold(double threshold);

  /** Return the value to use during a global threshold */
  double GetAutomaticSegmentationGlobalThreshold(){return m_AutomaticSegmentationGlobalThreshold;};

  /** Check if all thresholds exist */
  bool CheckNumberOfThresholds();

  /** Set the volume mask for the manual segmentation */
  void SetManualVolumeMask(mafNode *volume);

  /** Get the volume mask for the manual segmentation */
  mafNode *GetManualVolumeMask(){return m_ManualVolumeMask;};

  /** Return the vtkDataSet of the automatic segmentation */
  vtkDataSet *GetAutomaticOutput();

protected:
  medDataPipeCustomSegmentationVolume();
  virtual ~medDataPipeCustomSegmentationVolume();

  /** Check that the range has right values */
  int AutomaticCheckRange(int startSlice,int endSlice,int indexToExclude = -1);

  /** function called before of data pipe execution */
  /*virtual*/ void PreExecute();

  /** Apply the automatic segmentation */
  void ApplyAutomaticSegmentation();

  /** Apply the manual segmentation */
  void ApplyManualSegmentation();

  /** function called to updated the data pipe output */
  /*virtual*/ void Execute();

  vtkRectilinearGrid *m_RG;
  vtkStructuredPoints *m_SP;

  vtkRectilinearGrid *m_AutomaticRG;
  vtkStructuredPoints *m_AutomaticSP;
  vtkRectilinearGrid *m_ManualRG;
  vtkStructuredPoints *m_ManualSP;

  bool m_ChangedManualData;
  bool m_ChangedAutomaticData;

  mafNode *m_Volume;
  mafNode *m_ManualVolumeMask;

  //Stuff for automatic threshold
  int m_AutomaticSegmentationThresholdModality;
  double m_AutomaticSegmentationGlobalThreshold;
  std::vector<int*> m_AutomaticSegmentationRanges;
  std::vector<double> m_AutomaticSegmentationThresholds;
  //////////////////////////////////////////////////////////////////////////


private:
  medDataPipeCustomSegmentationVolume(const medDataPipeCustomSegmentationVolume&); // Not implemented
  void operator=(const medDataPipeCustomSegmentationVolume&); // Not implemented  
};
#endif /* __medDataPipeCustomSegmentationVolume_h */
