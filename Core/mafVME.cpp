/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVME.cpp,v $
  Language:  C++
  Date:      $Date: 2005-02-22 17:13:01 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVME_cxx
#define __mafVME_cxx

#include "mafVME.h"

//#include "mflItemArray.txx"
//#include "mflMatrixVector.h"
//#include "mafVMEItemArray.h"
//#include "mflDataInterpolatorHolder.h"
//#include "mflMatrixInterpolatorHolder.h"
//#include "mflMatrixPipeDirectCinematic.h"
#include "mafOBB.h"
//#include "mflSmartPointer.h"
//#include "mflDefines.h"
//#include "mflTransform.h"
//#include "vtkLinearTransform.h"

#include <map>
#include <assert.h>

//-------------------------------------------------------------------------
mafVME::mafVME()
//-------------------------------------------------------------------------
{
  // Tags used for Observers
	MatrixVector        = mflMatrixVector::New();
  DataArray           = mafVMEItemArray::New();
  DataArray->SetVME(this);

  // Pipes are dynamically created
  m_MatrixPipe          = NULL;
  m_DataPipe            = NULL;
  AbsMatrixPipe       = NULL;

  // if no m_MatrixPipe is given, provide a static pose matrix
  CurrentMatrix       = mflSmartPointer<mflMatrix>();

  CurrentTime         = 0;
  VisibleToTraverse   = 1;
  Crypting            = -1;
}

//-------------------------------------------------------------------------
mafVME::~mafVME()
//-------------------------------------------------------------------------
{
  // advise observers this is being destroyed
  InvokeEvent(mafVME::DestroyEvent,this);

  vtkDEL(MatrixVector);
  vtkDEL(DataArray);

  // Pipes must be destroyed in the right orde
  // to take into consideration dependencies
  if (m_DataPipe)
  {
    RemoveObserver(DataUpdateTag);
    m_DataPipe->SetVME(NULL);
    vtkDEL(m_DataPipe);
  }

  if (AbsMatrixPipe)
  {
    RemoveObserver(AbsMatrixUpdateTag);
    AbsMatrixPipe->SetVME(NULL);
    vtkDEL(AbsMatrixPipe);
  }
    
  if (m_MatrixPipe)
  { 
    RemoveObserver(MatrixUpdateTag);
    vtkDEL(m_MatrixPipe);
  }
}

//------------------------------------------------------------------------------
int mafVME::Initialize()
//------------------------------------------------------------------------------
{
  if (this->Initialized)
    return -1;

  if (this->InternalInitialize() == 0)
  {
    this->Initialized=1;
    return 0;
  }

  return -1;

}

//------------------------------------------------------------------------------
void mafVME::Shutdown()
//------------------------------------------------------------------------------
{
  if (Initialized)
  {
    InternalShutdown();
    Initialized = 0;
  }
}

//-------------------------------------------------------------------------
void mafVME::SetDefaultDataPipe()
//-------------------------------------------------------------------------
{
  // Default Data Pipe
  mflString pipe_name=GetDefaultDataPipe();
  
  if (pipe_name.IsEmpty())
  {
    SetDataPipe(NULL);
    //CurrentData=NULL;
  }
  else
  {
    SetDataPipe((mflDataPipe *)mflCreateInstance(pipe_name));
    if (m_DataPipe)
      m_DataPipe->UnRegister(this); // Factory return an object with RefCount==1
  }
}

//-------------------------------------------------------------------------
const char *mafVME::GetDefaultDataPipe()
//-------------------------------------------------------------------------
{
  return "mflDataInterpolatorHolder";
}

//-------------------------------------------------------------------------
void mafVME::SetDefaultMatrixPipe()
//-------------------------------------------------------------------------
{
  // Default Matrix Pipe
  mflString pipe_name=GetDefaultMatrixPipe();
  if (pipe_name.IsEmpty())
  {
    SetMatrixPipe(NULL);
  }
  else
  {
    SetMatrixPipe((mflMatrixPipe *)mflCreateInstance(pipe_name));
    if (m_MatrixPipe)
      m_MatrixPipe->UnRegister(this);
  }
}

//-------------------------------------------------------------------------
const char *mafVME::GetDefaultMatrixPipe()
//-------------------------------------------------------------------------
{
  return "mflMatrixInterpolatorHolder";
}

//-------------------------------------------------------------------------
mflMatrixPipeDirectCinematic *mafVME::GetAbsMatrixPipe()
//-------------------------------------------------------------------------
{
  if (!AbsMatrixPipe)
  {
    // the matrix pipe for computing the ABS matrix (direct cinematic)
    vtkNEW(AbsMatrixPipe);
    AbsMatrixPipe->SetVME(this);
    AbsMatrixPipe->SetCurrentTime(CurrentTime);

    // TODO: evaluate if this anymore necessary 
    AbsMatrixUpdateTag= mflAgent::PlugEventSource(this->AbsMatrixPipe,this->AbsMatrixUpdateCallback,this,mflMatrixPipe::MatrixUpdateEvent);
  }

  return AbsMatrixPipe;
}

//-------------------------------------------------------------------------
const char *mafVME::GetDataType()
//-------------------------------------------------------------------------
{
  vtkDataSet *data=this->GetOutput();  
  return data?data->GetTypeName():NULL;
}

//-------------------------------------------------------------------------
mafVME *mafVME::MakeCopy(mafVME *a)
//-------------------------------------------------------------------------
{
  mafVME* newvme=a->NewInstance();
  newvme->DeepCopy(a);
  return newvme;
}

//-------------------------------------------------------------------------
int mafVME::DeepCopy(mafVME *a)
//-------------------------------------------------------------------------
{  
  if (this->CanCopy(a))
  {
    // Copy arrays.
		if (this->TagArray)
			this->TagArray->DeepCopy(a->GetTagArray());

    this->DataArray->DeepCopy(a->GetDataArray());

    this->MatrixVector->DeepCopy(a->MatrixVector);

    this->SetMatrixPipe(a->GetMatrixPipe()?a->GetMatrixPipe()->MakeACopy():NULL);
    this->SetDataPipe(a->GetDataPipe()?a->GetDataPipe()->MakeACopy():NULL);

    // attributes
    this->SetName(a->GetName());

    // Runtime properties
    //this->AutoUpdateAbsMatrix=a->GetAutoUpdateAbsMatrix();
    this->SetCurrentTime(a->GetCurrentTime());

    return VTK_OK;
  }
  else
  {
    vtkErrorMacro("Cannot copy VME of type "<<a->GetTypeName()<<" into a VME \
    VME of type "<<this->GetTypeName());

    return VTK_ERROR;
  }
}

//-------------------------------------------------------------------------
int mafVME::ShallowCopy(mafVME *a)
//-------------------------------------------------------------------------
{  
  if (this->CanCopy(a))
  {
    // Copy tag & matrix array.
    if (this->TagArray)
			this->TagArray->DeepCopy(a->GetTagArray());

    MatrixVector->DeepCopy(a->GetMatrixVector());    

    // shallow copy data array
    DataArray->ShallowCopy(a->GetDataArray());

    

    this->SetMatrixPipe(a->GetMatrixPipe()?a->GetMatrixPipe()->MakeACopy():NULL);
    this->SetDataPipe(a->GetDataPipe()?a->GetDataPipe()->MakeACopy():NULL);

    // attributes
    this->SetName(a->GetName());

    // Runtime properties
    //this->AutoUpdateAbsMatrix=a->GetAutoUpdateAbsMatrix();
    this->SetCurrentTime(a->GetCurrentTime());

    return VTK_OK;
  }
  else
  {
    vtkErrorMacro("Cannot copy VME of type "<<a->GetTypeName()<<" into a VME \
    VME of type "<<this->GetTypeName());

    return VTK_ERROR;
  }
}

//-------------------------------------------------------------------------
bool mafVME::CanCopy(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (!vme)
    return false;

  if (vme->IsA(this->GetTypeName()))
  {
    return true;
  }

  return false;
}

//-------------------------------------------------------------------------
int mafVME::GetItem(int id,mafVMEItem *&item)
//-------------------------------------------------------------------------
{
  return this->DataArray->GetItemByIndex(id,item);
}

//-------------------------------------------------------------------------
mafVMEItem *mafVME::GetItem(int id)
//-------------------------------------------------------------------------
{
  mafVMEItem *item;
  return (this->GetItem(id,item)==VTK_OK?item:NULL);
}

//-------------------------------------------------------------------------
int mafVME::AddItem(mafVMEItem *item)
//-------------------------------------------------------------------------
{
  if (item)
  {
    return this->DataArray->SetItem(item);
  }

  return VTK_ERROR;
}
  

//-------------------------------------------------------------------------
int mafVME::RemoveItem(int id)
//-------------------------------------------------------------------------
{
  return this->DataArray->RemoveItem(id);
}

//-------------------------------------------------------------------------
int mafVME::RemoveItem(mafVMEItem *item)
//-------------------------------------------------------------------------
{
  int idx;

  idx=this->DataArray->FindItem(item);
  if (idx>=0)
  {
    return this->DataArray->RemoveItem(idx);
  }
  else
  {
    return VTK_ERROR;
  }
}

//-------------------------------------------------------------------------
void mafVME::RemoveAllItems()
//-------------------------------------------------------------------------
{
  this->DataArray->RemoveAllItems();
}

//-------------------------------------------------------------------------
int mafVME::GetNumberOfItems()
//-------------------------------------------------------------------------
{
  return this->DataArray->GetNumberOfItems();
}

/*//------------------------------------------------------------------------------
int mafVME::SetParent(vtkTree *parent)
//-------------------------------------------------------------------------
{
  if (mafVME *parent_vme=mafVME::SafeDownCast(parent))
  {
    if (this->CanReparentTo(parent_vme)==VTK_OK)
    {  
      this->Superclass::SetParent(parent_vme);

      if (parent_vme==NULL)
      {
        this->InvokeEvent(mafVME::DetachFromTreeEvent,this);
      }
      else
      {
        this->InvokeEvent(mafVME::AttachToTreeEvent,this);
      }

      // this forces the the pipe to Update its input and input frame
      if (AbsMatrixPipe)
        this->AbsMatrixPipe->SetVME(this);

      return VTK_OK;
    }

    //modified by Stefano 27-10-2004 (beg)
    //Changed the error macro to give feedback about vme names
    
    vtkErrorMacro("Cannot reparent the VME: " << this->GetName() << " under the "<<parent->GetTypeName() 
      << " named " << parent_vme->GetName());
    //modified by Stefano 27-10-2004 (end)
  }
  else
  {
    // reparenting to NULL is admitted
    if (parent==NULL)
    {
      this->Superclass::SetParent(parent);
      return VTK_OK;
    }
  }

  return VTK_ERROR;
}
*/
//-------------------------------------------------------------------------
/*void mafVME::UnRegister(vtkObjectBase *a)
//-------------------------------------------------------------------------
{
  // If this object is still referenced only by its children
  // and items the object is deleted, removing all children and
  // items
  if (this->ReferenceCount==(this->GetNumberOfChildren()+this->GetNumberOfItems()))
  {
    
    // if the unregistering object is an Item simply return
    if (mafVMEItem *b=mafVMEItem::SafeDownCast(a))
    {
      vtkIdType ret;
      if (this->DataArray->FindItem(b))
        return;    
    }
   
    // if the unregister object is 
    if (mafVME *b=mafVME::SafeDownCast(a))
    {
      if (this->IsAChild(b))
        return;
    }

    this->RemoveAllNodes();
    this->RemoveAllItems();
  }
  this->vtkObject::UnRegister(this);
}
*/
//-------------------------------------------------------------------------
/*void mafVME::UnRegister(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  vtkIdType ret;
  this->DataArray->FindItem(a,ret);
  if (ret==VTK_OK)
  {
    this->vtkObject::UnRegister(a);
  }
  
  this->UnRegister((vtkObject *)a);
  
}
*/

//-------------------------------------------------------------------------
int mafVME::FindItem(mflTimeStamp t,mafVMEItem *&item)
//-------------------------------------------------------------------------
{
  
  return this->DataArray->FindItem(t,item);

}

//-------------------------------------------------------------------------
mafVME *mafVME::FindInTree(const char *name,const char *value,int type)
//-------------------------------------------------------------------------
{
  vtkTagArray *tarray=this->GetTagArray();

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
  
  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    mafVME *curr=this->GetChild(i);
    
    if (mafVME *ret=curr->FindInTree(name,value,type))
      return ret;
  }

  return NULL;
}

//-------------------------------------------------------------------------
mafVME *mafVME::FindInTreeByName(const char *name)
//-------------------------------------------------------------------------
{
  if (this->Name==name)
  {
     return this;
  }
  
  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    mafVME *curr=this->GetChild(i);
    
    if (mafVME *ret=curr->FindInTreeByName(name))
      return ret;
  }

  return NULL;
}

//-------------------------------------------------------------------------
int mafVME::FindChildIdx(const char *name)
//-------------------------------------------------------------------------
{
  if (name)
  {
    for (int i=0;i<this->GetNumberOfChildren();i++)
    {
      mafVME *vme=this->GetChild(i);
      if (vme&&vtkString::Equals(vme->GetName(),name))
      {
        return i;
      }
    }
  }

  return -1;
}

//-------------------------------------------------------------------------
void mafVME::UpdateData(int idx)
//-------------------------------------------------------------------------
{
  mafVMEItem *item=this->GetItem(idx);
  if (item)
    item->UpdateData();
}


//-------------------------------------------------------------------------
void mafVME::UpdateAllData()
//-------------------------------------------------------------------------
{
  // update all the items in this VME. Does not
  // propagate in the sub tree since to do this 
  // we must use a VMEIterator, specifying the 
  // traversing modality
  for (int i=0;i<this->GetNumberOfItems();i++)
  {
    this->GetItem(i)->UpdateData();
  }
}

//-------------------------------------------------------------------------
// TODO: to be moved to mflMatrixPipeDirectCinematic
/*void mafVME::UpdateAbsMatrix(mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=t<0?this->CurrentTime:t;

  mflMatrix *absmat=this->AbsMatrix;
  
  mafVME *parent=this->GetParent();
  mflMatrix *pmat=NULL;

  if (parent)
  {
    //parent->UpdateAbsMatrix(t);
    pmat=parent->GetAbsMatrix(t);
  }
    
  
  if (absmat==NULL||(absmat->GetTimeStamp()!=t)|| \
    (absmat->GetMTime()<this->MatrixVector->GetMTime())|| \
  (pmat&&absmat->GetMTime()<pmat->GetMTime()))
  {
    if (absmat==NULL)
    {
      this->AbsMatrix=mflMatrix::New();
    }

    //DEBUG
    //vtkTransform *trans=vtkTransform::New();
    //double rxyz[3];
    //trans->PreMultiply();

    mflMatrix *mat=this->GetVMatrix()->GetMatrix(t);

    if (pmat)
    {
    
        this->AbsMatrix->DeepCopy(pmat);
        
        //DEBUG
        //trans->SetMatrix(pmat); 
        //trans->GetOrientation(rxyz);
       

    }
    else
    {
      this->AbsMatrix->Identity();
    }

    //trans->Concatenate(mat);
    //this->AbsMatrix->DeepCopy(trans->GetMatrix());
    //this->AbsMatrix->SetTimeStamp(this->GetCurrentTime());
    //trans->Delete();

    vtkMatrix4x4::Multiply4x4(this->AbsMatrix, mat,this->AbsMatrix);

    //DEBUG
    //trans->SetMatrix(this->AbsMatrix);    
    //trans->GetOrientation(rxyz);

    //trans->Delete();

    this->AbsMatrix->SetTimeStamp(t);
  }
}
*/

//-------------------------------------------------------------------------
void mafVME::UpdateData(mafVMEItem *item)
//-------------------------------------------------------------------------
{
  if (item)
  {
    if (this->GetParent())
    {
      // propagate the update event up to the root
      this->GetParent()->UpdateData(item);
    }
    // if this node hasn't a parent, since it's not
    // a root we can't do anything: this is an orphan
    // node...
  }
}

//-------------------------------------------------------------------------
// TODO: to be rewritten
void mafVME::SetCurrentTime(mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (t<0)
    t=0;

  this->CurrentTime=t;

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

  if (AbsMatrixPipe)
    this->AbsMatrixPipe->SetCurrentTime(t);

  this->Modified();

  // TODO: consider if to add a flag to diable event issuing
  this->InvokeEvent(mafVME::TimeEvent,this);
  
}

//-------------------------------------------------------------------------
void mafVME::SetTreeTime(mflTimeStamp t)
//-------------------------------------------------------------------------
{
  this->SetCurrentTime(t);

  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    this->GetChild(i)->SetTreeTime(t);
  }
}

//-------------------------------------------------------------------------
mafVMEIterator *mafVME::NewIterator()
//-------------------------------------------------------------------------
{
  mafVMEIterator *iter=mafVMEIterator::New();
  iter->SetRootNode(this);
  return iter;
}

//-------------------------------------------------------------------------
int mafVME::IsAnimated()
//-------------------------------------------------------------------------
{
  return ((this->GetNumberOfItems()>1)||(this->GetMatrixVector()->GetNumberOfMatrixes()>1));
}


//-------------------------------------------------------------------------
void mafVME::GetDataTimeBounds(mflTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  tbounds[0]=-1;
  tbounds[1]=-1;

  // find the time interval for the items in this VME
  for (int i=0;i<this->GetNumberOfItems();i++)
  {
    mafVMEItem *item=this->GetItem(i);
    if (item->GetTimeStamp()<tbounds[0]||tbounds[0]<0)
      tbounds[0]=item->GetTimeStamp();
    if (item->GetTimeStamp()>tbounds[1]||tbounds[1]<0)
      tbounds[1]=item->GetTimeStamp();
  }
}

//-------------------------------------------------------------------------
int mafVME::GetNumberOfLocalTimeStamps()
//-------------------------------------------------------------------------
{
  TimeVector timestamps;
  this->GetLocalTimeStamps(timestamps);
  return timestamps.size();
}

//-------------------------------------------------------------------------
int mafVME::GetNumberOfTimeStamps()
//-------------------------------------------------------------------------
{
  TimeVector timestamps;
  this->GetTimeStamps(timestamps);
  return timestamps.size();
}

//-------------------------------------------------------------------------
void mafVME::GetLocalTimeBounds(mflTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  tbounds[0]=-1;
  tbounds[1]=-1;

  this->DataArray->GetTimeBounds(tbounds);
  
  mflTimeStamp tmp[2];
  // does the same this with the matrix vector
  this->MatrixVector->GetTimeBounds(tmp);

  if (tmp[0]<0 || tmp[1]<0)
      return;

  if (tmp[0]<tbounds[0]||tbounds[0]<0)
      tbounds[0]=tmp[0];

  if (tmp[1]>tbounds[1]||tbounds[1]<0)
      tbounds[1]=tmp[1];

}

//-------------------------------------------------------------------------
void mafVME::GetTimeBounds(mflTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  this->GetLocalTimeBounds(tbounds);

  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    mflTimeStamp tmp[2];

    this->GetChild(i)->GetTimeBounds(tmp);
    if (tmp[0]<0 || tmp[1]<0)
      continue;

    if (tmp[0]<tbounds[0]||tbounds[0]<0)
      tbounds[0]=tmp[0];

    if (tmp[1]>tbounds[1]||tbounds[1]<0)
      tbounds[1]=tmp[1];
  }
}

//-------------------------------------------------------------------------
void mafVME::GetDataTimeStamps(mflTimeStamp *&kframes)
//-------------------------------------------------------------------------
{
  this->DataArray->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void mafVME::GetDataTimeStamps(TimeVector &kframes)
//-------------------------------------------------------------------------
{
  this->DataArray->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void mafVME::GetMatrixTimeStamps(mflTimeStamp *&kframes)
//-------------------------------------------------------------------------
{
  this->MatrixVector->GetTimeStamps(kframes);
}



//-------------------------------------------------------------------------
void mafVME::GetMatrixTimeStamps(TimeVector &kframes)
//-------------------------------------------------------------------------
{
  this->MatrixVector->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void mafVME::GetLocalTimeStamps(mflTimeStamp *&kframes)
//-------------------------------------------------------------------------
{ 
  TimeVector frames;

  this->GetLocalTimeStamps(frames);
  
  kframes=new mflTimeStamp[frames.size()];

  for (int i=0;i<frames.size();i++)
  {
    kframes[i]=frames[i];
  } 
}

//-------------------------------------------------------------------------
void mafVME::GetLocalTimeStamps(std::vector<mflTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();

  std::vector<mflTimeStamp> datatimestamps;
  std::vector<mflTimeStamp> matrixtimestamps;
  
  this->DataArray->GetTimeStamps(datatimestamps);
  this->MatrixVector->GetTimeStamps(matrixtimestamps);

  this->MergeTimeVectors(kframes,datatimestamps,matrixtimestamps);
}

//-------------------------------------------------------------------------
void mafVME::GetLocalTimeStamps(vtkDoubleArray *kframes)
//-------------------------------------------------------------------------
{
  assert(kframes);
  TimeVector frames;
  this->GetLocalTimeStamps(frames);
  kframes->SetNumberOfComponents(1);
  kframes->SetNumberOfTuples(frames.size());
  for (int i=0;i<frames.size();i++)
  {
    kframes->SetTuple(i,&(frames[i]));
  }
}

//-------------------------------------------------------------------------
void mafVME::MergeTimeVectors(std::vector<mflTimeStamp> &outv,std::vector<mflTimeStamp> v1,std::vector<mflTimeStamp>v2)
//-------------------------------------------------------------------------
{
  typedef std::map<mflTimeStamp,mflTimeStamp> TimeMap;

  TimeMap mergedlist;
  
  int pos;

  for (pos=0;pos<v1.size();pos++)
  {
    mergedlist[v1[pos]]=v1[pos];
  }
  
  for (pos=0;pos<v2.size();pos++)
  {
    mergedlist[v2[pos]]=v2[pos];
  }

  outv.resize(mergedlist.size());


  int i=0;
  for (TimeMap::iterator ptr=mergedlist.begin();ptr!=mergedlist.end();ptr++)
  {
    outv[i]=ptr->second;
    i++;
  }
}

//-------------------------------------------------------------------------
void mafVME::GetTimeStamps(mflTimeStamp *&kframes)
//-------------------------------------------------------------------------
{
  std::vector<mflTimeStamp> frames;

  this->GetTimeStamps(frames);

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
void mafVME::GetTimeStamps(TimeVector &kframes)
//-------------------------------------------------------------------------
{
  this->GetLocalTimeStamps(kframes);
  
  std::vector<mflTimeStamp> subKFrames;

  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    this->GetChild(i)->GetTimeStamps(subKFrames);

    this->MergeTimeVectors(kframes,kframes,subKFrames);
  }
}

//-------------------------------------------------------------------------
void mafVME::GetTimeStamps(vtkDoubleArray *kframes)
//-------------------------------------------------------------------------
{
  assert(kframes);
  TimeVector frames;
  this->GetTimeStamps(frames);
  kframes->SetNumberOfComponents(1);
  kframes->SetNumberOfTuples(frames.size());
  for (int i=0;i<frames.size();i++)
  {
    kframes->SetTuple(i,&(frames[i]));
  }
}

//-------------------------------------------------------------------------
void mafVME::GetAbsTimeStamps(mflTimeStamp *&kframes)
{
  std::vector<mflTimeStamp> frames;

  this->GetAbsTimeStamps(frames);

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
void mafVME::GetAbsTimeStamps(TimeVector &kframes)
{
  this->GetLocalTimeStamps(kframes);
  
  std::vector<mflTimeStamp> parentKFrames;

  for (mafVME *parent=this->GetParent();parent;parent=parent->GetParent())
  {

    parent->GetLocalTimeStamps(parentKFrames);

    this->MergeTimeVectors(kframes,kframes,parentKFrames);
  }
}


//-------------------------------------------------------------------------
void mafVME::GetVME4DBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  this->GetVME4DBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVME::GetVME4DBounds(mafOBB &bounds)
//-------------------------------------------------------------------------
{
  std::vector<mflTimeStamp> timestamps;

  this->GetTimeStamps(timestamps);

  bounds.Reset();

  if (GetDataPipe()) // allocate data pipe if not done yet
  {
    // THD SAFE implementation
    //mflDataPipe *datapipe=this->m_DataPipe->MakeACopy();

    mflSmartPointer<mflMatrix> itemAbsPose;
    mafOBB transformed_bounds;

    for (int i=0;i<timestamps.size();i++)
    {
      // THD SAFE implementation
      /*
      datapipe->SetCurrentTime(timestamps[i]);
      datapipe->UpdateCurrentBounds();
      mafOBB *itemBounds=datapipe->GetCurrentBounds();
      */

      this->m_DataPipe->SetCurrentTime(timestamps[i]);
      this->m_DataPipe->UpdateCurrentBounds();

      // must make a copy, otherwise I would transform the bounds inside the data pipe
      transformed_bounds.DeepCopy(this->m_DataPipe->GetCurrentBounds());
      this->GetAbsMatrix(itemAbsPose,timestamps[i]);
    
      transformed_bounds.ApplyTransform(itemAbsPose);

      bounds.MergeBounds(transformed_bounds);
    }

    this->m_DataPipe->SetCurrentTime(this->CurrentTime);
    this->m_DataPipe->UpdateCurrentBounds();
  }
  else if (CurrentData.GetPointer())
  {
    bounds.DeepCopy(CurrentData->GetBounds());
  }

}

//-------------------------------------------------------------------------
//void mafVME::GetLocal4DBounds(mflTimeStamp start, mflTimeStamp end, double bounds[6])
//{
//}

//-------------------------------------------------------------------------
//void mafVME::Get4DBounds(mflTimeStamp start, mflTimeStamp end, double bounds[6])
//{
//}

//-------------------------------------------------------------------------
void mafVME::GetVMESpaceBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  this->GetVMESpaceBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVME::GetVMESpaceBounds(mafOBB &bounds,mflTimeStamp t, mafVMEIterator *iter)
//-------------------------------------------------------------------------
{ 
  if ((iter&&iter->IsVisible(this))||this->IsVisible())
  {
    mflSmartPointer<mflMatrix> itemPose;
    
    if (t<0)
    {
      t=this->CurrentTime;
      itemPose=this->GetAbsMatrix();
    }
    else
    {
      this->GetAbsMatrix(itemPose,t);
    }
    
    this->GetVMELocalSpaceBounds(bounds,t,iter);  

    bounds.ApplyTransform(itemPose);
  }
}

//-------------------------------------------------------------------------
void mafVME::GetVMELocalSpaceBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  this->GetVMELocalSpaceBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVME::GetVMELocalSpaceBounds(mafOBB &bounds,mflTimeStamp t, mafVMEIterator *iter)
//-------------------------------------------------------------------------
{
   if (t<0)
    t=this->CurrentTime;

  bounds.Reset();

  if ((iter&&iter->IsVisible(this))||this->IsVisible())
  {
    
    if (GetDataPipe()) // allocate data pipe if not done yet
    {
      // We call directly the m_DataPipe UpdateBounds since this doesn't update
      // the output data, which is not necessary.
      // Must call explicitelly UpdateCurrentBounds() method, since datapipes
      // do not update automatically when time changes  
      this->m_DataPipe->SetCurrentTime(t);
      this->m_DataPipe->UpdateCurrentBounds();
      bounds.DeepCopy(this->m_DataPipe->GetCurrentBounds());

      // restore the right bounds for current time... 
      // TODO: modify the GetCurrentBounds to make it call UpdateCurentBounds explicitelly!
      this->m_DataPipe->SetCurrentTime(this->CurrentTime);
      this->m_DataPipe->UpdateCurrentBounds();
    
      // this is a thread safe implemetation
      /*mflDataPipe *datapipe=this->m_DataPipe->MakeACopy();
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
void mafVME::GetSpaceBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  this->GetSpaceBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVME::GetSpaceBounds(mafOBB &bounds,mflTimeStamp t, mafVMEIterator *iter)
//-------------------------------------------------------------------------
{
  if (t<0)
    t=this->CurrentTime;

  this->GetVMESpaceBounds(bounds,t,iter);
  
  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    mafVME *child=this->GetChild(i);
    mafOBB childBounds;

    child->GetSpaceBounds(childBounds,t);

    bounds.MergeBounds(childBounds);
  }
}

//-------------------------------------------------------------------------
void mafVME::Get4DBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mafOBB myBounds;
  this->Get4DBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVME::Get4DBounds(mafOBB &bounds)
//-------------------------------------------------------------------------
{

  std::vector<mflTimeStamp> timestamps;

  bounds.Reset();

  this->GetTimeStamps(timestamps);

  mafOBB frameBounds;
  for (int i=0;i<timestamps.size();i++)
  {
    this->GetSpaceBounds(frameBounds,timestamps[i]);
    bounds.MergeBounds(frameBounds); 
  }

}

//-------------------------------------------------------------------------
bool mafVME::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (!vme||!vme->IsA(this->GetTypeName()))
    return false;

  if (this->GetNumberOfItems()!=vme->GetNumberOfItems())
  {
    return false;
  }

  for (int i=0;i<this->GetNumberOfItems();i++)
  {
    if (!this->GetItem(i)->Equals(vme->GetItem(i)))
    {
      return false;
    }
  }

  if (!this->GetTagArray()->Equals(vme->GetTagArray()))
    return false;

  if (!this->GetMatrixVector()->Equals(vme->GetMatrixVector()))
    return false;

  if (!this->Name.Equals(vme->GetName()))
    return false;

  return true;
}

//-------------------------------------------------------------------------
bool mafVME::CompareTree(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (!this->Equals(vme))
    return false;

  if (vme->GetNumberOfChildren()!=this->GetNumberOfChildren())
  {
    return false;
  }

  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    if (!this->GetChild(i)->CompareTree(vme->GetChild(i)))
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------
mafVME *mafVME::CopyTree(mafVME *vme, mafVME *parent)
//-------------------------------------------------------------------------
{
  
  mafVME* v = vme->MakeCopy();

  v->ReparentTo(parent);

  if (parent)
  {
    v->Delete();
  }

  for(int i=0; i<vme->GetNumberOfChildren(); i++)
  {
    if (mafVME *child=vme->GetChild(i))
      if (child->IsVisible())
        mafVME::CopyTree(child,v);
  }

  return v;
}

//-------------------------------------------------------------------------
mafVME *mafVME::ReparentTo(mafVME *newparent)
//-------------------------------------------------------------------------
{
  // We cannot reparent to a subnode!!!
  if (!this->IsInTree(newparent))
  {
    // When we reparent to a different tree, or we simply
    // cut a tree, pre travers the sub tree to read data into memory
    // future release should read one item at once, write it
    // to disk and then release the data, or better simply copy the file
    // into the new place, this to be able to manage HUGE datasets.
    if (newparent==NULL||this->GetRoot()!=newparent->GetRoot())
    {
      mafVMEIterator *iter=this->NewIterator();
      for (mafVME *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
      {
        for (int i=0;i<vme->GetNumberOfItems();i++)
        {
          mafVMEItem *item=vme->GetItem(i);
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
    
    mafVME *oldparent=this->GetParent();

    this->Register(this);

    if (newparent)
    {
      if (newparent->AddChild(this)==VTK_ERROR)
      {
        vtkErrorMacro("Cannot Reparent node "<<this->GetName()<<" to node "<<newparent->GetName());
        return NULL;
      }

      this->SetCurrentTime(newparent->GetCurrentTime()); // update data & pose to parent CurrentTime
    }
    else
    {
      if (this->SetParent(NULL)==VTK_ERROR)
      {
        vtkErrorMacro("Cannot Reparent node "<<this->GetName()<<" to NULL");
        return NULL;
      }
    }

    if (this->AbsMatrixPipe)
    {
      this->AbsMatrixPipe->SetVME(this);
      this->AbsMatrixPipe->Update();
    }

    if (oldparent)
    {
      oldparent->RemoveChild(this);
    }

    mafVME *ret=(this->ReferenceCount==1)?NULL:this;
    
    this->UnRegister(this);

    return ret;
  }
  else
  {
    return NULL;
  }
}


//----------------------------------------------------------------------------
void mafVME::Import(mafVME *tree)
//-------------------------------------------------------------------------
{
  if (tree&&tree->GetNumberOfChildren()>0)
  {
    int num=tree->GetNumberOfChildren();
    for (int i=0;i<num;i++)
    {
      mafVME *vme=tree->GetFirstChild();
      vme->ReparentTo(this);
    }
  }
}

//-------------------------------------------------------------------------
mafVMEItem *mafVME::GetItemByTimeStamp(mflTimeStamp t)
//-------------------------------------------------------------------------
{
  return this->DataArray->GetItemByTimeStamp(t);
}

//-------------------------------------------------------------------------
void mafVME::SetPose(mflMatrix *mat)
//-------------------------------------------------------------------------
{
  this->GetMatrixVector()->SetMatrix(mat);
}

//-------------------------------------------------------------------------
void mafVME::SetPose(vtkMatrix4x4 *mat, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?t=this->CurrentTime:t;
  if (this->MatrixVector)
    this->GetMatrixVector()->SetMatrix(mat,t);
  else
    this->CurrentMatrix->DeepCopy(mat);
}

//-------------------------------------------------------------------------
void mafVME::SetPose(double x,double y,double z,double rx,double ry,double rz, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3],trxyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  this->SetPose(txyz,trxyz,t);
}

//-------------------------------------------------------------------------
void mafVME::SetPose(double xyz[3],double rxyz[3], mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?t=this->CurrentTime:t;

  mflSmartPointer<mflMatrix> matrix;

  mflTransform::SetOrientation(matrix,rxyz);

  mflTransform::SetPosition(matrix,xyz);

  matrix->SetTimeStamp(t);
  
  if (this->MatrixVector)
    this->MatrixVector->SetMatrix(matrix);
  else
    this->CurrentMatrix->DeepCopy(matrix);
}

//----------------------------------------------------------------------------
void mafVME::ApplyTransform(vtkLinearTransform *transform,int premultiply,mflTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?this->CurrentTime:t;
  mflSmartPointer<mflTransform> new_pose;
  mflSmartPointer<mflMatrix> pose;
  GetMatrix(pose,t);
  new_pose->SetMatrix(pose);
  new_pose->Concatenate(transform,premultiply);
  SetPose(new_pose->GetMatrix(),t);
}
//----------------------------------------------------------------------------
void mafVME::ApplyMatrix(vtkMatrix4x4 *matrix,int premultiply,mflTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?this->CurrentTime:t;
  mflSmartPointer<mflTransform> new_pose;
  mflSmartPointer<mflMatrix> pose;
  GetMatrix(pose,t);
  new_pose->SetMatrix(pose);
  new_pose->Concatenate(matrix,premultiply);
  SetPose(new_pose->GetMatrix(),t);
}

//-------------------------------------------------------------------------
void mafVME::SetPosition(double x,double y,double z, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  this->SetPosition(txyz,t);
}

//-------------------------------------------------------------------------
void mafVME::SetPosition(double xyz[3], mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mflSmartPointer<mflMatrix> matrix;
  
  mflTransform::SetPosition(matrix,xyz);

  mflMatrix *oldpose;
  this->MatrixVector->FindMatrix(t,oldpose);

  // recover the orientation only if a key matrix is present
  if (oldpose)
    mflTransform::CopyRotation(matrix,oldpose);

  matrix->SetTimeStamp(t);
  
  if (this->MatrixVector)
    this->MatrixVector->SetMatrix(matrix);
  else
    this->CurrentMatrix->DeepCopy(matrix);
}

//-------------------------------------------------------------------------
void mafVME::SetOrientation(double rx,double ry,double rz, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double trxyz[3];
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  this->SetOrientation(trxyz,t);
}

//-------------------------------------------------------------------------
void mafVME::SetOrientation(double rxyz[3], mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mflSmartPointer<mflMatrix> matrix;

  mflMatrix *oldpose;
  this->MatrixVector->FindMatrix(t,oldpose);
    
  mflTransform::SetOrientation(matrix,rxyz);
  
  // recover the position only if a key matrix is present
  if (oldpose)
  {
    double pos[3];
    mflTransform::GetPosition(oldpose,pos);
    mflTransform::SetPosition(matrix,pos);
  }

  matrix->SetTimeStamp(t);

  if (this->MatrixVector)
    this->MatrixVector->SetMatrix(matrix);
  else
    this->CurrentMatrix->DeepCopy(matrix);
}

//-------------------------------------------------------------------------
mflMatrix *mafVME::GetPose()
//-------------------------------------------------------------------------
{
  return (this->GetMatrixPipe())?this->m_MatrixPipe->GetMatrix():this->CurrentMatrix;
}

//-------------------------------------------------------------------------
void mafVME::GetPose(mflMatrix *matrix,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (matrix)
  {
    if (GetMatrixPipe()) // allocate matrix pipe if not done yet
    {
      if (t<0||t==this->CurrentTime)
      {
        matrix->DeepCopy(this->GetPose());
      }
      else
      {
        // disable rising of update event since this is
        // only a temporary change to the matrix
        int old_flag=this->m_MatrixPipe->GetUpdateMatrixObserverFlag();
        this->m_MatrixPipe->UpdateMatrixObserverOff();
        this->m_MatrixPipe->SetCurrentTime(t);
        matrix->DeepCopy(this->m_MatrixPipe->GetMatrix());
        // restore right time
        this->m_MatrixPipe->SetCurrentTime(this->CurrentTime);
        this->m_MatrixPipe->SetUpdateMatrixObserverFlag(old_flag);

        // THD SAFE implementation
        // create a temporary pipe to interpolate at time t
        /*mflMatrixPipe *newpipe=this->m_MatrixPipe->MakeACopy();
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
void mafVME::GetPose(double xyz[3],double rxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mflSmartPointer<mflMatrix> mat;
  
  this->GetMatrix(mat,t);
  
  mflTransform::GetOrientation(mat,rxyz);
  
  mflTransform::GetPosition(mat,xyz);
}

//-------------------------------------------------------------------------
void mafVME::GetPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double xyz[3],rxyz[3];

  this->GetPose(xyz,rxyz,t);

  x=xyz[0];
  y=xyz[1];
  z=xyz[2];

  rx=rxyz[0];
  ry=rxyz[1];
  rz=rxyz[2];
}

//-------------------------------------------------------------------------
void mafVME::GetPosition(double &x,double &y,double &z,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double xyz[3];
  this->GetPosition(xyz,t);
  x=xyz[0];
  y=xyz[1];
  z=xyz[2];

}

//-------------------------------------------------------------------------
void mafVME::GetPosition(double xyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (t<0||t==this->CurrentTime)
  {
    mflTransform::GetPosition(this->GetMatrix(),xyz); 
  }
  else
  {
    mflSmartPointer<mflMatrix> mat;
    this->GetMatrix(mat,t);
    mflTransform::GetPosition(mat,xyz); 
  }  
}

//-------------------------------------------------------------------------
void mafVME::GetOrientation(double &rx,double &ry,double &rz,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double rxyz[3];
  this->GetOrientation(rxyz,t);
  rx=rxyz[0];
  ry=rxyz[1];
  rz=rxyz[2];
}

//-------------------------------------------------------------------------
void mafVME::GetOrientation(double rxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (t<0||t==this->CurrentTime)
  {
    mflTransform::GetOrientation(this->GetMatrix(),rxyz);
  }
  else
  {
    mflSmartPointer<mflMatrix> mat;
    this->GetMatrix(mat,t);
    mflTransform::GetOrientation(mat,rxyz);
  }  
}

//-------------------------------------------------------------------------
void mafVME::GetAbsPose(mflMatrix *matrix,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (matrix)
  {
    if (t<0||t==this->CurrentTime)
    {
      matrix->DeepCopy(this->GetAbsPose());
    }
    else
    {
      mflMatrixPipeDirectCinematic *abspipe=GetAbsMatrixPipe();
      // disable rising of update event since this is
      // only a temporary change to the matrix
      int old_flag=abspipe->GetUpdateMatrixObserverFlag();
      abspipe->UpdateMatrixObserverOff();
      abspipe->SetCurrentTime(t);
      matrix->DeepCopy(abspipe->GetMatrix());
      
      // restore right time
      abspipe->SetCurrentTime(this->CurrentTime);
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
mflMatrix *mafVME::GetAbsPose()
//-------------------------------------------------------------------------
{
  return this->GetAbsMatrixPipe()->GetMatrix();
}

//-------------------------------------------------------------------------
void mafVME::GetAbsPose(double xyz[3],double rxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mflSmartPointer<mflMatrix> mat;
  this->GetAbsMatrix(mat,t);

  mflTransform::GetPosition(mat,xyz);

  mflTransform::GetOrientation(mat,rxyz);
}

//-------------------------------------------------------------------------
void mafVME::GetAbsPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double xyz[3],rxyz[3];

  this->GetAbsPose(xyz,rxyz,t);

  x=xyz[0];
  y=xyz[1];
  z=xyz[2];

  rx=rxyz[0];
  ry=rxyz[1];
  rz=rxyz[2];
}

//-------------------------------------------------------------------------
void mafVME::GetAbsPosition(double xyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (t<0||t==this->CurrentTime)
  {
    mflTransform::GetPosition(this->GetAbsMatrix(),xyz);
  }
  else
  {
    mflSmartPointer<mflMatrix> mat;
    this->GetAbsMatrix(mat,t);
    mflTransform::GetPosition(mat,xyz);
  }
  
}

//-------------------------------------------------------------------------
void mafVME::GetAbsPosition(double &x,double &y,double &z,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double xyz[3];

  this->GetAbsPosition(xyz,t);

  x=xyz[0];
  y=xyz[1];
  z=xyz[2];
}

//-------------------------------------------------------------------------
void mafVME::GetAbsOrientation(double rxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (t<0||t==this->CurrentTime)
  {
    mflTransform::GetOrientation(this->GetAbsMatrix(),rxyz);
  }
  else
  {
    mflSmartPointer<mflMatrix> mat;
    this->GetAbsMatrix(mat,t);
    mflTransform::GetOrientation(mat,rxyz);
  }
}

//-------------------------------------------------------------------------
void mafVME::GetAbsOrientation(double &rx,double &ry,double &rz,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double rxyz[3];

  this->GetAbsOrientation(rxyz,t);

  rx=rxyz[0];
  ry=rxyz[1];
  rz=rxyz[2];
}

//-------------------------------------------------------------------------
void mafVME::SetAbsPose(double x,double y,double z,double rx,double ry,double rz, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3],trxyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  this->SetAbsPose(txyz,trxyz,t);
}

//-------------------------------------------------------------------------
void mafVME::SetAbsPose(double xyz[3],double rxyz[3], mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?this->CurrentTime:t;
  
  mflSmartPointer<mflMatrix> matrix;

  mflTransform::SetOrientation(matrix,rxyz);

  mflTransform::SetPosition(matrix,xyz);

  matrix->SetTimeStamp(t);

  SetAbsPose(matrix);
}

//-------------------------------------------------------------------------
void mafVME::SetAbsPose(vtkMatrix4x4 *matrix,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?this->CurrentTime:t;

  mflSmartPointer<mflMatrix> mat;
  mat->DeepCopy(matrix);
  mat->SetTimeStamp(t);
  SetAbsPose(mat);
}
//-------------------------------------------------------------------------
void mafVME::SetAbsPose(mflMatrix *matrix)
//-------------------------------------------------------------------------
{
  if (this->Parent)
  {
    mflSmartPointer<mflMatrix> pmat;
    this->GetParent()->GetAbsMatrix(pmat,matrix->GetTimeStamp());

    pmat->Invert();

    vtkMatrix4x4::Multiply4x4(pmat,matrix,matrix);
  }
  
  this->SetPose(matrix);
}
//-------------------------------------------------------------------------
void mafVME::SetAbsPosition(double x,double y,double z, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  this->SetAbsPosition(txyz,t);
}

//-------------------------------------------------------------------------
void mafVME::SetAbsPosition(double xyz[3], mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?this->CurrentTime:t;

  mflSmartPointer<mflMatrix> matrix;
  
  mflTransform::SetPosition(matrix,xyz);

  matrix->SetTimeStamp(t);

  mflMatrix *oldpose;
  this->MatrixVector->FindMatrix(t,oldpose);

  // recover the orientation only if a key matrix is present
  if (oldpose)
  {
    mflSmartPointer<mflMatrix> oldmat;
    this->GetAbsMatrix(oldmat,t);
    mflTransform::CopyRotation(matrix,oldmat);
  }

  if (this->Parent)
  {
    mflSmartPointer<mflMatrix> pmat;
    this->GetParent()->GetAbsMatrix(pmat,t);
    
    pmat->Invert();

    vtkMatrix4x4::Multiply4x4(pmat,matrix,matrix);
  }

  this->SetPose(matrix);
}

//-------------------------------------------------------------------------
void mafVME::SetAbsOrientation(double rx,double ry,double rz, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double trxyz[3];
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  this->SetAbsOrientation(trxyz,t);
}

//-------------------------------------------------------------------------
void mafVME::SetAbsOrientation(double rxyz[3], mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?this->CurrentTime:t;

  mflSmartPointer<mflMatrix> matrix;
  
  mflTransform::SetOrientation(matrix,rxyz);

  matrix->SetTimeStamp(t);

  mflMatrix *oldpose;
  this->MatrixVector->FindMatrix(t,oldpose);

  // recover the position only if a key matrix is present
  if (oldpose)
  {
    double pos[3];
    this->GetAbsPosition(pos,t);
    mflTransform::SetPosition(matrix,pos);
  }

  if (this->Parent)
  {
    mflSmartPointer<mflMatrix> pmat;
    this->GetParent()->GetAbsMatrix(pmat,t);

    pmat->Invert();

    vtkMatrix4x4::Multiply4x4(pmat,matrix,matrix);
  }

  this->SetPose(matrix);
}

//----------------------------------------------------------------------------
void mafVME::ApplyAbsTransform(vtkLinearTransform *transform,int premultiply,mflTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?this->CurrentTime:t;
  mflSmartPointer<mflTransform> new_pose;
  mflSmartPointer<mflMatrix> pose;
  GetAbsMatrix(pose,t);
  new_pose->SetMatrix(pose);
  new_pose->Concatenate(transform,premultiply);
  SetAbsPose(new_pose->GetMatrix(),t);
}
//----------------------------------------------------------------------------
void mafVME::ApplyAbsMatrix(vtkMatrix4x4 *matrix,int premultiply,mflTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?this->CurrentTime:t;
  mflSmartPointer<mflTransform> new_pose;
  mflSmartPointer<mflMatrix> pose;
  GetAbsMatrix(pose,t);
  new_pose->SetMatrix(pose);
  new_pose->Concatenate(matrix,premultiply);
  SetAbsPose(new_pose->GetMatrix(),t);
}

/*
//----------------------------------------------------------------------------
void mafVME::SetScale(double sx,double sy,double sz,mflTimeStamp t)
{
  t=(t<0)?this->CurrentTime:t;

  mflSmartPointer<mflMatrix> matrix;

  matrix->DeepCopy(this->GetMatrix(t));
  
  mflTransform::Scale(matrix,sx,sy,sz);

  this->MatrixVector->SetMatrix(matrix);
}

//----------------------------------------------------------------------------
void mafVME::SetScale(double sxyz[3],mflTimeStamp t)
{
  this->SetScale(sxyz[0],sxyz[1],sxyz[2]);
}
*/

//----------------------------------------------------------------------------
void mafVME::GetScale(double sxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mflSmartPointer<mflMatrix> mat;
  this->GetMatrix(mat,t);
  mflTransform::GetScale(mat,sxyz);
}

//----------------------------------------------------------------------------
void mafVME::GetScale(double &sx,double &sy,double &sz,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double sxyz[3];
  this->GetScale(sxyz,t);
  sx=sxyz[0];
  sy=sxyz[1];
  sz=sxyz[2];
}

//-------------------------------------------------------------------------
void mafVME::OutputDataUpdateCallback(void *arg)
//-------------------------------------------------------------------------
{
  // route events toward VME observers
  mafVME *self=(mafVME *)arg;

  if (self->GetDataPipe()) // allocate data pipe if not done yet
  {
    if (self->m_DataPipe->GetOutput()!=self->CurrentData.GetPointer())
    {
      self->SetCurrentData(self->m_DataPipe->GetOutput());
    
      // advise observers the output data has changed
      self->InvokeEvent(mafVME::OutputDataChangedEvent);
    }

    self->InvokeEvent(mafVME::OutputDataUpdateEvent);
  }
  else
  {
    vtkErrorWithObjectMacro(self,"Received Update Output data event from NULL data pipe!");
  }
}

//-------------------------------------------------------------------------
void mafVME::MatrixUpdateCallback(void *arg)
//-------------------------------------------------------------------------
{
  // route events toward VME observers
  mafVME *self=(mafVME *)arg;
  self->InvokeEvent(mafVME::MatrixUpdateEvent,self);
}

//-------------------------------------------------------------------------
void mafVME::AbsMatrixUpdateCallback(void *arg)
//-------------------------------------------------------------------------
{
  // route events toward VME observers
  mafVME *self=(mafVME *)arg;
  self->InvokeEvent(mafVME::AbsMatrixUpdateEvent,self);
}

//-------------------------------------------------------------------------
void mafVME::PrintSelf(ostream& os, vtkIndent indent)
//-------------------------------------------------------------------------
{
  os << indent << "Name: "<<this->GetName()<<endl;

	os << indent << "Number of Children: "<<this->Children->GetNumberOfItems()<<endl;

	os << indent << "Tag Array Contents: \n";
	this->GetTagArray()->PrintSelf(os,indent.GetNextIndent());

  os << indent << "Current Time: "<<this->CurrentTime<<"\n";

  os << indent << "Current Matrix:\n";
  this->GetMatrix()->PrintSelf(os,indent.GetNextIndent());

  os << indent << "Current Absolute Matrix:\n";
  this->GetAbsMatrix()->PrintSelf(os,indent.GetNextIndent());

  mflTimeStamp tbounds[2];
  this->GetLocalTimeBounds(tbounds);
  os << indent << "Time Bounds: ("<<tbounds[0]<<","<<tbounds[1]<<"]"<<endl;

  mafOBB bounds;
  this->GetVMESpaceBounds(bounds);

  os << indent << "VME Space Bounds: ["<<bounds.Bounds[0]<<","<<bounds.Bounds[1]<<","<<bounds.Bounds[2]<<"," \
    <<bounds.Bounds[3]<<","<<bounds.Bounds[4]<<","<<bounds.Bounds[5]<<"]\n";
  

  //os << indent << "VME 4D Bounds:\n";
  //this->Get4DBounds()->PrintSelf(os,indent.GetNextIndent());

  os << indent << "Matrix Vector:\n";
  this->GetMatrixVector()->PrintSelf(os,indent.GetNextIndent());
  
  os << indent << "VME DataSet Array:\n";
  this->GetDataArray()->PrintSelf(os,indent.GetNextIndent());
  //os << indent << "Number of Items:"<<this->GetDataArray()->GetNumberOfItems()<<"\n";

  os << indent << "Matrix Pipe: ";
  if (this->GetMatrixPipe())
  {
    os << "\n";
    this->GetMatrixPipe()->PrintSelf(os,indent.GetNextIndent());
  }
  else
    os << "NULL\n";
  
  os << indent << "VME m_DataPipe: ";
  if (this->GetDataPipe()) // allocate data pipe if not done yet
  {
    os << "\n";
    this->GetDataPipe()->PrintSelf(os,indent.GetNextIndent());
  }
  else
    os << "NULL\n";

  if (!this->CurrentData)
  {
    os << indent << "Current Data: NULL\n";
  }
  else
  {
    os << indent << "Current Data: "<< this->CurrentData->GetTypeName()<<"("<<this->CurrentData<<")\n";
    this->GetCurrentData()->PrintSelf(os,indent.GetNextIndent());
  }
}

//-------------------------------------------------------------------------
int mafVME::SetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type)
//-------------------------------------------------------------------------
{
  if (AuxRefSys)
  {
    return this->SetAuxiliaryRefSys(AuxRefSys->GetMatrix(),RefSysName,type);
  }

  return VTK_ERROR;
}

//-------------------------------------------------------------------------
int mafVME::SetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type)
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
  
      this->GetTagArray()->AddTag(item);
	  
	    return VTK_OK;
	  }
  }

	return VTK_ERROR;
}

//-------------------------------------------------------------------------
int mafVME::GetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type)
//-------------------------------------------------------------------------
{
  if (AuxRefSys)
  {
    mflSmartPointer<vtkMatrix4x4> matrix;
    if (this->GetAuxiliaryRefSys(matrix,RefSysName,type)==0)
    {
      AuxRefSys->SetMatrix(matrix);

      return VTK_OK;
    }
  }

  return VTK_ERROR;
}

//-------------------------------------------------------------------------
int mafVME::GetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type)
//-------------------------------------------------------------------------
{
	//check for type existence
  if (type == MFL_LOCAL_FRAME_TAG || type ==MFL_GLOBAL_FRAME_TAG)
  {
    if (AuxRefSys)
    {
      if (vtkString::Compare(RefSysName,"Global") == 0)
		  {
        if (this->GetParent())
        {
          AuxRefSys->DeepCopy(this->GetParent()->GetAbsMatrix());
        }
        else
        {
          AuxRefSys->Identity();
        }

        return VTK_OK;
      }

		  
      vtkTagItem *item=this->GetTagArray()->GetTag(RefSysName);
	    
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
int mafVME::SetMatrixPipe(mflMatrixPipe *mpipe)
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
        m_MatrixPipe->SetCurrentTime(CurrentTime);
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
        CurrentMatrix=mflSmartPointer<mflMatrix>();
        CurrentMatrix->SetTimeStamp(this->CurrentTime);
      }

      // this forces the the pipe to Update its input and input frame
      if (AbsMatrixPipe)
        AbsMatrixPipe->SetVME(this);

      this->InvokeEvent(mafVME::MatrixPipeChangedEvent);

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
void mafVME::UpdateCurrentData()
//-------------------------------------------------------------------------
{
  // if output data is available force an update
  if (GetCurrentData()) 
  {
    GetCurrentData()->Update();
  }
}

//-------------------------------------------------------------------------
void mafVME::SetCurrentData(vtkDataSet *data)
//-------------------------------------------------------------------------
{
  if (data!=CurrentData.GetPointer())
  {
    CurrentData=data;
    Modified();
    InvokeEvent(mafVME::OutputDataChangedEvent); // advise observers data pointer has changed
  }
}

//-------------------------------------------------------------------------
void mafVME::SetCrypting(int crypting)
//-------------------------------------------------------------------------
{
  if(crypting > 0)
    Crypting = 1;
  else
    Crypting = 0;

  TagArray->SetTag(vtkTagItem("MFL_CRYPT_VME",Crypting));
  this->Modified();
}

//-------------------------------------------------------------------------
int mafVME::GetCrypting()
//-------------------------------------------------------------------------
{
  return RestoreNumericFromTag(GetTagArray(),"MFL_CRYPT_VME",Crypting,-1,0);
}

//-------------------------------------------------------------------------
vtkDataSet *mafVME::GetCurrentData()
//-------------------------------------------------------------------------
{
  if (GetDataPipe()) // allocate data pipe if not done yet
  {
    SetCurrentData(GetDataPipe()->GetOutput());
  }
 
  return CurrentData;
}

//-------------------------------------------------------------------------
int mafVME::SetDataPipe(mflDataPipe *dpipe)
//-------------------------------------------------------------------------
{
  if (dpipe==this->m_DataPipe)
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
      m_DataPipe->SetCurrentTime(CurrentTime);
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
    InvokeEvent(mafVME::DataPipeChangedEvent);

    return VTK_OK;
  }
  else
  {
    return VTK_ERROR;
  }
}

#endif