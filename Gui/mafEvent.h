/*=========================================================================

 Program: MAF2
 Module: mafEvent
 Authors: Silvano Imboden, Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafEvent_h
#define __mafEvent_h
//----------------------------------------------------------------------------
// includes:
//----------------------------------------------------------------------------
#include "mafDefines.h"   // mafDefines should alway be included as first
#include "mafDecl.h"
#include "mafEventBase.h" // base class for mafEvent
#include "mafObserver.h"

//----------------------------------------------------------------------------
// Forward References:
//----------------------------------------------------------------------------
class mafView;
class mafOp;
class mafVME;
class mafMatrix;
class mafString;

#ifdef MAF_USE_VTK
  class vtkObject;
  class vtkProp;  
#endif

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
  EXPORT_STL_VECTOR(MAF_EXPORT,mafVME *);
#endif
//----------------------------------------------------------------------------
// mafEvent :
//----------------------------------------------------------------------------
/** mafEvent - Class implementing MAF application events.
  This class implements a type of event object similar to original mafEvent, where
  all kind of possible information traveling around the MAF is defined. 
  @sa mafEventBase mafSubject mafObserver
*/
class MAF_EXPORT mafEvent : public mafEventBase
{
public:
  mafTypeMacro(mafEvent,mafEventBase);

  mafEvent();                                                         
  mafEvent(void *sender, int id,                           long arg=0);
  mafEvent(void *sender, int id, bool             b,       long arg=0);
  mafEvent(void *sender, int id, double           f,       long arg=0);
  mafEvent(void *sender, int id, mafString       *s,       long arg=0);
  mafEvent(void *sender, int id, mafString       *s, int x, int y, int width, int height,  long arg=0);
  mafEvent(void *sender, int id, mafVME         *vme,     bool b=false, long arg=0);
  mafEvent(void *sender, int id, mafView         *view,    wxWindow *win=NULL);
  mafEvent(void *sender, int id, mafOp					 *op,      long arg=0);
  mafEvent(void *sender, int id, mafObject       *mafobj,  long arg=0);
  mafEvent(void *sender, int id, mafObject       *mafobj,  mafString       *s, long arg=0);
  mafEvent(void *sender, int id, WidgetDataType  &widget_data,  long arg=0);
  mafEvent(void *sender, int id, mafMatrix    *m1,mafMatrix  *m2=NULL);

  virtual void DeepCopy(const mafEventBase *maf_event);

  long              GetArg()     {return m_Arg;};
  bool              GetBool()    {return m_Bool;};
  double            GetDouble()   {return m_Double;};
  mafString*        GetString();
  mafView*          GetView()    {return m_View;};
  mafVME*						GetVme()     {return m_Vme;};
  mafOp*					  GetOp()      {return m_Op;};
  mafMatrix*        GetMatrix()    {return m_Matrix;};
  mafMatrix*        GetOldMatrix() {return m_OldMatrix;};
  mafObject*        GetMafObject() {return m_MafObject;}

  int GetX() {return m_x;};
  int GetY() {return m_y;};
  int GetWidth() {return m_width;};
  int GetHeight() {return m_height;};

  /** set call data, data sent by sender (event's invoker) to all observers. 
  Be aware that the vmeVector argument will be empty after the Set */
  void SetVmeVector(std::vector<mafVME*> vmeVector);

  /** return call data, data sent by sender (event's invoker) to all observers */
  std::vector<mafVME*> GetVmeVector();

  void GetWidgetData(WidgetDataType &widget_data);

  void SetArg(long arg)         { m_Arg = arg;};
  void SetBool(bool b)          { m_Bool = b;};
  void SetDouble(double f)      { m_Double = f;};
  void SetString(mafString *s);
  void SetView(mafView* view)   { m_View = view;};
  void SetVme(mafVME* vme)     { m_Vme = vme;};
  void SetOp(mafOp* op)         { m_Op = op;};
  void SetMatrix(mafMatrix* mat)       { m_Matrix = mat;};
  void SetOldMatrix(mafMatrix* mat2)   { m_OldMatrix =mat2;};
  void SetMafObject(mafObject* obj)    { m_MafObject = obj;}
  void SetWidgetData(WidgetDataType &widget_data);


	/** Returns Pointer */
	void * GetPointer() const { return m_Pointer; }

	/** Sets Pointer */
	void SetPointer(void * pointer) { m_Pointer = pointer; }

protected:
  long						 m_Arg;
  bool             m_Bool;
  double           m_Double;
  mafString       *m_MAFString;

  mafVME					*m_Vme;
  mafView         *m_View;
  mafOp						*m_Op;
  mafMatrix       *m_Matrix;
  mafMatrix       *m_OldMatrix;
  mafObject       *m_MafObject;
  std::vector<mafVME*> m_VmeVector;
  WidgetDataType   m_WidgetData;
	void						*m_Pointer;

  int m_x;
  int m_y;
  int m_width;
  int m_height;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_WX
public:
  mafEvent(void *sender, int id, wxWindow        *win,    long arg=0);
  mafEvent(void *sender, int id, wxUpdateUIEvent *e,      long arg=0);
  mafEvent(void *sender, int id, wxObject        *wxobj,  long arg=0);

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
#ifdef MAF_USE_VTK
public:
  mafEvent(void *sender, int id, vtkProp         *prop,   mafVME *vme=NULL);
  mafEvent(void *sender, int id, vtkObject       *vtkobj, long arg=0);
  mafEvent(void *sender, int id, vtkObject       *vtkobj, mafString *s);

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
  mafEvent* Copy();
  void Log();
  
protected:

  void Init(void *sender, int id, long arg=0);
};
#endif /* __mafEvent_h */
