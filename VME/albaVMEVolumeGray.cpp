/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVolumeGray
 Authors: Marco Petrone
 
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

#include "albaVMEVolumeGray.h"

#include "albaGUI.h"
#include "albaGUIDialogTransferFunction2D.h"

#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"
#include "vtkDataSetAttributes.h"
#include "vtkDataArray.h"
#include "vtkDataSet.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEVolumeGray)
//-------------------------------------------------------------------------
albaVMEVolumeGray::albaVMEVolumeGray()
{
}
//-------------------------------------------------------------------------
albaVMEVolumeGray::~albaVMEVolumeGray()
{
  // data pipe destroyed in albaVME
  // data vector destroyed in albaVMEGeneric
}
//-------------------------------------------------------------------------
albaVMEOutput *albaVMEVolumeGray::GetOutput()
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputVolume::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
int albaVMEVolumeGray::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
{
  assert(data);
  if (data->IsA("vtkImageData")||data->IsA("vtkRectilinearGrid"))
  {
    if (data->GetPointData()&&data->GetPointData()->GetScalars() != NULL &&
      data->GetPointData()->GetScalars()->GetNumberOfComponents()==1)
      return Superclass::SetData(data,t,mode);
  }
  
  albaErrorMacro("Trying to set the wrong type of data inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
albaGUI* albaVMEVolumeGray::CreateGui()
{
  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  //m_Gui->Button(ID_VOLUME_TRANSFER_FUNCTION, "transfer function", "", "Visualize Transfer function dialog.");

  return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEVolumeGray::OnEvent(albaEventBase *alba_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  albaEvent *e = albaEvent::SafeDownCast(alba_event);
  if (e != NULL && e->GetSender() == m_Gui)
  {
    switch(e->GetId())
    {
      case ID_VOLUME_TRANSFER_FUNCTION:
      {
        albaGUIDialogTransferFunction2D *dlg = new albaGUIDialogTransferFunction2D();
        dlg->ShowModal(this);
        cppDEL(dlg);
      }
      break;
      default:
        Superclass::OnEvent(alba_event);
    }
  }
  else
  {
    Superclass::OnEvent(alba_event);
  }
}
