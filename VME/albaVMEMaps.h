/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMaps
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEMaps_h
#define __albaVMEMaps_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVME.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMESurface;
class albaVMEVolume;
class albaTransform;
class albaVMEOutputSurface;
class mmaMaterial;
class vtkColorTransferFunction;

class vtkALBADistanceFilter;
class vtkPolyData;
class vtkPolyDataNormals;
class vtkLookupTable;


/** Class name: albaVMEMaps.
VME with input a surface and a volume, and producing a distance/density scalar surface.  
*/

class ALBA_EXPORT albaVMEMaps : public albaVME
{
public:
  albaTypeMacro(albaVMEMaps,albaVME);

	/** IDs for the GUI */
	enum VME_MAPS_WIDGET_ID
	{
		ID_DENSITY_DISTANCE = Superclass::ID_LAST,
		ID_LAST
	};

  albaVMEMaps();
  virtual ~albaVMEMaps();

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Maps into this one. */
  virtual int DeepCopy(albaVME *a);

  /** Compare with another VME-Maps. */
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
  //virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  //virtual bool IsDataAvailable();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeSurface");};

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  //static char ** GetIcon();

  /** Set the source volume. */
  void SetVolume(albaVMEVolume *volume);

  /** Get the source volume. */
  albaVMEVolume *GetVolume();

  /** 
  Set data for the give timestamp. 
  Return ALBA_OK if succeeded, ALBA_ERROR if they kind of data is not accepted by
  this type of VME. */
  //virtual int SetData(vtkPolyData *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);

  /** Get distance-density filter mode. */
  int GetDensityDistance(){return m_DensityDistance;};

  /** Set distance-density filter mode. */
  void SetDensityDistance(int densityDistance);

  /** Get first threshold distance-density filter parameter. */
  int GetFirstThreshold(){return m_FirstThreshold;};

  /** Set first threshold distance-density filter parameter. */
  void SetFirstThreshold(int firstThreshold);

  /** Get second threshold distance-density filter parameter. */
  int GetSecondThreshold(){return m_SecondThreshold;};

  /** Set second threshold distance-density filter parameter. */
  void SetSecondThreshold(int secondThreshold);
  
  /** Get max distance distance-density filter parameter. */
  int GetMaxDistance(){return m_MaxDistance;};

  /** Set max distance distance-density filter parameter. */
  void SetMaxDistance(int maxDistance);

  /** Set the link to the maps.*/
  void SetMappedVMELink(albaVME *node);

  /** Get the link to the mapped vme.*/
  albaVME *GetMappedVMELink();

  /** Set the volume link to the maps.*/
  void SetSourceVMELink(albaVME *node);

  /** Get the link to the source vme.*/
  albaVME *GetSourceVMELink();

  /** Get the scalar range of the output vtkDataSet. */
  void GetScalarRange(double range[2]);

protected:

  //albaVMEMaps();
  //virtual ~albaVMEMaps();

  /** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

  vtkLookupTable *CreateTable();

  /** Update distance density filter. */
  void UpdateFilter();

  /** private to avoid calling by external classes */
  //virtual int SetData(vtkDataSet *data, albaTimeStamp t, int mode=ALBA_VME_COPY_DATA);

  albaString                 m_MappedName;

  vtkPolyDataNormals        *m_Normals;
  vtkALBADistanceFilter      *m_DistanceFilter;
  albaVMEVolume              *m_Volume;
  albaTransform              *m_Transform;
  vtkPolyData               *m_PolyData;
  vtkLookupTable            *m_Table;

  int m_DensityDistance;
  int m_FirstThreshold;
  int m_SecondThreshold;
  int m_MaxDistance;

};
#endif
