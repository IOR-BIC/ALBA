/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdTrackerSet.h,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:57 $
  Version:   $Revision: 1.1 $
  Authors:   Michele Diegoli & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmdTrackerSet_h
#define __mmdTrackerSet_h

#ifdef __GNUG__
    #pragma interface "mmdTrackerSet.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafDeviceSet.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmdTracker;
class vtkPlaneSource;

/** class for managing multiple tracker device
  This class manages multiple track device, e.g. tracker devices producing
  multiple pose matrixes. Each pose matrix stream is identified as a "subdevice".
  Subdevices should be created in the InternalInitialize.
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
  MFL_EVT_DEC(AddDeviceEvent);
  MFL_EVT_DEC(RemoveDeviceEvent);

  static mmdTrackerSet *New();
  vtkTypeMacro(mmdTrackerSet,mafDeviceSet);

  // Description:
  // Return the numnber of trackers composing this tracker set
  int GetNumberOfTrackers() {return this->GetNumberOfDevices();}

  // Description:
  // Add a new device, and return an ID. If the returned ID==0 the operation has failed!
  void AddDevice(mmdTracker *device);

  // Description:
  // Return a device handler given its ID or name.
  mmdTracker *GetDevice(unsigned long id);
  mmdTracker *GetDevice(const char *name);
  
  // Description:
  // 
  //virtual void ProcessEvent(mflEvent *event);

protected:
  mmdTrackerSet();
  virtual ~mmdTrackerSet();

  // Description:
  // redefined as proteced functions
  void AddDevice(mafDevice *device) {Superclass::AddDevice(device);}
  int RemoveDevice(mafDevice *device) {return this->Superclass::RemoveDevice(device);}

private:
  mmdTrackerSet(const mmdTrackerSet&);  // Not implemented.
  void operator=(const mmdTrackerSet&);  // Not implemented.
};

#endif 
