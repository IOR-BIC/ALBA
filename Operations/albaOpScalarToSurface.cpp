/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpScalarToSurface
 Authors: Paolo Quadrani
 
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


#include "albaOpScalarToSurface.h"
#include <wx/busyinfo.h>

#include "albaDecl.h"

#include "albaVME.h"
#include "albaVMEScalar.h"
#include "albaVMESurface.h"
#include "albaVMEOutputScalar.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkDelaunay2D.h"

#include <vector>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpScalarToSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpScalarToSurface::albaOpScalarToSurface(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

  m_Surface = NULL;
}

//----------------------------------------------------------------------------
albaOpScalarToSurface::~albaOpScalarToSurface()
//----------------------------------------------------------------------------
{
  albaDEL(m_Surface);
}

//----------------------------------------------------------------------------
albaOp* albaOpScalarToSurface::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpScalarToSurface(m_Label);
}

//----------------------------------------------------------------------------
bool albaOpScalarToSurface::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVMEScalar) && ((albaVMEScalar*)node)->IsAnimated());
}

//----------------------------------------------------------------------------
void albaOpScalarToSurface::OpRun()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyCursor wait;
  }

  albaString surface_name = m_Input->GetName();
  surface_name += " surface";
  albaNEW(m_Surface);
  m_Surface->SetName(surface_name.GetCStr());
  
  vtkALBASmartPointer<vtkAppendPolyData> scalar_surface;
  vtkALBASmartPointer<vtkDelaunay2D> delaunay;

  std::vector<vtkPolyData *> items;
  vtkPolyData *scalar_item;
  vtkPolyData *current_item;
  albaVMEScalar *scalar = albaVMEScalar::SafeDownCast(m_Input);
  double ts = scalar->GetTimeStamp();
  std::vector<albaTimeStamp> kframes;
  scalar->GetTimeStamps(kframes);
  for (int t = 0; t < kframes.size(); t++)
  {
    scalar->SetTimeStamp(kframes[t]);
    current_item = (vtkPolyData *)scalar->GetScalarOutput()->GetVTKData();
    scalar_item = vtkPolyData::New();
    scalar_item->DeepCopy(current_item);
    items.push_back(scalar_item);
    scalar_surface->AddInputData(items[t]);
  }
  scalar_surface->Update();
  scalar->SetTimeStamp(ts);
  if (!m_TestMode)
  {
    albaEventMacro(albaEvent(this, BIND_TO_PROGRESSBAR,delaunay));
  }
  delaunay->SetInputConnection(scalar_surface->GetOutputPort());
  delaunay->Update();
  m_Surface->SetData(delaunay->GetOutput(),ts);
  m_Output = m_Surface;

  for (int i = 0; i < kframes.size(); i++)
    items[i]->Delete();
  items.clear();
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
