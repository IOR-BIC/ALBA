/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateVmeSurface
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaOpCreateVmeSurface.h"
#include "albaVMESurface.h"
#include "vtkConeSource.h"
#include "vtkPolyData.h"
//----------------------------------------------------------------------------
albaOpCreateVmeSurface::albaOpCreateVmeSurface(wxString label)
: albaOp(label)
//----------------------------------------------------------------------------
{
  m_Canundo = true;
  m_vme = NULL;
}
//----------------------------------------------------------------------------
albaOpCreateVmeSurface::~albaOpCreateVmeSurface()
//----------------------------------------------------------------------------
{
  albaDEL(m_vme);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateVmeSurface::Copy()
//----------------------------------------------------------------------------
{
  return new albaOpCreateVmeSurface(m_Label);
}
//----------------------------------------------------------------------------
void albaOpCreateVmeSurface::OnEvent(albaEvent& e)
//----------------------------------------------------------------------------
{
  albaEventMacro(e);
}
//----------------------------------------------------------------------------
void albaOpCreateVmeSurface::OpRun()
//----------------------------------------------------------------------------
{
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void albaOpCreateVmeSurface::OpDo()
//----------------------------------------------------------------------------
{
  assert(!m_vme);

  albaNEW(m_vme);
  static int counter = 0;
  wxString name = albaString::Format("vme surface %d",counter++);
  m_vme->SetName(name);
  m_vme->ReparentTo( (albaVME*)m_Input);

  vtkConeSource *cs = vtkConeSource::New();
  cs->SetResolution(20);
  cs->CappingOn();
  cs->Update();
  m_vme->SetData(cs->GetOutput(),0);
  cs->Delete();

  albaEventMacro(albaEvent(this,VME_ADD,m_vme));
}
//----------------------------------------------------------------------------
void albaOpCreateVmeSurface::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_vme);
  albaEventMacro(albaEvent(this,VME_REMOVE,m_vme));
  albaDEL(m_vme);
}
