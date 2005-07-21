/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: mafAgent.cpp,v $
Language:  C++
Date:      $Date: 2005-07-21 12:00:05 $
Version:   $Revision: 1.9 $

=========================================================================*/
#include "mafAgent.h"
#include "vtkObjectFactory.h"

#include "mafEventBase.h"
#include "mafEventSource.h"
#include "mmuIdFactory.h"
#include <assert.h>

#ifdef MAF_USE_VTK
#include "vtkCallbackCommand.h"
#include "vtkOldStyleCallbackCommand.h"
#include "vtkObject.h"
#include "vtkMAFSmartPointer.h"
#endif


//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafAgent::AGENT_INITIALIZE);

//------------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafAgent);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAgent::mafAgent():m_Initialized(false)
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  m_EventCallbackCommand=NULL;
#endif
}

//------------------------------------------------------------------------------
mafAgent::~mafAgent()
//------------------------------------------------------------------------------
{
  // Try to destroy the command, but since UnPlug sources must be done explicitly and
  // I don't keep the list of sources, it could be this command is still connected 
  // to some sources. For this reason I have to reset the callback pointer, or an access violation
  // could be rosed (the callback command doesn't register the object! [for luck...]).
  if (m_EventCallbackCommand)
  {
    m_EventCallbackCommand->SetClientData(NULL); 
    m_EventCallbackCommand->SetCallback(NULL);
    m_EventCallbackCommand->Delete();
  }
  for (int i=0;i<m_Channels.size();i++)
  {
    cppDEL(m_Channels[i]);
  }
}

//------------------------------------------------------------------------------
int mafAgent::Initialize()
//------------------------------------------------------------------------------
{
  if (m_Initialized)
    return MAF_ERROR;

  if (InternalInitialize() == MAF_OK)
  {
    m_Initialized=true;
    return MAF_OK;
  }

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
void mafAgent::Shutdown()
//------------------------------------------------------------------------------
{
  if (m_Initialized)
  {
    InternalShutdown();
    m_Initialized = false;
  }
}

//------------------------------------------------------------------------------
bool mafAgent::HasObservers(mafID channel)
//------------------------------------------------------------------------------
{
  assert(channel>=0);
  return (m_Channels.size()>channel && m_Channels[channel]!=NULL) ? m_Channels[channel]->HasObservers() : false;
}

//------------------------------------------------------------------------------
void mafAgent::GetObservers(mafID channel,std::vector<mafObserver *> &olist)
//------------------------------------------------------------------------------
{
  olist.clear();
  if (m_Channels.size()>channel && m_Channels[channel]!=NULL)
  {
    m_Channels[channel]->GetObservers(olist);
  }
}
//------------------------------------------------------------------------------
void mafAgent::AddObserver(mafObserver *listener,mafID channel, int priority)
//------------------------------------------------------------------------------
{
  assert(listener);
  assert(listener!=this); // avoid loops

  if (channel == MCH_UP)
  {
    SetListener(listener);
    return;
  }

  // add as observer of the channel with the right channel ID
  int i;
  for (i=0;i<m_Channels.size();i++)
  {
    if (m_Channels[i]->GetChannel()==channel)
    {
      m_Channels[i]->AddObserver(listener,priority);
      return;
    }
  }

  // if no channel with the right channel Id was found
  // create a new one.
  mafEventSource *newch =new mafEventSource();
  newch->SetChannel(channel);
  m_Channels.push_back(newch);

  newch->AddObserver(listener,priority);
}

//------------------------------------------------------------------------------
void mafAgent::RemoveObserver(mafObserver *listener)
//------------------------------------------------------------------------------
{  
  assert(listener);
  if (!listener)
  {
    mafErrorMacro("NULL observer provided, cannot RemoveObserver it.");
    return;
  }

  // remove from any channel
  for (int i=0;i<m_Channels.size();i++)
  {
    assert(m_Channels[i]);
    m_Channels[i]->RemoveObserver(listener);
  }
}
//------------------------------------------------------------------------------
void mafAgent::RemoveAllObservers()
//------------------------------------------------------------------------------
{
  for (int i=0;i<m_Channels.size();i++)
  {
    m_Channels[i]->RemoveAllObservers();
  }
}

//------------------------------------------------------------------------------
void mafAgent::UnPlugEventSource(mafAgent *source)
//------------------------------------------------------------------------------
{
  assert(source);
  if (source)
  {
    source->RemoveObserver(this);
    //this->Modified();
  }
}

//------------------------------------------------------------------------------
void mafAgent::PlugEventSource(mafAgent *source,mafID channel, int priority)
//------------------------------------------------------------------------------
{
  assert(source);
  source->AddObserver(this,channel,priority);
}

#ifdef MAF_USE_VTK
//------------------------------------------------------------------------------
void mafAgent::PlugEventSource(vtkObject *source,mafID channel, int priority)
//------------------------------------------------------------------------------
{
  if (!m_EventCallbackCommand) // Alloc this object only when necessary
  {
    vtkNEW(m_EventCallbackCommand); // to this object is delegated the due to receive incoming callbacks
    m_EventCallbackCommand->SetClientData(this); 
    m_EventCallbackCommand->SetCallback(mafAgent::InternalProcessVTKEvents);
  }

  assert(source);
  if (!source)
  {
    mafErrorMacro("NULL source provided, cannot Plug it.");
  }

  source->AddObserver(channel,m_EventCallbackCommand,priority); 
}

//------------------------------------------------------------------------------
int mafAgent::PlugEventSource(vtkObject *source,void (*f)(void *), void *self, mafID channel, int priority)
//------------------------------------------------------------------------------
{ 
  assert(source);
  assert(f);

  if (f==NULL)
    return 0;

  vtkMAFSmartPointer<vtkOldStyleCallbackCommand> router; // to this object is delegated the due to receive incoming callbacks

  router->SetClientData(self);
  router->SetCallback(f);
  
  return source->AddObserver(channel,router,priority);
}

//------------------------------------------------------------------------------
void mafAgent::UnPlugEventSource(vtkObject *source)
//------------------------------------------------------------------------------
{
  assert(source);
  if (source)
  {
    source->RemoveObserver(m_EventCallbackCommand);
  }
  else
  {
    mafErrorMacro("NULL source provided, cannot UnPlug it.");
  }
}
#endif // MAF_USE_VTK

//------------------------------------------------------------------------------
void mafAgent::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);
 
  if (event->GetId()==AGENT_INITIALIZE)
    Initialize();
  
  if (event->GetSender()!=this) // avoid loops!
  {
    // default behavior is to pass on the event on the same channel
    InvokeEvent(event,event->GetChannel());
  }
}

//------------------------------------------------------------------------------
void mafAgent::InternalProcessVTKEvents(vtkObject* sender, unsigned long eventid, void* clientdata, void* calldata)
//------------------------------------------------------------------------------
{
  mafAgent* self = reinterpret_cast<mafAgent *>( clientdata );
  self->OnEvent(&mafEventBase(sender,eventid,clientdata,MCH_VTK));
}

//------------------------------------------------------------------------------
void mafAgent::InvokeEvent(int id, mafID channel,void *data)
//------------------------------------------------------------------------------
{
  this->InvokeEvent(&mafEventBase(this,id,data,channel),channel);
}

//------------------------------------------------------------------------------
void mafAgent::InvokeEvent(mafEventBase *event, mafID channel)
//------------------------------------------------------------------------------
{
  if ( channel == -1 )
    channel=event->GetChannel();

  if (channel==MCH_UP)
  {
    mafEventSender::InvokeEvent(event); // send event to listener
    return;
  }

  for (int i=0;i<m_Channels.size();i++)
  {
    if (m_Channels[i]->GetChannel()==channel || channel == MCH_ANY)
      m_Channels[i]->InvokeEvent(event); // broadcast event on the right channel
  }
}

//------------------------------------------------------------------------------
void mafAgent::InvokeEvent(mafEventBase &event, mafID channel)
//------------------------------------------------------------------------------
{
  InvokeEvent(&event,channel);
}
