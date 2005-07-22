/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMESurface.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-22 13:51:20 $
  Version:   $Revision: 1.13 $
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



#include "mafVMESurface.h"
#include "mafVME.h"
#include "mafMatrixInterpolator.h"
#include "mafDataVector.h"
#include "mafVTKInterpolator.h"
#include "mafVMEItemVTK.h"
#include "mafAbsMatrixPipe.h"
#include "mmaMaterial.h"
#include "mmgGui.h"
#include "mmgMaterialButton.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"

#include "mafPipeSurface.h" //SIL. 21-4-2005: 
//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMESurface)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMESurface::mafVMESurface()
//-------------------------------------------------------------------------
{
  m_MaterialButton = NULL;
}

//-------------------------------------------------------------------------
mafVMESurface::~mafVMESurface()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGeneric

  //cppDEL(m_MaterialButton);
}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMESurface::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputSurface::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMESurface::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();

    return MAF_OK;
  }

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMESurface::SetData(vtkPolyData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  return Superclass::SetData(data,t,mode);
}
//-------------------------------------------------------------------------
int mafVMESurface::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);

  if (polydata && polydata->GetPoints() && polydata->GetVerts()->GetNumberOfCells()==0 && \
     (polydata->GetPolys()->GetNumberOfCells() > 0 || polydata->GetStrips()->GetNumberOfCells() > 0) && \
      polydata->GetLines()->GetNumberOfCells() == 0)
  {
    return Superclass::SetData(data,t,mode);
  }
  
  mafErrorMacro("Trying to set the wrong type of fata inside a VME Image :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
mmgGui* mafVMESurface::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_MaterialButton = new mmgMaterialButton(this,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  return m_Gui;
}

//-------------------------------------------------------------------------
void mafVMESurface::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      default:
        Superclass::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMESurface::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((mafVMEOutputSurface *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}
//-------------------------------------------------------------------------
char** mafVMESurface::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}
