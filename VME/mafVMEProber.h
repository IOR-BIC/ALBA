/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEProber.h,v $
  Language:  C++
  Date:      $Date: 2007-11-27 15:19:20 $
  Version:   $Revision: 1.5 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEProber_h
#define __mafVMEProber_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafEvent.h"
#include "mafVMEVolume.h"
#include "mafVMEOutputSurface.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafTransform;
class mmaMaterial;
class mafDataPipeCustomProber;

class MAF_EXPORT mafVMEProber : public mafVME
{
public:
  mafTypeMacro(mafVMEProber, mafVME);

  enum PROBER_WIDGET_ID
  {
    ID_VOLUME_LINK = Superclass::ID_LAST,
    ID_SURFACE_LINK,
    ID_MODALITY,
    ID_DISTANCE_TYPE,
    ID_LAST
  };

  static bool VolumeAccept(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMEVolume));};

  static bool OutputSurfaceAccept(mafNode *node) {return(node != NULL && ((mafVME *)node)->GetOutput()->IsMAFType(mafVMEOutputSurface));};

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeSurface");};

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** Copy the contents of another mafVMEProber into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another mafVMEProber. */
  virtual bool Equals(mafVME *vme);

  /** Get the link to the volume.*/
  mafNode *GetVolumeLink();

  /** Get the link to the surface.*/
  mafNode *GetSurfaceLink();

  /** Set the link to the volume.*/
  void SetVolumeLink(mafNode *volume);

  /** Set the link to the surface.*/
  void SetSurfaceLink(mafNode *surface);

  /** Set the mode to Density or Distance Map.*/
  void SetMode(int mode);
  
  /** Set the Density mode Map.*/
  void SetModeToDensity();

  /** Set the Distance mode Map.*/
  void SetModeToDistance();

  /** Get the mode.*/
  int GetMode();

  /** Set the threshold for Distance mode Map.*/
	void SetDistanceThreshold(float thr);

  /** Get the threshold of Distance mode Map.*/
	float GetDistanceThreshold();

  /** Set/Get the max distance for Distance mode Map.*/
	float GetMaxDistance();

  /** Set/Get the max distance for Distance mode Map.*/
	void SetMaxDistance(float max_dist);

  /** Set the Distance mode to produce scalar data or vector data.*/
  void SetDistanceMode(int mode);

  /** Set the Distance mode to produce scalar data.*/
  void SetDistanceModeToScalar();

  /** Set the Distance mode to produce vector data.*/
	void SetDistanceModeToVector();

  /** Get the distance mode type.*/
	int GetDistanceMode();

  /** Set/Get the High Density Threshold for Density mode Map.*/
	float GetHighDensity();

  /** Set/Get the High Density Threshold for Density mode Map.*/
	void SetHighDensity(float high_dens);

	/** Set/Get the Low Density Threshold for Density mode Map.*/
	float GetLowDensity();

  /** Set/Get the Low Density Threshold for Density mode Map.*/
	void SetLowDensity(float low_dens);

  /** Set the pose matrix for the Prober. */
  void SetMatrix(const mafMatrix &mat);

  /** Clear the parameter 'kframes' because mafVMEProber has no timestamp. */
  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();

  /** return icon */
  static char** GetIcon();

protected:
  mafVMEProber();
  virtual ~mafVMEProber(); 

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

	float  m_DistThreshold;
	float  m_MaxDistance;
	int		 m_DistanceModeType;
	int		 m_ProberMode;
	float  m_HighDensity;
	float  m_LowDensity;
  mafString m_VolumeName;
  mafString m_SurfaceName;
  
  mafTransform *m_Transform;

  mafDataPipeCustomProber *m_ProbingDataPipe;

private:
  mafVMEProber(const mafVMEProber&); // Not implemented
  void operator=(const mafVMEProber&); // Not implemented
};
#endif
