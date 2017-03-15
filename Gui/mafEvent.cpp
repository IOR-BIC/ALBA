/*=========================================================================

 Program: MAF2
 Module: mafEvent
 Authors: Marco Petrone, Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
#include "mafVME.h"
#include "mafString.h"
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
  mafEvent::mafEvent()																																		  { Init(NULL,NO_EVENT,0);}
  mafEvent::mafEvent(void *sender, int id,                            long arg)							{ Init(sender, id, arg);}
  mafEvent::mafEvent(void *sender, int id, bool             b,        long arg)							{ Init(sender, id, arg); m_Bool =b;}
  mafEvent::mafEvent(void *sender, int id, double           f,        long arg)             { Init(sender, id, arg); m_Double=f;}
  mafEvent::mafEvent(void *sender, int id, mafString        *s,       long arg)							{ Init(sender, id, arg); m_MAFString =s;}
  mafEvent::mafEvent(void *sender, int id, mafString       *s, int x, int y, int width, int height,  long arg)  { Init(sender, id, arg); m_MAFString =s; m_x = x; m_y = y; m_width = width; m_height = height;}
  mafEvent::mafEvent(void *sender, int id, mafView          *view,    wxWindow *win)				{ Init(sender, id, 0);   m_View =view; m_Win  =win;}
  mafEvent::mafEvent(void *sender, int id, mafVME          *vme,     bool b,long arg)			{ Init(sender, id, arg); m_Vme  =vme; m_Bool = b;}
  mafEvent::mafEvent(void *sender, int id, mafOp            *op,      long arg)							{ Init(sender, id, arg);   m_Op   =op;}
  mafEvent::mafEvent(void *sender, int id, mafMatrix        *m1,mafMatrix  *m2)					    { Init(sender, id, 0);   m_Matrix =m1; m_OldMatrix =m2;}
  mafEvent::mafEvent(void *sender, int id, mafObject        *mafobj,  long arg)						  { Init(sender, id, arg); m_MafObject = mafobj;}
  mafEvent::mafEvent(void *sender, int id, mafObject        *mafobj,  mafString *s,long arg){ Init(sender, id, arg); m_MafObject = mafobj; m_MAFString =s;}
  mafEvent::mafEvent(void *sender, int id, WidgetDataType   &widget_data,  long arg)
  {
    Init(sender, id, arg);
    m_WidgetData.dType = widget_data.dType;
    m_WidgetData.dValue= widget_data.dValue;
    m_WidgetData.fValue= widget_data.fValue;
    m_WidgetData.iValue= widget_data.iValue;
    m_WidgetData.sValue= widget_data.sValue;
  }
#ifdef MAF_USE_WX
  mafEvent::mafEvent(void *sender, int id, wxWindow        *win,    long arg)							{ Init(sender, id, arg); m_Win  =win;}
  mafEvent::mafEvent(void *sender, int id, wxUpdateUIEvent *e,      long arg)							{ Init(sender, id, arg); m_UpdateUIEvent = e;																	}
  mafEvent::mafEvent(void *sender, int id, wxObject        *wxobj,  long arg)							{ Init(sender, id, arg); m_WxObj = wxobj;}
#endif
#ifdef MAF_USE_VTK
  mafEvent::mafEvent(void *sender, int id, vtkObject       *vtkobj, long arg)							{ Init(sender, id, arg); m_VtkObj = vtkobj;}
  mafEvent::mafEvent(void *sender, int id, vtkObject       *vtkobj, mafString *s)         { Init(sender, id, 0);   m_VtkObj = vtkobj;m_MAFString =s;}
  mafEvent::mafEvent(void *sender, int id, vtkProp         *prop,   mafVME *vme)   			{ Init(sender, id, 0);   m_VtkProp= prop; m_Vme = vme;}
#endif
		
//----------------------------------------------------------------------------
void mafEvent::Log() 
//----------------------------------------------------------------------------
{
  mafString s = "[EV]";
               s << " sender= "<< (long)m_Sender;
               s << " ID= "    << mafIdString(m_Id).c_str();
  if(m_Arg)    s << " arg= "   << m_Arg;
  if(m_Bool)   s << " bool= "  << (int)m_Bool;
  if(m_Double)  s << " double= " << m_Double;
  if(m_MAFString) s << " string= "<< *m_MAFString;
  if(m_View)   s << " view= "  << (long)m_View<<" : " << m_View->GetLabel();
  if(m_x)      s << " x= " << m_x;
  if(m_y)      s << " y= " << m_y;
  if(m_width)  s << " width= " << m_width;
  if(m_height) s << " height= " << m_height;
  if(m_Vme)    s << " vme= "   << (long)m_Vme <<" : " << m_Vme->GetName();
  if(m_Op)     s << " op= "    << (long)m_Op  <<" : " << m_Op->m_Label;
#ifdef MAF_USE_WX
  if(m_Win)    s << " win= "   << (long)m_Win;
  if(m_UpdateUIEvent)   s << " ui_evt= "<< (long)m_UpdateUIEvent;
  if(m_WxObj)  s << " wxobj= " << (long)m_WxObj;
#endif
#ifdef MAF_USE_VTK
  if(m_VtkProp)   s << " prop= "  << (long)m_VtkProp;
  if(m_Matrix) s << " matrix= "<< (long)m_Matrix;
  if(m_OldMatrix)s << " matrix= "<< (long)m_OldMatrix;
  if(m_VtkObj) s << " vtkobj= "<< (long)m_VtkObj << " : " << m_VtkObj->GetClassName();
#endif
  if(m_MafObject) s << " mafobj= " << (long)m_MafObject << " : " << m_MafObject->GetTypeName();

  mafLogMessage(s);
}
//----------------------------------------------------------------------------
mafEvent* mafEvent::Copy() 
//----------------------------------------------------------------------------
{
  mafEvent *e	= new mafEvent(m_Sender,m_Id,m_Bool,m_Arg);
  e->m_Double		= m_Double;
  e->m_MAFString= m_MAFString;
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
#ifdef MAF_USE_WX
  e->m_Win			= m_Win;
  e->m_UpdateUIEvent     = m_UpdateUIEvent;
  e->m_WxObj    = m_WxObj;
#endif
#ifdef MAF_USE_VTK
  e->m_VtkProp		  = m_VtkProp;
  e->m_Matrix   = m_Matrix;
  e->m_OldMatrix  = m_OldMatrix;
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
  m_Arg     = ((mafEvent *)maf_event)->GetArg();
  m_Bool    = ((mafEvent *)maf_event)->GetBool();
  m_Double  = ((mafEvent *)maf_event)->GetDouble();
  m_MAFString  = ((mafEvent *)maf_event)->GetString();
  m_Vme     = ((mafEvent *)maf_event)->GetVme();
  m_View    = ((mafEvent *)maf_event)->GetView();
  m_Op      = ((mafEvent *)maf_event)->GetOp();
  m_Matrix  = ((mafEvent *)maf_event)->GetMatrix();
  m_OldMatrix = ((mafEvent *)maf_event)->GetOldMatrix();
  m_MafObject = ((mafEvent *)maf_event)->GetMafObject();
  m_x         = ((mafEvent *)maf_event)->GetX();
  m_y         = ((mafEvent *)maf_event)->GetY();
  m_width     = ((mafEvent *)maf_event)->GetWidth();
  m_height    = ((mafEvent *)maf_event)->GetHeight();
  ((mafEvent *)maf_event)->GetWidgetData(m_WidgetData);
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
  m_Arg    = arg;
  m_Bool   = false; 
  m_Double  = 0; 
  m_MAFString = NULL; 
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
void mafEvent::GetWidgetData(WidgetDataType &widget_data)
//----------------------------------------------------------------------------
{
  widget_data.dType = m_WidgetData.dType;
  widget_data.dValue= m_WidgetData.dValue;
  widget_data.fValue= m_WidgetData.fValue;
  widget_data.iValue= m_WidgetData.iValue;
  widget_data.sValue= m_WidgetData.sValue;
}
//----------------------------------------------------------------------------
void mafEvent::SetWidgetData(WidgetDataType &widget_data)
//----------------------------------------------------------------------------
{
  m_WidgetData.dType = widget_data.dType;
  m_WidgetData.dValue= widget_data.dValue;
  m_WidgetData.fValue= widget_data.fValue;
  m_WidgetData.iValue= widget_data.iValue;
  m_WidgetData.sValue= widget_data.sValue;
}
//------------------------------------------------------------------------------
void mafEvent::SetVmeVector(std::vector<mafVME*> vmeVector)
//------------------------------------------------------------------------------
{
  m_VmeVector.swap(vmeVector);
}

//------------------------------------------------------------------------------
std::vector<mafVME*> mafEvent::GetVmeVector()
//------------------------------------------------------------------------------
{
  return m_VmeVector;
}
//------------------------------------------------------------------------------
mafString* mafEvent::GetString()
//------------------------------------------------------------------------------
{
  return m_MAFString;
}
//------------------------------------------------------------------------------
void mafEvent::SetString( mafString *s )
//------------------------------------------------------------------------------
{
  m_MAFString = s;
}
