/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEvent
 Authors: Silvano Imboden, Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaEvent_h
#define __albaEvent_h
//----------------------------------------------------------------------------
// includes:
//----------------------------------------------------------------------------
#include "albaDefines.h"   // albaDefines should alway be included as first
#include "albaDecl.h"
#include "albaEventBase.h" // base class for albaEvent
#include "albaObserver.h"

//----------------------------------------------------------------------------
// Forward References:
//----------------------------------------------------------------------------
class albaView;
class albaOp;
class albaVME;
class albaMatrix;
class albaString;

#ifdef ALBA_USE_VTK
  class vtkObject;
  class vtkProp;  
#endif

	class wxUpdateUIEvent;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
  EXPORT_STL_VECTOR(ALBA_EXPORT,albaVME *);
#endif
//----------------------------------------------------------------------------
// albaEvent :
//----------------------------------------------------------------------------
/** albaEvent - Class implementing ALBA application events.
  This class implements a type of event object similar to original albaEvent, where
  all kind of possible information traveling around the ALBA is defined. 
  @sa albaEventBase albaSubject albaObserver
*/
class ALBA_EXPORT albaEvent : public albaEventBase
{
public:
  albaTypeMacro(albaEvent,albaEventBase);

  albaEvent();                                                         
  albaEvent(void *sender, int id,                           long arg=0);
  albaEvent(void *sender, int id, bool             b,       long arg=0);
  albaEvent(void *sender, int id, double           f,       long arg=0);
  albaEvent(void *sender, int id, albaString       *s,       long arg=0);
  albaEvent(void *sender, int id, albaString       *s, int x, int y, int width, int height,  long arg=0);
  albaEvent(void *sender, int id, albaVME         *vme,     bool b=false, long arg=0);
  albaEvent(void *sender, int id, albaView         *view,    wxWindow *win=NULL);
  albaEvent(void *sender, int id, albaOp					 *op,      long arg=0);
  albaEvent(void *sender, int id, albaObject       *albaobj,  long arg=0);
  albaEvent(void *sender, int id, albaObject       *albaobj,  albaString       *s, long arg=0);
  albaEvent(void *sender, int id, WidgetDataType  &widget_data,  long arg=0);
  albaEvent(void *sender, int id, albaMatrix    *m1,albaMatrix  *m2=NULL);

  virtual void DeepCopy(const albaEventBase *alba_event);

  long              GetArg()     {return m_Arg;};
  bool              GetBool()    {return m_Bool;};
  double            GetDouble()   {return m_Double;};
  albaString*        GetString();
  albaView*          GetView()    {return m_View;};
  albaVME*						GetVme()     {return m_Vme;};
  albaOp*					  GetOp()      {return m_Op;};
  albaMatrix*        GetMatrix()    {return m_Matrix;};
  albaMatrix*        GetOldMatrix() {return m_OldMatrix;};
  albaObject*        GetMafObject() {return m_MafObject;}

  int GetX() {return m_x;};
  int GetY() {return m_y;};
  int GetWidth() {return m_width;};
  int GetHeight() {return m_height;};

  /** set call data, data sent by sender (event's invoker) to all observers. 
  Be aware that the vmeVector argument will be empty after the Set */
  void SetVmeVector(std::vector<albaVME*> vmeVector);

  /** return call data, data sent by sender (event's invoker) to all observers */
  std::vector<albaVME*> GetVmeVector();

  void GetWidgetData(WidgetDataType &widget_data);

  void SetArg(long arg)         { m_Arg = arg;};
  void SetBool(bool b)          { m_Bool = b;};
  void SetDouble(double f)      { m_Double = f;};
  void SetString(albaString *s);
  void SetView(albaView* view)   { m_View = view;};
  void SetVme(albaVME* vme)     { m_Vme = vme;};
  void SetOp(albaOp* op)         { m_Op = op;};
  void SetMatrix(albaMatrix* mat)       { m_Matrix = mat;};
  void SetOldMatrix(albaMatrix* mat2)   { m_OldMatrix =mat2;};
  void SetMafObject(albaObject* obj)    { m_MafObject = obj;}
  void SetWidgetData(WidgetDataType &widget_data);


	/** Returns Pointer */
	void * GetPointer() const { return m_Pointer; }

	/** Sets Pointer */
	void SetPointer(void * pointer) { m_Pointer = pointer; }

protected:
  long						 m_Arg;
  bool             m_Bool;
  double           m_Double;
  albaString       *m_ALBAString;

  albaVME					*m_Vme;
  albaView         *m_View;
  albaOp						*m_Op;
  albaMatrix       *m_Matrix;
  albaMatrix       *m_OldMatrix;
  albaObject       *m_MafObject;
  std::vector<albaVME*> m_VmeVector;
  WidgetDataType   m_WidgetData;
	void						*m_Pointer;

  int m_x;
  int m_y;
  int m_width;
  int m_height;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef ALBA_USE_WX
public:
  albaEvent(void *sender, int id, wxWindow        *win,    long arg=0);
  albaEvent(void *sender, int id, wxUpdateUIEvent *e,      long arg=0);
  albaEvent(void *sender, int id, wxObject        *wxobj,  long arg=0);

  wxWindow*        GetWin()       {return m_Win;};
  wxUpdateUIEvent* GetUIEvent()   {return m_UpdateUIEvent;};
  wxObject*        GetWxObj()     {return m_WxObj;};

  void SetWin(wxWindow* win)            { m_Win = win;};
  void SetUIEvent(wxUpdateUIEvent *e)   { m_UpdateUIEvent =e;};
  void SetWxObj(wxObject *wxobj)        { m_WxObj = wxobj;};

protected:
  wxWindow        *m_Win;
  wxUpdateUIEvent *m_UpdateUIEvent; 
  wxObject        *m_WxObj; 
#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef ALBA_USE_VTK
public:
  albaEvent(void *sender, int id, vtkProp         *prop,   albaVME *vme=NULL);
  albaEvent(void *sender, int id, vtkObject       *vtkobj, long arg=0);
  albaEvent(void *sender, int id, vtkObject       *vtkobj, albaString *s);

  vtkProp*         GetProp()      {return m_VtkProp;};
  vtkObject*       GetVtkObj()    {return m_VtkObj;};

  void SetProp(vtkProp* prop)             { m_VtkProp = prop;};
  void SetVtkObj(vtkObject *vtkobj)       { m_VtkObj = vtkobj;};

protected:
  vtkProp         *m_VtkProp;
  vtkObject       *m_VtkObj; 
#endif  

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

public:
  albaEvent* Copy();
  void Log();
  
protected:

  void Init(void *sender, int id, long arg=0);
};
#endif /* __albaEvent_h */
