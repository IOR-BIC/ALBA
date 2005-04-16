/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testRWIDlg.h,v $
  Language:  C++
  Date:      $Date: 2005-04-16 10:00:19 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __testRWIDlg_H__
#define __testRWIDlg_H__
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
// testRWIDlg :
//----------------------------------------------------------------------------
/**
sample showing the usage of RWIBase
*/
class testRWIDlg : public mmgDialog
{
public:
	testRWIDlg (const wxString& title);
	virtual ~testRWIDlg (); 
protected:
	virtual void OnCloseWindow(wxCloseEvent& event);

  vtkConeSource *CS;
  vtkPolyDataMapper *PDM;
  vtkActor *A;
  mafRWI *RWI;
};
#endif
