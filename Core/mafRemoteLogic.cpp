/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafRemoteLogic.cpp,v $
Language:  C++
Date:      $Date: 2006-11-21 16:25:29 $
Version:   $Revision: 1.8 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafRemoteLogic.h"
#include <wx/tokenzr.h>

#include "mafDecl.h"
#include "mafViewManager.h"
#include "mafOpManager.h"
#include "mafView.h"
#include "mafOp.h"

#include "mafEventBase.h"
#include "mmdMouse.h"
#include "mmdRemoteMouse.h"
#include "mmdClientMAF.h"

#include "mafNodeIterator.h"
#include "mafVME.h"
#include "mafEventInteraction.h"
#include "mafTransform.h"

#ifdef __WIN32__
  #include "typeinfo.h"
#endif

//----------------------------------------------------------------------------
mafRemoteLogic::mafRemoteLogic(mafObserver *Listener, mafViewManager *view_manager, mafOpManager *operation_manager)
//----------------------------------------------------------------------------
{
  m_Listener          = Listener;
  m_ViewManager       = view_manager;
  m_OperationManager  = operation_manager;
  
  m_RemoteMsg         = "";
  m_CommandSeparator  = ":";

  m_RemoteMouse = NULL;

  m_ViewManager->SetRemoteListener(this);
  m_OperationManager->SetRemoteListener(this);
}
//----------------------------------------------------------------------------
mafRemoteLogic::~mafRemoteLogic() 
//----------------------------------------------------------------------------
{
  m_ClientUnit->RemoveObserver(this);
  m_OperationManager->SetRemoteListener(NULL);
  m_ViewManager->SetRemoteListener(NULL);

  m_ViewManager      = NULL;
  m_OperationManager = NULL;
}
//----------------------------------------------------------------------------
void mafRemoteLogic::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
//  if(m_ClientUnit && !m_ClientUnit->IsConnected() && m_ClientUnit->IsBusy())
//    return;

  if (mafEvent *e = mafEvent::SafeDownCast(event)) 
  {
    mafID id = e->GetId();
    if(id == ClientUnit::RemoteMessage_ID) 
    {
      mafString msg = *e->GetString();
      RemoteMessage(msg,e->GetBool());
    }
    else if(event->GetChannel() == REMOTE_COMMAND_CHANNEL)
    {
      m_RemoteMsg = "";
      // build the command event
      if(id == VIEW_DELETE || id == VIEW_SELECTED)
      {
        mafView *view = e->GetView();
        mafString view_id;
        view_id << (view->m_Id - VIEW_START);
        mafString view_mult;
        view_mult << view->m_Mult;
        if(id == VIEW_DELETE)
          m_RemoteMsg = "VIEW_DELETE";
        else if(id == VIEW_SELECTED)
          m_RemoteMsg = "VIEW_SELECTED";
        m_RemoteMsg += m_CommandSeparator;
        m_RemoteMsg += view_id;
        m_RemoteMsg += m_CommandSeparator;
        m_RemoteMsg += view_mult;
      }
      else if (id == VIEW_CREATE)
      {
        mafView *view = e->GetView();
        mafString view_id;
        view_id << (view->m_Id - VIEW_START);
        m_RemoteMsg = "VIEW_CREATE";
        m_RemoteMsg += m_CommandSeparator;
        m_RemoteMsg += view_id;
      }
      else if(id == mafOpManager::OPERATION_INTERFACE_EVENT)
      {
        wxString widget_id = wxString::Format("%d",e->GetArg());
        WidgetDataType widget_data;
        e->GetWidgetData(widget_data);
        switch(widget_data.dType) 
        {
          case INT_DATA:
          {
            m_RemoteMsg = "WidgetInt";
            m_RemoteMsg << m_CommandSeparator;
            m_RemoteMsg << widget_id;
            m_RemoteMsg << m_CommandSeparator;
            m_RemoteMsg << widget_data.iValue;
          }
          break;
          case FLOAT_DATA:
          {
            m_RemoteMsg = "WidgetFloat";
            m_RemoteMsg << m_CommandSeparator;
            m_RemoteMsg << widget_id;
            m_RemoteMsg << m_CommandSeparator;
            m_RemoteMsg << widget_data.fValue;
          }
          break;
          case DOUBLE_DATA:
          {
            m_RemoteMsg = "WidgetDouble";
            m_RemoteMsg << m_CommandSeparator;
            m_RemoteMsg << widget_id;
            m_RemoteMsg << m_CommandSeparator;
            m_RemoteMsg << widget_data.dValue;
          }
          break;
          case STRING_DATA:
          {
            m_RemoteMsg = "WidgetString";
            m_RemoteMsg << m_CommandSeparator;
            m_RemoteMsg << widget_id;
            m_RemoteMsg << m_CommandSeparator;
            m_RemoteMsg << widget_data.sValue;
          }
          break;
          default:
            m_RemoteMsg = "WidgetID";
            m_RemoteMsg += m_CommandSeparator;
            m_RemoteMsg += widget_id;
          break;
        }
      }
      else if(id == mafOpManager::RUN_OPERATION_EVENT)
      {
        m_RemoteMsg = "RunOperation";
        m_RemoteMsg << m_CommandSeparator;
        m_RemoteMsg << e->GetArg();
      }
      else if (id == mmdButtonsPad::BUTTON_DOWN ||
        id == mmdButtonsPad::BUTTON_UP   ||
        id == mmdMouse::MOUSE_2D_MOVE)
      {
        unsigned long modifiers = ((mafEventInteraction *)event)->GetModifiers();
        double pos[2];
        ((mafEventInteraction *)event)->Get2DPosition(pos);
        m_RemoteMsg = "MouseDevice";
        m_RemoteMsg << m_CommandSeparator;
        m_RemoteMsg << id;
        m_RemoteMsg << m_CommandSeparator;
        m_RemoteMsg << pos[0];
        m_RemoteMsg << m_CommandSeparator;
        m_RemoteMsg << pos[1];
        m_RemoteMsg << m_CommandSeparator;
        m_RemoteMsg << (int)modifiers;
        m_RemoteMsg << m_CommandSeparator;
        m_RemoteMsg << ((mafEventInteraction *)event)->GetButton();
      }
      else if (id == mmdMouse::MOUSE_CHAR_EVENT)
      {
      }

      if(!m_RemoteMsg.IsEmpty()) // Send the message to the server
        RemoteMessage(m_RemoteMsg);
      return;
    }
  }
}
//----------------------------------------------------------------------------
void mafRemoteLogic::RemoteMessage(mafString &cmd, bool to_server)
//----------------------------------------------------------------------------
{
  if(to_server)
  {
    m_ClientUnit->SendMessageToServer(cmd);
  }
  else
  {
    // unpack the message and send the command to MAF
    wxString delimiter = m_CommandSeparator.GetCStr();
    wxString cmd_string = cmd.GetCStr();
    wxStringTokenizer tkz(cmd_string, delimiter);
    int token = tkz.CountTokens();
    wxString command  = tkz.GetNextToken();
    wxString data_cmd = "";
    if(tkz.HasMoreTokens())
      data_cmd = tkz.GetNextToken();
    
    if(command == "VIEW_CREATE")
    {
      long v_id;
      data_cmd.ToLong(&v_id);
      m_ViewManager->m_FromRemote = true;
      mafEventMacro(mafEvent(this,VIEW_CREATE,(long)(v_id + VIEW_START)));
      if (m_RemoteMouse)
      {
        mafEventBase eb(this,VIEW_SELECT, m_ViewManager->GetSelectedView(), REMOTE_COMMAND_CHANNEL);
        m_RemoteMouse->OnEvent(&eb);
      }
      m_ViewManager->m_FromRemote = false;
    }
    else if(command == "SynchronizeView")
    {
      long v_id, v_mult;
      data_cmd.ToLong(&v_id);
      data_cmd = tkz.GetNextToken();
      data_cmd.ToLong(&v_mult);
      if(m_ViewManager->GetView(v_id,v_mult) == NULL)
      {
        m_ViewManager->m_FromRemote = true;
        mafEventMacro(mafEvent(this,VIEW_CREATE,v_id));
        m_ViewManager->m_FromRemote = false;
      }
      while(tkz.HasMoreTokens())
      {
        data_cmd = tkz.GetNextToken();
        data_cmd.ToLong(&v_id);
        data_cmd = tkz.GetNextToken();
        data_cmd.ToLong(&v_mult);
        if(m_ViewManager->GetView(v_id,v_mult) == NULL)
        {
          m_ViewManager->m_FromRemote = true;
          mafEventMacro(mafEvent(this,VIEW_CREATE,v_id));
          m_ViewManager->m_FromRemote = false;
        }
      }
    }
    else if (command == "GetViewLayout")
    {
      mafView *v = m_ViewManager->GetList();
      bool send_msg = false;
      mafString sync_cmd = "SynchronizeView";

      for(; v; v = v->m_Next) // find previous(view)
      {
        send_msg = true;
        sync_cmd << m_CommandSeparator;
        sync_cmd << v->m_Id;
        sync_cmd << m_CommandSeparator;
        sync_cmd << v->m_Mult;
      }

      if(send_msg)
        m_ClientUnit->SendMessageToServer(sync_cmd);
    }
    else if (command == "GetViewList")
    {
      mafView **v = m_ViewManager->GetListTemplate();
      bool send_msg = false;
      mafString sync_cmd = "GetViewList";
      int i = 0;
      while (v[i] != NULL) 
      {
        send_msg = true;
        sync_cmd << m_CommandSeparator;
        sync_cmd << v[i]->GetLabel().c_str();
        i++;
      }

      if(send_msg)
        m_ClientUnit->SendMessageToServer(sync_cmd);
    }
    else if (command == "GetViewSelected")
    {
      mafView *view = m_ViewManager->GetSelectedView();
      if (view) 
      {
        mafString view_id;
        view_id << (view->m_Id - VIEW_START);
        mafString view_mult;
        view_mult << view->m_Mult;
        m_RemoteMsg = "VIEW_SELECTED";
        m_RemoteMsg += m_CommandSeparator;
        m_RemoteMsg += view_id;
        m_RemoteMsg += m_CommandSeparator;
        m_RemoteMsg += view_mult;
        m_ClientUnit->SendMessageToServer(m_RemoteMsg);
      }
    }
    else if(command == "VIEW_DELETE")
    {
      long v_id, v_mult;
      data_cmd.ToLong(&v_id);
      wxString mult = tkz.GetNextToken();
      mult.ToLong(&v_mult);
      mafView *view = m_ViewManager->GetView(v_id,v_mult);
      if (view)
      {
        view->GetFrame()->Close();
      }
      if (m_RemoteMouse) 
      {
        m_RemoteMouse->OnEvent(&mafEvent(this,VIEW_DELETE,view));
      }
    }
    else if(command == "VIEW_SELECTED")
    {
      long v_id, v_mult;
      data_cmd.ToLong(&v_id);
      wxString mult = tkz.GetNextToken();
      mult.ToLong(&v_mult);
      mafView *view = m_ViewManager->GetView(v_id,v_mult);
      m_ViewManager->m_FromRemote = true;
      if(view != NULL)
      {
        m_ViewManager->Activate(view);
        if (m_RemoteMouse) 
        {
          m_RemoteMouse->OnEvent(&mafEvent(this,VIEW_SELECTED,view));
        }
      }
      m_ViewManager->m_FromRemote = false;
    }
    else if (command == "MaximizeSelectedView")
    {
      mafView *view = m_ViewManager->GetSelectedView();
      if (view)
      {
        m_ViewManager->m_FromRemote = true;
        view->GetFrame()->Maximize();
        m_ViewManager->m_FromRemote = false;
      }
    }
    else if (command == "NormalSizeSelectedView")
    {
      mafView *view = m_ViewManager->GetSelectedView();
      if (view) 
      {
        view->GetFrame()->Maximize(false);
      }
    }
    else if (command == "GetTreeVME")
    {
      m_RemoteMsg = "GetTreeVME";
      mafNode *root = m_ViewManager->GetCurrentRoot();
      mafNodeIterator *iter = root->NewIterator();
      for(mafNode *vme = iter->GetFirstNode(); vme; vme = iter->GetNextNode())
      {
        m_RemoteMsg << m_CommandSeparator;
        m_RemoteMsg << vme->GetName();
        m_RemoteMsg << m_CommandSeparator;
        m_RemoteMsg << vme->GetId();
      }
      iter->Delete();
      m_ClientUnit->SendMessageToServer(m_RemoteMsg);
    }
    else if(command == "VME_SELECT")
    {
      long vme_id;
      data_cmd.ToLong(&vme_id);
      mafEventMacro(mafEvent(this,VME_SELECT,vme_id));
    }
    else if(command == "VME_SHOW")
    {
      long vme_id;
      data_cmd.ToLong(&vme_id);
      mafNode *root = m_ViewManager->GetCurrentRoot();
      mafNode *vme_to_show = root->FindInTreeById(vme_id);
      mafView *v = m_ViewManager->GetSelectedView();
      mafEventMacro(mafEvent(this,VME_SHOW,vme_to_show,v->GetNodePipe(vme_to_show) == NULL));
    }
    else if(command == "CameraReset")
    {
      m_ViewManager->m_FromRemote = true;
      m_ViewManager->CameraReset(data_cmd != "");
      m_ViewManager->m_FromRemote = false;
    }
    else if(command == "RunOperation")
    {
      long op_id;
      mafString op_type;
      m_OperationManager->m_FromRemote = true;
      if (data_cmd.ToLong(&op_id))
      {
        m_OperationManager->OpRun(op_id);
      }
      else
      {
        op_type = data_cmd.c_str();
        m_OperationManager->OpRun(op_type);
      }
      m_OperationManager->m_FromRemote = false;
    }
    else if(command == "WidgetID")
    {
      long widget_id;
      data_cmd.ToLong(&widget_id);
      m_OperationManager->m_FromRemote = true;
      m_OperationManager->GetRunningOperation()->OnEvent(&mafEvent(this,widget_id));
      m_OperationManager->m_FromRemote = false;
    }
    else if(command == "WidgetInt")
    {
      long widget_id;
      data_cmd.ToLong(&widget_id);
      wxString widget_string_value = "";
      if(tkz.HasMoreTokens())
        widget_string_value = tkz.GetNextToken();
      long widget_value;
      widget_string_value.ToLong(&widget_value);
      WidgetDataType w_data;
      w_data.dType  = INT_DATA;
      w_data.iValue = widget_value;
      m_OperationManager->GetRunningOperation()->OpGui()->SetWidgetValue(widget_id, w_data);
    }
    else if(command == "WidgetFloat")
    {
      long widget_id;
      data_cmd.ToLong(&widget_id);
      wxString widget_string_value = "";
      if(tkz.HasMoreTokens())
        widget_string_value = tkz.GetNextToken();
      double widget_value;
      widget_string_value.ToDouble(&widget_value);
      WidgetDataType w_data;
      w_data.dType  = FLOAT_DATA;
      w_data.fValue = widget_value;
      m_OperationManager->GetRunningOperation()->OpGui()->SetWidgetValue(widget_id, w_data);
    }
    else if(command == "WidgetDouble")
    {
      long widget_id;
      data_cmd.ToLong(&widget_id);
      wxString widget_string_value = "";
      if(tkz.HasMoreTokens())
        widget_string_value = tkz.GetNextToken();
      double widget_value;
      widget_string_value.ToDouble(&widget_value);
      WidgetDataType w_data;
      w_data.dType  = DOUBLE_DATA;
      w_data.dValue = widget_value;
      m_OperationManager->GetRunningOperation()->OpGui()->SetWidgetValue(widget_id, w_data);
    }
    else if (command == "WidgetString")
    {
      long widget_id;
      data_cmd.ToLong(&widget_id);
      wxString widget_string_value = "";
      if(tkz.HasMoreTokens())
        widget_string_value = tkz.GetNextToken();
      WidgetDataType w_data;
      w_data.dType  = STRING_DATA;
      w_data.sValue = widget_string_value;
      m_OperationManager->GetRunningOperation()->OpGui()->SetWidgetValue(widget_id, w_data);
    }
    else if (command == "MouseDevice" && m_RemoteMouse)
    {
      unsigned long modifiers;
      long widget_id, button;
      double normalized[2];
      data_cmd.ToLong(&widget_id);
      tkz.GetNextToken().ToDouble(&normalized[0]);
      tkz.GetNextToken().ToDouble(&normalized[1]);
      tkz.GetNextToken().ToULong(&modifiers);
      tkz.GetNextToken().ToLong(&button);

      mafEventInteraction ei;
      ei.SetSender(this);
      ei.SetId(widget_id);
      ei.Set2DPosition(normalized);
      ei.SetModifiers(modifiers);
      ei.SetButton(button);

      m_RemoteMouse->OnEvent(&ei);
    }
    else if (command == "OpenFile")
    {
      mafString fileToOpen;
      fileToOpen = data_cmd.c_str();
      fileToOpen += m_CommandSeparator;
      if(tkz.HasMoreTokens())
        fileToOpen += tkz.GetNextToken().c_str();
      mafEventMacro(mafEvent(this, MENU_FILE_OPEN, &fileToOpen));
    }
  }
}
//----------------------------------------------------------------------------
bool mafRemoteLogic::Connect(wxIPV4address &addr)
//----------------------------------------------------------------------------
{
  return (m_ClientUnit->ConnectClient(addr) == MAF_OK);
}
//----------------------------------------------------------------------------
void mafRemoteLogic::Disconnect()
//----------------------------------------------------------------------------
{
  if (m_ClientUnit->IsConnected())
  {
    m_ClientUnit->DisconnectClient();
  }
}
//----------------------------------------------------------------------------
void mafRemoteLogic::SetRemoteMouse(mmdRemoteMouse *rMouse)
//----------------------------------------------------------------------------
{
  m_RemoteMouse = rMouse;
}
//----------------------------------------------------------------------------
void mafRemoteLogic::SetClientUnit(mmdClientMAF *clientUnit)
//----------------------------------------------------------------------------
{
  m_ClientUnit = clientUnit;
  m_ClientUnit->AddObserver(this, MCH_INPUT);
}
//----------------------------------------------------------------------------
bool mafRemoteLogic::IsSocketConnected()
//----------------------------------------------------------------------------
{
  return m_ClientUnit->IsConnected();
}
//----------------------------------------------------------------------------
void mafRemoteLogic::VmeSelected(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafString cmd = "VME_SELECT";
  cmd << m_CommandSeparator;
  cmd << vme->GetId();
  RemoteMessage(cmd);
}
//----------------------------------------------------------------------------
void mafRemoteLogic::SynchronizeApplication()
//----------------------------------------------------------------------------
{
  // Send local layout
  mafView *v = m_ViewManager->GetList();
  bool send_msg = false;
  mafString cmd = "SynchronizeView";

  for(; v; v = v->m_Next) // find previous(view)
  {
    send_msg = true;
    cmd << m_CommandSeparator;
    cmd << v->m_Id - VIEW_START;
    cmd << m_CommandSeparator;
    cmd << v->m_Mult;
  }

  if(send_msg)
  {
    RemoteMessage(cmd);
  }

  // Retrieve Remote layout.
  cmd = "GetViewLayout";
  RemoteMessage(cmd);
}
