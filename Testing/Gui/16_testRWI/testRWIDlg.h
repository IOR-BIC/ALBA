/*=========================================================================

 Program: MAF2
 Module: testRWIDlg
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
