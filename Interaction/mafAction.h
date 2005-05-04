/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAction.h,v $
  Language:  C++
  Date:      $Date: 2005-05-04 16:27:46 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafAction_h
#define __mafAction_h

#ifdef __GNUG__
    #pragma interface "mafAction.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mflAgent.h"
#include "vtkCallbackCommand.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
template <class T> class vtkTemplatedList;
class mafDevice;
class mafInteractor;
class mflXMLWriter;
class vtkXMLDataElement;
class vtkXMLDataParser;
class vtkRenderer;

/** this class routes events from devices to interactors.
    Detailed description not yet written...
 */
class mafAction : public mflAgent
{
public:
    /** @ingroup Events */
  /** @{ */
  /** 
      Request for binding a device to an action. Event type is
      mafEventBase and event argument is the device ID to be 
      resolved in the device pointer by the InteractionManager */
  MFL_EVT_DEC(BindDeviceToActionEvent);
  MFL_EVT_DEC(QueryForConnectedDeviceEvent) ///< Issued by interactors to query for connected devices. @sa ConnectedDeviceEvent
  MFL_EVT_DEC(PluggedDeviceEvent) ///< Issued by devices when connected to actions or when queried.
  MFL_EVT_DEC(UnPluggedDeviceEvent) ///< Issued by devices when connected to actions or when queried.

  enum { SHARED_ACTION = 0, EXCLUSIVE_ACTION };
  
  static mafAction *New();
  vtkTypeMacro(mafAction,mflAgent);

  /**  
    Set/Get the current renderer. The current renderer is set to each new interactors
    connected to the action */
  virtual void SetRenderer(vtkRenderer *ren);
  vtkRenderer *GetRenderer() {return Renderer;}

  /** 
    Set the type of action. semantic to be defined... */
  int GetType() {return Type;}
  void SetType(int t) {if (t==SHARED_ACTION||t==EXCLUSIVE_ACTION) Type=t;Modified();}
  void SetTypeToShared() {SetType(SHARED_ACTION);}
  void SetTypeToExclusive() {SetType(EXCLUSIVE_ACTION);}

  /** To be called when a new renderer is selected to update all interactors*/
  //void SelectRenderer(vtkRenderer *view)
  
  /** Bind/Unbind a device to this action */
  void BindDevice(mafDevice *device);
  void UnBindDevice(mafDevice *device);

  /** Bind/Unbind an interactor to this action */
  void BindInteractor(mafInteractor *inter);
  void UnBindInteractor(mafInteractor *inter);

  /** Get list of devices assigned to this action */
  vtkTemplatedList<mafDevice> *GetDevices() {return Devices;}

  /** 
    Store of action's bindings to an XML file. To store an XML Writer must 
    be passed as argument. */
  virtual int Store(mflXMLWriter *writer);

  /** 
    Restore of action's bindings from an XML file. To restore, the node
    of the XML structure from where starting the restoring must be passed
    as argument. */
  virtual int Restore(vtkXMLDataElement *node,vtkXMLDataParser *parser);

  /**  Redefined to answer queries about connected devices */
  virtual void ProcessEvent(mflEvent *event,mafID channel=mflAgent::DefaultChannel);

protected:
  mafAction();
  virtual ~mafAction();

  int   Type;
  vtkRenderer *Renderer;
  vtkTemplatedList<mafDevice> *Devices;

private:
  mafAction(const mafAction&);  // Not implemented.
  void operator=(const mafAction&);  // Not implemented.
};

#endif 
