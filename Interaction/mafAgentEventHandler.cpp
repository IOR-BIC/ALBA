/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: mafAgentEventHandler.cpp,v $
Language:  C++
Date:      $Date: 2005-06-23 16:57:14 $
Version:   $Revision: 1.3 $

=========================================================================*/

#include "mafDecl.h"
#include "mafAgentEventHandler.h"

//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
enum DISPATCH_ENUM {ID_DISPATCH_EVENT = MINID};
enum WX_EVENT_MAF {wxEVT_MAF = wxEVT_USER_FIRST+1234};

//----------------------------------------------------------------------------
class mafWXEventHandler:public wxEvtHandler
//----------------------------------------------------------------------------
{
protected:
  virtual bool ProcessEvent(wxEvent& event);
public:
  mafAgentEventHandler *m_Dispatcher;
};

//----------------------------------------------------------------------------
bool mafWXEventHandler::ProcessEvent(wxEvent& event)
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
mafCxxTypeMacro(mafAgentEventHandler);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAgentEventHandler::mafAgentEventHandler()
//------------------------------------------------------------------------------
{
  m_EventHandler = new mafWXEventHandler;
  m_EventHandler->m_Dispatcher=this;
}

//------------------------------------------------------------------------------
mafAgentEventHandler::~mafAgentEventHandler()
//------------------------------------------------------------------------------
{
  delete m_EventHandler;
  //mafWarningMacro("Destroying mafAgentEventHandler");
}

//------------------------------------------------------------------------------
void mafAgentEventHandler::RequestForDispatching()
//------------------------------------------------------------------------------
{
  wxIdleEvent wx_event;
  wx_event.SetId(ID_DISPATCH_EVENT);
  wxPostEvent(m_EventHandler,wx_event);
}
