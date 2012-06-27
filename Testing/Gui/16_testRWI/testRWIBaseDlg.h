/*=========================================================================

 Program: MAF2
 Module: testRWIBaseDlg
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testRWIBaseDlg_H__
#define __testRWIBaseDlg_H__
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
// testRWIBaseDlg :
//----------------------------------------------------------------------------
/**
sample showing the usage of RWIBase
*/
class testRWIBaseDlg : public mafGUIDialog
{
public:
	testRWIBaseDlg (const wxString& title);
	virtual ~testRWIBaseDlg (); 
protected:

  vtkConeSource *CS;
  vtkPolyDataMapper *PDM;
  vtkActor *A;
  vtkRenderer *R;
  vtkRenderWindow *RW; 
  mafRWIBase *RWI;
};
#endif
