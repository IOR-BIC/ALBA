/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEvent.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-26 11:08:46 $
  Version:   $Revision: 1.2 $
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


#ifdef MAF_USE_VTK
  #include "vtkObject.h"
  #include "vtkProp.h"
  #include "vtkMatrix4x4.h"
#endif
//----------------------------------------------------------------------------
// TypeMacro
//----------------------------------------------------------------------------
mafCxxTypeMacro(mafEvent);
//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
enum 
{
  NO_EVENT =9999//= wxID_HIGHEST +1
};
//----------------------------------------------------------------------------
// mafEvent CTOR's
//----------------------------------------------------------------------------
  mafEvent::mafEvent()																																		{ Init(NULL,NO_EVENT,0);																Initialized();}
  mafEvent::mafEvent(void *sender, int id,                          long arg)							{ Init(sender, id, arg);																Initialized();}
  mafEvent::mafEvent(void *sender, int id, bool            b,       long arg)							{ Init(sender, id, arg); m_bool =b;											Initialized();}
  mafEvent::mafEvent(void *sender, int id, float           f,       long arg)							{ Init(sender, id, arg); m_float=f;											Initialized();}
  mafEvent::mafEvent(void *sender, int id, mafString      *s,       long arg)							{ Init(sender, id, arg); m_string =s;										Initialized();}
  mafEvent::mafEvent(void *sender, int id, mafView         *view,   wxWindow *win)				{ Init(sender, id, 0);   m_view =view; m_win  =win;			Initialized();}
  mafEvent::mafEvent(void *sender, int id, mafNode          *vme,    bool b,long arg)			{ Init(sender, id, arg); m_vme  =vme; m_bool = b;				Initialized();}
  mafEvent::mafEvent(void *sender, int id, mafOp           *op,     long arg)							{ Init(sender, id, arg);   m_op   =op;									Initialized();}
#ifdef MAF_USE_WX
  mafEvent::mafEvent(void *sender, int id, wxWindow        *win,    long arg)							{ Init(sender, id, arg); m_win  =win;										Initialized();}
  mafEvent::mafEvent(void *sender, int id, wxUpdateUIEvent *e,      long arg)							{ Init(sender, id, arg); m_uiev = e;																	}
  mafEvent::mafEvent(void *sender, int id, wxObject        *wxobj,  long arg)							{ Init(sender, id, arg); m_wxobj = wxobj;							  Initialized();}
#endif
#ifdef MAF_USE_VTK
  mafEvent::mafEvent(void *sender, int id, vtkObject       *vtkobj, long arg)							{ Init(sender, id, arg); m_vtkobj = vtkobj;							Initialized();}
  mafEvent::mafEvent(void *sender, int id, vtkMatrix4x4    *m1,vtkMatrix4x4  *m2)					{ Init(sender, id, 0);   m_matrix =m1; m_matrix2 =m2;		Initialized();}
  mafEvent::mafEvent(void *sender, int id, vtkObject       *vtkobj, mafString *s)         { Init(sender, id, 0);   m_vtkobj = vtkobj;m_string =s; Initialized();}
  mafEvent::mafEvent(void *sender, int id, vtkProp         *prop,   mafNode *vme)   				{ Init(sender, id, 0);   m_prop =prop; m_vme=vme;				Initialized();}
#endif
		
//----------------------------------------------------------------------------
void mafEvent::Log() 
//----------------------------------------------------------------------------
{
  mafString s = "[EV]";
               s << " sender= "<< (long)m_sender;
               s << " ID= "    << mafIdString(m_id).c_str();
  if(m_arg)    s << " arg= "   << m_arg;
  if(m_bool)   s << " bool= "  << (int)m_bool;
  if(m_float)  s << " float= " << m_float;
  if(m_string) s << " string= "<< *m_string;
  if(m_view)   s << " view= "  << (long)m_view<<" : " << m_view->GetLabel();
  if(m_vme)    s << " vme= "   << (long)m_vme <<" : " << m_vme->GetName();
  if(m_op)     s << " op= "    << (long)m_op  <<" : " << m_op->m_Label;
#ifdef MAF_USE_WX
  if(m_win)    s << " win= "   << (long)m_win;
  if(m_uiev)   s << " ui_evt= "<< (long)m_uiev;
  if(m_wxobj)  s << " wxobj= " << (long)m_wxobj;
#endif
#ifdef MAF_USE_VTK
  if(m_prop)   s << " prop= "  << (long)m_prop;
  if(m_matrix) s << " matrix= "<< (long)m_matrix;
  if(m_matrix2)s << " matrix= "<< (long)m_matrix2;
  if(m_vtkobj) s << " vtkobj= "<< (long)m_vtkobj << " : " << m_vtkobj->GetClassName();
#endif

  mafLogMessage(s);
}
//----------------------------------------------------------------------------
mafEvent* mafEvent::Copy() 
//----------------------------------------------------------------------------
{
  mafEvent *e	= new mafEvent(m_sender,m_id,m_bool,m_arg);
  e->m_float		= m_float;
  e->m_string		= m_string;
  e->m_view		  = m_view;
  e->m_vme			= m_vme;
  e->m_op		    = m_op;
#ifdef MAF_USE_WX
  e->m_win			= m_win;
  e->m_uiev     = m_uiev;
  e->m_wxobj    = m_wxobj;
#endif
#ifdef MAF_USE_VTK
  e->m_prop		  = m_prop;
  e->m_matrix   = m_matrix;
  e->m_matrix2  = m_matrix2;
  e->m_vtkobj   = m_vtkobj;
#endif

  return e;
}
//----------------------------------------------------------------------------
void mafEvent::Init(void *sender, int id, long arg) 
//----------------------------------------------------------------------------
{
  m_sender = sender;
  m_id     = id; 
  m_arg    = arg;
  m_bool   = false; 
  m_float  = 0; 
  m_string = NULL; 
  m_view   = NULL; 
  m_vme    = NULL; 
  m_op	   = NULL; 
#ifdef MAF_USE_WX
  m_win    = NULL;
  m_uiev   = NULL;
  m_wxobj  = NULL;
#endif
#ifdef MAF_USE_VTK
  m_prop   = NULL;
  m_matrix = NULL;
  m_matrix2= NULL;
  m_vtkobj = NULL;
#endif
}
//----------------------------------------------------------------------------
void mafEvent::Initialized() 
//----------------------------------------------------------------------------
{
  if(m_logmode) Log();
}
//----------------------------------------------------------------------------
/** turn on/off Auto-Logging of every Event */
void mafEvent::SetLogMode(int logmode)
//----------------------------------------------------------------------------
{
	m_logmode = logmode;
}
//----------------------------------------------------------------------------
int mafEvent::m_logmode = 0;
//----------------------------------------------------------------------------
