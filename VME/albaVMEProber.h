/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEProber
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEProber_h
#define __albaVMEProber_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVME.h"
#include "albaEvent.h"
#include "albaVMEVolume.h"
#include "albaVMEOutputSurface.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaTransform;
class mmaMaterial;
class albaDataPipeCustomProber;

class ALBA_EXPORT albaVMEProber : public albaVME
{
public:
  albaTypeMacro(albaVMEProber, albaVME);

  enum PROBER_WIDGET_ID
  {
    ID_VOLUME_LINK = Superclass::ID_LAST,
    ID_SURFACE_LINK,
    ID_MODALITY,
    ID_DISTANCE_TYPE,
    ID_LAST
  };

  static bool VolumeAccept(albaVME*node) {return(node != NULL && node->IsALBAType(albaVMEVolume));};

  static bool OutputSurfaceAccept(albaVME*node) {return(node != NULL && node->GetOutput()->IsALBAType(albaVMEOutputSurface));};

  /** Precess events coming from other objects */ 
  virtual void OnEvent(albaEventBase *alba_event);

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeSurface");};

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** Copy the contents of another albaVMEProber into this one. */
  virtual int DeepCopy(albaVME *a);

  /** Compare with another albaVMEProber. */
  virtual bool Equals(albaVME *vme);

  /** Get the link to the volume.*/
  albaVME *GetVolumeLink();

  /** Get the link to the surface.*/
  albaVME *GetSurfaceLink();

  /** Set the link to the volume.*/
  void SetVolumeLink(albaVME *volume);

  /** Set the link to the surface.*/
  void SetSurfaceLink(albaVME *surface);

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
  void SetMatrix(const albaMatrix &mat);

  /** Clear the parameter 'kframes' because albaVMEProber has no timestamp. */
  void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();

  /** return icon */
  static char** GetIcon();

protected:
  albaVMEProber();
  virtual ~albaVMEProber(); 

  /** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

	float  m_DistThreshold;
	float  m_MaxDistance;
	int		 m_DistanceModeType;
	int		 m_ProberMode;
	float  m_HighDensity;
	float  m_LowDensity;
  albaString m_VolumeName;
  albaString m_SurfaceName;
  
  albaTransform *m_Transform;

  albaDataPipeCustomProber *m_ProbingDataPipe;

private:
  albaVMEProber(const albaVMEProber&); // Not implemented
  void operator=(const albaVMEProber&); // Not implemented
};
#endif
