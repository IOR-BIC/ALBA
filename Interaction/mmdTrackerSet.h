/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdTrackerSet.h,v $
  Language:  C++
  Date:      $Date: 2005-05-19 16:27:40 $
  Version:   $Revision: 1.2 $
  Authors:   Michele Diegoli & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmdTrackerSet_h
#define __mmdTrackerSet_h

#include "mafDeviceSet.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmdTracker;
class vtkPlaneSource;

/** class for managing multiple tracker device
  This class manages multiple track device, e.g. tracker devices producing
  multiple pose matrices. Each pose matrix stream is identified as a "sub device".
  Sub devices should be created in the InternalInitialize.
  @sa  mmdTracker mafDevice mflDispatcher mflAgent mflEvent
  @todo
  - add support for setting tracked volume for all sub-trackers
*/
class mmdTrackerSet : public mafDeviceSet
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  MAF_ID_DEC(DEVICE_ADD);
  MAF_ID_DEC(DEVICE_REMOVE);

  mafTypeMacro(mmdTrackerSet,mafDeviceSet);

  /** Return the number of trackers composing this tracker set */
  int GetNumberOfTrackers() {return this->GetNumberOfDevices();}

  /**
   Add a new device, and return an ID. If the returned ID==0 the operation has failed! */
  void AddDevice(mmdTracker *device);

  /** Return a device handler given its ID or name. */
  mmdTracker *GetDevice(unsigned long id);
  mmdTracker *GetDevice(const char *name);

protected:
  mmdTrackerSet();
  virtual ~mmdTrackerSet();

  /**  redefined as protected functions */
  void AddDevice(mafDevice *device) {Superclass::AddDevice(device);}
  int RemoveDevice(mafDevice *device) {return this->Superclass::RemoveDevice(device);}

private:
  mmdTrackerSet(const mmdTrackerSet&);  // Not implemented.
  void operator=(const mmdTrackerSet&);  // Not implemented.
};

#endif 
