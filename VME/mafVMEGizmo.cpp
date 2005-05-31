/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGizmo.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-31 23:51:47 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone, Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEGizmo_cxx
#define __mafVMEGizmo_cxx

#include "mafVMEGizmo.h"
#include "mafTransform.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"
#include "mafTagArray.h"

#include "vtkPolyData.h"
#include "vtkMAFDataPipe.h"

//-------------------------------------------------------------------------
mafVMEGizmo::mafVMEGizmo()
//-------------------------------------------------------------------------
{
  m_GizmoData = NULL;
  mafNEW(m_Transform);
  mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe=mafDataPipeCustom::New();
  SetDataPipe(dpipe);
  
  // modified by Stefano 14-3-2005: gizmos set to not visible in the tree and node chooser
  GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
}

//-------------------------------------------------------------------------
mafVMEGizmo::~mafVMEGizmo()
//-------------------------------------------------------------------------
{
  vtkDEL(m_GizmoData);
  mafDEL(m_Transform);
  SetOutput(NULL);
}
//-------------------------------------------------------------------------
int mafVMEGizmo::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEGizmo *vme_gizmo=mafVMEGizmo::SafeDownCast(a);
    m_Transform->SetMatrix(vme_gizmo->m_Transform->GetMatrix());
    SetData(vme_gizmo->GetData());
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMEGizmo::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    // test only matrices
    return (m_Transform->GetMatrix() == ((mafVMEGizmo *)vme)->m_Transform->GetMatrix());
  }
  return false;
}
//-------------------------------------------------------------------------
void mafVMEGizmo::SetData(vtkPolyData *data)
//-------------------------------------------------------------------------
{
  assert(data);  // just check if data is set to NULL...
  if (data!=m_GizmoData)
  {
    vtkDEL(m_GizmoData);
    m_GizmoData = data;
    m_GizmoData->Register(NULL);
    
    // set data as input to VTK 
    mafDataPipeCustom *dpipe=mafDataPipeCustom::SafeDownCast(GetDataPipe());
    dpipe->GetVTKDataPipe()->SetNthInput(0,data);
    
    Modified();
  }
}

//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMEGizmo::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputSurface *)GetOutput();
}

//-------------------------------------------------------------------------
void mafVMEGizmo::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMEGizmo::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
void mafVMEGizmo::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}
 
#endif
