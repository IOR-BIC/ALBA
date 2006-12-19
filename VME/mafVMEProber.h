/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEProber.h,v $
  Language:  C++
  Date:      $Date: 2006-12-19 11:37:43 $
  Version:   $Revision: 1.3 $
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
#include "mmgVMEChooserAccept.h"
#include "mafVMEVolume.h"
#include "mafVMEOutputSurface.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafTransform;
class mmaMaterial;
class vtkPolyDataNormals;
class vtkDistanceFilter;

class MAF_EXPORT mafVMEProber : public mafVME
{
public:
  enum VMEPROBER_MODALITY
  {
    DENSITY_MODE = 0,
    DISTANCE_MODE,
    DISTANCE_MODE_SCALAR = 0,
    DISTANCE_MODE_VECTOR
  };

  mafTypeMacro(mafVMEProber, mafVME);

  enum PROBER_WIDGET_ID
  {
    ID_VOLUME_LINK = Superclass::ID_LAST,
    ID_SURFACE_LINK,
    ID_MODALITY,
    ID_DISTANCE_TYPE,
    ID_LAST
  };

  class mafVMEVolumeAccept : public mmgVMEChooserAccept
  {
  public:

    mafVMEVolumeAccept() {};
    ~mafVMEVolumeAccept() {};

    bool Validate(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMEVolume));};
  };
  mafVMEVolumeAccept *m_VMEVolumeAccept;

  class mafVMESurfaceAccept : public mmgVMEChooserAccept
  {
  public:

    mafVMESurfaceAccept() {};
    ~mafVMESurfaceAccept() {};

    bool Validate(mafNode *node) {return(node != NULL && ((mafVME *)node)->GetOutput()->IsMAFType(mafVMEOutputSurface));};
  };
  mafVMESurfaceAccept *m_VMESurfaceAccept;

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
//	int GetMode() {return m_ProberMode;};
  int GetMode();

  /** Set the threshold for Distance mode Map.*/
//	void SetDistanceThreshold(float thr) {m_DistThreshold = thr; Modified();};
	void SetDistanceThreshold(float thr);

  /** Get the threshold of Distance mode Map.*/
//	float GetDistanceThreshold() {return m_DistThreshold;};
	float GetDistanceThreshold();

  /** Set/Get the max distance for Distance mode Map.*/
//	float GetMaxDistance() {return m_MaxDistance;};
	float GetMaxDistance();

  /** Set/Get the max distance for Distance mode Map.*/
//	void SetMaxDistance(float max_dist) {m_MaxDistance = max_dist; Modified();};
	void SetMaxDistance(float max_dist);

  /** Set the Distance mode to produce scalar data or vector data.*/
  void SetDistanceMode(int mode);

  /** Set the Distance mode to produce scalar data.*/
//	void SetDistanceModeToScalar() {m_DistanceModeType = DISTANCE_MODE_SCALAR; Modified();};
  void SetDistanceModeToScalar() {this->SetDistanceMode(mafVMEProber::DISTANCE_MODE_SCALAR);};

  /** Set the Distance mode to produce vector data.*/
//	void SetDistanceModeToVector() {m_DistanceModeType = DISTANCE_MODE_VECTOR; Modified();};
	void SetDistanceModeToVector() {this->SetDistanceMode(mafVMEProber::DISTANCE_MODE_VECTOR);};

  /** Get the distance mode type.*/
//	int GetDistanceType() {return m_DistanceModeType;};
	int GetDistanceMode();

  /** Set/Get the High Density Threshold for Density mode Map.*/
//	float GetHighDensity (){return m_HighDensity;};
	float GetHighDensity();

  /** Set/Get the High Density Threshold for Density mode Map.*/
//	void SetHighDensity(float high_dens) {m_HighDensity = high_dens; Modified();};
	void SetHighDensity(float high_dens);

	/** Set/Get the Low Density Threshold for Density mode Map.*/
//	float GetLowDensity() {return m_LowDensity;};
	float GetLowDensity();

  /** Set/Get the Low Density Threshold for Density mode Map.*/
//	void SetLowDensity(float low_dens) {m_LowDensity = low_dens; Modified();};
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

  /** update the output data structure */
  virtual void InternalUpdate();

	float  m_DistThreshold;
	float  m_MaxDistance;
	int		 m_DistanceModeType;
	int		 m_ProberMode;
	float  m_HighDensity;
	float  m_LowDensity;
  mafString m_VolumeName;
  mafString m_SurfaceName;
  
  vtkPolyDataNormals *m_Normals;
  //vtkDistanceFilter  *m_ProberFilter;
  vtkDistanceFilter  *m_Prober;

  mafTransform *m_Transform;

private:
  mafVMEProber(const mafVMEProber&); // Not implemented
  void operator=(const mafVMEProber&); // Not implemented
};
#endif
