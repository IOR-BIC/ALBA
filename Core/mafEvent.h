/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEvent.h,v $
  Language:  C++
  Date:      $Date: 2005-03-21 17:49:39 $
  Version:   $Revision: 1.4 $
  Authors:   Silvano Imboden, Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafEvent_h
#define __mafEvent_h

//#include "mafIncludeWx.h"
#include   "mafEventBase.h"
#include   "mafString.h"

//----------------------------------------------------------------------------
// Forward References:
//----------------------------------------------------------------------------
//class mafView;
//class mafOp;
//class mflVME;
//class vtkObject;
//class vtkProp;
//class vtkMatrix4x4;

//extern mafString mafIdString(int id);  ---- defined in mafDecl

//----------------------------------------------------------------------------
// mafEvent :
//----------------------------------------------------------------------------
/** mafEvent - Class implementing MAF application events.
  This class implements a type of event object similar to original mafEvent, where
  all kind of possible information traveling around the MAF is defined. 
  @sa mafEventBase mafSubject mafObserver
*/
class mafEvent 
{
public:
  mafEvent();                                                         
  mafEvent(void *sender, int id,                          long arg=0);
  mafEvent(void *sender, int id, bool            b,       long arg=0);
  mafEvent(void *sender, int id, float           f,       long arg=0);
  mafEvent(void *sender, int id, mafString      *s,       long arg=0);

//mafEvent(void *sender, int id, wxWindow        *win,    long arg=0);
//mafEvent(void *sender, int id, wxUpdateUIEvent *e,      long arg=0);
//mafEvent(void *sender, int id, wxObject        *wxobj,  long arg=0);

//mafEvent(void *sender, int id, mflVME          *vme,    bool b=false, long arg=0);

//mafEvent(void *sender, int id, mafView         *view,   wxWindow *win=NULL);
//mafEvent(void *sender, int id, mafOp					 *op,     long arg=0);

//mafEvent(void *sender, int id, vtkProp         *prop,   mflVME *vme=NULL);
//mafEvent(void *sender, int id, vtkObject       *vtkobj, long arg=0);
//mafEvent(void *sender, int id, vtkObject       *vtkobj, mafString *s);
//mafEvent(void *sender, int id, vtkMatrix4x4    *m1,vtkMatrix4x4  *m2=NULL);
  
	void*            GetSender();
  int              GetId();
  long             GetArg();
  bool             GetBool();
  float            GetFloat();
  mafString*       GetString();
//wxWindow*        GetWin();
//wxUpdateUIEvent* GetUIEvent();
//wxObject*        GetWxObj();
//mafView*         GetView();
//mflVME*          GetVme();
//mafOp*					 GetOp();
//vtkProp*         GetProp();
//vtkMatrix4x4*    GetMatrix();
//vtkMatrix4x4*    GetOldMatrix();
//vtkObject*       GetVtkObj();

  void SetSender(void* sender);
  void SetId(int id);
  void SetArg(long arg);
  void SetBool(bool b);
  void SetFloat(float f);
  void SetString(mafString *s);
//void SetWin(wxWindow* win);
//void SetUIEvent(wxUpdateUIEvent *e);
//void SetWxObj(wxObject *wxobj);
//void SetView(mafView* view);
//void SetVme(mflVME* vme);
//void SetOp(mafOp* op);
//void SetProp(vtkProp* prop);
//void SetMatrix(vtkMatrix4x4* mat);
//void SetOldMatrix(vtkMatrix4x4* mat2);
//void SetVtkObj(vtkObject *vtkobj);

  mafEvent* Copy();

  void Log();

  static void     SetLogMode(int logmode);
  static int      m_logmode;
  
  protected:
  void Init(void *sender, int id, long arg=0);
  void Initialized();

  void            *m_sender;         
  int              m_id;       
  long             m_arg;        
  bool             m_bool;        
  float            m_float;        
  mafString       *m_string;        
//wxWindow        *m_win;
//wxUpdateUIEvent *m_uiev; 
//wxObject        *m_wxobj; 
//mafView         *m_view;
//mflVME          *m_vme;
//mafOp						*m_op;
//vtkProp         *m_prop;
//vtkMatrix4x4    *m_matrix;
//vtkMatrix4x4    *m_matrix2;
//vtkObject       *m_vtkobj; 
};

#endif /* __mafEvent_h */
