/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testView.h,v $
  Language:  C++
  Date:      $Date: 2005-04-27 13:12:02 $
  Version:   $Revision: 1.1 $
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

  virtual mafView*  Copy(mafEventListener *Listener);
  virtual int 	    GetNodeStatus(mafNode *vme);
  virtual void	    VmeCreatePipe(mafNode *vme);
  virtual void	    VmeDeletePipe(mafNode *vme);

protected:
};
#endif
