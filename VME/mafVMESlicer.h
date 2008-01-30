/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMESlicer.h,v $
  Language:  C++
  Date:      $Date: 2008-01-30 14:42:20 $
  Version:   $Revision: 1.13 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMESlicer_h
#define __mafVMESlicer_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVMEVolume.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkVolumeSlicer;
class vtkTransformPolyDataFilter;
class mafNode;
class mmaMaterial;
class mafVMEOutputSurface;

/** mafVMESlicer - a procedural VME computing the slice of its parent VME.
  mafVMESlicer is a node implementing a slicer of a VME (currently only VME-Volume).
  The sliced volume is the partent VME.
  @sa mafVMEVolume
  @todo
  - 
  */
class MAF_EXPORT mafVMESlicer : public mafVME
{
public:

  mafTypeMacro(mafVMESlicer,mafVME);

  enum SLICER_WIDGET_ID
  {
    ID_VOLUME_LINK = Superclass::ID_LAST,
    ID_LAST
  };

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Slicer into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-Slicer. */
  virtual bool Equals(mafVME *vme);

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** return the right type of output */  
  mafVMEOutputSurface *GetSurfaceOutput();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
      is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeSurfaceTextured");};
  
  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

  /** Set the link to the slicer.*/
  void SetSlicedVMELink(mafNode *node);

  /** Get the link to the slicing vme.*/
  mafNode *GetSlicedVMELink();

  static bool VolumeAccept(mafNode* node) {return(node != NULL  && node->IsMAFType(mafVMEVolume));};

protected:
  mafVMESlicer();
  virtual ~mafVMESlicer();

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

	mafTransform*     m_CopyTransform; ///< pose matrix for the slicer plane
  mafTransform*     m_Transform; ///< pose matrix for the slicer plane
  vtkVolumeSlicer*  m_PSlicer;  ///< slicer object used to extract the cut contour
  vtkVolumeSlicer*  m_ISlicer;  ///< slicer object used to compute the slice image

  vtkTransformPolyDataFilter *m_BackTransform;
  //vtkTransformPolyDataFilter *m_BackTransformParent;

  mafString         m_SlicedName;
  int               m_TextureRes;
  double            m_Xspc;
  double            m_Yspc;

private:
  mafVMESlicer(const mafVMESlicer&); // Not implemented
  void operator=(const mafVMESlicer&); // Not implemented
};
#endif
