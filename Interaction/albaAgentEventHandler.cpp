/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: albaAgentEventHandler.cpp,v $
Language:  C++
Date:      $Date: 2006-06-14 14:46:33 $
Version:   $Revision: 1.4 $

=========================================================================*/

#include "albaDefines.h" //SIL


#include "albaDecl.h"
#include "albaAgentEventHandler.h"

//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
enum DISPATCH_ENUM {ID_DISPATCH_EVENT = MINID};
enum WX_EVENT_ALBA { wxEVT_ALBA = 12000 /* SIL: wxEVT_USER_FIRST*/ + 1234 };

//----------------------------------------------------------------------------
class albaWXEventHandler:public wxEvtHandler
//----------------------------------------------------------------------------
{
protected:
  virtual bool ProcessEvent(wxEvent& event);
public:
  albaAgentEventHandler *m_Dispatcher;
};

//----------------------------------------------------------------------------
bool albaWXEventHandler::ProcessEvent(wxEvent& event)
//----------------------------------------------------------------------------
{
  if (event.GetId()==ID_DISPATCH_EVENT)
  {
    if (m_Dispatcher)
    {
      m_Dispatcher->DispatchEvents();
    }
  }
  
  return true;
}

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaAgentEventHandler);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaAgentEventHandler::albaAgentEventHandler()
//------------------------------------------------------------------------------
{
  m_EventHandler = new albaWXEventHandler;
  m_EventHandler->m_Dispatcher=this;
}

//------------------------------------------------------------------------------
albaAgentEventHandler::~albaAgentEventHandler()
//------------------------------------------------------------------------------
{
  delete m_EventHandler;
  //albaWarningMacro("Destroying albaAgentEventHandler");
}

//------------------------------------------------------------------------------
void albaAgentEventHandler::RequestForDispatching()
//------------------------------------------------------------------------------
{
  wxIdleEvent wx_event;
  wx_event.SetId(ID_DISPATCH_EVENT);
  wxPostEvent(m_EventHandler,wx_event);
}
