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
#ifndef __albaVMESegmentationVolume_h
#define __albaVMESegmentationVolume_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVME.h"
#include "albaVMEVolumeGray.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaTransform;
class mmaVolumeMaterial;
class albaDataPipeCustomSegmentationVolume;
class albaAttributeSegmentationVolume;

class ALBA_EXPORT albaVMESegmentationVolume : public albaVME
{
public:
  albaTypeMacro(albaVMESegmentationVolume, albaVME);

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
  /*virtual*/ albaString GetVisualPipe() {return albaString("albaPipeBox");};

  /** Precess events coming from other objects */ 
  /*virtual*/ void OnEvent(albaEventBase *alba_event);

  /** Return pointer to material attribute. */
  mmaVolumeMaterial *GetMaterial();

  /** Copy the contents of another albaVMESegmentationVolume into this one. */
  /*virtual*/ int DeepCopy(albaVME *a);

  /** Compare with another albaVMESegmentationVolume. */
  /*virtual*/ bool Equals(albaVME *vme);

  /** Set the pose matrix for the Prober. */
  void SetMatrix(const albaMatrix &mat);

  /** Clear the parameter 'kframes' because albaVMESegmentationVolume has no timestamp. */
  void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  /*virtual*/ bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  /*virtual*/ bool IsDataAvailable();

  /** return icon */
  static char** GetIcon();

  /** Set the link to the volume.*/
  int SetVolumeLink(albaVME *volume);

  /** Get the link to the volume.*/
  albaVME *GetVolumeLink();

  /** Return the vtkDataSet of the automatic segmentation */
  vtkDataSet *GetAutomaticOutput();

  /** Return the vtkDataSet of the manual segmentation */
  vtkDataSet *GetManualOutput();

  /** Return the vtkDataSet of the refinement segmentation */
  vtkDataSet *GetRefinementOutput();

  /** Return the vtkDataSet of the region growing segmentation */
  vtkDataSet *GetRegionGrowingOutput();

  /** Set the volume mask for the manual segmentation */
  void SetManualVolumeMask(albaVME *volume);

  /** Get the volume mask for the manual segmentation */
  albaVME *GetManualVolumeMask();

  /** Set the volume mask for the refinement segmentation */
  void SetRefinementVolumeMask(albaVME *volume);

  /** Get the volume mask for the refinement segmentation */
  albaVME *GetRefinementVolumeMask();

  /** Add a new range with a particular threshold */
  int AddRange(int startSlice,int endSlice,double threshold, double upperThershold=0);

  /** Return the value of the range of the position index */
  int GetRange(int index,int &startSlice, int &endSlice, double &threshold, double &upperThreshold);

  /** Return the value of the range of the position index */
  int GetRange(int index,int &startSlice, int &endSlice, double &threshold);

  /** Update the value of the range of the position index - return ALBA_ERROR if the index isn't correct*/
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
  /*virtual*/ albaVMEOutput *GetOutput();

  /** Return true if node is of type albaVMEVolume. */
  static bool VolumeAccept(albaVME*node) {return(node != NULL && node->IsA("albaVMEVolume"));}

protected:
  /** constructor. */
  albaVMESegmentationVolume();
  /** destructor. */
  virtual ~albaVMESegmentationVolume(); 

  /** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

  /** used to initialize and create the material attribute if not yet present */
  /*virtual*/ int InternalInitialize();

  /*virtual*/ int InternalStore(albaStorageElement *parent);
  /*virtual*/ int InternalRestore(albaStorageElement *node);

  /** called to prepare the update of the output */
  /*virtual*/ void InternalPreUpdate();

  /** Return the segmentation volume attribute */
  albaAttributeSegmentationVolume *GetVolumeAttribute();

  albaString m_VolumeName;

  albaTransform *m_Transform;

  albaDataPipeCustomSegmentationVolume *m_SegmentingDataPipe;

  albaAttributeSegmentationVolume *m_VolumeAttribute;

private:
  /** Not implemented */
  albaVMESegmentationVolume(const albaVMESegmentationVolume&); 
  /** Operator = Not implemented*/
  void operator=(const albaVMESegmentationVolume&);
};
#endif
