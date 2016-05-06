/*=========================================================================

 Program: MAF2
 Module: testView
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class mafVME;
class mafObserver;

//----------------------------------------------------------------------------
// testView :
//----------------------------------------------------------------------------
/** 
 an example view that can display every vme which self provide a VisualPipe
*/
class __declspec( dllexport ) testView: public mafViewVTK
{
public:
  testView(wxString label) : mafViewVTK(label) {};

  virtual mafView*  Copy(mafObserver *Listener);
  virtual int 	    GetNodeStatus(mafVME *vme);
  virtual void	    VmeCreatePipe(mafVME *vme);
  virtual void	    VmeDeletePipe(mafVME *vme);

protected:
};
#endif
