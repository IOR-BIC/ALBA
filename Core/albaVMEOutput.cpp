/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutput
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



#include "albaVMEOutput.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaAbsMatrixPipe.h"
#include "albaDataPipe.h"
#include "albaSmartPointer.h"
#include "albaTransform.h"
#include "albaIndent.h"
#include "albaGUI.h"
#include <assert.h>

//-------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaVMEOutput)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutput::albaVMEOutput()
//-------------------------------------------------------------------------
{
  m_VME = NULL;
  m_Transform = albaTransform::New(); // Transform is created by VME 
  m_Gui = NULL;
}

//-------------------------------------------------------------------------
albaVMEOutput::~albaVMEOutput()
//-------------------------------------------------------------------------
{
  cppDEL(m_Gui);
}

#ifdef ALBA_USE_VTK
//-------------------------------------------------------------------------
vtkDataSet *albaVMEOutput::GetVTKData()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  return m_VME&&m_VME->GetDataPipe()?m_VME->GetDataPipe()->GetVTKData():NULL;
}
#endif

//-------------------------------------------------------------------------
void albaVMEOutput::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);

  m_VME->Update();
}

//-------------------------------------------------------------------------
void albaVMEOutput::SetTransform(albaTransformBase *trans)
//-------------------------------------------------------------------------
{
  assert(trans);
  m_Transform=trans;
}
//-------------------------------------------------------------------------
void albaVMEOutput::SetBounds(const albaOBB &bounds)
//-------------------------------------------------------------------------
{
  m_Bounds=bounds;
}
//-------------------------------------------------------------------------
void albaVMEOutput::GetLocalTimeBounds(albaTimeStamp tbounds[2]) const
//-------------------------------------------------------------------------
{
  
  m_VME->GetLocalTimeBounds(tbounds);
  
}
//-------------------------------------------------------------------------
void albaVMEOutput::GetTimeBounds(albaTimeStamp tbounds[2]) const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  
  //GetLocalTimeBounds(tbounds);
  m_VME->GetLocalTimeBounds(tbounds);
  
  for (int i=0;i<m_VME->GetNumberOfChildren();i++)
  {
    albaTimeStamp tmp[2];
    
    if (albaVME *child_vme=m_VME->GetChild(i))
    {
      child_vme->GetOutput()->GetTimeBounds(tmp);
      if (tmp[0]<0 || tmp[1]<0)
        continue;

      if (tmp[0]<tbounds[0]||tbounds[0]<0)
        tbounds[0]=tmp[0];

      if (tmp[1]>tbounds[1]||tbounds[1]<0)
        tbounds[1]=tmp[1];
    }    
  }
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetVME4DBounds(double bounds[6]) const
//-------------------------------------------------------------------------
{
  albaOBB myBounds;
  GetVME4DBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetVME4DBounds(albaOBB &bounds) const
//-------------------------------------------------------------------------
{
  assert(m_VME);

  std::vector<albaTimeStamp> timestamps;

  m_VME->GetTimeStamps(timestamps);

  bounds.Reset();

  if (albaDataPipe *dpipe=m_VME->GetDataPipe()) // if a datapipe is present
  {
    albaMatrix itemAbsPose;
    albaOBB transformed_bounds;

    for (int i=0;i<timestamps.size();i++)
    {
      dpipe->SetTimeStamp(timestamps[i]);
      dpipe->UpdateBounds();

      // must make a copy, otherwise I would transform the bounds inside the data pipe
      transformed_bounds.DeepCopy(dpipe->GetBounds());
      GetAbsMatrix(itemAbsPose,timestamps[i]);
    
      transformed_bounds.ApplyTransform(itemAbsPose);

      bounds.MergeBounds(transformed_bounds);
    }

    dpipe->SetTimeStamp(m_VME->GetTimeStamp());
    dpipe->UpdateBounds();
  }
  else
  {
    bounds=m_Bounds;
  }

}
//-------------------------------------------------------------------------
void albaVMEOutput::GetVMEBounds(double bounds[6]) const
//-------------------------------------------------------------------------
{
  albaOBB myBounds;
  GetVMEBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetVMEBounds(albaOBB &bounds,albaTimeStamp t, albaVMEIterator *iter) const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  if ((iter&&iter->IsVisible(m_VME))||(!iter&&m_VME->IsVisible()))
  {
    albaMatrix itemPose;
    
    GetAbsMatrix(itemPose,t);
    
    GetVMELocalBounds(bounds,t,iter);  

    bounds.ApplyTransform(itemPose);
  }
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetVMELocalBounds(double bounds[6]) const
//-------------------------------------------------------------------------
{
  albaOBB myBounds;
  GetVMELocalBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetVMELocalBounds(albaOBB &bounds,albaTimeStamp t, albaVMEIterator *iter) const
//-------------------------------------------------------------------------
{
  if (t<0)
    t=m_VME->GetTimeStamp();

  bounds.Reset();

  if ((iter&&iter->IsVisible(m_VME))||(!iter&&m_VME->IsVisible()))
  {
    GetDataBounds(bounds,t);
  }
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetDataBounds(albaOBB &bounds,albaTimeStamp t) const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  if (m_VME->GetDataPipe()) // allocate data pipe if not done yet
  {
    // We call directly the albaDataPipe::UpdateBounds() since this should simply update the
    // DataPipe "bounds" structure but not the output data when not necessary.
    // We must call explicitly UpdateBounds() method, since data pipes
    // do not update automatically the bounds when time changes.
    albaDataPipe *dpipe=m_VME->GetDataPipe();
    albaTimeStamp old_time=dpipe->GetTimeStamp();
    
    //if (t!=old_time)
    //{
      dpipe->SetTimeStamp(t);
      dpipe->UpdateBounds();
    //}

    bounds.DeepCopy(dpipe->GetBounds());

    if (t!=old_time)
    {
      // Restore the right bounds for current time... 
      // TODO: modify the GetBounds to make it call UpdateCurentBounds explicitly!
      dpipe->SetTimeStamp(m_VME->GetTimeStamp());
      dpipe->UpdateBounds();
    }
  }
  else
  {
    bounds=m_Bounds;
  }
}
//-------------------------------------------------------------------------
void albaVMEOutput::GetBounds(double bounds[6]) const
//-------------------------------------------------------------------------
{
  albaOBB myBounds;
  GetBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetBounds(albaOBB &bounds,albaTimeStamp t, albaVMEIterator *iter) const
//-------------------------------------------------------------------------
{
  assert(m_VME);

  if (t<0)
    t=m_VME->GetTimeStamp();

  GetVMEBounds(bounds,t,iter);
  
  for (int i=0;i<m_VME->GetNumberOfChildren();i++)
  {
    albaVME *child=m_VME->GetChild(i);
    if (child)
    {
      albaOBB childBounds;
      child->GetOutput()->GetBounds(childBounds,t);
      bounds.MergeBounds(childBounds);
    }    
  }
}

//-------------------------------------------------------------------------
void albaVMEOutput::Get4DBounds(double bounds[6]) const
//-------------------------------------------------------------------------
{
  albaOBB myBounds;
  Get4DBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void albaVMEOutput::Get4DBounds(albaOBB &bounds) const
//-------------------------------------------------------------------------
{
  assert(m_VME);

  std::vector<albaTimeStamp> timestamps;

  bounds.Reset();

  m_VME->GetTimeStamps(timestamps);

  albaOBB frameBounds;
  for (int i=0;i<timestamps.size();i++)
  {
    GetBounds(frameBounds,timestamps[i]);
    bounds.MergeBounds(frameBounds); 
  }

}

//-------------------------------------------------------------------------
albaTransformBase * albaVMEOutput::GetTransform() const
//-------------------------------------------------------------------------
{
  // if VME supports a matrix pipe return its pointer
  if (m_VME)
  {
    if (albaMatrixPipe *mpipe=m_VME->GetMatrixPipe())
    {
      return mpipe;
    }
  }

  assert(m_Transform);

  // return internal transform
  return m_Transform;
}
//-------------------------------------------------------------------------
albaMatrix *albaVMEOutput::GetMatrix() const
//-------------------------------------------------------------------------
{
  return GetTransform()->GetMatrixPointer();
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetMatrix(albaMatrix &matrix,albaTimeStamp t) const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  if (albaMatrixPipe *mpipe=m_VME->GetMatrixPipe()) // check if a matrix pipe is present
  {
    if (t<0||t==m_VME->GetTimeStamp())
    {
      matrix.DeepCopy(GetMatrix());
    }
    else
    {
      // disable rising of update event since this is
      // only a temporary change to the matrix
      bool old_flag=mpipe->GetUpdateMatrixObserverFlag();
      mpipe->UpdateMatrixObserverOff();
      mpipe->SetTimeStamp(t);
      matrix=mpipe->GetMatrix();

      // restore right time
      mpipe->SetTimeStamp(m_VME->GetTimeStamp());
      mpipe->SetUpdateMatrixObserverFlag(old_flag);
    }
  }
  else
  {
    albaMatrix *mat=GetMatrix();
    matrix.DeepCopy(mat);
  }
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetPose(double xyz[3],double rxyz[3],albaTimeStamp t) const
//-------------------------------------------------------------------------
{
  albaMatrix mat;
  
  GetMatrix(mat,t);
  
  albaTransform::GetOrientation(mat,rxyz);
  
  albaTransform::GetPosition(mat,xyz);
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,albaTimeStamp t) const
//-------------------------------------------------------------------------
{
  double xyz[3],rxyz[3];

  GetPose(xyz,rxyz,t);

  x=xyz[0];
  y=xyz[1];
  z=xyz[2];

  rx=rxyz[0];
  ry=rxyz[1];
  rz=rxyz[2];
}


//-------------------------------------------------------------------------
albaTransformBase * albaVMEOutput::GetAbsTransform() const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  return m_VME->GetAbsMatrixPipe();
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetAbsMatrix(albaMatrix &matrix,albaTimeStamp t) const
//-------------------------------------------------------------------------
{
  assert(m_VME);


  if (t<0||t==m_VME->GetTimeStamp())
  {
    matrix.DeepCopy(GetAbsMatrix());
  }
  else
  {
    albaAbsMatrixPipe *abspipe=m_VME->GetAbsMatrixPipe();
    // disable rising of update event since this is
    // only a temporary change to the matrix
    bool old_flag=abspipe->GetUpdateMatrixObserverFlag();
    abspipe->UpdateMatrixObserverOff();
    abspipe->SetTimeStamp(t);
    matrix=abspipe->GetMatrix();
    
    // restore right time
    abspipe->SetTimeStamp(m_VME->GetTimeStamp());
    abspipe->SetUpdateMatrixObserverFlag(old_flag);
  }
}

//-------------------------------------------------------------------------
albaMatrix *albaVMEOutput::GetAbsMatrix() const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->GetAbsMatrixPipe()->Update();
  return m_VME->GetAbsMatrixPipe()->GetMatrixPointer();
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetAbsPose(double xyz[3],double rxyz[3],albaTimeStamp t) const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  albaMatrix mat;
  GetAbsMatrix(mat,t);

	if(xyz != NULL)
		albaTransform::GetPosition(mat,xyz);

	if (rxyz != NULL)
		albaTransform::GetOrientation(mat, rxyz);
}

//-------------------------------------------------------------------------
void albaVMEOutput::GetAbsPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,albaTimeStamp t) const
//-------------------------------------------------------------------------
{
  double xyz[3],rxyz[3];

  GetAbsPose(xyz,rxyz,t);

  x=xyz[0];
  y=xyz[1];
  z=xyz[2];

  rx=rxyz[0];
  ry=rxyz[1];
  rz=rxyz[2];
}


//-------------------------------------------------------------------------
albaTimeStamp albaVMEOutput::GetTimeStamp() const
//-------------------------------------------------------------------------
{
  return m_VME->GetTimeStamp();
}

//-------------------------------------------------------------------------
void albaVMEOutput::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  albaIndent indent(tabs);

  os << indent << "Current Time: "<<m_VME->GetTimeStamp()<<"\n";

  os << indent << "Current Matrix:\n";
  GetMatrix()->Print(os,indent.GetNextIndent());

  os << indent << "Current Absolute Matrix:\n";
  albaMatrix m = m_VME->GetAbsMatrixPipe()->GetMatrix();
  m.Print(os,indent.GetNextIndent());

  albaTimeStamp tbounds[2];
  GetLocalTimeBounds(tbounds);
  os << indent << "Time Bounds: ("<<tbounds[0]<<","<<tbounds[1]<<"]"<<std::endl;

  albaOBB bounds;
  GetVMEBounds(bounds);
  os << indent << "VME 3D Bounds:"<<std::endl;
  bounds.Print(os,indent.GetNextIndent());

  os << indent << "VME 4D Bounds:"<<std::endl;
  Get4DBounds(bounds);
  bounds.Print(os,indent.GetNextIndent());

  os << indent << "DataType: "<<m_DataType<<std::endl;
}

//-------------------------------------------------------------------------
albaGUI *albaVMEOutput::GetGui()
//-------------------------------------------------------------------------
{
  if (m_Gui==NULL) CreateGui();
  assert(m_Gui);
  return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEOutput::DeleteGui()
//-------------------------------------------------------------------------
{
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
albaGUI* albaVMEOutput::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new albaGUI(m_VME);

  albaString type = GetTypeName(); 
  m_Gui->Label("type: ", type, true);
	m_Gui->Divider();
  return m_Gui;
}
