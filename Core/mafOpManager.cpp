/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpManager.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-31 23:47:04 $
  Version:   $Revision: 1.7 $
  Authors:   Silvano Imboden
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


#include <wx/tokenzr.h>
#include "mafDecl.h"
#include "mafOp.h"
#include "mafOpStack.h"
#include "mafOpContextStack.h"
#include "mafOpSelect.h"
//@@@ #include "mafAction.h"
#include "mafOpManager.h"
#include "mafNode.h"
#include "vtkMatrix4x4.h"

/**
There are 4 paths in OpProcessor

1) OpRun -> run the op's GUI, ...... in asynchronous way can call OpDo
used into operations op,imp,exp

2) OpExec -> run OpDo in synchronous way
used by nested select,transform,cut,copy,paste, op

3) OpUndo -> do pop e run in synchronous way op->OpUndo
used by undo

4) OpRedo -> do pop e run in synchronous way op->OpRedo
used by redo

During the Run an operation can run nested operations,
the nested operations can be cancelled individually.

the nested operations send OP_RUN_STARTING and OP_RUN_TERMINATED to the caller operation.
instead to Logic - in Logic don't need any more running_op_counter, in theory.
In practice it needs for View-Settings.

No is dangerous - if the operations don't manage OP_RUN_STARTING and OP_RUN_TERMINATED 
these return and reach logic - is better to avoid.

During the Exec, can be run nested operations
but could not be done the undo.
*/
//----------------------------------------------------------------------------
mafOpManager::mafOpManager()
//----------------------------------------------------------------------------
{
  m_Listener	= NULL;
  m_running_op= NULL;
	m_selected	= NULL;
	m_warn			= false;
  
	m_menu[OPTYPE_IMPORTER] = NULL;
  m_menu[OPTYPE_EXPORTER] = NULL;
  m_menu[OPTYPE_OP] = NULL;

  m_opselect    = new mafOpSelect();
  m_opcut       = new mafOpCut();
  m_opcopy      = new mafOpCopy();
  m_oppaste     = new mafOpPaste();
  //m_optransform = new mafOpTransform();

	m_toolbar = NULL;
	m_menubar = NULL;

  m_numAccel = 0;
  m_numop = 0;
	for(int i=0; i<MAXOP; i++ ) 
    m_opv[i] = NULL;
}
//----------------------------------------------------------------------------
mafOpManager::~mafOpManager()
//----------------------------------------------------------------------------
{
	for(int i=0; i<m_numop; i++) delete m_opv[i];

  delete m_opselect;
  delete m_opcut;
  delete m_opcopy;
  delete m_oppaste;
	//delete m_optransform;
}
//----------------------------------------------------------------------------
void mafOpManager::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  mafOp* o = NULL; 

  if (mafEvent *e = mafEvent::SafeDownCast(event))
  {
    switch (e->GetId())
    {
      case OP_EXEC:
        o = e->GetOp();
        if(o) 
          OpExec(o);
      break; 
      case OP_RUN:
        o = e->GetOp();
        if(o) 
          OpRun(o);
      break; 
      case OP_RUN_OK:
        o = (mafOp*) e->GetSender();
        assert(o == m_context.Caller());
        OpRunOk(o);
      break; 
      case OP_RUN_CANCEL:
        o = (mafOp*) e->GetSender();
        assert(o == m_context.Caller());
        OpRunCancel(o);
      break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafOpManager::OpAdd(mafOp *op, wxString menuPath)
//----------------------------------------------------------------------------
{
  assert(m_numop < MAXOP);
  m_opv[m_numop] = op;
  op->m_OpMenuPath = menuPath;
	op->m_Id = m_numop + OP_USER;
  m_numop++;
}
//----------------------------------------------------------------------------
void mafOpManager::FillMenu (wxMenu* import, wxMenu* mexport, wxMenu* operations)
//----------------------------------------------------------------------------
{
  int submenu_id = 1;
  
  m_menu[OPTYPE_IMPORTER] = import;
  m_menu[OPTYPE_EXPORTER] = mexport;
  m_menu[OPTYPE_OP] = operations;
   
	for(int i=0; i<m_numop; i++)
  {
    mafOp *o = m_opv[i];
    if (o->m_OpMenuPath != "")
    {
      wxMenu *sub_menu = NULL;
        
      int item = m_menu[o->GetType()]->FindItem(o->m_OpMenuPath);
      if (item != wxNOT_FOUND)
      {
         wxMenuItem *menu_item = m_menu[o->GetType()]->FindItem(item);
         if (menu_item)
           sub_menu = menu_item->GetSubMenu();
      }
      else
      {
        sub_menu = new wxMenu;
        m_menu[o->GetType()]->Append(submenu_id++,o->m_OpMenuPath,sub_menu);
      }
      
      if(sub_menu)
        sub_menu->Append(o->m_Id, o->m_Label, o->m_Label);
      else
        wxLogMessage("error in FillMenu");
    }
    else
    {
      m_menu[o->GetType()]->Append(o->m_Id, o->m_Label, o->m_Label);
    }
    SetAccelerator(o);
  }
  wxAcceleratorTable accel(MAXOP, m_OpAccelEntries);
  if (accel.Ok())
    mafGetFrame()->SetAcceleratorTable(accel);
}
//----------------------------------------------------------------------------
void mafOpManager::SetAccelerator(mafOp *op)
//----------------------------------------------------------------------------
{
  wxString accelerator, flag = "", extra_flag = "", key_code = "";
  int flag_num;
  accelerator = op->m_Label;
  wxStringTokenizer tkz(accelerator, "\t");
  int token = tkz.CountTokens();

  if (token > 1)
  {
    accelerator  = tkz.GetNextToken();
    accelerator  = tkz.GetNextToken();
    wxStringTokenizer tkz2(accelerator, "+");
    token = tkz2.CountTokens();
    if (token == 2)
    {
      flag = tkz2.GetNextToken();
      key_code = tkz2.GetNextToken();
    }
    else
    {
      flag = tkz2.GetNextToken();
      extra_flag = tkz2.GetNextToken();
      key_code = tkz2.GetNextToken();
    }
    if (flag == "Ctrl")
      flag_num = wxACCEL_CTRL;
    else if(flag == "Alt")
      flag_num = wxACCEL_ALT;
    else if(flag == "Shift")
      flag_num = wxACCEL_SHIFT;

    if (extra_flag == "Ctrl")
      flag_num |= wxACCEL_CTRL;
    else if(extra_flag == "Alt")
      flag_num |= wxACCEL_ALT;
    else if(extra_flag == "Shift")
      flag_num |= wxACCEL_SHIFT;

    m_OpAccelEntries[m_numAccel++].Set(flag_num,  (int) *key_code.c_str(), op->m_Id);
  }
}
//----------------------------------------------------------------------------
void mafOpManager::VmeSelected(mafNode* v)   
//----------------------------------------------------------------------------
{
  m_selected = v;
  EnableOp();
}
//----------------------------------------------------------------------------
mafNode* mafOpManager::GetSelectedVme()
//----------------------------------------------------------------------------
{
	return m_selected;
}
//----------------------------------------------------------------------------
void mafOpManager::OpSelect(mafNode* vme)   
//----------------------------------------------------------------------------
{
	if(vme == m_selected ) 
    return;
	m_opselect->SetNewSel(vme);
	OpExec(m_opselect);
}
/*
//----------------------------------------------------------------------------
void mafOpManager::OpTransform(vtkMatrix4x4* new_matrix,vtkMatrix4x4* old_matrix)
//----------------------------------------------------------------------------
{
	m_optransform->SetNewMatrix(new_matrix);
	m_optransform->SetOldMatrix(old_matrix);
	OpExec(m_optransform); 
}
*/
//----------------------------------------------------------------------------
void mafOpManager::ClearUndoStack()
//----------------------------------------------------------------------------
{
   m_context.Clear();
   m_opcut->ClipboardClear();
	 EnableOp();
}
//----------------------------------------------------------------------------
void mafOpManager::EnableToolbar(bool CanEnable) 
//----------------------------------------------------------------------------
{
	if(!CanEnable)
	{
/*		m_toolbar->EnableTool(MENU_FILE_NEW,false);	//Removed by Paolo 15-9-2003: HAL men
		m_toolbar->EnableTool(MENU_FILE_OPEN,false);
		m_toolbar->EnableTool(MENU_FILE_SAVE,false);*/
		m_toolbar->EnableTool(OP_UNDO,false);
		m_toolbar->EnableTool(OP_REDO,false);
		m_toolbar->EnableTool(OP_CUT,false);
		m_toolbar->EnableTool(OP_COPY,false);
		m_toolbar->EnableTool(OP_PASTE,false);
	}
	else
	{
/*		m_toolbar->EnableTool(MENU_FILE_NEW,true);
		m_toolbar->EnableTool(MENU_FILE_OPEN,true);
		m_toolbar->EnableTool(MENU_FILE_SAVE,true);*/
		m_toolbar->EnableTool(OP_UNDO,	!m_context.Undo_IsEmpty());
		m_toolbar->EnableTool(OP_REDO,	!m_context.Redo_IsEmpty());
		m_toolbar->EnableTool(OP_CUT,		m_opcut->Accept(m_selected) );
		m_toolbar->EnableTool(OP_COPY,	m_opcopy->Accept(m_selected) );
		m_toolbar->EnableTool(OP_PASTE,	m_oppaste->Accept(m_selected) );
	}
}
//----------------------------------------------------------------------------
void mafOpManager::EnableOp(bool CanEnable) 
//----------------------------------------------------------------------------
{
  mafOp *o = NULL;
	if(m_menubar)
  {
    if(!CanEnable)
	  {
		  if(m_menubar->FindItem(OP_DELETE))	m_menubar->Enable(OP_DELETE,false);
		  if(m_menubar->FindItem(OP_CUT))			m_menubar->Enable(OP_CUT,false);
		  if(m_menubar->FindItem(OP_COPY))		m_menubar->Enable(OP_COPY,false);
		  if(m_menubar->FindItem(OP_PASTE))		m_menubar->Enable(OP_PASTE,false);
		  if(m_menubar->FindItem(OP_UNDO))		m_menubar->Enable(OP_UNDO,false); 
		  if(m_menubar->FindItem(OP_REDO))		m_menubar->Enable(OP_REDO,false); 

		  for(int i=0; i<m_numop; i++)
		  {
        mafOp *o = m_opv[i]; 
        if(m_menubar->FindItem(o->m_Id))
          m_menubar->Enable(o->m_Id,false); 
      }
	  }
	  else
	  {
		  if(m_menubar->FindItem(OP_UNDO))		m_menubar->Enable(OP_UNDO,    !m_context.Undo_IsEmpty());
		  if(m_menubar->FindItem(OP_REDO))		m_menubar->Enable(OP_REDO,    !m_context.Redo_IsEmpty());
		  if(m_menubar->FindItem(OP_DELETE))	m_menubar->Enable(OP_DELETE,  m_opcut->Accept(m_selected));
		  if(m_menubar->FindItem(OP_CUT))			m_menubar->Enable(OP_CUT,     m_opcut->Accept(m_selected));
		  if(m_menubar->FindItem(OP_COPY))		m_menubar->Enable(OP_COPY,    m_opcopy->Accept(m_selected));
		  if(m_menubar->FindItem(OP_PASTE))		m_menubar->Enable(OP_PASTE,   m_oppaste->Accept(m_selected)); 

		  for(int i=0; i<m_numop; i++)
		  {
        mafOp *o = m_opv[i]; 
        if(m_menubar->FindItem(o->m_Id))
          m_menubar->Enable(o->m_Id,o->Accept(m_selected)); 
      }
	  }
  }
	if(m_toolbar) EnableToolbar(CanEnable);
}
//----------------------------------------------------------------------------
bool mafOpManager::WarnUser(mafOp *op)
//----------------------------------------------------------------------------
{
	bool go = true;
	if(m_warn && !op->CanUndo())
	{
		wxMessageDialog dialog(
							mafGetFrame(),
							"This operation can not UnDo. Continue?",
							"UnDo Warning",
							wxYES_NO|wxYES_DEFAULT
							);
		if(dialog.ShowModal() == wxID_NO) go = false;
	}
  return go;
}
//----------------------------------------------------------------------------
void mafOpManager::OpRun(int op_id)
//----------------------------------------------------------------------------
{
  switch(op_id)
  {
	  case OP_UNDO:
			 OpUndo();
	  break;
	  case OP_REDO:
			 OpRedo();
	  break;
	  case OP_DELETE:
	  case OP_CUT:
			 OpExec(m_opcut);
	  break;
	  case OP_COPY:
			 OpExec(m_opcopy);
	  break;
	  case OP_PASTE:
			 OpExec(m_oppaste);
	  break;
	  default:
		{
      int index = op_id - OP_USER;
		  if(index >=0 && index <m_numop) OpRun(m_opv[index]);
		}
	  break;
	}
}
//----------------------------------------------------------------------------
void mafOpManager::OpRun(mafOp *op)   
//----------------------------------------------------------------------------
{
	//Code to manage Natural Preserving
 //SIL. 9-4-2005: should be re-inserted in a new Medical-Oriented OpManager
	/*
  if(m_selected->GetTagArray()->FindTag("VME_NATURE") != -1)
	{
		wxString nat("NATURAL");
		if(m_selected->GetTagArray()->GetTag("VME_NATURE")->GetValue() == nat && !op->IsNaturalPreserving())
		{
			wxString warning_msg = "To presere natural VME a copy is required! This should take a lot of memory and time. continue?";
			wxString synthetic_name = "Synthetic ";
			wxMessageDialog dialog(mafGetFrame(),warning_msg, "Natural VME Preserving Warning",wxYES_NO|wxYES_DEFAULT);
			if(dialog.ShowModal() == wxID_NO) 
				return;
      mafNode *synthetic_vme = m_selected->MakeCopy();
			synthetic_vme->GetTagArray()->GetTag("VME_NATURE")->SetValue("SYNTHETIC");
			synthetic_vme->ReparentTo(m_selected->GetParent());
			synthetic_name.Append(m_selected->GetName());
			synthetic_vme->SetName(synthetic_name);
			mafEventMacro(mafEvent(this,VME_ADD,synthetic_vme));
      OpSelect(synthetic_vme);
			mafEventMacro(mafEvent(this,VME_SHOW,synthetic_vme,true));
			synthetic_vme->Delete();
		}
	}
  */
	EnableOp(false);
  //Notify(OP_RUN_STARTING); //SIL. 17-9-2004: --- moved after m_running_op has been set

	mafOp *o = op->Copy();
  o->m_Id = op->m_Id;    //Paolo 15/09/2004 The operation ID is not copyed from the Copy() method.
	o->SetListener(this);
	o->SetInput(m_selected);
  o->SetMouseAction(m_MouseAction);
  m_running_op = o;

  Notify(OP_RUN_STARTING);  //SIL. 17-9-2004: - moved here in order to notify which op is started

  m_context.Push(o);
  o->OpRun();
}
//----------------------------------------------------------------------------
void mafOpManager::OpRunOk(mafOp *op)
//----------------------------------------------------------------------------
{	
  m_context.Pop();

  m_running_op = NULL;
	if(WarnUser(op))
	{
		OpDo(op);
	}
  Notify(OP_RUN_TERMINATED);
	if(m_context.Caller() == NULL) 	
    EnableOp();
}
//----------------------------------------------------------------------------
void mafOpManager::OpRunCancel(mafOp *op)
//----------------------------------------------------------------------------
{
  m_context.Pop();

	m_running_op = NULL;
	delete op;

  Notify(OP_RUN_TERMINATED);

	if(m_context.Caller() == NULL) 	
    EnableOp();
}
//----------------------------------------------------------------------------
void mafOpManager::OpExec(mafOp *op)
/** call this to exec an operation without user interaction but with undo/redo services */
//----------------------------------------------------------------------------
{
	EnableOp(false);

	assert(op);
	if(op->Accept(m_selected))
  {
    if(WarnUser(op))
	  {
		  mafOp *o = op->Copy();
		  o->SetListener(this);
		  o->SetInput(m_selected);
		  OpDo(o);
	  }
  }
	EnableOp();
}
//----------------------------------------------------------------------------
void mafOpManager::OpDo(mafOp *op)
//----------------------------------------------------------------------------
{
 	if(!WarnUser(op)) 
  {
    delete op;
		return;
	}
  
  m_context.Redo_Clear();
  op->OpDo();
  
  wxLogMessage("do=%s",op->m_Label.c_str());

  if(op->CanUndo()) 
  {
	  m_context.Undo_Push(op);
  }
  else
  {
	  m_context.Undo_Clear();
    delete op;  
  }
}
//----------------------------------------------------------------------------
void mafOpManager::OpUndo()   
//----------------------------------------------------------------------------
{
  if( m_context.Undo_IsEmpty()) 
  {
    wxLogMessage("empty undo stack");
    return;
  }
	EnableOp(false);

	mafOp* op = m_context.Undo_Pop();
	wxLogMessage("undo=%s",op->m_Label.c_str());
	op->OpUndo();
	m_context.Redo_Push(op);

	EnableOp();
}
//----------------------------------------------------------------------------
void mafOpManager::OpRedo()   
//----------------------------------------------------------------------------
{
  if( m_context.Redo_IsEmpty())
  {
    wxLogMessage("empty redo stack");
    return;
  }
	EnableOp(false);

	mafOp* op = m_context.Redo_Pop();
	wxLogMessage("redo=%s",op->m_Label.c_str());
	op->OpDo();
	m_context.Undo_Push(op);

	EnableOp();
}
//----------------------------------------------------------------------------
void mafOpManager::Notify(int msg)   
//----------------------------------------------------------------------------
{
	if(m_context.Caller() == NULL)
	// not a nested operation - notify logic
		mafEventMacro(mafEvent(this,msg,m_running_op));   //SIL. 17-9-2004: added the m_running_op at the event (may be NULL)
	//else
	// nested operation - notify caller
		    // m_context.Caller()->OnEvent(mafEvent(this,msg));   
        // NO - it is dangerous - if the caller don't handle the msg,
				// msg will be bounced and then forwarded to Logic
				// better wait until this feature is really needed 
}
//----------------------------------------------------------------------------
bool mafOpManager::StopCurrentOperation()
//----------------------------------------------------------------------------
{
  assert(false); //SIL. 2-7-2004: -- Seems that no-one is using this -- but tell me if the program stop here -- thanks
  mafOp *prev_running_op = m_running_op;
	
	while(m_running_op) //loop danger !!!
	{
    if(m_running_op->OkEnabled())
		  m_running_op->ForceStopWithOk();
    else
		  m_running_op->ForceStopWithCancel();

    //if(prev_running_op == m_running_op)
		//{
      //previous try failed, be more bad
			//OpRunCancel(m_running_op); - wrong: op-gui will be still plugged
		//}

    if(prev_running_op == m_running_op)
		{
			break; //second try failed, give it up and break to prevent loops
		}
	}
  return (m_running_op == NULL);
}
//----------------------------------------------------------------------------
bool mafOpManager::ForceStopWithOk()
//----------------------------------------------------------------------------
{
  if(!m_running_op) 
    return false;
  mafOp *to_be_killed = m_running_op;

  if(m_running_op->OkEnabled())
    m_running_op->ForceStopWithOk();
  
  return m_running_op != to_be_killed;
}
//----------------------------------------------------------------------------
bool mafOpManager::ForceStopWithCancel()
//----------------------------------------------------------------------------
{
  if(!m_running_op) 
    return false;
  mafOp *to_be_killed = m_running_op;

  m_running_op->ForceStopWithCancel();

  return m_running_op != to_be_killed;
}
/* - temporary commented out until mafAction is reintroduced
//----------------------------------------------------------------------------
void mafOpManager::SetMouseAction(mafAction *action)
//----------------------------------------------------------------------------
{
  m_MouseAction = action;
}
*/
