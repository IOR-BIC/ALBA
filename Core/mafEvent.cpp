/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEvent.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-21 17:49:28 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone, Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafEvent.h"

//#include "mflVME.h"
//#include "mafView.h"
//#include "mafOp.h"
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
//mafEvent::mafEvent(void *sender, int id, wxWindow        *win,    long arg)							{ Init(sender, id, arg); m_win  =win;										Initialized();}
//mafEvent::mafEvent(void *sender, int id, wxUpdateUIEvent *e,      long arg)							{ Init(sender, id, arg); m_uiev = e;																	}
//mafEvent::mafEvent(void *sender, int id, wxObject        *wxobj,  long arg)							{ Init(sender, id, arg); m_wxobj = wxobj;							  Initialized();}
//mafEvent::mafEvent(void *sender, int id, mafView         *view,   wxWindow *win)				{ Init(sender, id, 0);   m_view =view; m_win  =win;			Initialized();}
//mafEvent::mafEvent(void *sender, int id, mflVME          *vme,    bool b,long arg)			{ Init(sender, id, arg); m_vme  =vme; m_bool = b;				Initialized();}
//mafEvent::mafEvent(void *sender, int id, mafOp           *op,     long arg)							{ Init(sender, id, arg);   m_op   =op;									Initialized();}
//mafEvent::mafEvent(void *sender, int id, vtkObject       *vtkobj, long arg)							{ Init(sender, id, arg); m_vtkobj = vtkobj;							Initialized();}
//mafEvent::mafEvent(void *sender, int id, vtkMatrix4x4    *m1,vtkMatrix4x4  *m2)					{ Init(sender, id, 0);   m_matrix =m1; m_matrix2 =m2;		Initialized();}
//mafEvent::mafEvent(void *sender, int id, vtkObject       *vtkobj, mafString *s)          { Init(sender, id, 0);   m_vtkobj = vtkobj;m_string =s; Initialized();}
//mafEvent::mafEvent(void *sender, int id, vtkProp         *prop,   mflVME *vme)   				{ Init(sender, id, 0);   m_prop =prop; m_vme=vme;				Initialized();}
//----------------------------------------------------------------------------
// mafEvent Get's
//----------------------------------------------------------------------------
void*            mafEvent::GetSender()    {return m_sender;}
int              mafEvent::GetId()        {return m_id;}
long             mafEvent::GetArg()       {return m_arg;}
bool             mafEvent::GetBool()      {return m_bool;}
float            mafEvent::GetFloat()     {return m_float;}
mafString*       mafEvent::GetString()    {return m_string;}
//wxWindow*        mafEvent::GetWin()       {return m_win;}
//wxUpdateUIEvent* mafEvent::GetUIEvent()   {return m_uiev;}
//wxObject*        mafEvent::GetWxObj()     {return m_wxobj;}
//mafView*         mafEvent::GetView()      {return m_view;}
//mflVME*          mafEvent::GetVme()       {return m_vme;}
//mafOp*					 mafEvent::GetOp()        {return m_op;}
//vtkProp*         mafEvent::GetProp()      {return m_prop;}
//vtkMatrix4x4*    mafEvent::GetMatrix()    {return m_matrix;}
//vtkMatrix4x4*    mafEvent::GetOldMatrix() {return m_matrix2;}
//vtkObject*       mafEvent::GetVtkObj()    {return m_vtkobj;}
		
//----------------------------------------------------------------------------
// mafEvent Set's
//----------------------------------------------------------------------------
void mafEvent::SetSender(void* sender)          { m_sender = sender;}
void mafEvent::SetId(int id)                    { m_id = id;}
void mafEvent::SetArg(long arg)                 { m_arg = arg;}
void mafEvent::SetBool(bool b)                  { m_bool = b;}
void mafEvent::SetFloat(float f)                { m_float = f;}
void mafEvent::SetString(mafString *s)          { m_string = s;}
//void mafEvent::SetWin(wxWindow* win)            { m_win = win;}
//void mafEvent::SetUIEvent(wxUpdateUIEvent *e)   { m_uiev =e;}
//void mafEvent::SetWxObj(wxObject *wxobj)        { m_wxobj = wxobj;}
//void mafEvent::SetView(mafView* view)           { m_view = view;}
//void mafEvent::SetVme(mflVME* vme)              { m_vme = vme;}
//void mafEvent::SetOp(mafOp* op)                 { m_op = op;}
//void mafEvent::SetProp(vtkProp* prop)           { m_prop = prop;}
//void mafEvent::SetMatrix(vtkMatrix4x4* mat)     { m_matrix = mat;}
//void mafEvent::SetOldMatrix(vtkMatrix4x4* mat2) { m_matrix2 =mat2;}
//void mafEvent::SetVtkObj(vtkObject *vtkobj)     { m_vtkobj = vtkobj;}
   
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
//  if(m_win)    s << " win= "   << (long)m_win;
//  if(m_uiev)   s << " ui_evt= "<< (long)m_uiev;
//  if(m_wxobj)  s << " wxobj= " << (long)m_wxobj;
//  if(m_view)   s << " view= "  << (long)m_view<<" : " << m_view->m_label;
//  if(m_vme)    s << " vme= "   << (long)m_vme <<" : " << m_vme->GetName();
//  if(m_op)     s << " op= "    << (long)m_op  <<" : " << m_op->m_label;
//  if(m_prop)   s << " prop= "  << (long)m_prop;
//  if(m_matrix) s << " matrix= "<< (long)m_matrix;
//  if(m_matrix2)s << " matrix= "<< (long)m_matrix2;
//  if(m_vtkobj) s << " vtkobj= "<< (long)m_vtkobj << " : " << m_vtkobj->GetClassName();

  mafLogMessage(s);
}
//----------------------------------------------------------------------------
mafEvent* mafEvent::Copy() 
//----------------------------------------------------------------------------
{
  mafEvent *e	= new mafEvent(m_sender,m_id,m_bool,m_arg);
  e->m_float		= m_float;
  e->m_string		= m_string;
//  e->m_win			= m_win;
//  e->m_uiev     = m_uiev;
//  e->m_wxobj    = m_wxobj;
//  e->m_view		  = m_view;
//  e->m_vme			= m_vme;
//  e->m_op		    = m_op;
//  e->m_prop		  = m_prop;
//  e->m_matrix   = m_matrix;
//  e->m_matrix2  = m_matrix2;
//  e->m_vtkobj   = m_vtkobj;

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
//  m_win    = NULL;
//  m_uiev   = NULL;
//  m_wxobj  = NULL;
//  m_view   = NULL; 
//  m_vme    = NULL; 
//  m_op	   = NULL; 
//  m_prop   = NULL;
//  m_matrix = NULL;
//  m_matrix2= NULL;
//  m_vtkobj = NULL;
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
