/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEvent.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:06:01 $
  Version:   $Revision: 1.9 $
  Authors:   Silvano Imboden, Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafEvent_h
#define __mafEvent_h

//----------------------------------------------------------------------------
// includes:
//----------------------------------------------------------------------------
#include "mafDefines.h"   // mafDefines should alway be included as first
#include "mafEventBase.h" // base class for mafEvent
#include "mafObserver.h"  // base class for mafEventListener
#include "mafString.h"    // used by mafEvent
//----------------------------------------------------------------------------
// Forward References:
//----------------------------------------------------------------------------
class mafView;
class mafOp;
class mafNode;

#ifdef MAF_USE_VTK
  class vtkObject;
  class vtkProp;
  class vtkMatrix4x4;
#endif
//----------------------------------------------------------------------------
// mafEvent :
//----------------------------------------------------------------------------
/** mafEvent - Class implementing MAF application events.
  This class implements a type of event object similar to original mafEvent, where
  all kind of possible information traveling around the MAF is defined. 
  @sa mafEventBase mafSubject mafObserver
*/
class mafEvent : public mafEventBase
{
public:
  mafTypeMacro(mafEvent,mafEventBase);

  mafEvent();                                                         
  mafEvent(void *sender, int id,                          long arg=0);
  mafEvent(void *sender, int id, bool            b,       long arg=0);
  mafEvent(void *sender, int id, float           f,       long arg=0);
  mafEvent(void *sender, int id, mafString      *s,       long arg=0);
  mafEvent(void *sender, int id, mafNode         *vme,    bool b=false, long arg=0);
  mafEvent(void *sender, int id, mafView         *view,   wxWindow *win=NULL);
  mafEvent(void *sender, int id, mafOp					 *op,     long arg=0);

	void*            GetSender()  {return m_sender;}
  int              GetId()      {return m_id;};
  long             GetArg()     {return m_arg;};
  bool             GetBool()    {return m_bool;};
  float            GetFloat()   {return m_float;};
  mafString*       GetString()  {return m_string;};
  mafView*         GetView()    {return m_view;};
  mafNode*         GetVme()     {return m_vme;};
  mafOp*					 GetOp()      {return m_op;};

  void SetSender(void* sender)  { m_sender = sender;};
  void SetId(int id)            { m_id = id;};
  void SetArg(long arg)         { m_arg = arg;};
  void SetBool(bool b)          { m_bool = b;};
  void SetFloat(float f)        { m_float = f;};
  void SetString(mafString *s)  { m_string = s;};
  void SetView(mafView* view)   { m_view = view;};
  void SetVme(mafNode* vme)     { m_vme = vme;};
  void SetOp(mafOp* op)         { m_op = op;};

protected:
  void            *m_sender;         
  int              m_id;       
  long             m_arg;        
  bool             m_bool;        
  float            m_float;        
  mafString       *m_string;        

  mafNode         *m_vme;
  mafView         *m_view;
  mafOp						*m_op;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_WX
public:
  mafEvent(void *sender, int id, wxWindow        *win,    long arg=0);
  mafEvent(void *sender, int id, wxUpdateUIEvent *e,      long arg=0);
  mafEvent(void *sender, int id, wxObject        *wxobj,  long arg=0);

  wxWindow*        GetWin()       {return m_win;};
  wxUpdateUIEvent* GetUIEvent()   {return m_uiev;};
  wxObject*        GetWxObj()     {return m_wxobj;};

  void SetWin(wxWindow* win)            { m_win = win;};
  void SetUIEvent(wxUpdateUIEvent *e)   { m_uiev =e;};
  void SetWxObj(wxObject *wxobj)        { m_wxobj = wxobj;};

protected:
  wxWindow        *m_win;
  wxUpdateUIEvent *m_uiev; 
  wxObject        *m_wxobj; 
#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_VTK
public:
  mafEvent(void *sender, int id, vtkProp         *prop,   mafNode *vme=NULL);
  mafEvent(void *sender, int id, vtkObject       *vtkobj, long arg=0);
  mafEvent(void *sender, int id, vtkObject       *vtkobj, mafString *s);
  mafEvent(void *sender, int id, vtkMatrix4x4    *m1,vtkMatrix4x4  *m2=NULL);

  vtkProp*         GetProp()      {return m_prop;};
  vtkMatrix4x4*    GetMatrix()    {return m_matrix;};
  vtkMatrix4x4*    GetOldMatrix() {return m_matrix2;};
  vtkObject*       GetVtkObj()    {return m_vtkobj;};

  void SetProp(vtkProp* prop)             { m_prop = prop;};
  void SetMatrix(vtkMatrix4x4* mat)       { m_matrix = mat;};
  void SetOldMatrix(vtkMatrix4x4* mat2)   { m_matrix2 =mat2;};
  void SetVtkObj(vtkObject *vtkobj)       { m_vtkobj = vtkobj;};

protected:
  vtkProp         *m_prop;
  vtkMatrix4x4    *m_matrix;
  vtkMatrix4x4    *m_matrix2;
  vtkObject       *m_vtkobj; 
#endif  

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

public:
  mafEvent* Copy();

  void Log();
  static void     SetLogMode(int logmode);
  static int      m_logmode;
  
protected:

  void Init(void *sender, int id, long arg=0);
  void Initialized();
};


//------------------------------------------------------------------------------
// mafEventListener
//------------------------------------------------------------------------------
/** 
SubClass of mafObserver for back-compatibility. 
mafObserver::OnEvent(mafEventBase *e)  -- the new mafEvent take a pointer to mafEventBase
mafEventListener::OnEvent(mafEvent &e) -- the previous mafEvent take a pointer to mafEventBase
*/
//NOTE: mafEventListener is declared inside mafEvent for back compatibility
class MAF_EXPORT mafEventListener : public mafObserver
{
public:
  mafEventListener() {}
  virtual ~mafEventListener() {}

  /** simulate the old OnEvent for backcompatibility.
      the argument is a reference to a mafEvent  */
  virtual void OnEvent(mafEvent &e) {} ;

  /** new OnEvent (the argument is a pointer to a mafEventBase),
      here it is redirected to the old OnEvent */
  virtual void OnEvent(mafEventBase *e)  \
  {                                      \
    if(e->IsMAFType(mafEvent))              \
        OnEvent( *((mafEvent*)e) );      \
    else                                 \
        assert(false);                   \
  };
};


#endif /* __mafEvent_h */
