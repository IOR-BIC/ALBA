/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testRWIDlg.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:41 $
  Version:   $Revision: 1.2 $
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
#include "mafGUIDialog.h"

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
class testRWIDlg : public mafGUIDialog
{
public:
	testRWIDlg (const wxString& title);
	virtual ~testRWIDlg (); 
protected:
  vtkConeSource *CS;
  vtkPolyDataMapper *PDM;
  vtkActor *A;
  mafRWI *RWI;
};
#endif
