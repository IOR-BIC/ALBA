/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutput.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-11 15:46:26 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEOutput_cxx
#define __mafVMEOutput_cxx

#include "mafVMEOutput.h"
#include "mafAbsMatrixPipe.h"
#include "mafOBB.h"
#include "mafSmartPointer.h"
#include "mafTransform.h"
#include "mafIndent.h"

#include <assert.h>

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

//-------------------------------------------------------------------------
int mafVMEOutput::IsAnimated()
//-------------------------------------------------------------------------
{
  return m_VME->IsAnimated();
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetLocalTimeBounds(mafTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  mafVME::TimeVector tstamps;
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
void mafVMEOutput::GetTimeBounds(mafTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  assert(m_VME);

  GetLocalTimeBounds(tbounds);

  for (int i=0;i<m_VME->GetNumberOfChildren();i++)
  {
    mafTimeStamp tmp[2];

    m_VME->GetChild(i)->GetTimeBounds(tmp);
    if (tmp[0]<0 || tmp[1]<0)
      continue;

    if (tmp[0]<tbounds[0]||tbounds[0]<0)
      tbounds[0]=tmp[0];

    if (tmp[1]>tbounds[1]||tbounds[1]<0)
      tbounds[1]=tmp[1];
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVME4DBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  GetVME4DBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVME4DBounds(mafOBB &bounds)
//-------------------------------------------------------------------------
{
  assert(m_VME);

  std::vector<mafTimeStamp> timestamps;

  m_VME->GetTimeStamps(timestamps);

  bounds.Reset();

  if (mafDataPipe *dpipe=m_VME->GetDataPipe()) // if a datapipe is present
  {
    // THD SAFE implementation
    //mflDataPipe *datapipe=dpipe->MakeACopy();

    mafSmartPointer<mafMatrix> itemAbsPose;
    mafOBB transformed_bounds;

    for (int i=0;i<timestamps.size();i++)
    {
      // THD SAFE implementation
      /*
      datapipe->SetCurrentTime(timestamps[i]);
      datapipe->UpdateBounds();
      mafOBB *itemBounds=datapipe->GetBounds();
      */

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
  else if (CurrentData.GetPointer())
  {
    bounds.DeepCopy(CurrentData->GetBounds());
  }

}

//-------------------------------------------------------------------------
//void mafVMEOutput::GetLocal4DBounds(mafTimeStamp start, mafTimeStamp end, double bounds[6])
//{
//}

//-------------------------------------------------------------------------
//void mafVMEOutput::Get4DBounds(mafTimeStamp start, mafTimeStamp end, double bounds[6])
//{
//}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVMEBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  GetVMEBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVMEBounds(mafOBB &bounds,mafTimeStamp t, mafNodeIterator *iter)
//-------------------------------------------------------------------------
{
  assert(m_VME);
  if ((iter&&iter->IsVisible(this))||IsVisible())
  {
    mafSmartPointer<mafMatrix> itemPose;
    
    if (t<0)
    {
      t=m_VME->GetCurrentTime();
      itemPose=GetAbsMatrix();
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
void mafVMEOutput::GetVMELocalBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  GetVMELocalBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVMELocalBounds(mafOBB &bounds,mafTimeStamp t, mafNodeIterator *iter)
//-------------------------------------------------------------------------
{
  if (t<0)
    t=m_VME->GetCurrentTime();

  bounds.Reset();

  if ((iter&&iter->IsVisible(this))||m_VME->IsVisible())
  {
    GetDataBounds(bounds,t);
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetDataBounds(mafOBB &bounds,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (m_VME->GetDataPipe()) // allocate data pipe if not done yet
  {
    // We call directly the m_DataPipe UpdateBounds since this doesn't update
    // the output data, which is not necessary.
    // Must call explicitly UpdateBounds() method, since datapipes
    // do not update automatically when time changes  
    m_DataPipe->SetCurrentTime(t);
    m_DataPipe->UpdateBounds();
    bounds.DeepCopy(m_DataPipe->GetBounds());

    // restore the right bounds for current time... 
    // TODO: modify the GetBounds to make it call UpdateCurentBounds explicitelly!
    m_DataPipe->SetCurrentTime(m_VME->GetCurrentTime());
    m_DataPipe->UpdateBounds();    
  }
}
//-------------------------------------------------------------------------
void mafVMEOutput::GetBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  GetBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetBounds(mafOBB &bounds,mafTimeStamp t, mafNodeIterator *iter)
//-------------------------------------------------------------------------
{
  if (t<0)
    t=m_VME->GetCurrentTime();

  GetVMEBounds(bounds,t,iter);
  
  for (int i=0;i<GetNumberOfChildren();i++)
  {
    mafVMEOutput *child=GetChild(i);
    mafOBB childBounds;

    child->GetBounds(childBounds,t);

    bounds.MergeBounds(childBounds);
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::Get4DBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  Get4DBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::Get4DBounds(mafOBB &bounds)
//-------------------------------------------------------------------------
{

  std::vector<mafTimeStamp> timestamps;

  bounds.Reset();

  GetTimeStamps(timestamps);

  mafOBB frameBounds;
  for (int i=0;i<timestamps.size();i++)
  {
    GetBounds(frameBounds,timestamps[i]);
    bounds.MergeBounds(frameBounds); 
  }

}

//-------------------------------------------------------------------------
mafMatrix *mafVMEOutput::GetMatrix()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  return (m_VME->GetMatrixPipe())?m_VME->GetMatrixPipe()->GetMatrix():m_Matrix;
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetMatrix(mafMatrix &matrix,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  assert(m_VME);

  if (matrix)
  {
    if (mafMatrixPipe *mpipe=m_VME->GetMatrixPipe()) // check if a matrix pipe is present
    {
      if (t<0||t==m_VME->GetCurrentTime())
      {
        matrix.DeepCopy(GetPose());
      }
      else
      {
        // disable rising of update event since this is
        // only a temporary change to the matrix
        int old_flag=mpipe->GetUpdateMatrixObserverFlag();
        mpipe->UpdateMatrixObserverOff();
        mpipe->SetCurrentTime(t);
        matrix.DeepCopy(mpipe->GetMatrix());

        // restore right time
        mpipe->SetCurrentTime(m_VME->GetCurrentTime());
        mpipe->SetUpdateMatrixObserverFlag(old_flag);
      }
    }
    else
    {
      matrix->DeepCopy(m_Matrix);
    }
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetPose(double xyz[3],double rxyz[3],mafTimeStamp t)
//-------------------------------------------------------------------------
{
  mafMatrix mat;
  
  GetMatrix(mat,t);
  
  mafTransform::GetOrientation(mat,rxyz);
  
  mafTransform::GetPosition(mat,xyz);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mafTimeStamp t)
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
void mafVMEOutput::GetAbsMatrix(mafMatrix &matrix,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  assert(m_VME);

  if (matrix)
  {
    if (t<0||t==m_VME->GetCurrentTime())
    {
      matrix.DeepCopy(GetAbsMatrix());
    }
    else
    {
      mafAbsMatrixPipe *abspipe=m_VME->GetAbsMatrixPipe();
      // disable rising of update event since this is
      // only a temporary change to the matrix
      int old_flag=abspipe->GetUpdateMatrixObserverFlag();
      abspipe->UpdateMatrixObserverOff();
      abspipe->SetCurrentTime(t);
      matrix.DeepCopy(abspipe->GetMatrix());
      
      // restore right time
      abspipe->SetCurrentTime(m_VME->GetCurrentTime());
      abspipe->SetUpdateMatrixObserverFlag(old_flag);
    }
  }
}

//-------------------------------------------------------------------------
mafMatrix *mafVMEOutput::GetAbsMatrix()
//-------------------------------------------------------------------------
{
  return GetAbsMatrixPipe()->GetMatrix();
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsPose(double xyz[3],double rxyz[3],mafTimeStamp t)
//-------------------------------------------------------------------------
{
  mafSmartPointer<mafMatrix> mat;
  GetAbsMatrix(mat,t);

  mafTransform::GetPosition(mat,xyz);

  mafTransform::GetOrientation(mat,rxyz);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mafTimeStamp t)
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
mafTimeStamp mafVMEOutput::GetCurrentTime()
//-------------------------------------------------------------------------
{
  return m_VME->GetCurrentTime();
}

//-------------------------------------------------------------------------
void mafVMEOutput::Print(std::ostream& os, const int tabs)
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "Current Time: "<<m_VME->GetCurrentTime()<<"\n";

  os << indent << "Current Matrix:\n";
  GetMatrix()->Print(os,indent.GetNextIndent());

  os << indent << "Current Absolute Matrix:\n";
  GetAbsMatrix()->Print(os,indent.GetNextIndent());

  mafTimeStamp tbounds[2];
  GetLocalTimeBounds(tbounds);
  os << indent << "Time Bounds: ("<<tbounds[0]<<","<<tbounds[1]<<"]"<<endl;

  mafOBB bounds;
  GetVMEBounds(bounds);
  os << indent << "VME 3D Bounds:"
  bounds.Print(os,indent.GetNextIndent())

  os << indent << "VME 4D Bounds:\n";
  Get4DBounds()->PrintSelf(os,indent.GetNextIndent());

  os << indent << "DataType: "<<m_DataType<<std::endl;

  /*if (!CurrentData)
  {
    os << indent << "Current Data: NULL\n";
  }
  else
  {
    os << indent << "Current Data: "<< CurrentData->GetTypeName()<<"("<<CurrentData<<")\n";
    GetCurrentData()->PrintSelf(os,indent.GetNextIndent());
  }*/
}

#endif