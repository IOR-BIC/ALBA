/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEvent.cpp,v $
  Language:  C++
  Date:      $Date: 2006-03-24 16:22:06 $
  Version:   $Revision: 1.10 $
  Authors:   Marco Petrone, Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafEvent.h"

#include "mafView.h"
#include "mafOp.h"
#include "mafNode.h"
#include "mafMatrix.h"


#ifdef MAF_USE_VTK
  #include "vtkObject.h"
  #include "vtkProp.h"
#endif
//----------------------------------------------------------------------------
// TypeMacro
//----------------------------------------------------------------------------
mafCxxTypeMacro(mafEvent);
//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
enum HIGHEST_EVENT_ID
{
  NO_EVENT =9999//= wxID_HIGHEST +1
};
//----------------------------------------------------------------------------
// mafEvent CTOR's
//----------------------------------------------------------------------------
  mafEvent::mafEvent()																																		{ Init(NULL,NO_EVENT,0);																Initialized();}
  mafEvent::mafEvent(void *sender, int id,                          long arg)							{ Init(sender, id, arg);																Initialized();}
  mafEvent::mafEvent(void *sender, int id, bool            b,       long arg)							{ Init(sender, id, arg); m_bool =b;											Initialized();}
  mafEvent::mafEvent(void *sender, int id, double           f,       long arg)							{ Init(sender, id, arg); m_double=f;											Initialized();}
  mafEvent::mafEvent(void *sender, int id, mafString      *s,       long arg)							{ Init(sender, id, arg); m_string =s;										Initialized();}
  mafEvent::mafEvent(void *sender, int id, mafView         *view,   wxWindow *win)				{ Init(sender, id, 0);   m_view =view; m_Win  =win;			Initialized();}
  mafEvent::mafEvent(void *sender, int id, mafNode          *vme,    bool b,long arg)			{ Init(sender, id, arg); m_vme  =vme; m_bool = b;				Initialized();}
  mafEvent::mafEvent(void *sender, int id, mafOp           *op,     long arg)							{ Init(sender, id, arg);   m_op   =op;									Initialized();}
  mafEvent::mafEvent(void *sender, int id, mafMatrix    *m1,mafMatrix  *m2)					      { Init(sender, id, 0);   m_matrix =m1; m_matrix2 =m2;		Initialized();}
  mafEvent::mafEvent(void *sender, int id, mafObject        *mafobj,  long arg)						{ Init(sender, id, arg); m_MafObject = mafobj;			    Initialized();}
#ifdef MAF_USE_WX
  mafEvent::mafEvent(void *sender, int id, wxWindow        *win,    long arg)							{ Init(sender, id, arg); m_Win  =win;										Initialized();}
  mafEvent::mafEvent(void *sender, int id, wxUpdateUIEvent *e,      long arg)							{ Init(sender, id, arg); m_UpdateUIEvent = e;																	}
  mafEvent::mafEvent(void *sender, int id, wxObject        *wxobj,  long arg)							{ Init(sender, id, arg); m_WxObj = wxobj;							  Initialized();}
#endif
#ifdef MAF_USE_VTK
  mafEvent::mafEvent(void *sender, int id, vtkObject       *vtkobj, long arg)							{ Init(sender, id, arg); m_VtkObj = vtkobj;							Initialized();}
  mafEvent::mafEvent(void *sender, int id, vtkObject       *vtkobj, mafString *s)         { Init(sender, id, 0);   m_VtkObj = vtkobj;m_string =s; Initialized();}
  mafEvent::mafEvent(void *sender, int id, vtkProp         *prop,   mafNode *vme)   			{ Init(sender, id, 0);   m_VtkProp= prop; m_vme = vme;  Initialized();}
#endif
		
//----------------------------------------------------------------------------
void mafEvent::Log() 
//----------------------------------------------------------------------------
{
  mafString s = "[EV]";
               s << " sender= "<< (long)m_Sender;
               s << " ID= "    << mafIdString(m_Id).c_str();
  if(m_arg)    s << " arg= "   << m_arg;
  if(m_bool)   s << " bool= "  << (int)m_bool;
  if(m_double)  s << " double= " << m_double;
  if(m_string) s << " string= "<< *m_string;
  if(m_view)   s << " view= "  << (long)m_view<<" : " << m_view->GetLabel();
  if(m_vme)    s << " vme= "   << (long)m_vme <<" : " << m_vme->GetName();
  if(m_op)     s << " op= "    << (long)m_op  <<" : " << m_op->m_Label;
#ifdef MAF_USE_WX
  if(m_Win)    s << " win= "   << (long)m_Win;
  if(m_UpdateUIEvent)   s << " ui_evt= "<< (long)m_UpdateUIEvent;
  if(m_WxObj)  s << " wxobj= " << (long)m_WxObj;
#endif
#ifdef MAF_USE_VTK
  if(m_VtkProp)   s << " prop= "  << (long)m_VtkProp;
  if(m_matrix) s << " matrix= "<< (long)m_matrix;
  if(m_matrix2)s << " matrix= "<< (long)m_matrix2;
  if(m_VtkObj) s << " vtkobj= "<< (long)m_VtkObj << " : " << m_VtkObj->GetClassName();
#endif
  if(m_MafObject) s << " mafobj= " << (long)m_MafObject << " : " << m_MafObject->GetTypeName();

  mafLogMessage(s);
}
//----------------------------------------------------------------------------
mafEvent* mafEvent::Copy() 
//----------------------------------------------------------------------------
{
  mafEvent *e	= new mafEvent(m_Sender,m_Id,m_bool,m_arg);
  e->m_double		= m_double;
  e->m_string		= m_string;
  e->m_view		  = m_view;
  e->m_vme			= m_vme;
  e->m_op		    = m_op;
  e->m_MafObject= m_MafObject;
#ifdef MAF_USE_WX
  e->m_Win			= m_Win;
  e->m_UpdateUIEvent     = m_UpdateUIEvent;
  e->m_WxObj    = m_WxObj;
#endif
#ifdef MAF_USE_VTK
  e->m_VtkProp		  = m_VtkProp;
  e->m_matrix   = m_matrix;
  e->m_matrix2  = m_matrix2;
  e->m_VtkObj   = m_VtkObj;
#endif

  return e;
}
//----------------------------------------------------------------------------
void mafEvent::DeepCopy(const mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  m_Sender  = ((mafEvent *)maf_event)->GetSender();
  m_Id      = ((mafEvent *)maf_event)->GetId();
  m_arg     = ((mafEvent *)maf_event)->GetArg();
  m_bool    = ((mafEvent *)maf_event)->GetBool();
  m_double  = ((mafEvent *)maf_event)->GetDouble();
  m_string  = ((mafEvent *)maf_event)->GetString();
  m_vme     = ((mafEvent *)maf_event)->GetVme();
  m_view    = ((mafEvent *)maf_event)->GetView();
  m_op      = ((mafEvent *)maf_event)->GetOp();
  m_matrix  = ((mafEvent *)maf_event)->GetMatrix();
  m_matrix2 = ((mafEvent *)maf_event)->GetOldMatrix();
  m_MafObject = ((mafEvent *)maf_event)->GetMafObject();
#ifdef MAF_USE_WX
  m_WxObj   = ((mafEvent *)maf_event)->GetWxObj();
  m_Win     = ((mafEvent *)maf_event)->GetWin();
#endif
#ifdef MAF_USE_VTK
  m_VtkObj  = ((mafEvent *)maf_event)->GetVtkObj();
#endif
}
//----------------------------------------------------------------------------
void mafEvent::Init(void *sender, int id, long arg) 
//----------------------------------------------------------------------------
{
  m_Sender = sender;
  m_Id     = id; 
  m_arg    = arg;
  m_bool   = false; 
  m_double  = 0; 
  m_string = NULL; 
  m_view   = NULL; 
  m_vme    = NULL; 
  m_op	   = NULL; 
  m_MafObject = NULL;
  m_matrix = NULL;
  m_matrix2= NULL;
#ifdef MAF_USE_WX
  m_Win    = NULL;
  m_UpdateUIEvent   = NULL;
  m_WxObj  = NULL;
#endif
#ifdef MAF_USE_VTK
  m_VtkProp   = NULL;
  m_VtkObj = NULL;
#endif
}
//----------------------------------------------------------------------------
void mafEvent::Initialized() 
//----------------------------------------------------------------------------
{
  if(m_LogMode) Log();
}
//----------------------------------------------------------------------------
/** turn on/off Auto-Logging of every Event */
void mafEvent::SetLogMode(int logmode)
//----------------------------------------------------------------------------
{
	m_LogMode = logmode;
}
//----------------------------------------------------------------------------
int mafEvent::m_LogMode = 0;
//----------------------------------------------------------------------------
