/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutput.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-10 15:33:25 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEOutput_cxx
#define __mafVMEOutput_cxx

#include "mafVMEOutput.h"

#include "mafDataVector.h"
#include "mafMatrixVector.h"
#include "mafAbsMatrixPipe.h"
#include "mafOBB.h"
#include "mafSmartPointer.h"
#include "mafTransform.h"

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
mafAbsMatrixPipe *mafVMEOutput::GetAbsMatrixPipe()
//-------------------------------------------------------------------------
{
  return m_AbsMatrixPipe;
  if (!m_AbsMatrixPipe)
  {
    // the matrix pipe for computing the ABS matrix (direct cinematic)
    vtkNEW(m_AbsMatrixPipe);
    

    // TODO: evaluate if this anymore necessary 
    AbsMatrixUpdateTag= mflAgent::PlugEventSource(m_AbsMatrixPipe,AbsMatrixUpdateCallback,this,mflMatrixPipe::MatrixUpdateEvent);
  }

  return m_AbsMatrixPipe;
}

//-------------------------------------------------------------------------
const char *mafVMEOutput::GetDataType()
//-------------------------------------------------------------------------
{
  vtkDataSet *data=GetOutput();  
  return data?data->GetTypeName():NULL;
}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMEOutput::MakeCopy(mafVMEOutput *a)
//-------------------------------------------------------------------------
{
  mafVMEOutput* newvme=a->NewInstance();
  newvme->DeepCopy(a);
  return newvme;
}

//-------------------------------------------------------------------------
int mafVMEOutput::DeepCopy(mafVMEOutput *a)
//-------------------------------------------------------------------------
{  
  if (CanCopy(a))
  {
    // Copy arrays.
		if (TagArray)
			TagArray->DeepCopy(a->GetTagArray());

    DataArray->DeepCopy(a->GetDataArray());

    MatrixVector->DeepCopy(a->MatrixVector);

    SetMatrixPipe(a->GetMatrixPipe()?a->GetMatrixPipe()->MakeACopy():NULL);
    SetDataPipe(a->GetDataPipe()?a->GetDataPipe()->MakeACopy():NULL);

    // attributes
    SetName(a->GetName());

    // Runtime properties
    //AutoUpdateAbsMatrix=a->GetAutoUpdateAbsMatrix();
    SetCurrentTime(a->GetCurrentTime());

    return VTK_OK;
  }
  else
  {
    vtkErrorMacro("Cannot copy VME of type "<<a->GetTypeName()<<" into a VME \
    VME of type "<<GetTypeName());

    return VTK_ERROR;
  }
}

//-------------------------------------------------------------------------
int mafVMEOutput::ShallowCopy(mafVMEOutput *a)
//-------------------------------------------------------------------------
{  
  if (CanCopy(a))
  {
    // Copy tag & matrix array.
    if (TagArray)
			TagArray->DeepCopy(a->GetTagArray());

    MatrixVector->DeepCopy(a->GetMatrixVector());    

    // shallow copy data array
    DataArray->ShallowCopy(a->GetDataArray());

    

    SetMatrixPipe(a->GetMatrixPipe()?a->GetMatrixPipe()->MakeACopy():NULL);
    SetDataPipe(a->GetDataPipe()?a->GetDataPipe()->MakeACopy():NULL);

    // attributes
    SetName(a->GetName());

    // Runtime properties
    //AutoUpdateAbsMatrix=a->GetAutoUpdateAbsMatrix();
    SetCurrentTime(a->GetCurrentTime());

    return VTK_OK;
  }
  else
  {
    vtkErrorMacro("Cannot copy VME of type "<<a->GetTypeName()<<" into a VME \
    VME of type "<<GetTypeName());

    return VTK_ERROR;
  }
}

//-------------------------------------------------------------------------
bool mafVMEOutput::CanCopy(mafVMEOutput *vme)
//-------------------------------------------------------------------------
{
  if (!vme)
    return false;

  if (vme->IsA(GetTypeName()))
  {
    return true;
  }

  return false;
}

//-------------------------------------------------------------------------
int mafVMEOutput::GetItem(int id,mafVMEOutputItem *&item)
//-------------------------------------------------------------------------
{
  return DataArray->GetItemByIndex(id,item);
}

//-------------------------------------------------------------------------
mafVMEOutputItem *mafVMEOutput::GetItem(int id)
//-------------------------------------------------------------------------
{
  mafVMEOutputItem *item;
  return (GetItem(id,item)==VTK_OK?item:NULL);
}

//-------------------------------------------------------------------------
int mafVMEOutput::AddItem(mafVMEOutputItem *item)
//-------------------------------------------------------------------------
{
  if (item)
  {
    return DataArray->SetItem(item);
  }

  return VTK_ERROR;
}
  

//-------------------------------------------------------------------------
int mafVMEOutput::RemoveItem(int id)
//-------------------------------------------------------------------------
{
  return DataArray->RemoveItem(id);
}

//-------------------------------------------------------------------------
int mafVMEOutput::RemoveItem(mafVMEOutputItem *item)
//-------------------------------------------------------------------------
{
  int idx;

  idx=DataArray->FindItem(item);
  if (idx>=0)
  {
    return DataArray->RemoveItem(idx);
  }
  else
  {
    return VTK_ERROR;
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::RemoveAllItems()
//-------------------------------------------------------------------------
{
  DataArray->RemoveAllItems();
}

//-------------------------------------------------------------------------
int mafVMEOutput::GetNumberOfItems()
//-------------------------------------------------------------------------
{
  return DataArray->GetNumberOfItems();
}

/*//------------------------------------------------------------------------------
int mafVMEOutput::SetParent(vtkTree *parent)
//-------------------------------------------------------------------------
{
  if (mafVMEOutput *parent_vme=mafVMEOutput::SafeDownCast(parent))
  {
    if (CanReparentTo(parent_vme)==VTK_OK)
    {  
      Superclass::SetParent(parent_vme);

      if (parent_vme==NULL)
      {
        InvokeEvent(mafVMEOutput::DetachFromTreeEvent,this);
      }
      else
      {
        InvokeEvent(mafVMEOutput::AttachToTreeEvent,this);
      }

      // this forces the the pipe to Update its input and input frame
      if (m_AbsMatrixPipe)
        m_AbsMatrixPipe->SetVME(this);

      return VTK_OK;
    }

    //modified by Stefano 27-10-2004 (beg)
    //Changed the error macro to give feedback about vme names
    
    vtkErrorMacro("Cannot reparent the VME: " << GetName() << " under the "<<parent->GetTypeName() 
      << " named " << parent_vme->GetName());
    //modified by Stefano 27-10-2004 (end)
  }
  else
  {
    // reparenting to NULL is admitted
    if (parent==NULL)
    {
      Superclass::SetParent(parent);
      return VTK_OK;
    }
  }

  return VTK_ERROR;
}
*/
//-------------------------------------------------------------------------
/*void mafVMEOutput::UnRegister(vtkObjectBase *a)
//-------------------------------------------------------------------------
{
  // If this object is still referenced only by its children
  // and items the object is deleted, removing all children and
  // items
  if (ReferenceCount==(GetNumberOfChildren()+GetNumberOfItems()))
  {
    
    // if the unregistering object is an Item simply return
    if (mafVMEOutputItem *b=mafVMEOutputItem::SafeDownCast(a))
    {
      vtkIdType ret;
      if (DataArray->FindItem(b))
        return;    
    }
   
    // if the unregister object is 
    if (mafVMEOutput *b=mafVMEOutput::SafeDownCast(a))
    {
      if (IsAChild(b))
        return;
    }

    RemoveAllNodes();
    RemoveAllItems();
  }
  vtkObject::UnRegister(this);
}
*/
//-------------------------------------------------------------------------
/*void mafVMEOutput::UnRegister(mafVMEOutputItem *a)
//-------------------------------------------------------------------------
{
  vtkIdType ret;
  DataArray->FindItem(a,ret);
  if (ret==VTK_OK)
  {
    vtkObject::UnRegister(a);
  }
  
  UnRegister((vtkObject *)a);
  
}
*/

//-------------------------------------------------------------------------
int mafVMEOutput::FindItem(mflTimeStamp t,mafVMEOutputItem *&item)
//-------------------------------------------------------------------------
{
  
  return DataArray->FindItem(t,item);

}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMEOutput::FindInTree(const char *name,const char *value,int type)
//-------------------------------------------------------------------------
{
  vtkTagArray *tarray=GetTagArray();

  if (vtkTagItem *item=tarray->GetTag(name))
  {
    if (item->GetStaticType()==type)
    {
      if (vtkString::Compare(item->GetValue(),value))
      {
        return this;
      }
    }
  }
  
  for (int i=0;i<GetNumberOfChildren();i++)
  {
    mafVMEOutput *curr=GetChild(i);
    
    if (mafVMEOutput *ret=curr->FindInTree(name,value,type))
      return ret;
  }

  return NULL;
}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMEOutput::FindInTreeByName(const char *name)
//-------------------------------------------------------------------------
{
  if (Name==name)
  {
     return this;
  }
  
  for (int i=0;i<GetNumberOfChildren();i++)
  {
    mafVMEOutput *curr=GetChild(i);
    
    if (mafVMEOutput *ret=curr->FindInTreeByName(name))
      return ret;
  }

  return NULL;
}

//-------------------------------------------------------------------------
int mafVMEOutput::FindChildIdx(const char *name)
//-------------------------------------------------------------------------
{
  if (name)
  {
    for (int i=0;i<GetNumberOfChildren();i++)
    {
      mafVMEOutput *vme=GetChild(i);
      if (vme&&vtkString::Equals(vme->GetName(),name))
      {
        return i;
      }
    }
  }

  return -1;
}

//-------------------------------------------------------------------------
void mafVMEOutput::UpdateData(int idx)
//-------------------------------------------------------------------------
{
  mafVMEOutputItem *item=GetItem(idx);
  if (item)
    item->UpdateData();
}


//-------------------------------------------------------------------------
void mafVMEOutput::UpdateAllData()
//-------------------------------------------------------------------------
{
  // update all the items in this VME. Does not
  // propagate in the sub tree since to do this 
  // we must use a VMEIterator, specifying the 
  // traversing modality
  for (int i=0;i<GetNumberOfItems();i++)
  {
    GetItem(i)->UpdateData();
  }
}

//-------------------------------------------------------------------------
// TODO: to be moved to mafAbsMatrixPipe
/*void mafVMEOutput::UpdateAbsMatrix(mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=t<0?m_CurrentTime:t;

  mflMatrix *absmat=AbsMatrix;
  
  mafVMEOutput *parent=GetParent();
  mflMatrix *pmat=NULL;

  if (parent)
  {
    //parent->UpdateAbsMatrix(t);
    pmat=parent->GetAbsMatrix(t);
  }
    
  
  if (absmat==NULL||(absmat->GetTimeStamp()!=t)|| \
    (absmat->GetMTime()<MatrixVector->GetMTime())|| \
  (pmat&&absmat->GetMTime()<pmat->GetMTime()))
  {
    if (absmat==NULL)
    {
      AbsMatrix=mflMatrix::New();
    }

    //DEBUG
    //vtkTransform *trans=vtkTransform::New();
    //double rxyz[3];
    //trans->PreMultiply();

    mflMatrix *mat=GetVMatrix()->GetMatrix(t);

    if (pmat)
    {
    
        AbsMatrix->DeepCopy(pmat);
        
        //DEBUG
        //trans->SetMatrix(pmat); 
        //trans->GetOrientation(rxyz);
       

    }
    else
    {
      AbsMatrix->Identity();
    }

    //trans->Concatenate(mat);
    //AbsMatrix->DeepCopy(trans->GetMatrix());
    //AbsMatrix->SetTimeStamp(GetCurrentTime());
    //trans->Delete();

    vtkMatrix4x4::Multiply4x4(AbsMatrix, mat,AbsMatrix);

    //DEBUG
    //trans->SetMatrix(AbsMatrix);    
    //trans->GetOrientation(rxyz);

    //trans->Delete();

    AbsMatrix->SetTimeStamp(t);
  }
}
*/

//-------------------------------------------------------------------------
void mafVMEOutput::UpdateData(mafVMEOutputItem *item)
//-------------------------------------------------------------------------
{
  if (item)
  {
    if (GetParent())
    {
      // propagate the update event up to the root
      GetParent()->UpdateData(item);
    }
    // if this node hasn't a parent, since it's not
    // a root we can't do anything: this is an orphan
    // node...
  }
}

//-------------------------------------------------------------------------
// TODO: to be rewritten
void mafVMEOutput::SetCurrentTime(mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (t<0)
    t=0;

  m_CurrentTime=t;

  // Must keep a time variable also on the
  // pipes to allow multiple pipes contemporary 
  // working at different times
  // 
  if (m_DataPipe)
    m_DataPipe->SetCurrentTime(t);

  if (m_MatrixPipe)
    m_MatrixPipe->SetCurrentTime(t);
  else if (CurrentMatrix.GetPointer())
    CurrentMatrix->SetTimeStamp(t);

  if (m_AbsMatrixPipe)
    m_AbsMatrixPipe->SetCurrentTime(t);

  Modified();

  // TODO: consider if to add a flag to diable event issuing
  InvokeEvent(mafVMEOutput::TimeEvent,this);
  
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetTreeTime(mflTimeStamp t)
//-------------------------------------------------------------------------
{
  SetCurrentTime(t);

  for (int i=0;i<GetNumberOfChildren();i++)
  {
    GetChild(i)->SetTreeTime(t);
  }
}

//-------------------------------------------------------------------------
mafVMEOutputIterator *mafVMEOutput::NewIterator()
//-------------------------------------------------------------------------
{
  mafVMEOutputIterator *iter=mafVMEOutputIterator::New();
  iter->SetRootNode(this);
  return iter;
}

//-------------------------------------------------------------------------
int mafVMEOutput::IsAnimated()
//-------------------------------------------------------------------------
{
  return ((GetNumberOfItems()>1)||(GetMatrixVector()->GetNumberOfMatrixes()>1));
}


//-------------------------------------------------------------------------
void mafVMEOutput::GetDataTimeBounds(mflTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  tbounds[0]=-1;
  tbounds[1]=-1;

  // find the time interval for the items in this VME
  for (int i=0;i<GetNumberOfItems();i++)
  {
    mafVMEOutputItem *item=GetItem(i);
    if (item->GetTimeStamp()<tbounds[0]||tbounds[0]<0)
      tbounds[0]=item->GetTimeStamp();
    if (item->GetTimeStamp()>tbounds[1]||tbounds[1]<0)
      tbounds[1]=item->GetTimeStamp();
  }
}

//-------------------------------------------------------------------------
int mafVMEOutput::GetNumberOfLocalTimeStamps()
//-------------------------------------------------------------------------
{
  TimeVector timestamps;
  GetLocalTimeStamps(timestamps);
  return timestamps.size();
}

//-------------------------------------------------------------------------
int mafVMEOutput::GetNumberOfTimeStamps()
//-------------------------------------------------------------------------
{
  TimeVector timestamps;
  GetTimeStamps(timestamps);
  return timestamps.size();
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetLocalTimeBounds(mflTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  tbounds[0]=-1;
  tbounds[1]=-1;

  DataArray->GetTimeBounds(tbounds);
  
  mflTimeStamp tmp[2];
  // does the same this with the matrix vector
  MatrixVector->GetTimeBounds(tmp);

  if (tmp[0]<0 || tmp[1]<0)
      return;

  if (tmp[0]<tbounds[0]||tbounds[0]<0)
      tbounds[0]=tmp[0];

  if (tmp[1]>tbounds[1]||tbounds[1]<0)
      tbounds[1]=tmp[1];

}

//-------------------------------------------------------------------------
void mafVMEOutput::GetTimeBounds(mflTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  GetLocalTimeBounds(tbounds);

  for (int i=0;i<GetNumberOfChildren();i++)
  {
    mflTimeStamp tmp[2];

    GetChild(i)->GetTimeBounds(tmp);
    if (tmp[0]<0 || tmp[1]<0)
      continue;

    if (tmp[0]<tbounds[0]||tbounds[0]<0)
      tbounds[0]=tmp[0];

    if (tmp[1]>tbounds[1]||tbounds[1]<0)
      tbounds[1]=tmp[1];
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetDataTimeStamps(mflTimeStamp *&kframes)
//-------------------------------------------------------------------------
{
  DataArray->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetDataTimeStamps(TimeVector &kframes)
//-------------------------------------------------------------------------
{
  DataArray->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetMatrixTimeStamps(mflTimeStamp *&kframes)
//-------------------------------------------------------------------------
{
  MatrixVector->GetTimeStamps(kframes);
}



//-------------------------------------------------------------------------
void mafVMEOutput::GetMatrixTimeStamps(TimeVector &kframes)
//-------------------------------------------------------------------------
{
  MatrixVector->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetLocalTimeStamps(mflTimeStamp *&kframes)
//-------------------------------------------------------------------------
{ 
  TimeVector frames;

  GetLocalTimeStamps(frames);
  
  kframes=new mflTimeStamp[frames.size()];

  for (int i=0;i<frames.size();i++)
  {
    kframes[i]=frames[i];
  } 
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetLocalTimeStamps(std::vector<mflTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();

  std::vector<mflTimeStamp> datatimestamps;
  std::vector<mflTimeStamp> matrixtimestamps;
  
  DataArray->GetTimeStamps(datatimestamps);
  MatrixVector->GetTimeStamps(matrixtimestamps);

  MergeTimeVectors(kframes,datatimestamps,matrixtimestamps);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetLocalTimeStamps(vtkDoubleArray *kframes)
//-------------------------------------------------------------------------
{
  assert(kframes);
  TimeVector frames;
  GetLocalTimeStamps(frames);
  kframes->SetNumberOfComponents(1);
  kframes->SetNumberOfTuples(frames.size());
  for (int i=0;i<frames.size();i++)
  {
    kframes->SetTuple(i,&(frames[i]));
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetTimeStamps(mflTimeStamp *&kframes)
//-------------------------------------------------------------------------
{
  std::vector<mflTimeStamp> frames;

  GetTimeStamps(frames);

  if(frames.size()>0)
  {
    kframes=new mflTimeStamp[frames.size()];
  }
  else
  {
    kframes=NULL;
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetTimeStamps(TimeVector &kframes)
//-------------------------------------------------------------------------
{
  GetLocalTimeStamps(kframes);
  
  std::vector<mflTimeStamp> subKFrames;

  for (int i=0;i<GetNumberOfChildren();i++)
  {
    GetChild(i)->GetTimeStamps(subKFrames);

    MergeTimeVectors(kframes,kframes,subKFrames);
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetTimeStamps(vtkDoubleArray *kframes)
//-------------------------------------------------------------------------
{
  assert(kframes);
  TimeVector frames;
  GetTimeStamps(frames);
  kframes->SetNumberOfComponents(1);
  kframes->SetNumberOfTuples(frames.size());
  for (int i=0;i<frames.size();i++)
  {
    kframes->SetTuple(i,&(frames[i]));
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsTimeStamps(mflTimeStamp *&kframes)
{
  std::vector<mflTimeStamp> frames;

  GetAbsTimeStamps(frames);

  if(frames.size()>0)
  {
    kframes=new mflTimeStamp[frames.size()];
  }
  else
  {
    kframes=NULL;
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsTimeStamps(TimeVector &kframes)
{
  GetLocalTimeStamps(kframes);
  
  std::vector<mflTimeStamp> parentKFrames;

  for (mafVMEOutput *parent=GetParent();parent;parent=parent->GetParent())
  {

    parent->GetLocalTimeStamps(parentKFrames);

    MergeTimeVectors(kframes,kframes,parentKFrames);
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
  std::vector<mflTimeStamp> timestamps;

  GetTimeStamps(timestamps);

  bounds.Reset();

  if (GetDataPipe()) // allocate data pipe if not done yet
  {
    // THD SAFE implementation
    //mflDataPipe *datapipe=m_DataPipe->MakeACopy();

    mafSmartPointer<mflMatrix> itemAbsPose;
    mafOBB transformed_bounds;

    for (int i=0;i<timestamps.size();i++)
    {
      // THD SAFE implementation
      /*
      datapipe->SetCurrentTime(timestamps[i]);
      datapipe->UpdateCurrentBounds();
      mafOBB *itemBounds=datapipe->GetCurrentBounds();
      */

      m_DataPipe->SetCurrentTime(timestamps[i]);
      m_DataPipe->UpdateCurrentBounds();

      // must make a copy, otherwise I would transform the bounds inside the data pipe
      transformed_bounds.DeepCopy(m_DataPipe->GetCurrentBounds());
      GetAbsMatrix(itemAbsPose,timestamps[i]);
    
      transformed_bounds.ApplyTransform(itemAbsPose);

      bounds.MergeBounds(transformed_bounds);
    }

    m_DataPipe->SetCurrentTime(m_CurrentTime);
    m_DataPipe->UpdateCurrentBounds();
  }
  else if (CurrentData.GetPointer())
  {
    bounds.DeepCopy(CurrentData->GetBounds());
  }

}

//-------------------------------------------------------------------------
//void mafVMEOutput::GetLocal4DBounds(mflTimeStamp start, mflTimeStamp end, double bounds[6])
//{
//}

//-------------------------------------------------------------------------
//void mafVMEOutput::Get4DBounds(mflTimeStamp start, mflTimeStamp end, double bounds[6])
//{
//}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVMESpaceBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  GetVMESpaceBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVMESpaceBounds(mafOBB &bounds,mflTimeStamp t, mafVMEOutputIterator *iter)
//-------------------------------------------------------------------------
{ 
  if ((iter&&iter->IsVisible(this))||IsVisible())
  {
    mafSmartPointer<mflMatrix> itemPose;
    
    if (t<0)
    {
      t=m_CurrentTime;
      itemPose=GetAbsMatrix();
    }
    else
    {
      GetAbsMatrix(itemPose,t);
    }
    
    GetVMELocalSpaceBounds(bounds,t,iter);  

    bounds.ApplyTransform(itemPose);
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVMELocalSpaceBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  GetVMELocalSpaceBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetVMELocalSpaceBounds(mafOBB &bounds,mflTimeStamp t, mafVMEOutputIterator *iter)
//-------------------------------------------------------------------------
{
   if (t<0)
    t=m_CurrentTime;

  bounds.Reset();

  if ((iter&&iter->IsVisible(this))||IsVisible())
  {
    
    if (GetDataPipe()) // allocate data pipe if not done yet
    {
      // We call directly the m_DataPipe UpdateBounds since this doesn't update
      // the output data, which is not necessary.
      // Must call explicitelly UpdateCurrentBounds() method, since datapipes
      // do not update automatically when time changes  
      m_DataPipe->SetCurrentTime(t);
      m_DataPipe->UpdateCurrentBounds();
      bounds.DeepCopy(m_DataPipe->GetCurrentBounds());

      // restore the right bounds for current time... 
      // TODO: modify the GetCurrentBounds to make it call UpdateCurentBounds explicitelly!
      m_DataPipe->SetCurrentTime(m_CurrentTime);
      m_DataPipe->UpdateCurrentBounds();
    
      // this is a thread safe implemetation
      /*mflDataPipe *datapipe=m_DataPipe->MakeACopy();
      datapipe->SetCurrentTime(t);
      datapipe->UpdateCurrentBounds();
      itemBounds=datapipe->GetCurrentBounds();
      datapipe->Delete();
      */
    }
    else if (CurrentData.GetPointer())
    {
      bounds.DeepCopy(CurrentData->GetBounds());
    }
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetSpaceBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  GetSpaceBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetSpaceBounds(mafOBB &bounds,mflTimeStamp t, mafVMEOutputIterator *iter)
//-------------------------------------------------------------------------
{
  if (t<0)
    t=m_CurrentTime;

  GetVMESpaceBounds(bounds,t,iter);
  
  for (int i=0;i<GetNumberOfChildren();i++)
  {
    mafVMEOutput *child=GetChild(i);
    mafOBB childBounds;

    child->GetSpaceBounds(childBounds,t);

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

  std::vector<mflTimeStamp> timestamps;

  bounds.Reset();

  GetTimeStamps(timestamps);

  mafOBB frameBounds;
  for (int i=0;i<timestamps.size();i++)
  {
    GetSpaceBounds(frameBounds,timestamps[i]);
    bounds.MergeBounds(frameBounds); 
  }

}

//-------------------------------------------------------------------------
bool mafVMEOutput::Equals(mafVMEOutput *vme)
//-------------------------------------------------------------------------
{
  if (!vme||!vme->IsA(GetTypeName()))
    return false;

  if (GetNumberOfItems()!=vme->GetNumberOfItems())
  {
    return false;
  }

  for (int i=0;i<GetNumberOfItems();i++)
  {
    if (!GetItem(i)->Equals(vme->GetItem(i)))
    {
      return false;
    }
  }

  if (!GetTagArray()->Equals(vme->GetTagArray()))
    return false;

  if (!GetMatrixVector()->Equals(vme->GetMatrixVector()))
    return false;

  if (!Name.Equals(vme->GetName()))
    return false;

  return true;
}

//-------------------------------------------------------------------------
bool mafVMEOutput::CompareTree(mafVMEOutput *vme)
//-------------------------------------------------------------------------
{
  if (!Equals(vme))
    return false;

  if (vme->GetNumberOfChildren()!=GetNumberOfChildren())
  {
    return false;
  }

  for (int i=0;i<GetNumberOfChildren();i++)
  {
    if (!GetChild(i)->CompareTree(vme->GetChild(i)))
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------
mafVMEOutput *mafVMEOutput::CopyTree(mafVMEOutput *vme, mafVMEOutput *parent)
//-------------------------------------------------------------------------
{
  
  mafVMEOutput* v = vme->MakeCopy();

  v->ReparentTo(parent);

  if (parent)
  {
    v->Delete();
  }

  for(int i=0; i<vme->GetNumberOfChildren(); i++)
  {
    if (mafVMEOutput *child=vme->GetChild(i))
      if (child->IsVisible())
        mafVMEOutput::CopyTree(child,v);
  }

  return v;
}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMEOutput::ReparentTo(mafVMEOutput *newparent)
//-------------------------------------------------------------------------
{
  // We cannot reparent to a subnode!!!
  if (!IsInTree(newparent))
  {
    // When we reparent to a different tree, or we simply
    // cut a tree, pre travers the sub tree to read data into memory
    // future release should read one item at once, write it
    // to disk and then release the data, or better simply copy the file
    // into the new place, this to be able to manage HUGE datasets.
    if (newparent==NULL||GetRoot()!=newparent->GetRoot())
    {
      mafVMEOutputIterator *iter=NewIterator();
      for (mafVMEOutput *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
      {
        for (int i=0;i<vme->GetNumberOfItems();i++)
        {
          mafVMEOutputItem *item=vme->GetItem(i);
          if (item)
          {
            // read the data from disk and set the Id to -1
            // to advise the reader to write it again on disk.
            // Also remove the old file name...
            vtkDataSet *data=item->GetData();
            if (data)
            {
              item->SetId(-1);
              item->SetFileName("");
            }
          }
          else
          {
            vtkGenericWarningMacro("Debug: found NULL node");
          }
        }
      }

      iter->Delete();
    }

    // Add this node to the new parent children list and
    // remove it from old parent children list.
    // We first add it to the new parent, thus it is registered
    // from the new parent, the we remove it from the list of the old parent.
    // We must keep the oldparent pointer somewhere since it is oeverwritten
    // by AddChild.
    
    mafVMEOutput *oldparent=GetParent();

    Register(this);

    if (newparent)
    {
      if (newparent->AddChild(this)==VTK_ERROR)
      {
        vtkErrorMacro("Cannot Reparent node "<<GetName()<<" to node "<<newparent->GetName());
        return NULL;
      }

      SetCurrentTime(newparent->GetCurrentTime()); // update data & pose to parent m_CurrentTime
    }
    else
    {
      if (SetParent(NULL)==VTK_ERROR)
      {
        vtkErrorMacro("Cannot Reparent node "<<GetName()<<" to NULL");
        return NULL;
      }
    }

    if (m_AbsMatrixPipe)
    {
      m_AbsMatrixPipe->SetVME(this);
      m_AbsMatrixPipe->Update();
    }

    if (oldparent)
    {
      oldparent->RemoveChild(this);
    }

    mafVMEOutput *ret=(ReferenceCount==1)?NULL:this;
    
    UnRegister(this);

    return ret;
  }
  else
  {
    return NULL;
  }
}


//----------------------------------------------------------------------------
void mafVMEOutput::Import(mafVMEOutput *tree)
//-------------------------------------------------------------------------
{
  if (tree&&tree->GetNumberOfChildren()>0)
  {
    int num=tree->GetNumberOfChildren();
    for (int i=0;i<num;i++)
    {
      mafVMEOutput *vme=tree->GetFirstChild();
      vme->ReparentTo(this);
    }
  }
}

//-------------------------------------------------------------------------
mafVMEOutputItem *mafVMEOutput::GetItemByTimeStamp(mflTimeStamp t)
//-------------------------------------------------------------------------
{
  return DataArray->GetItemByTimeStamp(t);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetPose(mflMatrix *mat)
//-------------------------------------------------------------------------
{
  GetMatrixVector()->SetMatrix(mat);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetPose(vtkMatrix4x4 *mat, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?t=m_CurrentTime:t;
  if (MatrixVector)
    GetMatrixVector()->SetMatrix(mat,t);
  else
    CurrentMatrix->DeepCopy(mat);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetPose(double x,double y,double z,double rx,double ry,double rz, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3],trxyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  SetPose(txyz,trxyz,t);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetPose(double xyz[3],double rxyz[3], mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?t=m_CurrentTime:t;

  mafSmartPointer<mflMatrix> matrix;

  mafTransform::SetOrientation(matrix,rxyz);

  mafTransform::SetPosition(matrix,xyz);

  matrix->SetTimeStamp(t);
  
  if (MatrixVector)
    MatrixVector->SetMatrix(matrix);
  else
    CurrentMatrix->DeepCopy(matrix);
}

//----------------------------------------------------------------------------
void mafVMEOutput::ApplyTransform(vtkLinearTransform *transform,int premultiply,mflTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;
  mafSmartPointer<mafTransform> new_pose;
  mafSmartPointer<mflMatrix> pose;
  GetMatrix(pose,t);
  new_pose->SetMatrix(pose);
  new_pose->Concatenate(transform,premultiply);
  SetPose(new_pose->GetMatrix(),t);
}
//----------------------------------------------------------------------------
void mafVMEOutput::ApplyMatrix(vtkMatrix4x4 *matrix,int premultiply,mflTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;
  mafSmartPointer<mafTransform> new_pose;
  mafSmartPointer<mflMatrix> pose;
  GetMatrix(pose,t);
  new_pose->SetMatrix(pose);
  new_pose->Concatenate(matrix,premultiply);
  SetPose(new_pose->GetMatrix(),t);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetPosition(double x,double y,double z, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  SetPosition(txyz,t);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetPosition(double xyz[3], mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mafSmartPointer<mflMatrix> matrix;
  
  mafTransform::SetPosition(matrix,xyz);

  mflMatrix *oldpose;
  MatrixVector->FindMatrix(t,oldpose);

  // recover the orientation only if a key matrix is present
  if (oldpose)
    mafTransform::CopyRotation(matrix,oldpose);

  matrix->SetTimeStamp(t);
  
  if (MatrixVector)
    MatrixVector->SetMatrix(matrix);
  else
    CurrentMatrix->DeepCopy(matrix);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetOrientation(double rx,double ry,double rz, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double trxyz[3];
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  SetOrientation(trxyz,t);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetOrientation(double rxyz[3], mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mafSmartPointer<mflMatrix> matrix;

  mflMatrix *oldpose;
  MatrixVector->FindMatrix(t,oldpose);
    
  mafTransform::SetOrientation(matrix,rxyz);
  
  // recover the position only if a key matrix is present
  if (oldpose)
  {
    double pos[3];
    mafTransform::GetPosition(oldpose,pos);
    mafTransform::SetPosition(matrix,pos);
  }

  matrix->SetTimeStamp(t);

  if (MatrixVector)
    MatrixVector->SetMatrix(matrix);
  else
    CurrentMatrix->DeepCopy(matrix);
}

//-------------------------------------------------------------------------
mflMatrix *mafVMEOutput::GetPose()
//-------------------------------------------------------------------------
{
  return (GetMatrixPipe())?m_MatrixPipe->GetMatrix():CurrentMatrix;
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetPose(mflMatrix *matrix,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (matrix)
  {
    if (GetMatrixPipe()) // allocate matrix pipe if not done yet
    {
      if (t<0||t==m_CurrentTime)
      {
        matrix->DeepCopy(GetPose());
      }
      else
      {
        // disable rising of update event since this is
        // only a temporary change to the matrix
        int old_flag=m_MatrixPipe->GetUpdateMatrixObserverFlag();
        m_MatrixPipe->UpdateMatrixObserverOff();
        m_MatrixPipe->SetCurrentTime(t);
        matrix->DeepCopy(m_MatrixPipe->GetMatrix());
        // restore right time
        m_MatrixPipe->SetCurrentTime(m_CurrentTime);
        m_MatrixPipe->SetUpdateMatrixObserverFlag(old_flag);

        // THD SAFE implementation
        // create a temporary pipe to interpolate at time t
        /*mflMatrixPipe *newpipe=m_MatrixPipe->MakeACopy();
        newpipe->SetCurrentTime(t);
        matrix->DeepCopy(newpipe->GetMatrix());
        newpipe->Delete();
        */
      }
    }
    else
    {
      matrix->DeepCopy(CurrentMatrix);
    }
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetPose(double xyz[3],double rxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mafSmartPointer<mflMatrix> mat;
  
  GetMatrix(mat,t);
  
  mafTransform::GetOrientation(mat,rxyz);
  
  mafTransform::GetPosition(mat,xyz);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mflTimeStamp t)
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
void mafVMEOutput::GetPosition(double &x,double &y,double &z,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double xyz[3];
  GetPosition(xyz,t);
  x=xyz[0];
  y=xyz[1];
  z=xyz[2];

}

//-------------------------------------------------------------------------
void mafVMEOutput::GetPosition(double xyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (t<0||t==m_CurrentTime)
  {
    mafTransform::GetPosition(GetMatrix(),xyz); 
  }
  else
  {
    mafSmartPointer<mflMatrix> mat;
    GetMatrix(mat,t);
    mafTransform::GetPosition(mat,xyz); 
  }  
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetOrientation(double &rx,double &ry,double &rz,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double rxyz[3];
  GetOrientation(rxyz,t);
  rx=rxyz[0];
  ry=rxyz[1];
  rz=rxyz[2];
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetOrientation(double rxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (t<0||t==m_CurrentTime)
  {
    mafTransform::GetOrientation(GetMatrix(),rxyz);
  }
  else
  {
    mafSmartPointer<mflMatrix> mat;
    GetMatrix(mat,t);
    mafTransform::GetOrientation(mat,rxyz);
  }  
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsPose(mflMatrix *matrix,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (matrix)
  {
    if (t<0||t==m_CurrentTime)
    {
      matrix->DeepCopy(GetAbsPose());
    }
    else
    {
      mafAbsMatrixPipe *abspipe=GetAbsMatrixPipe();
      // disable rising of update event since this is
      // only a temporary change to the matrix
      int old_flag=abspipe->GetUpdateMatrixObserverFlag();
      abspipe->UpdateMatrixObserverOff();
      abspipe->SetCurrentTime(t);
      matrix->DeepCopy(abspipe->GetMatrix());
      
      // restore right time
      abspipe->SetCurrentTime(m_CurrentTime);
      abspipe->SetUpdateMatrixObserverFlag(old_flag);

      // THD SAFE implementation
      // create a temporary pipe to interpolate at time t
      /*mflMatrixPipe *newpipe=abspipe->MakeACopy();
      newpipe->SetCurrentTime(t);
      matrix->DeepCopy(newpipe->GetMatrix());
      newpipe->Delete();
      */
    }
  }
}

//-------------------------------------------------------------------------
mflMatrix *mafVMEOutput::GetAbsPose()
//-------------------------------------------------------------------------
{
  return GetAbsMatrixPipe()->GetMatrix();
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsPose(double xyz[3],double rxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mafSmartPointer<mflMatrix> mat;
  GetAbsMatrix(mat,t);

  mafTransform::GetPosition(mat,xyz);

  mafTransform::GetOrientation(mat,rxyz);
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mflTimeStamp t)
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
void mafVMEOutput::GetAbsPosition(double xyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (t<0||t==m_CurrentTime)
  {
    mafTransform::GetPosition(GetAbsMatrix(),xyz);
  }
  else
  {
    mafSmartPointer<mflMatrix> mat;
    GetAbsMatrix(mat,t);
    mafTransform::GetPosition(mat,xyz);
  }
  
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsPosition(double &x,double &y,double &z,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double xyz[3];

  GetAbsPosition(xyz,t);

  x=xyz[0];
  y=xyz[1];
  z=xyz[2];
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsOrientation(double rxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (t<0||t==m_CurrentTime)
  {
    mafTransform::GetOrientation(GetAbsMatrix(),rxyz);
  }
  else
  {
    mafSmartPointer<mflMatrix> mat;
    GetAbsMatrix(mat,t);
    mafTransform::GetOrientation(mat,rxyz);
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::GetAbsOrientation(double &rx,double &ry,double &rz,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double rxyz[3];

  GetAbsOrientation(rxyz,t);

  rx=rxyz[0];
  ry=rxyz[1];
  rz=rxyz[2];
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetAbsPose(double x,double y,double z,double rx,double ry,double rz, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3],trxyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  SetAbsPose(txyz,trxyz,t);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetAbsPose(double xyz[3],double rxyz[3], mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;
  
  mafSmartPointer<mflMatrix> matrix;

  mafTransform::SetOrientation(matrix,rxyz);

  mafTransform::SetPosition(matrix,xyz);

  matrix->SetTimeStamp(t);

  SetAbsPose(matrix);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetAbsPose(vtkMatrix4x4 *matrix,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;

  mafSmartPointer<mflMatrix> mat;
  mat->DeepCopy(matrix);
  mat->SetTimeStamp(t);
  SetAbsPose(mat);
}
//-------------------------------------------------------------------------
void mafVMEOutput::SetAbsPose(mflMatrix *matrix)
//-------------------------------------------------------------------------
{
  if (Parent)
  {
    mafSmartPointer<mflMatrix> pmat;
    GetParent()->GetAbsMatrix(pmat,matrix->GetTimeStamp());

    pmat->Invert();

    vtkMatrix4x4::Multiply4x4(pmat,matrix,matrix);
  }
  
  SetPose(matrix);
}
//-------------------------------------------------------------------------
void mafVMEOutput::SetAbsPosition(double x,double y,double z, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  SetAbsPosition(txyz,t);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetAbsPosition(double xyz[3], mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;

  mafSmartPointer<mflMatrix> matrix;
  
  mafTransform::SetPosition(matrix,xyz);

  matrix->SetTimeStamp(t);

  mflMatrix *oldpose;
  MatrixVector->FindMatrix(t,oldpose);

  // recover the orientation only if a key matrix is present
  if (oldpose)
  {
    mafSmartPointer<mflMatrix> oldmat;
    GetAbsMatrix(oldmat,t);
    mafTransform::CopyRotation(matrix,oldmat);
  }

  if (Parent)
  {
    mafSmartPointer<mflMatrix> pmat;
    GetParent()->GetAbsMatrix(pmat,t);
    
    pmat->Invert();

    vtkMatrix4x4::Multiply4x4(pmat,matrix,matrix);
  }

  SetPose(matrix);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetAbsOrientation(double rx,double ry,double rz, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double trxyz[3];
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  SetAbsOrientation(trxyz,t);
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetAbsOrientation(double rxyz[3], mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;

  mafSmartPointer<mflMatrix> matrix;
  
  mafTransform::SetOrientation(matrix,rxyz);

  matrix->SetTimeStamp(t);

  mflMatrix *oldpose;
  MatrixVector->FindMatrix(t,oldpose);

  // recover the position only if a key matrix is present
  if (oldpose)
  {
    double pos[3];
    GetAbsPosition(pos,t);
    mafTransform::SetPosition(matrix,pos);
  }

  if (Parent)
  {
    mafSmartPointer<mflMatrix> pmat;
    GetParent()->GetAbsMatrix(pmat,t);

    pmat->Invert();

    vtkMatrix4x4::Multiply4x4(pmat,matrix,matrix);
  }

  SetPose(matrix);
}

//----------------------------------------------------------------------------
void mafVMEOutput::ApplyAbsTransform(vtkLinearTransform *transform,int premultiply,mflTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;
  mafSmartPointer<mafTransform> new_pose;
  mafSmartPointer<mflMatrix> pose;
  GetAbsMatrix(pose,t);
  new_pose->SetMatrix(pose);
  new_pose->Concatenate(transform,premultiply);
  SetAbsPose(new_pose->GetMatrix(),t);
}
//----------------------------------------------------------------------------
void mafVMEOutput::ApplyAbsMatrix(vtkMatrix4x4 *matrix,int premultiply,mflTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;
  mafSmartPointer<mafTransform> new_pose;
  mafSmartPointer<mflMatrix> pose;
  GetAbsMatrix(pose,t);
  new_pose->SetMatrix(pose);
  new_pose->Concatenate(matrix,premultiply);
  SetAbsPose(new_pose->GetMatrix(),t);
}

/*
//----------------------------------------------------------------------------
void mafVMEOutput::SetScale(double sx,double sy,double sz,mflTimeStamp t)
{
  t=(t<0)?m_CurrentTime:t;

  mafSmartPointer<mflMatrix> matrix;

  matrix->DeepCopy(GetMatrix(t));
  
  mafTransform::Scale(matrix,sx,sy,sz);

  MatrixVector->SetMatrix(matrix);
}

//----------------------------------------------------------------------------
void mafVMEOutput::SetScale(double sxyz[3],mflTimeStamp t)
{
  SetScale(sxyz[0],sxyz[1],sxyz[2]);
}
*/

//----------------------------------------------------------------------------
void mafVMEOutput::GetScale(double sxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mafSmartPointer<mflMatrix> mat;
  GetMatrix(mat,t);
  mafTransform::GetScale(mat,sxyz);
}

//----------------------------------------------------------------------------
void mafVMEOutput::GetScale(double &sx,double &sy,double &sz,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double sxyz[3];
  GetScale(sxyz,t);
  sx=sxyz[0];
  sy=sxyz[1];
  sz=sxyz[2];
}

//-------------------------------------------------------------------------
void mafVMEOutput::OutputDataUpdateCallback(void *arg)
//-------------------------------------------------------------------------
{
  // route events toward VME observers
  mafVMEOutput *self=(mafVMEOutput *)arg;

  if (self->GetDataPipe()) // allocate data pipe if not done yet
  {
    if (self->m_DataPipe->GetOutput()!=self->CurrentData.GetPointer())
    {
      self->SetCurrentData(self->m_DataPipe->GetOutput());
    
      // advise observers the output data has changed
      self->InvokeEvent(mafVMEOutput::OutputDataChangedEvent);
    }

    self->InvokeEvent(mafVMEOutput::OutputDataUpdateEvent);
  }
  else
  {
    vtkErrorWithObjectMacro(self,"Received Update Output data event from NULL data pipe!");
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::MatrixUpdateCallback(void *arg)
//-------------------------------------------------------------------------
{
  // route events toward VME observers
  mafVMEOutput *self=(mafVMEOutput *)arg;
  self->InvokeEvent(mafVMEOutput::MatrixUpdateEvent,self);
}

//-------------------------------------------------------------------------
void mafVMEOutput::AbsMatrixUpdateCallback(void *arg)
//-------------------------------------------------------------------------
{
  // route events toward VME observers
  mafVMEOutput *self=(mafVMEOutput *)arg;
  self->InvokeEvent(mafVMEOutput::AbsMatrixUpdateEvent,self);
}

//-------------------------------------------------------------------------
void mafVMEOutput::PrintSelf(ostream& os, vtkIndent indent)
//-------------------------------------------------------------------------
{
  os << indent << "Name: "<<GetName()<<endl;

	os << indent << "Number of Children: "<<Children->GetNumberOfItems()<<endl;

	os << indent << "Tag Array Contents: \n";
	GetTagArray()->PrintSelf(os,indent.GetNextIndent());

  os << indent << "Current Time: "<<m_CurrentTime<<"\n";

  os << indent << "Current Matrix:\n";
  GetMatrix()->PrintSelf(os,indent.GetNextIndent());

  os << indent << "Current Absolute Matrix:\n";
  GetAbsMatrix()->PrintSelf(os,indent.GetNextIndent());

  mflTimeStamp tbounds[2];
  GetLocalTimeBounds(tbounds);
  os << indent << "Time Bounds: ("<<tbounds[0]<<","<<tbounds[1]<<"]"<<endl;

  mafOBB bounds;
  GetVMESpaceBounds(bounds);

  os << indent << "VME Space Bounds: ["<<bounds.Bounds[0]<<","<<bounds.Bounds[1]<<","<<bounds.Bounds[2]<<"," \
    <<bounds.Bounds[3]<<","<<bounds.Bounds[4]<<","<<bounds.Bounds[5]<<"]\n";
  

  //os << indent << "VME 4D Bounds:\n";
  //Get4DBounds()->PrintSelf(os,indent.GetNextIndent());

  os << indent << "Matrix Vector:\n";
  GetMatrixVector()->PrintSelf(os,indent.GetNextIndent());
  
  os << indent << "VME DataSet Array:\n";
  GetDataArray()->PrintSelf(os,indent.GetNextIndent());
  //os << indent << "Number of Items:"<<GetDataArray()->GetNumberOfItems()<<"\n";

  os << indent << "Matrix Pipe: ";
  if (GetMatrixPipe())
  {
    os << "\n";
    GetMatrixPipe()->PrintSelf(os,indent.GetNextIndent());
  }
  else
    os << "NULL\n";
  
  os << indent << "VME m_DataPipe: ";
  if (GetDataPipe()) // allocate data pipe if not done yet
  {
    os << "\n";
    GetDataPipe()->PrintSelf(os,indent.GetNextIndent());
  }
  else
    os << "NULL\n";

  if (!CurrentData)
  {
    os << indent << "Current Data: NULL\n";
  }
  else
  {
    os << indent << "Current Data: "<< CurrentData->GetTypeName()<<"("<<CurrentData<<")\n";
    GetCurrentData()->PrintSelf(os,indent.GetNextIndent());
  }
}

//-------------------------------------------------------------------------
int mafVMEOutput::SetAuxiliaryRefSys(mafTransform *AuxRefSys, const char *RefSysName, int type)
//-------------------------------------------------------------------------
{
  if (AuxRefSys)
  {
    return SetAuxiliaryRefSys(AuxRefSys->GetMatrix(),RefSysName,type);
  }

  return VTK_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEOutput::SetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type)
//-------------------------------------------------------------------------
{
  if (AuxRefSys)
  {
	  //check for type existence
	  if (type == MFL_LOCAL_FRAME_TAG || type ==MFL_GLOBAL_FRAME_TAG)
	  {
      vtkTagItem item;
	    item.SetName(RefSysName);

	    for ( int i = 0; i < 4; i++)
	    {
		    for ( int j = 0; j < 4; j++)
		    {
		    item.AddComponent(AuxRefSys->GetElement(i,j));
		    }
	    }

	    item.SetType(type);
  
      GetTagArray()->AddTag(item);
	  
	    return VTK_OK;
	  }
  }

	return VTK_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEOutput::GetAuxiliaryRefSys(mafTransform *AuxRefSys, const char *RefSysName, int type)
//-------------------------------------------------------------------------
{
  if (AuxRefSys)
  {
    mafSmartPointer<vtkMatrix4x4> matrix;
    if (GetAuxiliaryRefSys(matrix,RefSysName,type)==0)
    {
      AuxRefSys->SetMatrix(matrix);

      return VTK_OK;
    }
  }

  return VTK_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEOutput::GetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type)
//-------------------------------------------------------------------------
{
	//check for type existence
  if (type == MFL_LOCAL_FRAME_TAG || type ==MFL_GLOBAL_FRAME_TAG)
  {
    if (AuxRefSys)
    {
      if (vtkString::Compare(RefSysName,"Global") == 0)
		  {
        if (GetParent())
        {
          AuxRefSys->DeepCopy(GetParent()->GetAbsMatrix());
        }
        else
        {
          AuxRefSys->Identity();
        }

        return VTK_OK;
      }

		  
      vtkTagItem *item=GetTagArray()->GetTag(RefSysName);
	    
      if (item)
      {
			  if (item->GetStaticType() == type)
			  {			
			      //copy from tag item in frame
					  int item_component = 1;

					  for ( int i = 0; i < 4; i++)
					  {
						  for ( int j = 0; j < 4; j++)
						  {
              
						    AuxRefSys->SetElement(i, j, item->GetComponentAsDouble(item_component));
						    item_component++;
						  }
					  }
          
					  return VTK_OK;
			  }
		  }
      else if (RefSysName == "Default")
      {
        // if not Default reference system was specified return the Local reference system
        // i.e. the identity!
        AuxRefSys->Identity();
        return VTK_OK;
      }
	  }
  }
  return VTK_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEOutput::SetMatrixPipe(mflMatrixPipe *mpipe)
//-------------------------------------------------------------------------
{
  if (mpipe!=m_MatrixPipe)
  {
    if (mpipe==NULL||mpipe->SetVME(this)==VTK_OK)
    { 
      // if we had an observer...
      if (m_MatrixPipe && MatrixUpdateTag)
      {
        m_MatrixPipe->RemoveObserver(MatrixUpdateTag);
        m_MatrixPipe->SetVME(NULL);
        m_MatrixPipe->SetCurrentTime(m_CurrentTime);
      }

      vtkSetObjectBodyMacro(m_MatrixPipe,mflMatrixPipe,mpipe);

      if (mpipe)
      {
        // attach oberver to new pipe (to be substitute with DefaultChannel communication)
        mflAgent::PlugEventSource(mpipe,MatrixUpdateCallback,this,mflMatrixPipe::MatrixUpdateEvent);
        CurrentMatrix=mpipe->GetMatrix();
      }
      else
      {
        // When no Matrix pipe is set, simple provide
        // an identity matrix
        CurrentMatrix=mafSmartPointer<mflMatrix>();
        CurrentMatrix->SetTimeStamp(m_CurrentTime);
      }

      // this forces the the pipe to Update its input and input frame
      if (m_AbsMatrixPipe)
        m_AbsMatrixPipe->SetVME(this);

      InvokeEvent(mafVMEOutput::MatrixPipeChangedEvent);

      return VTK_OK;
    }
    else
    {
      return VTK_ERROR;
    }
  }

  return VTK_OK;
}

//-------------------------------------------------------------------------
void mafVMEOutput::UpdateCurrentData()
//-------------------------------------------------------------------------
{
  // if output data is available force an update
  if (GetCurrentData()) 
  {
    GetCurrentData()->Update();
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetCurrentData(vtkDataSet *data)
//-------------------------------------------------------------------------
{
  if (data!=CurrentData.GetPointer())
  {
    CurrentData=data;
    Modified();
    InvokeEvent(mafVMEOutput::OutputDataChangedEvent); // advise observers data pointer has changed
  }
}

//-------------------------------------------------------------------------
void mafVMEOutput::SetCrypting(int crypting)
//-------------------------------------------------------------------------
{
  if(crypting > 0)
    Crypting = 1;
  else
    Crypting = 0;

  TagArray->SetTag(vtkTagItem("MFL_CRYPT_VME",Crypting));
  Modified();
}

//-------------------------------------------------------------------------
int mafVMEOutput::GetCrypting()
//-------------------------------------------------------------------------
{
  return RestoreNumericFromTag(GetTagArray(),"MFL_CRYPT_VME",Crypting,-1,0);
}

//-------------------------------------------------------------------------
vtkDataSet *mafVMEOutput::GetCurrentData()
//-------------------------------------------------------------------------
{
  if (GetDataPipe()) // allocate data pipe if not done yet
  {
    SetCurrentData(GetDataPipe()->GetOutput());
  }
 
  return CurrentData;
}

//-------------------------------------------------------------------------
int mafVMEOutput::SetDataPipe(mflDataPipe *dpipe)
//-------------------------------------------------------------------------
{
  if (dpipe==m_DataPipe)
    return VTK_OK;

  if (dpipe==NULL||dpipe->SetVME(this)==VTK_OK)
  { 
    // if we had an observer...
    if (m_DataPipe && DataUpdateTag)
    {
      m_DataPipe->RemoveObserver(DataUpdateTag);
      m_DataPipe->SetVME(NULL);
    }

    vtkSetObjectBodyMacro(m_DataPipe,mflDataPipe,dpipe);
    
    if (m_DataPipe)
    {
      //SetCurrentData(m_DataPipe->GetOutput());
      m_DataPipe->SetCurrentTime(m_CurrentTime);
    }
    else
    {
      SetCurrentData(NULL);
    }

    // must plug the event source after calling UpdateCurrentData, since 
    // UpdateCurrentData calls invoke explicitelly the OutputDataChangedEvent,
    // this avoids a double calling.
    if (m_DataPipe)
    {
      mflAgent::PlugEventSource(dpipe,OutputDataUpdateCallback,this,mflDataPipe::OutputUpdateEvent);
    }

    // advise listeners the data pipe has changed
    InvokeEvent(mafVMEOutput::DataPipeChangedEvent);

    return VTK_OK;
  }
  else
  {
    return VTK_ERROR;
  }
}

#endif