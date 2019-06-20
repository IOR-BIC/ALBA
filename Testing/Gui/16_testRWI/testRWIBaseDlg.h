/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testRWIBaseDlg
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


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
#include "albaGUIDialog.h"

#include "albaRWIBase.h"
#include "albaRWI.h"
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
class testRWIBaseDlg : public albaGUIDialog
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
  albaRWIBase *RWI;
};
#endif
