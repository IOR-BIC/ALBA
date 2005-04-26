/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: mafAgent.cpp,v $
Language:  C++
Date:      $Date: 2005-04-26 18:32:33 $
Version:   $Revision: 1.1 $

=========================================================================*/
#include "mafAgent.h"
#include "vtkObjectFactory.h"

#include "mafEventBase.h"
#include "mflDefines.h"
#include "vtkOldStyleCallbackCommand.h"

#include <assert.h>


//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(MCH_UP)
MAF_ID_IMP(mafAgent::DownStreamChannel)
MAF_ID_IMP(mafAgent::InitializeEvent);
MAF_ID_IMP(mafAgent::VTKObserverChannel); 

//------------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafAgent);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAgent::mafAgent():m_m_Listener(NULL),m_EventCallbackCommand(NULL),m_Initialized(false)
//------------------------------------------------------------------------------
{ 
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
void mafAgent::UnPlugEventSource(mafAgent *source)
//------------------------------------------------------------------------------
{
  assert(source);
  if (!source)
  {
    mafErrorMacro("NULL source provided, cannot UnPlug it.");
    return;
  }

  // remove from any channel
  for (int i=0;i<source->m_Channels.size();i++)
  {
    assert(source->m_Channels[i]);
    source->m_Channels[i]->RemoveObserver(this);
  }
}

//------------------------------------------------------------------------------
void mafAgent::UnPlugAllListeners()
//------------------------------------------------------------------------------
{
  for (int i=0;i<m_Channels[i].size();i++)
  {
    m_Channels[i]->RemoveObserver();
  }
}

//------------------------------------------------------------------------------
void mafAgent::PlugEventSource(mafAgent *source,mafID channel, float priority)
//------------------------------------------------------------------------------
{
  assert(source);
  if (!source)
  {
    mafErrorMacro("NULL source provided, cannot Plug it.");
  }

  assert(source!=this);
  // add as observer of the channel with the right channel ID
  int i;
  for (i=0;i<source->m_Channels.size();i++)
  {
    if (source->m_Channels[i]->GetChannel()==channel)
    {
      source->m_Channels[i]->AddObserver(this,priority);
      return;
    }
  }
  
  // if no channel with the right channel Id was found
  // create a new one.
  mafEventSource *newch =new mafEventSource();
  newch->SetChannel(channel);
  source->m_Channels.push_back(newch);
  
  newch->AddObserver(this,priority);
}

#ifdef MAF_USE_VTK
//------------------------------------------------------------------------------
void mafAgent::PlugEventSource(vtkObject *source,unsigned long channel, float priority)
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

  assert(source!=this);
  source->AddObserver(channel,m_EventCallbackCommand,priority); 
}

//------------------------------------------------------------------------------
int mafAgent::PlugEventSource(vtkObject *source,void (*f)(void *), void *self, unsigned long channel, float priority)
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
 
  if (event->GetID()==InitializeEvent)
    Initialize();
  
  if (event->GetSender()!=this) // avoid loops!
  {
    // default behavior is to pass on the event on the same channel
    ForwardEvent(event,event->GetChannel());
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
void mafAgent::PlugListener(mafAgent *listener,unsigned long channel, float priority)
//------------------------------------------------------------------------------
{
  if (listener)
  {
    assert(listener!=this);
    listener->PlugEventSource(this,channel,priority);
    //this->Modified();
  }
}

//------------------------------------------------------------------------------
void mafAgent::UnPlugListener(mafAgent *listener)
//------------------------------------------------------------------------------
{  
  if (listener)
  {
    if (listener == m_Listener)
      this->SetListener(NULL);

    listener->UnPlugEventSource(this);
    //this->Modified();
  }
}

//------------------------------------------------------------------------------
void mafAgent::ForwardEvent(int id, mafID channel,void *data)
//------------------------------------------------------------------------------
{
  this->ForwardEvent(&mafEventBase(this,id,data,channel),channel);
}

//------------------------------------------------------------------------------
void mafAgent::ForwardEvent(mafEventBase *event, mafID channel)
//------------------------------------------------------------------------------
{
 for (int i=0;i<m_Channels.size();i++)
 {
   if (m_Channels[i].GetChannel()==channel || channel == MCH_ANY)
     m_Channels[i].InvokeEvent(event); // broadcast event on the right channel
 }
}