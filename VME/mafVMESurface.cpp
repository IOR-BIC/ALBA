/*=========================================================================

 Program: MAF2
 Module: mafVMESurface
 Authors: Marco Petrone
 
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



#include "mafVMESurface.h"
#include "mafVME.h"
#include "mafMatrixInterpolator.h"
#include "mafDataVector.h"
#include "mafDataPipeInterpolatorVTK.h"
#include "mafVMEItemVTK.h"
#include "mafAbsMatrixPipe.h"
#include "mmaMaterial.h"
#include "mafGUI.h"
#include "mafGUIMaterialButton.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMESurface)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMESurface::mafVMESurface()
//-------------------------------------------------------------------------
{
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
    ((mafVMEOutputSurface *)GetOutput())->SetMaterial(GetMaterial());
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMESurface::SetData(vtkPolyData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);

  if(polydata)
  {
    if (polydata->GetPoints() && (polydata->GetVerts()==NULL || polydata->GetVerts()->GetNumberOfCells()==0) && \
      ((polydata->GetPolys()!=NULL && polydata->GetPolys()->GetNumberOfCells() > 0) || (polydata->GetStrips()!=NULL && polydata->GetStrips()->GetNumberOfCells() > 0)) && \
      (polydata->GetLines()==NULL || polydata->GetLines()->GetNumberOfCells() == 0))
    {
      int res = Superclass::SetData(polydata,t,mode);
      GetOutput()->Update();
      return res;
    }
  }
  return MAF_ERROR;
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
  
  return MAF_ERROR;
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
