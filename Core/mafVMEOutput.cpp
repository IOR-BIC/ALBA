/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutput.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:03:35 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafVMEOutput.h"
#include "mafVME.h"
#include "mafNodeIterator.h"
#include "mafAbsMatrixPipe.h"
#include "mafDataPipe.h"
#include "mafSmartPointer.h"
#include "mafTransform.h"
#include "mafIndent.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafVMEOutput)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutput::mafVMEOutput()
//-------------------------------------------------------------------------
{
  m_VME = NULL;
}

//-------------------------------------------------------------------------
mafVMEOutput::~mafVMEOutput()
//-------------------------------------------------------------------------
{
}

#ifdef MAF_USE_VTK
//-------------------------------------------------------------------------
vtkDataSet *mafVMEOutput::GetVTKData()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  return m_VME&&m_VME->GetDataPipe()?m_VME->GetDataPipe()->GetVTKData():NULL;
}
#endif

//-------------------------------------------------------------------------
void mafVMEOutput::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);

  if (m_VME)
  {
    if (m_VME->GetDataPipe())
      m_VME->GetDataPipe()->Update();

    if (m_VME->GetMatrixPipe())
      m_VME->GetMatrixPipe()->Update();
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetTransform(mafTransformBase *trans)
//-------------------------------------------------------------------------
{
  assert(trans);
  m_Transform=trans;
}
//-------------------------------------------------------------------------
void mafVMEOutput::SetBounds(const mafOBB &bounds)
//-------------------------------------------------------------------------
{
  m_Bounds=bounds;
}
//-------------------------------------------------------------------------
void mafVMEOutput::GetLocalTimeBounds(mafTimeStamp tbounds[2]) const
//-------------------------------------------------------------------------
{
  std::vector<mafTimeStamp> tstamps;
  m_VME->GetLocalTimeStamps(tstamps);
  if (tstamps.size()>0)
  {
    tbounds[0]=tstamps[0];
    tbounds[1]=tstamps[tstamps.size()-1];
  }
  else
  {
    // set to invalid time bounds (i.e. infinite)
    tbounds[0]=0;
    tbounds[1]=-1;
  }
  
}
//-------------------------------------------------------------------------
void mafVMEOutput::GetTimeBounds(mafTimeStamp tbounds[2]) const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  
  GetLocalTimeBounds(tbounds);
  
  for (int i=0;i<m_VME->GetNumberOfChildren();i++)
  {
    mafTimeStamp tmp[2];
    
    if (mafVME *child_vme=mafVME::SafeDownCast(m_VME->GetChild(i)))
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
void mafVMEOutput::GetVME4DBounds(double bounds[6]) const
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  GetVME4DBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVME4DBounds(mafOBB &bounds) const
//-------------------------------------------------------------------------
{
  assert(m_VME);

  std::vector<mafTimeStamp> timestamps;

  m_VME->GetTimeStamps(timestamps);

  bounds.Reset();

  if (mafDataPipe *dpipe=m_VME->GetDataPipe()) // if a datapipe is present
  {
    mafMatrix itemAbsPose;
    mafOBB transformed_bounds;

    for (int i=0;i<timestamps.size();i++)
    {
      dpipe->SetCurrentTime(timestamps[i]);
      dpipe->UpdateBounds();

      // must make a copy, otherwise I would transform the bounds inside the data pipe
      transformed_bounds.DeepCopy(dpipe->GetBounds());
      GetAbsMatrix(itemAbsPose,timestamps[i]);
    
      transformed_bounds.ApplyTransform(itemAbsPose);

      bounds.MergeBounds(transformed_bounds);
    }

    dpipe->SetCurrentTime(m_VME->GetCurrentTime());
    dpipe->UpdateBounds();
  }
  else
  {
    bounds=m_Bounds;
  }

}
//-------------------------------------------------------------------------
void mafVMEOutput::GetVMEBounds(double bounds[6]) const
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  GetVMEBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVMEBounds(mafOBB &bounds,mafTimeStamp t, mafNodeIterator *iter) const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  if ((iter&&iter->IsVisible(m_VME))||(!iter&&m_VME->IsVisible()))
  {
    mafMatrix itemPose;
    
    if (t<0)
    {
      t=m_VME->GetCurrentTime();
      GetAbsMatrix(itemPose);
    }
    else
    {
      GetAbsMatrix(itemPose,t);
    }
    
    GetVMELocalBounds(bounds,t,iter);  

    bounds.ApplyTransform(itemPose);
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVMELocalBounds(double bounds[6]) const
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  GetVMELocalBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVMELocalBounds(mafOBB &bounds,mafTimeStamp t, mafNodeIterator *iter) const
//-------------------------------------------------------------------------
{
  if (t<0)
    t=m_VME->GetCurrentTime();

  bounds.Reset();

  if ((iter&&iter->IsVisible(m_VME))||(!iter&&m_VME->IsVisible()))
  {
    GetDataBounds(bounds,t);
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetDataBounds(mafOBB &bounds,mafTimeStamp t) const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  if (m_VME->GetDataPipe()) // allocate data pipe if not done yet
  {
    // We call directly the mafDataPipe::UpdateBounds() since this should simply update the
    // DataPipe "bounds" structure but not the output data when not necessary.
    // We must call explicitly UpdateBounds() method, since data pipes
    // do not update automatically the bounds when time changes.
    mafDataPipe *dpipe=m_VME->GetDataPipe();
    dpipe->SetCurrentTime(t);
    dpipe->UpdateBounds();
    bounds.DeepCopy(dpipe->GetBounds());

    // Restore the right bounds for current time... 
    // TODO: modify the GetBounds to make it call UpdateCurentBounds explicitly!
    dpipe->SetCurrentTime(m_VME->GetCurrentTime());
    dpipe->UpdateBounds();    
  }
  else
  {
    bounds=m_Bounds;
  }
}
//-------------------------------------------------------------------------
void mafVMEOutput::GetBounds(double bounds[6]) const
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  GetBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetBounds(mafOBB &bounds,mafTimeStamp t, mafNodeIterator *iter) const
//-------------------------------------------------------------------------
{
  assert(m_VME);

  if (t<0)
    t=m_VME->GetCurrentTime();

  GetVMEBounds(bounds,t,iter);
  
  for (int i=0;i<m_VME->GetNumberOfChildren();i++)
  {
    mafVME *child=mafVME::SafeDownCast(m_VME->GetChild(i));
    if (child)
    {
      mafOBB childBounds;
      child->GetOutput()->GetBounds(childBounds,t);
      bounds.MergeBounds(childBounds);
    }    
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::Get4DBounds(double bounds[6]) const
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  Get4DBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::Get4DBounds(mafOBB &bounds) const
//-------------------------------------------------------------------------
{
  assert(m_VME);

  std::vector<mafTimeStamp> timestamps;

  bounds.Reset();

  m_VME->GetTimeStamps(timestamps);

  mafOBB frameBounds;
  for (int i=0;i<timestamps.size();i++)
  {
    GetBounds(frameBounds,timestamps[i]);
    bounds.MergeBounds(frameBounds); 
  }

}

//-------------------------------------------------------------------------
mafTransformBase * mafVMEOutput::GetTransform() const
//-------------------------------------------------------------------------
{
  return m_Transform;
}
//-------------------------------------------------------------------------
mafMatrix *mafVMEOutput::GetMatrix() const
//-------------------------------------------------------------------------
{
  assert(m_Transform);
  return m_Transform->GetMatrixPointer();
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetMatrix(mafMatrix &matrix,mafTimeStamp t) const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  if (mafMatrixPipe *mpipe=m_VME->GetMatrixPipe()) // check if a matrix pipe is present
  {
    if (t<0||t==m_VME->GetCurrentTime())
    {
      matrix.DeepCopy(GetMatrix());
    }
    else
    {
      // disable rising of update event since this is
      // only a temporary change to the matrix
      bool old_flag=mpipe->GetUpdateMatrixObserverFlag();
      mpipe->UpdateMatrixObserverOff();
      mpipe->SetCurrentTime(t);
      matrix=mpipe->GetMatrix();

      // restore right time
      mpipe->SetCurrentTime(m_VME->GetCurrentTime());
      mpipe->SetUpdateMatrixObserverFlag(old_flag);
    }
  }
  else
  {
    mafMatrix *mat=GetMatrix();
    matrix.DeepCopy(mat);
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetPose(double xyz[3],double rxyz[3],mafTimeStamp t) const
//-------------------------------------------------------------------------
{
  mafMatrix mat;
  
  GetMatrix(mat,t);
  
  mafTransform::GetOrientation(mat,rxyz);
  
  mafTransform::GetPosition(mat,xyz);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mafTimeStamp t) const
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
mafTransformBase * mafVMEOutput::GetAbsTransform() const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  return m_VME->GetAbsMatrixPipe();
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsMatrix(mafMatrix &matrix,mafTimeStamp t) const
//-------------------------------------------------------------------------
{
  assert(m_VME);


  if (t<0||t==m_VME->GetCurrentTime())
  {
    matrix.DeepCopy(GetAbsMatrix());
  }
  else
  {
    mafAbsMatrixPipe *abspipe=m_VME->GetAbsMatrixPipe();
    // disable rising of update event since this is
    // only a temporary change to the matrix
    bool old_flag=abspipe->GetUpdateMatrixObserverFlag();
    abspipe->UpdateMatrixObserverOff();
    abspipe->SetCurrentTime(t);
    matrix=abspipe->GetMatrix();
    
    // restore right time
    abspipe->SetCurrentTime(m_VME->GetCurrentTime());
    abspipe->SetUpdateMatrixObserverFlag(old_flag);
  }
}

//-------------------------------------------------------------------------
mafMatrix *mafVMEOutput::GetAbsMatrix() const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  return m_VME->GetAbsMatrixPipe()->GetMatrixPointer();
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsPose(double xyz[3],double rxyz[3],mafTimeStamp t) const
//-------------------------------------------------------------------------
{
  assert(m_VME);
  mafMatrix mat;
  GetAbsMatrix(mat,t);

  mafTransform::GetPosition(mat,xyz);

  mafTransform::GetOrientation(mat,rxyz);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mafTimeStamp t) const
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
mafTimeStamp mafVMEOutput::GetCurrentTime() const
//-------------------------------------------------------------------------
{
  return m_VME->GetCurrentTime();
}

//-------------------------------------------------------------------------
void mafVMEOutput::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "Current Time: "<<m_VME->GetCurrentTime()<<"\n";

  os << indent << "Current Matrix:\n";
  GetMatrix()->Print(os,indent.GetNextIndent());

  os << indent << "Current Absolute Matrix:\n";
  m_VME->GetAbsMatrixPipe()->GetMatrix().Print(os,indent.GetNextIndent());

  mafTimeStamp tbounds[2];
  GetLocalTimeBounds(tbounds);
  os << indent << "Time Bounds: ("<<tbounds[0]<<","<<tbounds[1]<<"]"<<std::endl;

  mafOBB bounds;
  GetVMEBounds(bounds);
  os << indent << "VME 3D Bounds:"<<std::endl;
  bounds.Print(os,indent.GetNextIndent());

  os << indent << "VME 4D Bounds:"<<std::endl;
  Get4DBounds(bounds);
  bounds.Print(os,indent.GetNextIndent());

  os << indent << "DataType: "<<m_DataType<<std::endl;
}

