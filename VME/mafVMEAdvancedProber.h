/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEAdvancedProber.h,v $
  Language:  C++
  Date:      $Date: 2007-06-29 12:40:45 $
  Version:   $Revision: 1.7 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEAdvancedProber_h
#define __mafVMEAdvancedProber_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafEvent.h"
#include "mafVMEVolume.h"
#include "mafVMEOutputPolyline.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafTransform;
class mmaMaterial;
class vtkDistanceFilter;
class vtkProbeFilter;
class mafVMESurface;
class vtkImageData;
class vtkImageMapToColors;
class vtkPoints;
class vtkPlaneSource ;
class vtkLookupTable;
class mafVMEImage;
class mafVMEPolylineSpline;

//class vtkAppendPolyData;

class mafVMEAdvancedProber : public mafVME
{
public:
  enum VMEPANORAMIC_MODALITY
  {
    FIRST_MODE = 0,
  };

  mafTypeMacro(mafVMEAdvancedProber, mafVME);

  enum PANORAMIC_WIDGET_ID
  {
    ID_VOLUME_LINK = Superclass::ID_LAST,
    ID_SURFACE_LINK,
    ID_MODALITY,

    ID_ADD_PROFILE,
    ID_REMOVE_PROFILE,
    ID_SAMPLING,
		ID_SPLINE,
    ID_PROFILE_NUMBER,
    ID_PROFILE_DISTANCE,
    ID_LISTBOX,
    ID_UPDATE,
    ID_LAST
  };

  static bool VolumeAccept(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMEVolume));};

  static bool PolylineAccept(mafNode *node) {return(node != NULL && ((mafVME *)node)->IsA("mafVMEPolylineSpline"));};

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return "mafPipeSurfaceTextured";};

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** Copy the contents of another mafVMEAdvancedProber into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Get the link to the volume.*/
  mafNode *GetVolumeLink();

  /** Get the link to the surface.*/
  mafNode::mafLinksMap *GetPolylineLink();

  /** Set the link to the volume.*/
  void SetVolumeLink(mafNode *volume);

  /** Set the link to the surface.*/
  void SetPolylineLink(mafNode *polyline);

  /** Set the pose matrix for the Prober. */
  void SetMatrix(const mafMatrix &mat);

  /** Clear the parameter 'kframes' because mafVMEAdvancedProber has no timestamp. */
  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return icon */
  static char** GetIcon();

	vtkImageData* GetImage(){return m_Image;};

  /** update the output data structure */
  virtual vtkLookupTable *GetLUT(){return m_Lut;};

  /** AutomaticCaluclum On/Off */
  //void AutomaticCalculumOn(){m_AutomaticCalculum = 1;};
  //void AutomaticCalculumOff(){m_AutomaticCalculum = 0;};

  /** Set the sampling coefficient */
  void SetSamplingCoefficient(double value){m_SamplingCoefficient = value;};
  double GetSamplingCoefficient(){return m_SamplingCoefficient;};

protected:
  mafVMEAdvancedProber();
  virtual ~mafVMEAdvancedProber(); 

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

  //virtual void InternalPreUpdateOld();

  //check if it's necessary reprocess all panoramic
  bool CheckUpdatePanoramic(mafVMEPolylineSpline *vme);

  vtkPoints *m_ControlPoints;

  mafString m_VolumeName;
  std::vector<mafString> m_ProfilesNameList;
  
//  mafVMESurface        *m_Surface;
  vtkImageData *m_Image;
  vtkImageMapToColors *m_IMTC;
  vtkPoints *m_Points;
  vtkPlaneSource *m_Plane;
	vtkLookupTable *m_Lut;


  wxListBox *m_ListBox;
	//int m_SplineCoefficient;
  int m_AdditionalProfileNumber;

  double m_SamplingCoefficient; double m_ControlSamplingCoefficient;
  double m_ProfileDistance; double m_ControlProfileDistance;

  
  

  
  std::vector<vtkPoints *> m_PointsVector;
  std::vector<vtkPolyData *> m_PolyDataVector;
  std::vector<vtkProbeFilter *> m_DistanceFilterVector;

  mafTransform *m_Transform;
  //vtkAppendPolyData *append;

	int m_Fixed;
  //int m_AutomaticCalculum;

private:
  mafVMEAdvancedProber(const mafVMEAdvancedProber&); // Not implemented
  void operator=(const mafVMEAdvancedProber&); // Not implemented
};
#endif
