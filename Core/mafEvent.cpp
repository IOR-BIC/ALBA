/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEvent.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-04 12:18:53 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

// For compilers that support precompilation, includes "wx/wx.h".
//#include "wx/wxprec.h" 
//#include "wx/event.h"
//#pragma hdrstop

#include "mafEvent.h"
#include "mafView.h"
#include "mafOp.h"
//#include "mafDecl.h"

//#include "mflVME.h"

//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
enum 
{
    NO_EVENT =999999//= wxID_HIGHEST +1
};

//----------------------------------------------------------------------------
// mafEvent CTOR's
//----------------------------------------------------------------------------
mafEvent::mafEvent()                                                                      { Init(NULL,NO_EVENT,0);                            Initialized();}
mafEvent::mafEvent(void *sender, mafID id,                          long arg)             { Init(sender, id, arg);                            Initialized();}
mafEvent::mafEvent(void *sender, mafID id, bool            b,       long arg)             { Init(sender, id, arg); Bool =b;                   Initialized();}
mafEvent::mafEvent(void *sender, mafID id, float           f,       long arg)             { Init(sender, id, arg); Float=f;                   Initialized();}
mafEvent::mafEvent(void *sender, mafID id, wxString        *s,      long arg)             { Init(sender, id, arg); String =s;                 Initialized();}
mafEvent::mafEvent(void *sender, mafID id, const char      *s,      long arg)             { Init(sender, id, arg); SmartString =s;            Initialized();}
//mafEvent::mafEvent(void *sender, mafID id, mflVME          *vme,    bool b,long arg)      { Init(sender, id, arg); Vme  =vme; Bool = b;       Initialized();}
mafEvent::mafEvent(void *sender, mafID id, mafView         *view,   wxWindow *win)        { Init(sender, id, 0);   View =view; Win  =win;	 		Initialized();}
mafEvent::mafEvent(void *sender, mafID id, mafOp           *op,     long arg)             { Init(sender, id, arg);   Op   =op;                Initialized();}
mafEvent::mafEvent(void *sender, mafID id, wxWindow        *win,    long arg)             { Init(sender, id, arg); Win  =win;                 Initialized();}
mafEvent::mafEvent(void *sender, mafID id, wxUpdateUIEvent *e,      long arg)             { Init(sender, id, arg); UIEvent = e;																	}
//mafEvent::mafEvent(void *sender, mafID id, vtkProp         *prop,   mflVME *vme)          { Init(sender, id, 0);   Prop =prop; Vme=vme;       Initialized();}
//mafEvent::mafEvent(void *sender, mafID id, vtkObject       *vtkobj, long arg)             { Init(sender, id, arg); VtkObj = vtkobj;           Initialized();}
//mafEvent::mafEvent(void *sender, mafID id, vtkMatrix4x4    *m1,vtkMatrix4x4  *m2)         { Init(sender, id, 0);   Matrix =m1; Matrix2 =m2;   Initialized();}
//mafEvent::mafEvent(void *sender, mafID id, vtkObject       *vtkobj, wxString *s)          { Init(sender, id, 0);   VtkObj = vtkobj;String =s; Initialized();}

//----------------------------------------------------------------------------
// mafEvent Get's
//----------------------------------------------------------------------------
void*             mafEvent::GetSender()      {return Sender;}
int               mafEvent::GetId()          {return ID;}
long              mafEvent::GetArg()         {return Arg;}
bool              mafEvent::GetBool()        {return Bool;}
float             mafEvent::GetFloat()       {return Float;}
wxString*         mafEvent::GetString()      {return String;}
const char*       mafEvent::GetSmartString() {return SmartString;}
//mflVME*           mafEvent::GetVme()         {return Vme;}
mafView*          mafEvent::GetView()        {return View;}
mafOp*            mafEvent::GetOp()          {return Op;}
wxWindow*         mafEvent::GetWin()         {return Win;}
//vtkProp*          mafEvent::GetProp()        {return Prop;}
//vtkMatrix4x4*     mafEvent::GetMatrix()      {return Matrix;}
//vtkMatrix4x4*     mafEvent::GetOldMatrix()   {return Matrix2;}
wxUpdateUIEvent*  mafEvent::GetUIEvent()     {return UIEvent;}
//vtkObject*        mafEvent::GetVtkObj()      {return VtkObj;}
		
//----------------------------------------------------------------------------
// mafEvent Set's
//----------------------------------------------------------------------------
void mafEvent::SetSender(void* sender)          { Sender = sender;}
void mafEvent::SetId(int id)                    { ID = id;}
void mafEvent::SetArg(long arg)                 { Arg = arg;}
void mafEvent::SetBool(bool b)                  { Bool = b;}
void mafEvent::SetFloat(float f)                { Float = f;}
void mafEvent::SetString(wxString *s)           { String = s;}
void mafEvent::SetSmartString(const char *s)    { SmartString = s;}
//void mafEvent::SetVme(mflVME* vme)              { Vme = vme;}
void mafEvent::SetView(mafView* view)           { View = view;}
void mafEvent::SetOp(mafOp* op)                 { Op = op;}
void mafEvent::SetWin(wxWindow* win)            { Win = win;}
//void mafEvent::SetProp(vtkProp* prop)           { Prop = prop;}
//void mafEvent::SetMatrix(vtkMatrix4x4* mat)     { Matrix = mat;}
//void mafEvent::SetOldMatrix(vtkMatrix4x4* mat2) { Matrix2 =mat2;}
void mafEvent::SetUIEvent(wxUpdateUIEvent *e)   { UIEvent =e;}
//void mafEvent::SetVtkObj(vtkObject *vtkobj)     { VtkObj = vtkobj;}
    
//----------------------------------------------------------------------------
void mafEvent::Log() 
//----------------------------------------------------------------------------
{
  wxString  s = "[EV]";
                s << " Sender= "  << (long)Sender;
                s << " ID= "      << mafIdString(ID);
  if(Arg)       s << " Arg= "     << Arg;
  if(Bool)      s << " Bool= "    << (int)Bool;
  if(Float)     s << " Float= "   << Float;
  if(String)    s << " String= "  << *String;
                s << " SmartString= " << SmartString;
  if(Vme)       s << " Vme= "     << (long)Vme <<" : " << Vme->GetName();
  if(View)      s << " View= "    << (long)View<<" : " << View->m_label;
  if(m_op)      s << " Op= "      << (long)Op  <<" : " << m_op->m_label;
  if(m_win)     s << " Win= "     << (long)Win;
  if(m_prop)    s << " Prop= "    << (long)Prop;
  if(m_matrix)  s << " Matrix= "  << (long)Matrix.GetPointer();
  if(m_matrix2) s << " Matrix= "  << (long)Matrix2.GetPointer();
  if(m_uiev)    s << " UIEvent= " << (long)UIEvent;
  //if(m_vtkobj)  s << " VtkObj= "  << (long)VtkObj.GetPointer() << " : " << m_vtkobj->GetClassName();
               
  wxLogMessage(s);
}

//----------------------------------------------------------------------------
void mafEvent::DeepCopy(mflEvent *event) 
//----------------------------------------------------------------------------
{
  this->Superclass::DeepCopy(event);
  if (mafEvent *e=mafEvent::SafeDownCast(event))
  {
    e->m_arg      = m_arg;
    e->m_bool     = m_bool;
    e->m_float		= m_float;
    e->m_string		= m_string;
    e->m_vme			= m_vme;
    e->m_view		  = m_view;
    e->m_op		    = m_op;
    e->m_win			= m_win;
    e->m_prop		  = m_prop;
    e->m_matrix   = m_matrix;
    e->m_matrix2  = m_matrix2;
    e->m_uiev     = m_uiev;
    e->m_vtkobj   = m_vtkobj;
    e->m_smartstring = m_smartstring;
  }
}

//----------------------------------------------------------------------------
void mafEvent::Init(void *sender, mafID id, long arg) 
//----------------------------------------------------------------------------
{
  Sender   = sender;
  ID       = id; 
  m_arg    = arg;
  m_bool   = false; 
  m_float  = 0; 
  m_string = NULL; 
  m_vme    = NULL; 
  m_view   = NULL; 
  m_op	   = NULL; 
  m_win    = NULL;
  m_prop   = NULL;
  m_matrix = NULL;
  m_matrix2= NULL;
  m_uiev   = NULL;
  m_vtkobj = NULL;
}

//----------------------------------------------------------------------------
void mafEvent::Initialized() 
//----------------------------------------------------------------------------
{
  if(mafEvent::m_logmode) Log();
}
//----------------------------------------------------------------------------
int mafEvent::m_logmode = 0;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafEvent instantiators
//----------------------------------------------------------------------------
mafSmartEvent::mafSmartEvent()																																        :mflSmartEvent(mafEvent::New()) {this->UnRegister();                       }
mafSmartEvent::mafSmartEvent(void *sender, mafID id,                          long arg)				:mflSmartEvent(mafEvent::New(sender, id, arg)) {this->UnRegister();        }
mafSmartEvent::mafSmartEvent(void *sender, mafID id, bool            b,       long arg)				:mflSmartEvent(mafEvent::New(sender, id, b, arg)) {this->UnRegister();     }
mafSmartEvent::mafSmartEvent(void *sender, mafID id, float           f,       long arg)				:mflSmartEvent(mafEvent::New(sender, id, f, arg)) {this->UnRegister();     }
mafSmartEvent::mafSmartEvent(void *sender, mafID id, wxString        *s,      long arg)				:mflSmartEvent(mafEvent::New(sender, id, s, arg)) {this->UnRegister();     }
mafSmartEvent::mafSmartEvent(void *sender, mafID id, const char      *s,      long arg)				:mflSmartEvent(mafEvent::New(sender, id, s, arg)) {this->UnRegister();     }
mafSmartEvent::mafSmartEvent(void *sender, mafID id, mflVME          *vme,    bool b,long arg):mflSmartEvent(mafEvent::New(sender, id, vme, b, arg)) {this->UnRegister();}
mafSmartEvent::mafSmartEvent(void *sender, mafID id, mafView         *view,   wxWindow *win)	:mflSmartEvent(mafEvent::New(sender, id, view, win)) {this->UnRegister();  }
mafSmartEvent::mafSmartEvent(void *sender, mafID id, mafOp           *op,     long arg)				:mflSmartEvent(mafEvent::New(sender, id, op, arg)) {this->UnRegister();    }
mafSmartEvent::mafSmartEvent(void *sender, mafID id, wxWindow        *win,    long arg)				:mflSmartEvent(mafEvent::New(sender, id, win, arg)) {this->UnRegister();   }
mafSmartEvent::mafSmartEvent(void *sender, mafID id, wxUpdateUIEvent *e,      long arg)				:mflSmartEvent(mafEvent::New(sender, id, e, arg)) {this->UnRegister();     }
mafSmartEvent::mafSmartEvent(void *sender, mafID id, vtkProp         *prop,   mflVME *vme)   	:mflSmartEvent(mafEvent::New(sender, id, prop, vme)) {this->UnRegister();  }
mafSmartEvent::mafSmartEvent(void *sender, mafID id, vtkObject       *vtkobj, long arg)				:mflSmartEvent(mafEvent::New(sender, id, vtkobj, arg)) {this->UnRegister();}
mafSmartEvent::mafSmartEvent(void *sender, mafID id, vtkMatrix4x4    *m1,vtkMatrix4x4  *m2)		:mflSmartEvent(mafEvent::New(sender, id, m1, m2)) {this->UnRegister();     }
mafSmartEvent::mafSmartEvent(void *sender, mafID id, vtkObject       *vtkobj, wxString *s)    :mflSmartEvent(mafEvent::New(sender, id, vtkobj, s)) {this->UnRegister();  }


  



