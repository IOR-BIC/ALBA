/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: albaAgent.cpp,v $
Language:  C++
Date:      $Date: 2005-08-24 16:12:41 $
Version:   $Revision: 1.10 $

=========================================================================*/
#include "albaAgent.h"
#include "vtkObjectFactory.h"

#include "albaEventBase.h"
#include "albaEventBroadcaster.h"
#include "mmuIdFactory.h"
#include <assert.h>

#ifdef ALBA_USE_VTK
#include "vtkCallbackCommand.h"
#include "vtkOldStyleCallbackCommand.h"
#include "vtkObject.h"
#include "vtkALBASmartPointer.h"
#endif


//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
ALBA_ID_IMP(albaAgent::AGENT_INITIALIZE);

//------------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaAgent);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaAgent::albaAgent():m_Initialized(false)
//------------------------------------------------------------------------------
{
#ifdef ALBA_USE_VTK
  m_EventCallbackCommand=NULL;
#endif
}

//------------------------------------------------------------------------------
albaAgent::~albaAgent()
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
int albaAgent::Initialize()
//------------------------------------------------------------------------------
{
  if (m_Initialized)
    return ALBA_ERROR;

  if (InternalInitialize() == ALBA_OK)
  {
    m_Initialized=true;
    return ALBA_OK;
  }

  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
void albaAgent::Shutdown()
//------------------------------------------------------------------------------
{
  if (m_Initialized)
  {
    InternalShutdown();
    m_Initialized = false;
  }
}

//------------------------------------------------------------------------------
bool albaAgent::HasObservers(albaID channel)
//------------------------------------------------------------------------------
{
  assert(channel>=0);
	if (albaEventBroadcaster::HasObservers())
		return true;

  for (int i=0;i<m_Channels.size();i++)
    if (m_Channels[i]->GetChannel() == channel)
      return m_Channels[i]->HasObservers();

  return false;
}

//------------------------------------------------------------------------------
void albaAgent::GetObservers(albaID channel,std::vector<albaObserver *> &olist)
//------------------------------------------------------------------------------
{
  olist.clear();
	if (channel == MCH_UP)
	{
		albaEventBroadcaster::GetObservers(olist);
	}
	else
	{
		for (int i = 0; i < m_Channels.size(); i++)
			if (m_Channels[i]->GetChannel() == channel)
				m_Channels[i]->GetObservers(olist);
	}
}
//------------------------------------------------------------------------------
void albaAgent::AddObserver(albaObserver *listener,albaID channel)
//------------------------------------------------------------------------------
{
  assert(listener);
  assert(listener!=this); // avoid loops

  if (channel == MCH_UP)
  {
    albaEventBroadcaster::AddObserver(listener);
    return;
  }

  // add as observer of the channel with the right channel ID
  int i;
  for (i=0;i<m_Channels.size();i++)
  {
    if (m_Channels[i]->GetChannel()==channel)
    {
      m_Channels[i]->AddObserver(listener);
      return;
    }
  }

  // if no channel with the right channel Id was found
  // create a new one.
  albaEventBroadcaster *newch =new albaEventBroadcaster();
  newch->SetChannel(channel);
  m_Channels.push_back(newch);

  newch->AddObserver(listener);
}

//------------------------------------------------------------------------------
void albaAgent::RemoveObserver(albaObserver *listener)
//------------------------------------------------------------------------------
{  
  assert(listener);
  if (!listener)
  {
    albaErrorMacro("NULL observer provided, cannot RemoveObserver it.");
    return;
  }

	albaEventBroadcaster::RemoveObserver(listener);

  // remove from any channel
  for (int i=0;i<m_Channels.size();i++)
  {
    assert(m_Channels[i]);
    m_Channels[i]->RemoveObserver(listener);
  }
}
//------------------------------------------------------------------------------
void albaAgent::RemoveAllObservers()
//------------------------------------------------------------------------------
{
	albaEventBroadcaster::RemoveAllObservers();

  for (int i=0;i<m_Channels.size();i++)
  {
    m_Channels[i]->RemoveAllObservers();
  }
}

//------------------------------------------------------------------------------
void albaAgent::OnEvent(albaEventBase *event)
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
void albaAgent::InternalProcessVTKEvents(vtkObject* sender, unsigned long eventid, void* clientdata, void* calldata)
//------------------------------------------------------------------------------
{
  albaAgent* self = reinterpret_cast<albaAgent *>( clientdata );
  self->OnEvent(&albaEventBase(sender,eventid,clientdata,MCH_VTK));
}

//------------------------------------------------------------------------------
void albaAgent::InvokeEvent(void *sender, int id, albaID channel, void *data)
{
  this->InvokeEvent(&albaEventBase(sender,id,data,channel),channel);
}

//------------------------------------------------------------------------------
void albaAgent::InvokeEvent(albaEventBase *event, albaID channel)
//------------------------------------------------------------------------------
{
  if ( channel == -1 )
    channel=event->GetChannel();

  if (channel==MCH_UP)
  {
		albaEventBroadcaster::InvokeEvent(event); // send event to listener
    return;
  }

  for (int i=0;i<m_Channels.size();i++)
  {
    if (m_Channels[i]->GetChannel()==channel || channel == MCH_ANY)
      m_Channels[i]->InvokeEvent(event); // broadcast event on the right channel
  }
}

//------------------------------------------------------------------------------
void albaAgent::InvokeEvent(albaEventBase &event, albaID channel)
//------------------------------------------------------------------------------
{
  InvokeEvent(&event,channel);
}
