/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateVmeSurface.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 12:02:57 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOpCreateVmeSurface.h"
#include "mafVMESurface.h"
#include "vtkConeSource.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateVmeSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateVmeSurface::mafOpCreateVmeSurface(wxString label)
: mafOp(label)
//----------------------------------------------------------------------------
{
  m_Canundo = true;
  m_vme = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateVmeSurface::~mafOpCreateVmeSurface()
//----------------------------------------------------------------------------
{
  mafDEL(m_vme);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateVmeSurface::Copy()
//----------------------------------------------------------------------------
{
  return new mafOpCreateVmeSurface(m_Label);
}
//----------------------------------------------------------------------------
void mafOpCreateVmeSurface::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  mafEventMacro(*event);
}
//----------------------------------------------------------------------------
void mafOpCreateVmeSurface::OpRun()
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mafOpCreateVmeSurface::OpDo()
//----------------------------------------------------------------------------
{
  assert(!m_vme);

  mafNEW(m_vme);
  static int counter = 0;
  wxString name = wxString::Format("vme surface %d",counter++);
  m_vme->SetName(name);
  m_vme->ReparentTo( (mafVME*)m_Input);

  vtkConeSource *cs = vtkConeSource::New();
  cs->SetResolution(20);
  cs->CappingOn();
  cs->Update();
  m_vme->SetData(cs->GetOutput(),0);
  cs->Delete();

  mafEventMacro(mafEvent(this,VME_ADD,m_vme));
}
//----------------------------------------------------------------------------
void mafOpCreateVmeSurface::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_vme);
  mafEventMacro(mafEvent(this,VME_REMOVE,m_vme));
  mafDEL(m_vme);
}
