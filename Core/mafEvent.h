/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEvent.h,v $
  Language:  C++
  Date:      $Date: 2004-11-04 12:18:53 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden, Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafEvent_h
#define __mafEvent_h

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafEventBase.h"
//#include "mafString.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

class mafView;
class mafOp;
//class mflVME;
//class vtkObject;
//class vtkProp;
//class vtkMatrix4x4;

/** mafEvent - Class implementing MAF application events.
  This class implements a type of event object similar to original mafEvent, where
  all kind of possible information traveling around the MAF is defined. 
  @sa mafEventBase mafSubject mafObserver
*/
class mafEvent : public mafEventBase
{
public:
  mafTypeMacro(mafEvent,mafEventBase);

	void*            GetSender();
  int              GetId();
  long             GetArg();
  bool             GetBool();
  float            GetFloat();
  wxString*        GetString();
  const char*      GetSmartString();
//  mflVME*          GetVme();
  mafView*         GetView();
  mafOp*					 GetOp();
  wxWindow*        GetWin();
//  vtkProp*         GetProp();
//  vtkMatrix4x4*    GetMatrix();
//  vtkMatrix4x4*    GetOldMatrix();
  wxUpdateUIEvent* GetUIEvent();
//  vtkObject*       GetVtkObj();
	
  void SetSender(void* sender);
  void SetId(int id);
  void SetArg(long arg);
  void SetBool(bool b);
  void SetFloat(float f);
  void SetString(wxString *s);
  void SetSmartString(const char *s);
  void SetSmartString(mafString *s);
//  void SetVme(mflVME* vme);
  void SetView(mafView* view);
  void SetOp(mafOp* op);
  void SetWin(wxWindow* win);
//  void SetProp(vtkProp* prop);
//  void SetMatrix(vtkMatrix4x4* mat);
//  void SetOldMatrix(vtkMatrix4x4* mat2);
  void SetUIEvent(wxUpdateUIEvent *e);
//  void SetVtkObj(vtkObject *vtkobj);
  
  void          Log();
  static void   SetLogMode(int logmode) { mafEvent::LogMode = logmode;}
  static int    GetLogMode()            {return mafEvent::LogMode;}

  long             Arg;        
  bool             Bool;        
  float            Float;        
  wxString        *String;        
  mafView         *View;
  mafOp						*Op;
  wxWindow        *Win;
  vtkProp         *Prop;
  
  wxUpdateUIEvent *UIEvent;
  static int       LogMode;

  //mflAutoPointer<vtkMatrix4x4>  Matrix;
  //mflAutoPointer<vtkMatrix4x4>  Matrix2;
  //mflAutoPointer<mflVME>        Vme;
  //mflAutoPointer<vtkObject>     VTKObject;
  mafString                     SmartString;
  
protected:
  mafEvent();                                                         
  mafEvent(void *sender, mafID id,                          long arg=0);
  mafEvent(void *sender, mafID id, bool            b,       long arg=0);
  mafEvent(void *sender, mafID id, float           f,       long arg=0);
  mafEvent(void *sender, mafID id, wxString        *s,      long arg=0);
  mafEvent(void *sender, mafID id, const char      *s,      long arg=0);
//  mafEvent(void *sender, mafID id, mflVME          *vme,    bool b=false, long arg=0);
  mafEvent(void *sender, mafID id, mafView         *view,   wxWindow *win=NULL);
  mafEvent(void *sender, mafID id, mafOp					 *op,     long arg=0);
  mafEvent(void *sender, mafID id, wxWindow        *win,    long arg=0);
  mafEvent(void *sender, mafID id, wxUpdateUIEvent *e,      long arg=0);
//  mafEvent(void *sender, mafID id, vtkProp         *prop,   mflVME *vme=NULL);
//  mafEvent(void *sender, mafID id, vtkObject       *vtkobj, long arg=0);
//  mafEvent(void *sender, mafID id, vtkObject       *vtkobj, wxString *s);
//  mafEvent(void *sender, mafID id, vtkMatrix4x4    *m1,     vtkMatrix4x4  *m2=NULL);

  void Init(void *sender, mafID id, long arg=0);
  void Initialized();

  mafEvent(const mafEvent& c) {}
  //void operator=(const mafEvent&) {}
};

#endif /* __mafEvent_h */
