/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEvent
 Authors: Marco Petrone, Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaEvent.h"

#include "albaView.h"
#include "albaOp.h"
#include "albaVME.h"
#include "albaString.h"
#include "albaMatrix.h"


#ifdef ALBA_USE_VTK
  #include "vtkObject.h"
  #include "vtkProp.h"
#endif
//----------------------------------------------------------------------------
// TypeMacro
//----------------------------------------------------------------------------
albaCxxTypeMacro(albaEvent);
//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
enum HIGHEST_EVENT_ID
{
  NO_EVENT =9999//= wxID_HIGHEST +1
};
//----------------------------------------------------------------------------
// albaEvent CTOR's
//----------------------------------------------------------------------------
  albaEvent::albaEvent()																																		  { Init(NULL,NO_EVENT,0);}
  albaEvent::albaEvent(void *sender, int id,                            long arg)							{ Init(sender, id, arg);}
  albaEvent::albaEvent(void *sender, int id, bool             b,        long arg)							{ Init(sender, id, arg); m_Bool =b;}
  albaEvent::albaEvent(void *sender, int id, double           f,        long arg)             { Init(sender, id, arg); m_Double=f;}
  albaEvent::albaEvent(void *sender, int id, albaString        *s,       long arg)							{ Init(sender, id, arg); m_ALBAString =s;}
  albaEvent::albaEvent(void *sender, int id, albaString       *s, int x, int y, int width, int height,  long arg)  { Init(sender, id, arg); m_ALBAString =s; m_x = x; m_y = y; m_width = width; m_height = height;}
  albaEvent::albaEvent(void *sender, int id, albaView          *view,    wxWindow *win)				{ Init(sender, id, 0);   m_View =view; m_Win  =win;}
  albaEvent::albaEvent(void *sender, int id, albaVME          *vme,     bool b,long arg)			{ Init(sender, id, arg); m_Vme  =vme; m_Bool = b;}
  albaEvent::albaEvent(void *sender, int id, albaOp            *op,      long arg)							{ Init(sender, id, arg);   m_Op   =op;}
  albaEvent::albaEvent(void *sender, int id, albaMatrix        *m1,albaMatrix  *m2)					    { Init(sender, id, 0);   m_Matrix =m1; m_OldMatrix =m2;}
  albaEvent::albaEvent(void *sender, int id, albaObject        *albaobj,  long arg)						  { Init(sender, id, arg); m_MafObject = albaobj;}
  albaEvent::albaEvent(void *sender, int id, albaObject        *albaobj,  albaString *s,long arg){ Init(sender, id, arg); m_MafObject = albaobj; m_ALBAString =s;}
  albaEvent::albaEvent(void *sender, int id, WidgetDataType   &widget_data,  long arg)
  {
    Init(sender, id, arg);
    m_WidgetData.dType = widget_data.dType;
    m_WidgetData.dValue= widget_data.dValue;
    m_WidgetData.fValue= widget_data.fValue;
    m_WidgetData.iValue= widget_data.iValue;
    m_WidgetData.sValue= widget_data.sValue;
  }
#ifdef ALBA_USE_WX
  albaEvent::albaEvent(void *sender, int id, wxWindow        *win,    long arg)							{ Init(sender, id, arg); m_Win  =win;}
  albaEvent::albaEvent(void *sender, int id, wxUpdateUIEvent *e,      long arg)							{ Init(sender, id, arg); m_UpdateUIEvent = e;																	}
  albaEvent::albaEvent(void *sender, int id, wxObject        *wxobj,  long arg)							{ Init(sender, id, arg); m_WxObj = wxobj;}
#endif
#ifdef ALBA_USE_VTK
  albaEvent::albaEvent(void *sender, int id, vtkObject       *vtkobj, long arg)							{ Init(sender, id, arg); m_VtkObj = vtkobj;}
  albaEvent::albaEvent(void *sender, int id, vtkObject       *vtkobj, albaString *s)         { Init(sender, id, 0);   m_VtkObj = vtkobj;m_ALBAString =s;}
  albaEvent::albaEvent(void *sender, int id, vtkProp         *prop,   albaVME *vme)   			{ Init(sender, id, 0);   m_VtkProp= prop; m_Vme = vme;}
#endif
		
//----------------------------------------------------------------------------
void albaEvent::Log() 
//----------------------------------------------------------------------------
{
  albaString s = "[EV]";
               s << " sender= "<< (long)m_Sender;
               s << " ID= "    << albaIdString(m_Id).ToAscii();
  if(m_Arg)    s << " arg= "   << m_Arg;
  if(m_Bool)   s << " bool= "  << (int)m_Bool;
  if(m_Double)  s << " double= " << m_Double;
  if(m_ALBAString) s << " string= "<< *m_ALBAString;
  if(m_View)   s << " view= "  << (long)m_View<<" : " << m_View->GetLabel();
  if(m_x)      s << " x= " << m_x;
  if(m_y)      s << " y= " << m_y;
  if(m_width)  s << " width= " << m_width;
  if(m_height) s << " height= " << m_height;
  if(m_Vme)    s << " vme= "   << (long)m_Vme <<" : " << m_Vme->GetName();
  if(m_Op)     s << " op= "    << (long)m_Op  <<" : " << m_Op->m_Label;
#ifdef ALBA_USE_WX
  if(m_Win)    s << " win= "   << (long)m_Win;
  if(m_UpdateUIEvent)   s << " ui_evt= "<< (long)m_UpdateUIEvent;
  if(m_WxObj)  s << " wxobj= " << (long)m_WxObj;
#endif
#ifdef ALBA_USE_VTK
  if(m_VtkProp)   s << " prop= "  << (long)m_VtkProp;
  if(m_Matrix) s << " matrix= "<< (long)m_Matrix;
  if(m_OldMatrix)s << " matrix= "<< (long)m_OldMatrix;
  if(m_VtkObj) s << " vtkobj= "<< (long)m_VtkObj << " : " << m_VtkObj->GetClassName();
#endif
  if(m_MafObject) s << " albaobj= " << (long)m_MafObject << " : " << m_MafObject->GetTypeName();

  albaLogMessage(s);
}
//----------------------------------------------------------------------------
albaEvent* albaEvent::Copy() 
//----------------------------------------------------------------------------
{
  albaEvent *e	= new albaEvent(m_Sender,m_Id,m_Bool,m_Arg);
  e->m_Double		= m_Double;
  e->m_ALBAString= m_ALBAString;
  e->m_View		  = m_View;
  e->m_Vme			= m_Vme;
  e->m_Op		    = m_Op;
  e->m_MafObject= m_MafObject;
  e->m_x = m_x;
  e->m_y = m_y;
  e->m_width = m_width;
  e->m_height = m_height;
  e->m_WidgetData.dType = m_WidgetData.dType;
  e->m_WidgetData.dValue= m_WidgetData.dValue;
  e->m_WidgetData.fValue= m_WidgetData.fValue;
  e->m_WidgetData.iValue= m_WidgetData.iValue;
  e->m_WidgetData.sValue= m_WidgetData.sValue;
#ifdef ALBA_USE_WX
  e->m_Win			= m_Win;
  e->m_UpdateUIEvent     = m_UpdateUIEvent;
  e->m_WxObj    = m_WxObj;
#endif
#ifdef ALBA_USE_VTK
  e->m_VtkProp		  = m_VtkProp;
  e->m_Matrix   = m_Matrix;
  e->m_OldMatrix  = m_OldMatrix;
  e->m_VtkObj   = m_VtkObj;
#endif

  return e;
}
//----------------------------------------------------------------------------
void albaEvent::DeepCopy(const albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  m_Sender  = ((albaEvent *)alba_event)->GetSender();
  m_Id      = ((albaEvent *)alba_event)->GetId();
  m_Arg     = ((albaEvent *)alba_event)->GetArg();
  m_Bool    = ((albaEvent *)alba_event)->GetBool();
  m_Double  = ((albaEvent *)alba_event)->GetDouble();
  m_ALBAString  = ((albaEvent *)alba_event)->GetString();
  m_Vme     = ((albaEvent *)alba_event)->GetVme();
  m_View    = ((albaEvent *)alba_event)->GetView();
  m_Op      = ((albaEvent *)alba_event)->GetOp();
  m_Matrix  = ((albaEvent *)alba_event)->GetMatrix();
  m_OldMatrix = ((albaEvent *)alba_event)->GetOldMatrix();
  m_MafObject = ((albaEvent *)alba_event)->GetMafObject();
  m_x         = ((albaEvent *)alba_event)->GetX();
  m_y         = ((albaEvent *)alba_event)->GetY();
  m_width     = ((albaEvent *)alba_event)->GetWidth();
  m_height    = ((albaEvent *)alba_event)->GetHeight();
  ((albaEvent *)alba_event)->GetWidgetData(m_WidgetData);
#ifdef ALBA_USE_WX
  m_WxObj   = ((albaEvent *)alba_event)->GetWxObj();
  m_Win     = ((albaEvent *)alba_event)->GetWin();
#endif
#ifdef ALBA_USE_VTK
  m_VtkObj  = ((albaEvent *)alba_event)->GetVtkObj();
#endif
}
//----------------------------------------------------------------------------
void albaEvent::Init(void *sender, int id, long arg) 
//----------------------------------------------------------------------------
{
  m_Sender = sender;
  m_Id     = id; 
  m_Arg    = arg;
  m_Bool   = false; 
  m_Double  = 0; 
  m_ALBAString = NULL; 
  m_View   = NULL; 
  m_Vme    = NULL; 
  m_Op	   = NULL; 
  m_MafObject = NULL;
  m_Matrix = NULL;
  m_OldMatrix= NULL;
  m_x = 0;
  m_y = 0;
  m_width = 0;
  m_height = 0;
	m_Pointer = NULL;
#ifdef ALBA_USE_WX
  m_Win    = NULL;
  m_UpdateUIEvent   = NULL;
  m_WxObj  = NULL;
#endif
#ifdef ALBA_USE_VTK
  m_VtkProp   = NULL;
  m_VtkObj = NULL;
#endif
}

//----------------------------------------------------------------------------
void albaEvent::GetWidgetData(WidgetDataType &widget_data)
//----------------------------------------------------------------------------
{
  widget_data.dType = m_WidgetData.dType;
  widget_data.dValue= m_WidgetData.dValue;
  widget_data.fValue= m_WidgetData.fValue;
  widget_data.iValue= m_WidgetData.iValue;
  widget_data.sValue= m_WidgetData.sValue;
}
//----------------------------------------------------------------------------
void albaEvent::SetWidgetData(WidgetDataType &widget_data)
//----------------------------------------------------------------------------
{
  m_WidgetData.dType = widget_data.dType;
  m_WidgetData.dValue= widget_data.dValue;
  m_WidgetData.fValue= widget_data.fValue;
  m_WidgetData.iValue= widget_data.iValue;
  m_WidgetData.sValue= widget_data.sValue;
}
//------------------------------------------------------------------------------
void albaEvent::SetVmeVector(std::vector<albaVME*> vmeVector)
//------------------------------------------------------------------------------
{
  m_VmeVector.swap(vmeVector);
}

//------------------------------------------------------------------------------
std::vector<albaVME*> albaEvent::GetVmeVector()
//------------------------------------------------------------------------------
{
  return m_VmeVector;
}
//------------------------------------------------------------------------------
albaString* albaEvent::GetString()
//------------------------------------------------------------------------------
{
  return m_ALBAString;
}
//------------------------------------------------------------------------------
void albaEvent::SetString( albaString *s )
//------------------------------------------------------------------------------
{
  m_ALBAString = s;
}
