/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEVolumeGray.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-23 08:27:03 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone
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

#include "mafVMEVolumeGray.h"

#include "mmgGui.h"
#include "mafGUIDialogTransferFunction2D.h"

#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEVolumeGray)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEVolumeGray::mafVMEVolumeGray()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEVolumeGray::~mafVMEVolumeGray()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGeneric
}


//-------------------------------------------------------------------------
mafVMEOutput *mafVMEVolumeGray::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputVolume::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMEVolumeGray::SetData(vtkRectilinearGrid *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  data->Update();
  if (data->GetPointData()&&data->GetPointData()->GetNumberOfComponents()==1)
    return Superclass::SetData(data,t,mode);

  mafErrorMacro("Trying to set the wrong type of data inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEVolumeGray::SetData(vtkImageData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  data->Update();
  if (data->GetPointData()&&data->GetPointData()->GetNumberOfComponents()==1)
    return Superclass::SetData(data,t,mode);

  mafErrorMacro("Trying to set the wrong type of data inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
int mafVMEVolumeGray::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  if (data->IsA("vtkImageData")||data->IsA("vtkRectilinearGrid"))
  {
    data->Update();
    if (data->GetPointData()&&data->GetPointData()->GetNumberOfComponents()==1)
      return Superclass::SetData(data,t,mode);
  }
  
  mafErrorMacro("Trying to set the wrong type of data inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
mmgGui* mafVMEVolumeGray::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  //m_Gui->Button(ID_VOLUME_TRANSFER_FUNCTION, "transfer function", "", "Visualize Transfer function dialog.");

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEVolumeGray::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  mafEvent *e = mafEvent::SafeDownCast(maf_event);
  if (e != NULL && e->GetSender() == m_Gui)
  {
    switch(e->GetId())
    {
      case ID_VOLUME_TRANSFER_FUNCTION:
      {
        mafGUIDialogTransferFunction2D *dlg = new mafGUIDialogTransferFunction2D();
        dlg->ShowModal(this);
        cppDEL(dlg);
      }
      break;
      default:
        Superclass::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
