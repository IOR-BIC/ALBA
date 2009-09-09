/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEMaps.h,v $
Language:  C++
Date:      $Date: 2009-09-09 15:33:02 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medVMEMaps_h
#define __medVMEMaps_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMESurface.h"
#include "mafVMEVolume.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafTransform;
class mafVMEOutputSurface;
class vtkColorTransferFunction;

class vtkMAFDistanceFilter;
class vtkPolyData;
class vtkPolyDataNormals;


/** Class name: medVMEMaps.
VME with input a surface and a volume, and producing a distance/density scalar surface.  
*/

class MAF_EXPORT medVMEMaps : public mafVMESurface
{
public:
  
  /** IDs for the GUI */
  enum VME_MAPS_WIDGET_ID
  {
    ID_DENSITY_DISTANCE = Superclass::ID_LAST,
    /*ID_FIRST_THRESHOLD,
    ID_SECOND_THRESHOLD,
    ID_MAX_DISTANCE,
    ID_SELECT_VOLUME,
    ID_NUM_SECTIONS,
    ID_BAR_TIPOLOGY,
    ID_AREA,
    ID_AREA_DISTANCE,*/
    ID_LAST
  };

  mafTypeMacro(medVMEMaps,mafVMESurface);


  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Maps into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-Maps. */
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
  //virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  //virtual bool IsDataAvailable();

  /** Return the suggested pipe-typename for the visualization of this vme */
  //virtual mafString GetVisualPipe() {return mafString("mafPipeSurfaceTextured");};

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  //static char ** GetIcon();

  //static bool VolumeAccept(mafNode* node) {return(node != NULL  && node->IsMAFType(mafVMEVolume));};

  /** Set the source volume. */
  void SetVolume(mafVMEVolume *volume);

  /** Get the source volume. */
  mafVMEVolume *GetVolume();

  /** Set input vtkPolyData. */
  void SetVTKPolyData(vtkPolyData *data);

  /** Get input vtkPolyData. */
  vtkDataSet *GetVTKPolyData();

  void Selected(bool sel);

protected:

  medVMEMaps();
  virtual ~medVMEMaps();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

  /** update filter mode*/
  virtual void UpdateMaps();

  vtkPolyDataNormals        *m_Normals;
  vtkMAFDistanceFilter      *m_DistanceFilter;
  mafVMEVolume              *m_Volume;
  mafTransform              *m_Transform;
  vtkPolyData               *m_PolyData;;

  int m_DensityDistance;
  int m_FirstThreshold;
  int m_SecondThreshold;
  int m_MaxDistance;

};
#endif
