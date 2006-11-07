/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoScalarToSurface.cpp,v $
  Language:  C++
  Date:      $Date: 2006-11-07 12:13:18 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mmoScalarToSurface.h"
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
mafCxxTypeMacro(mmoScalarToSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoScalarToSurface::mmoScalarToSurface(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

  m_Surface = NULL;
}

//----------------------------------------------------------------------------
mmoScalarToSurface::~mmoScalarToSurface()
//----------------------------------------------------------------------------
{
  mafDEL(m_Surface);
}

//----------------------------------------------------------------------------
mafOp* mmoScalarToSurface::Copy()   
//----------------------------------------------------------------------------
{
	return new mmoScalarToSurface(m_Label);
}

//----------------------------------------------------------------------------
bool mmoScalarToSurface::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMEScalar) && ((mafVMEScalar*)node)->IsAnimated());
}

//----------------------------------------------------------------------------
void mmoScalarToSurface::OpRun()   
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
    scalar_surface->AddInput(items[t]);
  }
  scalar_surface->Update();
  scalar->SetTimeStamp(ts);
  if (!m_TestMode)
  {
    mafEventMacro(mafEvent(this, BIND_TO_PROGRESSBAR,delaunay));
  }
  delaunay->SetInput(scalar_surface->GetOutput());
  delaunay->Update();
  m_Surface->SetData(delaunay->GetOutput(),ts);
  m_Output = m_Surface;

  for (int i = 0; i < kframes.size(); i++)
    items[i]->Delete();
  items.clear();
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
