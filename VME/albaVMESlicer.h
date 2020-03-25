/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESlicer
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMESlicer_h
#define __albaVMESlicer_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVME.h"
#include "albaVMEVolume.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkALBAVolumeSlicer;
class vtkTransformPolyDataFilter;
class albaVME;
class mmaMaterial;
class albaVMEOutputSurface;

/** albaVMESlicer - a procedural VME computing the slice of its parent VME.
  albaVMESlicer is a node implementing a slicer of a VME (currently only VME-Volume).
  The sliced volume is the partent VME.
  @sa albaVMEVolume
  @todo
  - 
  */
class ALBA_EXPORT albaVMESlicer : public albaVME
{
public:

  /** Enable/Disable slicer texture vtk property automatic update from material (default behavior is 
  updating from material) */
  void SetUpdateVTKPropertiesFromMaterial(bool updateFromMaterial) {m_UpdateVTKPropertiesFromMaterial = updateFromMaterial;};
  bool GetUpdateVTKPropertiesFromMaterial() {return m_UpdateVTKPropertiesFromMaterial;};

  albaTypeMacro(albaVMESlicer,albaVME);

  enum SLICER_WIDGET_ID
  {
    ID_VOLUME_LINK = Superclass::ID_LAST,
    ID_LAST
  };

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Slicer into this one. */
  virtual int DeepCopy(albaVME *a);

  /** Compare with another VME-Slicer. */
  virtual bool Equals(albaVME *vme);

  /** Precess events coming from other objects */ 
  virtual void OnEvent(albaEventBase *alba_event);

  /** return the right type of output */  
  albaVMEOutputSurface *GetSurfaceOutput();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const albaMatrix &mat);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
      is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeSurfaceTextured");};
  
  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

  /** Set the link to the slicer.*/
  void SetSlicedVMELink(albaVME *node);

  /** Get the link to the slicing vme.*/
  albaVME *GetSlicedVMELink();

  static bool VolumeAccept(albaVME* node) {return(node != NULL  && node->IsALBAType(albaVMEVolume));};


  /** Set tri-linear interpolation to on */
  void SetTrilinearInterpolationOn(){m_TrilinearInterpolationOn = true;};

  /** Set tri-linear interpolation to off */
  void SetTrilinearInterpolationOff(){m_TrilinearInterpolationOn = false;};

  /** Set tri-linear interpolation */
  void SetTrilinearInterpolation(bool on);

	/** Enable/Disable GPU*/
	void SetEnableGPU(bool val=true);
protected:
  albaVMESlicer();
  virtual ~albaVMESlicer();

  /** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

	albaTransform*     m_CopyTransform; ///< pose matrix for the slicer plane
  albaTransform*     m_Transform; ///< pose matrix for the slicer plane
  vtkALBAVolumeSlicer*  m_PSlicer;  ///< slicer object used to extract the cut contour
  vtkALBAVolumeSlicer*  m_ISlicer;  ///< slicer object used to compute the slice image

  vtkTransformPolyDataFilter *m_BackTransform;
  //vtkTransformPolyDataFilter *m_BackTransformParent;

  albaString         m_SlicedName;
  int               m_TextureRes;
  double            m_Xspc;
  double            m_Yspc;

  bool m_UpdateVTKPropertiesFromMaterial;

  bool m_TrilinearInterpolationOn;

	bool m_EnableGPU;

private:
  albaVMESlicer(const albaVMESlicer&); // Not implemented
  void operator=(const albaVMESlicer&); // Not implemented
};
#endif
