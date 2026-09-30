/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSubdivide
 Authors: Matteo Giacomoni
 
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

#include "albaOpSubdivide.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaVMESurface.h"

#include "vtkLinearSubdivisionFilter.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkCell.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpSubdivide);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpSubdivide::albaOpSubdivide(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = false;

  m_OriginalPolydata = NULL;
  m_ResultPolydata = NULL;
}
//----------------------------------------------------------------------------
albaOpSubdivide::~albaOpSubdivide()
//----------------------------------------------------------------------------
{
  vtkDEL(m_OriginalPolydata);
  vtkDEL(m_ResultPolydata);
}
//----------------------------------------------------------------------------
bool albaOpSubdivide::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return ( node != NULL && node->IsA("albaVMESurface") );
}
//----------------------------------------------------------------------------
albaOp *albaOpSubdivide::Copy()   
//----------------------------------------------------------------------------
{
  return (new albaOpSubdivide(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void albaOpSubdivide::OpRun()   
//----------------------------------------------------------------------------
{
  albaVMESurface *surface = albaVMESurface::SafeDownCast(m_Input);
  surface->Update();

  vtkPolyData *polydataInput = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkNEW(m_OriginalPolydata);
  m_OriginalPolydata->DeepCopy(polydataInput);

  vtkNEW(m_ResultPolydata);
  m_ResultPolydata->DeepCopy(polydataInput);

  for(int i=0;i<polydataInput->GetNumberOfCells();i++)
  {
    if(polydataInput->GetCell(i)->GetNumberOfPoints()!=3)
    {
      if(m_TestMode == false)
        wxMessageBox(_("The mesh will be triangolarized to be subdivide"));

      vtkTriangleFilter *triangleFilter;
      vtkNEW(triangleFilter);

      triangleFilter->SetInputData(polydataInput);
      triangleFilter->Update();

      surface->SetData(triangleFilter->GetOutput(),surface->GetTimeStamp());
      surface->Update();

      m_ResultPolydata->DeepCopy(triangleFilter->GetOutput());

      vtkDEL(triangleFilter);

      break;
    }
  }

  if(m_TestMode == false)
    CreateGui();
}
//----------------------------------------------------------------------------
void albaOpSubdivide::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new albaGUI(this);

  // ToDO: add your custom widgets...

  m_NumberOfSubdivision = 1;
  m_Gui->Label(_("num. subdivision"));
  m_Gui->Integer(ID_NUMBER_OF_SUBDIVISION,"",&m_NumberOfSubdivision,0);

  m_Gui->Button(ID_SUBDIVIDE,_("subdivide"));
  m_Gui->Button(ID_PREVIEW,_("preview"));

  m_Gui->OkCancel();
  m_Gui->Divider();

  ShowGui();
}
//----------------------------------------------------------------------------
void albaOpSubdivide::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {	
      case ID_PREVIEW:
        {
				GetLogicManager()->CameraUpdate();
        }
      break;
      case ID_NUMBER_OF_SUBDIVISION:
        {
          if (m_NumberOfSubdivision >5)
          {
            if(m_TestMode == false)
              wxMessageBox("Warning! the number of subdivision is hight, it could require too many memory!");
          }
        }
        break;
      case ID_SUBDIVIDE:
        {
          Algorithm();
        }
        break;
      case wxOK:
        OpStop(OP_RUN_OK);        
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);        
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaOpSubdivide::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    albaVMESurface *surface = albaVMESurface::SafeDownCast(m_Input);
    surface->Update();
    
    surface->SetData(m_OriginalPolydata,surface->GetTimeStamp());
    surface->Update();
  }

  HideGui();
  albaEventMacro(albaEvent(this,result));  
}
//----------------------------------------------------------------------------
void albaOpSubdivide::OpUndo()
//----------------------------------------------------------------------------
{
  albaVMESurface *surface = albaVMESurface::SafeDownCast(m_Input);
  surface->Update();

  surface->SetData(m_OriginalPolydata,surface->GetTimeStamp());
  surface->Update();
}
//----------------------------------------------------------------------------
void albaOpSubdivide::OpDo()
//----------------------------------------------------------------------------
{
  albaVMESurface *surface = albaVMESurface::SafeDownCast(m_Input);
  surface->Update();

  surface->SetData(m_ResultPolydata,surface->GetTimeStamp());
  surface->Update();
}
//----------------------------------------------------------------------------
void albaOpSubdivide::Algorithm()
//----------------------------------------------------------------------------
{
  vtkLinearSubdivisionFilter *subdivisionFilter;
  vtkNEW(subdivisionFilter);

  albaVMESurface *surface = albaVMESurface::SafeDownCast(m_Input);
  surface->Update();

  vtkPolyData *polydataInput = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  subdivisionFilter->SetInputData(polydataInput);
  subdivisionFilter->SetNumberOfSubdivisions(m_NumberOfSubdivision);
  subdivisionFilter->Update();

  m_ResultPolydata->DeepCopy(subdivisionFilter->GetOutput());

  surface->SetData(m_ResultPolydata,surface->GetTimeStamp());
  surface->Update();

  vtkDEL(subdivisionFilter);
}
