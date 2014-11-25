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
#ifndef __medVMESegmentationVolume_h
#define __medVMESegmentationVolume_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafTransform;
class mmaVolumeMaterial;
class medDataPipeCustomSegmentationVolume;
class medAttributeSegmentationVolume;

class MAF_EXPORT medVMESegmentationVolume : public mafVME
{
public:
  mafTypeMacro(medVMESegmentationVolume, mafVME);

  enum THRESHOLD_TYPE
  {
    GLOBAL = 0,
    RANGE,
  };

  enum BRUSH_TYPE
  {
    CIRCLE = 0,
    SQUARE,
  };

  enum PLANE_TYPE
  {
    XY = 0,
    XZ,
    YZ,
  };
  
  enum SELECTION_TYPE
  {
    SELECT = 0,
    UNSELECT,
  };

  enum SEGMENTATION_VOLUME_WIDGET_ID
  {
    ID_VOLUME_LINK = Superclass::ID_LAST,
    ID_LAST
  };

  /** Return the suggested pipe-typename for the visualization of this vme */
  /*virtual*/ mafString GetVisualPipe() {return mafString("mafPipeBox");};

  /** Precess events coming from other objects */ 
  /*virtual*/ void OnEvent(mafEventBase *maf_event);

  /** Return pointer to material attribute. */
  mmaVolumeMaterial *GetMaterial();

  /** Copy the contents of another medVMESegmentationVolume into this one. */
  /*virtual*/ int DeepCopy(mafNode *a);

  /** Compare with another medVMESegmentationVolume. */
  /*virtual*/ bool Equals(mafVME *vme);

  /** Set the pose matrix for the Prober. */
  void SetMatrix(const mafMatrix &mat);

  /** Clear the parameter 'kframes' because medVMESegmentationVolume has no timestamp. */
  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  /*virtual*/ bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  /*virtual*/ bool IsDataAvailable();

  /** return icon */
  static char** GetIcon();

  /** Set the link to the volume.*/
  int SetVolumeLink(mafNode *volume);

  /** Get the link to the volume.*/
  mafNode *GetVolumeLink();

  /** Return the vtkDataSet of the automatic segmentation */
  vtkDataSet *GetAutomaticOutput();

  /** Return the vtkDataSet of the manual segmentation */
  vtkDataSet *GetManualOutput();

  /** Return the vtkDataSet of the refinement segmentation */
  vtkDataSet *GetRefinementOutput();

  /** Return the vtkDataSet of the region growing segmentation */
  vtkDataSet *GetRegionGrowingOutput();

  /** Set the volume mask for the manual segmentation */
  void SetManualVolumeMask(mafNode *volume);

  /** Get the volume mask for the manual segmentation */
  mafNode *GetManualVolumeMask();

  /** Set the volume mask for the refinement segmentation */
  void SetRefinementVolumeMask(mafNode *volume);

  /** Get the volume mask for the refinement segmentation */
  mafNode *GetRefinementVolumeMask();

  /** Add a new range with a particular threshold */
  int AddRange(int startSlice,int endSlice,double threshold, double upperThershold=0);

  /** Return the value of the range of the position index */
  int GetRange(int index,int &startSlice, int &endSlice, double &threshold, double &upperThreshold);

  /** Return the value of the range of the position index */
  int GetRange(int index,int &startSlice, int &endSlice, double &threshold);

  /** Update the value of the range of the position index - return MAF_ERROR if the index isn't correct*/
  int UpdateRange(int index,int startSlice, int endSlice, double threshold, double upperThershold=0);

  /** Delete the range of the position index */
  int DeleteRange(int index);

  /** Remove all ranges */
  int RemoveAllRanges();

  /** Return the number of ranges */
  int GetNumberOfRanges();

  /** Set the threshold modality for automatic segmentation: GLOBAL or RANGE */
  void SetAutomaticSegmentationThresholdModality(int modality);

  /** Get the threshold modality for automatic segmentation: GLOBAL or RANGE */
  int GetAutomaticSegmentationThresholdModality();

  /** Set Threshold filtering to double Threshold if modality is true */
  void SetDoubleThresholdModality(int modality);

  /** Return Threshold filtering modality, true if double Threshold is enabled */
  int GetDoubleThresholdModality();

  /** Set the value to use during a global threshold, on single threshold mode set only the lower threshold*/
  void SetAutomaticSegmentationGlobalThreshold(double lowerThreshold, double uppperThreshold=0);

  /** Return the value of upper threshold to use during a global threshold*/
  double GetAutomaticSegmentationGlobalUpperThreshold();

  /** Return the value to use during a global threshold, in double threshold mode return the lower threshold*/
  double GetAutomaticSegmentationGlobalThreshold();

  /** Check if all thresholds exist */
  bool CheckNumberOfThresholds();

  /** Set the region growing upper threshold */
  void SetRegionGrowingUpperThreshold(double value);

  /** Set the region growing lower threshold */
  void SetRegionGrowingLowerThreshold(double value);

  /** Return the region growing upper threshold */
  double GetRegionGrowingUpperThreshold();

  /** Return the region growing lower threshold */
  double GetRegionGrowingLowerThreshold();

  /** Add a new seed */
  int AddSeed(int seed[3]);

  /** Return the seed of the position index */
  int GetSeed(int index,int seed[3]);

  /** Delete the seed of the position index */
  int DeleteSeed(int index);
 
  /** Remove all seeds */
  int RemoveAllSeeds();

  /** Return the number of seeds */
  int GetNumberOfSeeds();

  /** Set the range of action of Region Growing algorithm */
  void SetRegionGrowingSliceRange(int startSlice, int endSlice);

  /** Return the range of action of Region Growing algorithm */
  int GetRegionGrowingSliceRange(int &startSlice, int &endSlice);

  /** return the right type of output */  
  /*virtual*/ mafVMEOutput *GetOutput();

  /** Return true if node is of type mafVMEVolume. */
  static bool VolumeAccept(mafNode *node) {return(node != NULL && node->IsA("mafVMEVolume"));}

protected:
  /** constructor. */
  medVMESegmentationVolume();
  /** destructor. */
  virtual ~medVMESegmentationVolume(); 

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  /** used to initialize and create the material attribute if not yet present */
  /*virtual*/ int InternalInitialize();

  /*virtual*/ int InternalStore(mafStorageElement *parent);
  /*virtual*/ int InternalRestore(mafStorageElement *node);

  /** called to prepare the update of the output */
  /*virtual*/ void InternalPreUpdate();

  /** Return the segmentation volume attribute */
  medAttributeSegmentationVolume *GetVolumeAttribute();

  mafString m_VolumeName;

  mafTransform *m_Transform;

  medDataPipeCustomSegmentationVolume *m_SegmentingDataPipe;

  medAttributeSegmentationVolume *m_VolumeAttribute;

private:
  /** Not implemented */
  medVMESegmentationVolume(const medVMESegmentationVolume&); 
  /** Operator = Not implemented*/
  void operator=(const medVMESegmentationVolume&);
};
#endif
