/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testRWIBaseDlg.h,v $
  Language:  C++
  Date:      $Date: 2005-04-16 10:00:18 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __testRWIBaseDlg_H__
#define __testRWIBaseDlg_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/dialog.h"
#include "mmgDialog.h"

#include "mafRWIBase.h"
#include "mafRWI.h"
#include "vtkPolyData.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
//----------------------------------------------------------------------------
// testRWIBaseDlg :
//----------------------------------------------------------------------------
/**
sample showing the usage of RWIBase
*/
class testRWIBaseDlg : public mmgDialog
{
public:
	testRWIBaseDlg (const wxString& title);
	virtual ~testRWIBaseDlg (); 
protected:
	virtual void OnCloseWindow(wxCloseEvent& event);

  vtkConeSource *CS;
  vtkPolyDataMapper *PDM;
  vtkActor *A;
  vtkRenderer *R;
  vtkRenderWindow *RW; 
  mafRWIBase *RWI;
};
#endif
