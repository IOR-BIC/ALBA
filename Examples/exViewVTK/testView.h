/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testView.h,v $
  Language:  C++
  Date:      $Date: 2005-10-18 15:32:41 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __testView_H__
#define __testView_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafView;
class mafNode;
class mafObserver;

//----------------------------------------------------------------------------
// testView :
//----------------------------------------------------------------------------
/** 
 an example view that can display every vme which self provide a VisualPipe
*/
class testView: public mafViewVTK
{
public:
  testView(wxString label) : mafViewVTK(label) {};

  virtual mafView*  Copy(mafObserver *Listener);
  virtual int 	    GetNodeStatus(mafNode *vme);
  virtual void	    VmeCreatePipe(mafNode *vme);
  virtual void	    VmeDeletePipe(mafNode *vme);

protected:
};
#endif
