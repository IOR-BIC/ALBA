/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeScalar.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-21 15:24:04 $
  Version:   $Revision: 1.1 $
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

#include "mafPipeScalar.h"
#include "mafPipeScalar.h"
#include "mafDecl.h"
#include "mafSceneNode.h"
#include "mmgGui.h"

#include "mafVME.h"
#include "mafVMEOutputScalar.h"
#include "mafVMEScalar.h"
#include "mafTagItem.h"
#include "mafTagArray.h"

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
#include "vtkLineSource.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty2D.h"
#include "vtkLookupTable.h"
#include "vtkWarpScalar.h"
#include "vtkPointSet.h"
//#include "vtkPolyDataWriter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeScalar);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeScalar::mafPipeScalar()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_LineSource    = NULL;
  m_WrapScalar    = NULL;
  m_Mapper        = NULL;
  m_Actor         = NULL;
  m_ActiveScalar  = 0;
  m_Order         = 1;
}
//----------------------------------------------------------------------------
void mafPipeScalar::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected = false;

  mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(m_Vme);
  assert(scalar);
  vnl_matrix<double> data = scalar->GetScalarOutput()->GetScalarData();
  assert(!data.empty());

  mafTagItem *item = m_Vme->GetTagArray()->GetTag("SCALAR_ORDER");
  if (item)
  {
    m_Order = (int)(item->GetValueAsDouble());
  }
  vnl_vector<double> vec;
  if (m_Order == 0)
  {
    vec = data.get_row(m_ActiveScalar);
  }
  else
  {
    vec = data.get_column(m_ActiveScalar);
  }
  unsigned num_on_scalars = vec.size();
  double *active_scalar = new double[num_on_scalars];
  vec.copy_out(active_scalar);

  vtkDoubleArray *scalar_array = vtkDoubleArray::New();
  scalar_array->SetArray(active_scalar, num_on_scalars, 1);

  double sr[2];
  vtkNEW(m_LineSource);
  m_LineSource->SetPoint1(0,0,0);
  m_LineSource->SetPoint2(num_on_scalars,0,0);
  m_LineSource->SetResolution(num_on_scalars);
  m_LineSource->Update();
  m_LineSource->GetOutput()->GetPointData()->SetScalars(scalar_array);
  m_LineSource->GetOutput()->GetScalarRange(sr);

  vtkLookupTable *lt = vtkLookupTable::New();
  lt->SetTableRange(sr);
  lt->Build();

  vtkNEW(m_WrapScalar);
  m_WrapScalar->SetInput((vtkPointSet *) m_LineSource->GetOutput());
  m_WrapScalar->SetNormal(0 , 0 , 1);
  m_WrapScalar->SetScaleFactor(1);
  m_WrapScalar->SetUseNormal(0);
  m_WrapScalar->SetXYPlane(0);
  m_WrapScalar->Update();

  vtkNEW(m_Mapper);
  m_Mapper->SetInput((vtkPolyData *)m_WrapScalar->GetOutput());
  //m_Mapper->SetInput(m_LineSource->GetOutput());
  m_Mapper->SetScalarRange(sr);
  m_Mapper->ScalarVisibilityOn();
  m_Mapper->SetLookupTable(lt);
  m_Mapper->SetScalarModeToUsePointData();

  vtkNEW(m_Actor);
  m_Actor->SetMapper(m_Mapper);

  m_AssemblyFront->AddPart(m_Actor);

  cppDEL(active_scalar);
  vtkDEL(scalar_array);
}
//----------------------------------------------------------------------------
mafPipeScalar::~mafPipeScalar()
//----------------------------------------------------------------------------
{
  m_AssemblyFront->RemovePart(m_Actor);

  vtkDEL(m_LineSource);
  vtkDEL(m_WrapScalar);
  vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
}
//----------------------------------------------------------------------------
void mafPipeScalar::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
}
//----------------------------------------------------------------------------
mmgGui *mafPipeScalar::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  m_Gui->Integer(ID_ACTIVE_SCALAR,"active scalar",&m_ActiveScalar,0,10);
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeScalar::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_ACTIVE_SCALAR:
        UpdateProperty();
      break;
    }
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
void mafPipeScalar::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  mafVMEOutputScalar *out_scalar = mafVMEOutputScalar::SafeDownCast(m_Vme->GetOutput());
  vnl_matrix<double> data = out_scalar->GetScalarData();

  mafTagItem *item = m_Vme->GetTagArray()->GetTag("SCALAR_ORDER");
  if (item)
  {
    m_Order = (int)(item->GetValueAsDouble());
  }
  vnl_vector<double> vec;
  if (m_Order == 0)
  {
    vec = data.get_row(m_ActiveScalar);
  }
  else
  {
    vec = data.get_column(m_ActiveScalar);
  }
  unsigned num_on_scalars = vec.size();
  double *active_scalar = new double[num_on_scalars];
  vec.copy_out(active_scalar);

  double sr[2];
  vtkDoubleArray *scalar_array = vtkDoubleArray::New();
  scalar_array->SetArray(active_scalar, num_on_scalars, 1);
  scalar_array->GetRange(sr);

  m_LineSource->SetPoint2(num_on_scalars,0,0);
  m_LineSource->SetResolution(num_on_scalars);
  m_LineSource->Update();
  m_LineSource->GetOutput()->GetPointData()->SetScalars(scalar_array);

  vtkLookupTable *lt = vtkLookupTable::SafeDownCast(m_Mapper->GetLookupTable());
  if (lt)
  {
    lt->SetTableRange(sr);
    lt->Build();
  }
  
  m_WrapScalar->Update();
  m_Mapper->SetScalarRange(sr);

  cppDEL(active_scalar);
  vtkDEL(scalar_array);
}
