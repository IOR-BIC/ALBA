/*=========================================================================

 Program: MAF2
 Module: mafOpScalarToSurface
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOpScalarToSurface.h"
#include <wx/busyinfo.h>

#include "mafDecl.h"

#include "mafVME.h"
#include "mafVMEScalar.h"
#include "mafVMESurface.h"
#include "mafVMEOutputScalar.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkDelaunay2D.h"

#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpScalarToSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpScalarToSurface::mafOpScalarToSurface(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

  m_Surface = NULL;
}

//----------------------------------------------------------------------------
mafOpScalarToSurface::~mafOpScalarToSurface()
//----------------------------------------------------------------------------
{
  mafDEL(m_Surface);
}

//----------------------------------------------------------------------------
mafOp* mafOpScalarToSurface::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpScalarToSurface(m_Label);
}

//----------------------------------------------------------------------------
bool mafOpScalarToSurface::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMEScalar) && ((mafVMEScalar*)node)->IsAnimated());
}

//----------------------------------------------------------------------------
void mafOpScalarToSurface::OpRun()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyCursor wait;
  }

  mafString surface_name = m_Input->GetName();
  surface_name += " surface";
  mafNEW(m_Surface);
  m_Surface->SetName(surface_name.GetCStr());
  
  vtkMAFSmartPointer<vtkAppendPolyData> scalar_surface;
  vtkMAFSmartPointer<vtkDelaunay2D> delaunay;

  std::vector<vtkPolyData *> items;
  vtkPolyData *scalar_item;
  vtkPolyData *current_item;
  mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(m_Input);
  double ts = scalar->GetTimeStamp();
  std::vector<mafTimeStamp> kframes;
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
    mafEventMacro(mafEvent(this, BIND_TO_PROGRESSBAR,delaunay));
  }
  delaunay->SetInputConnection(scalar_surface->GetOutputPort());
  delaunay->Update();
  m_Surface->SetData(delaunay->GetOutput(),ts);
  m_Output = m_Surface;

  for (int i = 0; i < kframes.size(); i++)
    items[i]->Delete();
  items.clear();
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
