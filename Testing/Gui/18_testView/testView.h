/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testView
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testView_H__
#define __testView_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaViewVTK.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaObserver;
//----------------------------------------------------------------------------
// testView :
//----------------------------------------------------------------------------
/** 
 an example view that can display every vme which self provide a VisualPipe
*/
class testView: public albaViewVTK
{
public:
  testView(wxString label) : albaViewVTK(label) {};

  virtual albaView*  Copy(albaObserver *Listener);
  virtual int 	    GetNodeStatus(albaNode *vme);
  virtual void	    VmeCreatePipe(albaNode *vme);
  virtual void	    VmeDeletePipe(albaNode *vme);

protected:
};
#endif
