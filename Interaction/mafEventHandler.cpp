/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: mafEventHandler.cpp,v $
Language:  C++
Date:      $Date: 2005-04-28 16:10:11 $
Version:   $Revision: 1.1 $

=========================================================================*/
/*#ifdef __GNUG__
    #pragma implementation "mafEventHandler.h"
#endif
*/
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h" 
#include "wx/event.h"
#pragma hdrstop

#include "mafDecl.h"

#include "mafEventHandler.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
// Constants
enum {ID_DISPATCH_EVENT = MINID};
enum {wxEVT_MAF = wxEVT_USER_FIRST+1234};
//----------------------------------------------------------------------------

class mafWXEventHandler:public wxEvtHandler
{
protected:
  virtual bool ProcessEvent(wxEvent& event);
public:
  mafEventHandler *Dispatcher;
};


bool mafWXEventHandler::ProcessEvent(wxEvent& event)
{
  if (event.GetId()==ID_DISPATCH_EVENT)
  {
    if (this->Dispatcher)
    {
      this->Dispatcher->DispatchEvents();
    }
  }
  
  return true;
}

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkStandardNewMacro(mafEventHandler);

//------------------------------------------------------------------------------
mafEventHandler::mafEventHandler()
//------------------------------------------------------------------------------
{
  m_EventHandler = new mafWXEventHandler;
  m_EventHandler->Dispatcher=this;
}

//------------------------------------------------------------------------------
mafEventHandler::~mafEventHandler()
//------------------------------------------------------------------------------
{
  delete m_EventHandler;
  //vtkGenericWarningMacro("Destroying mafEventHandler");
}

//------------------------------------------------------------------------------
void mafEventHandler::RequestForDispatching()
//------------------------------------------------------------------------------
{
  wxIdleEvent wx_event;
  wx_event.SetId(ID_DISPATCH_EVENT);
  wxPostEvent(m_EventHandler,wx_event);
}
