/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractorCompositorDevice.h,v $
  Language:  C++
  Date:      $Date: 2009-12-17 11:46:39 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafInteractorCompositorDevice_h
#define __mafInteractorCompositorDevice_h

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "mafInteractor.h"
#include "mafEvent.h"

#include <list>
#include <string>
#include <utility>

//----------------------------------------------------------------------------
// Forward references:
//----------------------------------------------------------------------------

/** Compose more instances of mafInteractors specific for different devices
This interactor composes different instances of interactors specific for different devices.
Each interactor is associated with a device type, specified as a string type, which is used
to discriminate if the source of the event is of the right type with respect to the interactor 
(only events from a device of that type are sent to that interactor).
@sa 
  - mafInteractorGeneric for an example of composing interactor
  - mafInteractorCompositorMouse for a compositor who joins different interactors to buttons
*/
class mafInteractorCompositorDevice : public mafInteractor
{
public:
  static mafInteractorCompositorDevice *New();
  vtkTypeMacro(mafInteractorCompositorDevice, mafInteractor);

  /** 
    Append an interactor to the list of composed ones. The interactor is associated
    with a device type (provided as a string argument). Composition oreder is important 
    when composing interactors associated to devices hierarchically related (i.e. an mmdTraker
    and mmdButtonsPad */
  void AppendInteractor(mafInteractor *i,const char *device_type);

  /** 
    Insert an interactor as first element to the list. the interactor is
    anyway associated with a device type. */
  void PrependInteractor(mafInteractor *i,const char *device_type);

  /** remove an interactor from the list of available ones */
  void RemoveInteractor(mafInteractor *i);

  /** return the list of interactors controlled by this class */
  std::list<std::pair<std::string,mafInteractor *> > *GetInteractors() {return &m_InteractorsList;}

  /**  Process events coming from tracker */
  virtual void ProcessEvent(mflEvent *event,mafID channel=mflAgent::DefaultChannel);

  //void SetMListener(mafEventListener *listener) {m_listener = listener;};
  //mafEventListener *GetMListener() {return m_listener;};

protected:

  /** initialization implies initialization of composed interactors */
  virtual int InternalInitialize();

  /** shutdown implies shutdown of composed interactors */
  virtual void InternalShutdown();

  mafInteractorCompositorDevice();
  ~mafInteractorCompositorDevice();

  mafInteractor *m_ActiveInteractor;

  std::list<std::pair<std::string,mafInteractor *> > m_InteractorsList;

  // mafInteractorCompositotor listener
  //mafEventListener *m_listener;

private:

  mafInteractorCompositorDevice(const mafInteractorCompositorDevice&);  // Not implemented.
  void operator=(const mafInteractorCompositorDevice&);   // Not implemented.  
};
#endif

