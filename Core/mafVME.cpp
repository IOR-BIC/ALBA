/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVME.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-29 21:17:45 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mflVME_cxx
#define __mflVME_cxx

#include "mflVME.h"

#include "vtkString.h"
#include "vtkVector.txx"
#include "mflVMEIterator.h"
#include "vtkPivotTransform.h"
#include "vtkDoubleArray.h"

#include "mflAgent.h"
#include "mflItemArray.txx"
#include "mflMatrixVector.h"
#include "mflVMEItemArray.h"
#include "mflDataInterpolatorHolder.h"
#include "mflMatrixInterpolatorHolder.h"
#include "mflMatrixPipeDirectCinematic.h"
#include "mflBounds.h"
#include "mflSmartPointer.h"
#include "mflDefines.h"
#include "mflVMEIterator.h"
#include "mflTransform.h"
#include "vtkLinearTransform.h"

#include <map>
#include <assert.h>

// This is for allocating unique VME IDs.
unsigned long mflVME::VMEIdCounter = 0;

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MFL_EVT_IMP(mflVME::TimeEvent);             // Event rised by time changes
MFL_EVT_IMP(mflVME::MatrixUpdateEvent);     // Event rised by pose Matrix changes
MFL_EVT_IMP(mflVME::AbsMatrixUpdateEvent);  // Event rised by AbsMatrix changes
MFL_EVT_IMP(mflVME::DetachFromTreeEvent);   // Event rised by detachment from tree
MFL_EVT_IMP(mflVME::AttachToTreeEvent);     // Event rised by attachment to tree
MFL_EVT_IMP(mflVME::DestroyEvent);          // Event rised by VME destroying
MFL_EVT_IMP(mflVME::DataPipeChangedEvent);  // Rised when the DataPipe is changed
MFL_EVT_IMP(mflVME::MatrixPipeChangedEvent);// Rised when the MatrixPipe is changed
MFL_EVT_IMP(mflVME::OutputDataChangedEvent);// Rised when the OutputData pointer is changed
MFL_EVT_IMP(mflVME::OutputDataUpdateEvent); // Rised by Current Data Updates (this re-routes the same DataPipe event)
//------------------------------------------------------------------------------

vtkCxxSetObjectMacro(mflVME,AbsMatrixPipe,mflMatrixPipeDirectCinematic)

//-------------------------------------------------------------------------
mflVME::mflVME()
//-------------------------------------------------------------------------
{
  // Tags used for Observers
  DataUpdateTag       = 0;
  MatrixUpdateTag     = 0;
  AbsMatrixUpdateTag  = 0;
  

	TagArray            = vtkTagArray::New();
	MatrixVector        = mflMatrixVector::New();
  DataArray           = mflVMEItemArray::New();
  DataArray->SetVME(this);

  // Pipes are dynamically created
  MatrixPipe          = NULL;
  DataPipe            = NULL;
  AbsMatrixPipe       = NULL;

  // if no MatrixPipe is given, provide a static pose matrix
  CurrentMatrix       = mflSmartPointer<mflMatrix>();

  CurrentTime         = 0;
  VisibleToTraverse   = 1;
  Crypting            = -1;
}

//-------------------------------------------------------------------------
mflVME::~mflVME()
//-------------------------------------------------------------------------
{
  // advise observers this is being destroyed
  InvokeEvent(mflVME::DestroyEvent,this);

  SetTagArray(NULL);

  vtkDEL(MatrixVector);
  vtkDEL(DataArray);

  // Pipes must be destroyed in the right orde
  // to take into consideration dependencies
  if (DataPipe)
  {
    RemoveObserver(DataUpdateTag);
    DataPipe->SetVME(NULL);
    vtkDEL(DataPipe);
  }

  if (AbsMatrixPipe)
  {
    RemoveObserver(AbsMatrixUpdateTag);
    AbsMatrixPipe->SetVME(NULL);
    vtkDEL(AbsMatrixPipe);
  }
    
  if (MatrixPipe)
  { 
    RemoveObserver(MatrixUpdateTag);
    vtkDEL(MatrixPipe);
  }
}

//------------------------------------------------------------------------------
int mflVME::Initialize()
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
void mflVME::Shutdown()
//------------------------------------------------------------------------------
{
  if (Initialized)
  {
    InternalShutdown();
    Initialized = 0;
  }
}

//-------------------------------------------------------------------------
void mflVME::SetDefaultDataPipe()
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
    if (DataPipe)
      DataPipe->UnRegister(this); // Factory return an object with RefCount==1
  }
}

//-------------------------------------------------------------------------
const char *mflVME::GetDefaultDataPipe()
//-------------------------------------------------------------------------
{
  return "mflDataInterpolatorHolder";
}

//-------------------------------------------------------------------------
void mflVME::SetDefaultMatrixPipe()
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
    if (MatrixPipe)
      MatrixPipe->UnRegister(this);
  }
}

//-------------------------------------------------------------------------
const char *mflVME::GetDefaultMatrixPipe()
//-------------------------------------------------------------------------
{
  return "mflMatrixInterpolatorHolder";
}

//-------------------------------------------------------------------------
mflMatrixPipeDirectCinematic *mflVME::GetAbsMatrixPipe()
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
const char *mflVME::GetDataType()
//-------------------------------------------------------------------------
{
  vtkDataSet *data=this->GetOutput();  
  return data?data->GetClassName():NULL;
}

//-------------------------------------------------------------------------
mflVME *mflVME::MakeCopy(mflVME *a)
//-------------------------------------------------------------------------
{
  mflVME* newvme=a->NewInstance();
  newvme->DeepCopy(a);
  return newvme;
}

//-------------------------------------------------------------------------
int mflVME::DeepCopy(mflVME *a)
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
    vtkErrorMacro("Cannot copy VME of type "<<a->GetClassName()<<" into a VME \
    VME of type "<<this->GetClassName());

    return VTK_ERROR;
  }
}

//-------------------------------------------------------------------------
int mflVME::ShallowCopy(mflVME *a)
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
    vtkErrorMacro("Cannot copy VME of type "<<a->GetClassName()<<" into a VME \
    VME of type "<<this->GetClassName());

    return VTK_ERROR;
  }
}

//-------------------------------------------------------------------------
bool mflVME::CanCopy(mflVME *vme)
//-------------------------------------------------------------------------
{
  if (!vme)
    return false;

  if (vme->IsA(this->GetClassName()))
  {
    return true;
  }

  return false;
}

//-------------------------------------------------------------------------
int mflVME::GetItem(int id,mflVMEItem *&item)
//-------------------------------------------------------------------------
{
  return this->DataArray->GetItemByIndex(id,item);
}

//-------------------------------------------------------------------------
mflVMEItem *mflVME::GetItem(int id)
//-------------------------------------------------------------------------
{
  mflVMEItem *item;
  return (this->GetItem(id,item)==VTK_OK?item:NULL);
}

//-------------------------------------------------------------------------
int mflVME::AddItem(mflVMEItem *item)
//-------------------------------------------------------------------------
{
  if (item)
  {
    return this->DataArray->SetItem(item);
  }

  return VTK_ERROR;
}
  

//-------------------------------------------------------------------------
int mflVME::RemoveItem(int id)
//-------------------------------------------------------------------------
{
  return this->DataArray->RemoveItem(id);
}

//-------------------------------------------------------------------------
int mflVME::RemoveItem(mflVMEItem *item)
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
void mflVME::RemoveAllItems()
//-------------------------------------------------------------------------
{
  this->DataArray->RemoveAllItems();
}

//-------------------------------------------------------------------------
int mflVME::GetNumberOfItems()
//-------------------------------------------------------------------------
{
  return this->DataArray->GetNumberOfItems();
}

/*//------------------------------------------------------------------------------
int mflVME::SetParent(vtkTree *parent)
//-------------------------------------------------------------------------
{
  if (mflVME *parent_vme=mflVME::SafeDownCast(parent))
  {
    if (this->CanReparentTo(parent_vme)==VTK_OK)
    {  
      this->Superclass::SetParent(parent_vme);

      if (parent_vme==NULL)
      {
        this->InvokeEvent(mflVME::DetachFromTreeEvent,this);
      }
      else
      {
        this->InvokeEvent(mflVME::AttachToTreeEvent,this);
      }

      // this forces the the pipe to Update its input and input frame
      if (AbsMatrixPipe)
        this->AbsMatrixPipe->SetVME(this);

      return VTK_OK;
    }

    //modified by Stefano 27-10-2004 (beg)
    //Changed the error macro to give feedback about vme names
    
    vtkErrorMacro("Cannot reparent the VME: " << this->GetName() << " under the "<<parent->GetClassName() 
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
/*void mflVME::UnRegister(vtkObjectBase *a)
//-------------------------------------------------------------------------
{
  // If this object is still referenced only by its children
  // and items the object is deleted, removing all children and
  // items
  if (this->ReferenceCount==(this->GetNumberOfChildren()+this->GetNumberOfItems()))
  {
    
    // if the unregistering object is an Item simply return
    if (mflVMEItem *b=mflVMEItem::SafeDownCast(a))
    {
      vtkIdType ret;
      if (this->DataArray->FindItem(b))
        return;    
    }
   
    // if the unregister object is 
    if (mflVME *b=mflVME::SafeDownCast(a))
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
/*void mflVME::UnRegister(mflVMEItem *a)
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
int mflVME::FindItem(mflTimeStamp t,mflVMEItem *&item)
//-------------------------------------------------------------------------
{
  
  return this->DataArray->FindItem(t,item);

}

//-------------------------------------------------------------------------
mflVME *mflVME::FindInTree(const char *name,const char *value,int type)
//-------------------------------------------------------------------------
{
  vtkTagArray *tarray=this->GetTagArray();

  if (vtkTagItem *item=tarray->GetTag(name))
  {
    if (item->GetType()==type)
    {
      if (vtkString::Compare(item->GetValue(),value))
      {
        return this;
      }
    }
  }
  
  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    mflVME *curr=this->GetChild(i);
    
    if (mflVME *ret=curr->FindInTree(name,value,type))
      return ret;
  }

  return NULL;
}

//-------------------------------------------------------------------------
mflVME *mflVME::FindInTreeByName(const char *name)
//-------------------------------------------------------------------------
{
  if (this->Name==name)
  {
     return this;
  }
  
  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    mflVME *curr=this->GetChild(i);
    
    if (mflVME *ret=curr->FindInTreeByName(name))
      return ret;
  }

  return NULL;
}

//-------------------------------------------------------------------------
int mflVME::FindChildIdx(const char *name)
//-------------------------------------------------------------------------
{
  if (name)
  {
    for (int i=0;i<this->GetNumberOfChildren();i++)
    {
      mflVME *vme=this->GetChild(i);
      if (vme&&vtkString::Equals(vme->GetName(),name))
      {
        return i;
      }
    }
  }

  return -1;
}

//-------------------------------------------------------------------------
void mflVME::UpdateData(int idx)
//-------------------------------------------------------------------------
{
  mflVMEItem *item=this->GetItem(idx);
  if (item)
    item->UpdateData();
}


//-------------------------------------------------------------------------
void mflVME::UpdateAllData()
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
/*void mflVME::UpdateAbsMatrix(mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=t<0?this->CurrentTime:t;

  mflMatrix *absmat=this->AbsMatrix;
  
  mflVME *parent=this->GetParent();
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
void mflVME::UpdateData(mflVMEItem *item)
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
void mflVME::SetCurrentTime(mflTimeStamp t)
//-------------------------------------------------------------------------
{
  if (t<0)
    t=0;

  this->CurrentTime=t;

  // Must keep a time variable also on the
  // pipes to allow multiple pipes contemporary 
  // working at different times
  // 
  if (DataPipe)
    DataPipe->SetCurrentTime(t);

  if (MatrixPipe)
    MatrixPipe->SetCurrentTime(t);
  else if (CurrentMatrix.GetPointer())
    CurrentMatrix->SetTimeStamp(t);

  if (AbsMatrixPipe)
    this->AbsMatrixPipe->SetCurrentTime(t);

  this->Modified();

  // TODO: consider if to add a flag to diable event issuing
  this->InvokeEvent(mflVME::TimeEvent,this);
  
}

//-------------------------------------------------------------------------
void mflVME::SetTreeTime(mflTimeStamp t)
//-------------------------------------------------------------------------
{
  this->SetCurrentTime(t);

  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    this->GetChild(i)->SetTreeTime(t);
  }
}

//-------------------------------------------------------------------------
mflVMEIterator *mflVME::NewIterator()
//-------------------------------------------------------------------------
{
  mflVMEIterator *iter=mflVMEIterator::New();
  iter->SetRootNode(this);
  return iter;
}

//-------------------------------------------------------------------------
int mflVME::IsAnimated()
//-------------------------------------------------------------------------
{
  return ((this->GetNumberOfItems()>1)||(this->GetMatrixVector()->GetNumberOfMatrixes()>1));
}


//-------------------------------------------------------------------------
void mflVME::GetDataTimeBounds(mflTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  tbounds[0]=-1;
  tbounds[1]=-1;

  // find the time interval for the items in this VME
  for (int i=0;i<this->GetNumberOfItems();i++)
  {
    mflVMEItem *item=this->GetItem(i);
    if (item->GetTimeStamp()<tbounds[0]||tbounds[0]<0)
      tbounds[0]=item->GetTimeStamp();
    if (item->GetTimeStamp()>tbounds[1]||tbounds[1]<0)
      tbounds[1]=item->GetTimeStamp();
  }
}

//-------------------------------------------------------------------------
int mflVME::GetNumberOfLocalTimeStamps()
//-------------------------------------------------------------------------
{
  TimeVector timestamps;
  this->GetLocalTimeStamps(timestamps);
  return timestamps.size();
}

//-------------------------------------------------------------------------
int mflVME::GetNumberOfTimeStamps()
//-------------------------------------------------------------------------
{
  TimeVector timestamps;
  this->GetTimeStamps(timestamps);
  return timestamps.size();
}

//-------------------------------------------------------------------------
void mflVME::GetLocalTimeBounds(mflTimeStamp tbounds[2])
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
void mflVME::GetTimeBounds(mflTimeStamp tbounds[2])
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
void mflVME::GetDataTimeStamps(mflTimeStamp *&kframes)
//-------------------------------------------------------------------------
{
  this->DataArray->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void mflVME::GetDataTimeStamps(TimeVector &kframes)
//-------------------------------------------------------------------------
{
  this->DataArray->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void mflVME::GetMatrixTimeStamps(mflTimeStamp *&kframes)
//-------------------------------------------------------------------------
{
  this->MatrixVector->GetTimeStamps(kframes);
}



//-------------------------------------------------------------------------
void mflVME::GetMatrixTimeStamps(TimeVector &kframes)
//-------------------------------------------------------------------------
{
  this->MatrixVector->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void mflVME::GetLocalTimeStamps(mflTimeStamp *&kframes)
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
void mflVME::GetLocalTimeStamps(std::vector<mflTimeStamp> &kframes)
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
void mflVME::GetLocalTimeStamps(vtkDoubleArray *kframes)
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
void mflVME::MergeTimeVectors(std::vector<mflTimeStamp> &outv,std::vector<mflTimeStamp> v1,std::vector<mflTimeStamp>v2)
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
void mflVME::GetTimeStamps(mflTimeStamp *&kframes)
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
void mflVME::GetTimeStamps(TimeVector &kframes)
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
void mflVME::GetTimeStamps(vtkDoubleArray *kframes)
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
void mflVME::GetAbsTimeStamps(mflTimeStamp *&kframes)
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
void mflVME::GetAbsTimeStamps(TimeVector &kframes)
{
  this->GetLocalTimeStamps(kframes);
  
  std::vector<mflTimeStamp> parentKFrames;

  for (mflVME *parent=this->GetParent();parent;parent=parent->GetParent())
  {

    parent->GetLocalTimeStamps(parentKFrames);

    this->MergeTimeVectors(kframes,kframes,parentKFrames);
  }
}


//-------------------------------------------------------------------------
void mflVME::GetVME4DBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mflBounds myBounds;
  this->GetVME4DBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mflVME::GetVME4DBounds(mflBounds &bounds)
//-------------------------------------------------------------------------
{
  std::vector<mflTimeStamp> timestamps;

  this->GetTimeStamps(timestamps);

  bounds.Reset();

  if (GetDataPipe()) // allocate data pipe if not done yet
  {
    // THD SAFE implementation
    //mflDataPipe *datapipe=this->DataPipe->MakeACopy();

    mflSmartPointer<mflMatrix> itemAbsPose;
    mflBounds transformed_bounds;

    for (int i=0;i<timestamps.size();i++)
    {
      // THD SAFE implementation
      /*
      datapipe->SetCurrentTime(timestamps[i]);
      datapipe->UpdateCurrentBounds();
      mflBounds *itemBounds=datapipe->GetCurrentBounds();
      */

      this->DataPipe->SetCurrentTime(timestamps[i]);
      this->DataPipe->UpdateCurrentBounds();

      // must make a copy, otherwise I would transform the bounds inside the data pipe
      transformed_bounds.DeepCopy(this->DataPipe->GetCurrentBounds());
      this->GetAbsMatrix(itemAbsPose,timestamps[i]);
    
      transformed_bounds.ApplyTransform(itemAbsPose);

      bounds.MergeBounds(transformed_bounds);
    }

    this->DataPipe->SetCurrentTime(this->CurrentTime);
    this->DataPipe->UpdateCurrentBounds();
  }
  else if (CurrentData.GetPointer())
  {
    bounds.DeepCopy(CurrentData->GetBounds());
  }

}

//-------------------------------------------------------------------------
//void mflVME::GetLocal4DBounds(mflTimeStamp start, mflTimeStamp end, double bounds[6])
//{
//}

//-------------------------------------------------------------------------
//void mflVME::Get4DBounds(mflTimeStamp start, mflTimeStamp end, double bounds[6])
//{
//}

//-------------------------------------------------------------------------
void mflVME::GetVMESpaceBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mflBounds myBounds;
  this->GetVMESpaceBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mflVME::GetVMESpaceBounds(mflBounds &bounds,mflTimeStamp t, mflVMEIterator *iter)
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
void mflVME::GetVMELocalSpaceBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mflBounds myBounds;
  this->GetVMELocalSpaceBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mflVME::GetVMELocalSpaceBounds(mflBounds &bounds,mflTimeStamp t, mflVMEIterator *iter)
//-------------------------------------------------------------------------
{
   if (t<0)
    t=this->CurrentTime;

  bounds.Reset();

  if ((iter&&iter->IsVisible(this))||this->IsVisible())
  {
    
    if (GetDataPipe()) // allocate data pipe if not done yet
    {
      // We call directly the DataPipe UpdateBounds since this doesn't update
      // the output data, which is not necessary.
      // Must call explicitelly UpdateCurrentBounds() method, since datapipes
      // do not update automatically when time changes  
      this->DataPipe->SetCurrentTime(t);
      this->DataPipe->UpdateCurrentBounds();
      bounds.DeepCopy(this->DataPipe->GetCurrentBounds());

      // restore the right bounds for current time... 
      // TODO: modify the GetCurrentBounds to make it call UpdateCurentBounds explicitelly!
      this->DataPipe->SetCurrentTime(this->CurrentTime);
      this->DataPipe->UpdateCurrentBounds();
    
      // this is a thread safe implemetation
      /*mflDataPipe *datapipe=this->DataPipe->MakeACopy();
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
void mflVME::GetSpaceBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mflBounds myBounds;
  this->GetSpaceBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mflVME::GetSpaceBounds(mflBounds &bounds,mflTimeStamp t, mflVMEIterator *iter)
//-------------------------------------------------------------------------
{
  if (t<0)
    t=this->CurrentTime;

  this->GetVMESpaceBounds(bounds,t,iter);
  
  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    mflVME *child=this->GetChild(i);
    mflBounds childBounds;

    child->GetSpaceBounds(childBounds,t);

    bounds.MergeBounds(childBounds);
  }
}

//-------------------------------------------------------------------------
void mflVME::Get4DBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  mflBounds myBounds;
  this->Get4DBounds(myBounds);
  myBounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mflVME::Get4DBounds(mflBounds &bounds)
//-------------------------------------------------------------------------
{

  std::vector<mflTimeStamp> timestamps;

  bounds.Reset();

  this->GetTimeStamps(timestamps);

  mflBounds frameBounds;
  for (int i=0;i<timestamps.size();i++)
  {
    this->GetSpaceBounds(frameBounds,timestamps[i]);
    bounds.MergeBounds(frameBounds); 
  }

}

//-------------------------------------------------------------------------
bool mflVME::Equals(mflVME *vme)
//-------------------------------------------------------------------------
{
  if (!vme||!vme->IsA(this->GetClassName()))
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
bool mflVME::CompareTree(mflVME *vme)
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
mflVME *mflVME::CopyTree(mflVME *vme, mflVME *parent)
//-------------------------------------------------------------------------
{
  
  mflVME* v = vme->MakeCopy();

  v->ReparentTo(parent);

  if (parent)
  {
    v->Delete();
  }

  for(int i=0; i<vme->GetNumberOfChildren(); i++)
  {
    if (mflVME *child=vme->GetChild(i))
      if (child->IsVisible())
        mflVME::CopyTree(child,v);
  }

  return v;
}

//-------------------------------------------------------------------------
mflVME *mflVME::ReparentTo(mflVME *newparent)
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
      mflVMEIterator *iter=this->NewIterator();
      for (mflVME *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
      {
        for (int i=0;i<vme->GetNumberOfItems();i++)
        {
          mflVMEItem *item=vme->GetItem(i);
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
    
    mflVME *oldparent=this->GetParent();

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

    mflVME *ret=(this->ReferenceCount==1)?NULL:this;
    
    this->UnRegister(this);

    return ret;
  }
  else
  {
    return NULL;
  }
}


//----------------------------------------------------------------------------
void mflVME::Import(mflVME *tree)
//-------------------------------------------------------------------------
{
  if (tree&&tree->GetNumberOfChildren()>0)
  {
    int num=tree->GetNumberOfChildren();
    for (int i=0;i<num;i++)
    {
      mflVME *vme=tree->GetFirstChild();
      vme->ReparentTo(this);
    }
  }
}

//-------------------------------------------------------------------------
mflVMEItem *mflVME::GetItemByTimeStamp(mflTimeStamp t)
//-------------------------------------------------------------------------
{
  return this->DataArray->GetItemByTimeStamp(t);
}

//-------------------------------------------------------------------------
void mflVME::SetPose(mflMatrix *mat)
//-------------------------------------------------------------------------
{
  this->GetMatrixVector()->SetMatrix(mat);
}

//-------------------------------------------------------------------------
void mflVME::SetPose(vtkMatrix4x4 *mat, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?t=this->CurrentTime:t;
  if (this->MatrixVector)
    this->GetMatrixVector()->SetMatrix(mat,t);
  else
    this->CurrentMatrix->DeepCopy(mat);
}

//-------------------------------------------------------------------------
void mflVME::SetPose(double x,double y,double z,double rx,double ry,double rz, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3],trxyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  this->SetPose(txyz,trxyz,t);
}

//-------------------------------------------------------------------------
void mflVME::SetPose(double xyz[3],double rxyz[3], mflTimeStamp t)
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
void mflVME::ApplyTransform(vtkLinearTransform *transform,int premultiply,mflTimeStamp t)
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
void mflVME::ApplyMatrix(vtkMatrix4x4 *matrix,int premultiply,mflTimeStamp t)
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
void mflVME::SetPosition(double x,double y,double z, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  this->SetPosition(txyz,t);
}

//-------------------------------------------------------------------------
void mflVME::SetPosition(double xyz[3], mflTimeStamp t)
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
void mflVME::SetOrientation(double rx,double ry,double rz, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double trxyz[3];
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  this->SetOrientation(trxyz,t);
}

//-------------------------------------------------------------------------
void mflVME::SetOrientation(double rxyz[3], mflTimeStamp t)
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
mflMatrix *mflVME::GetPose()
//-------------------------------------------------------------------------
{
  return (this->GetMatrixPipe())?this->MatrixPipe->GetMatrix():this->CurrentMatrix;
}

//-------------------------------------------------------------------------
void mflVME::GetPose(mflMatrix *matrix,mflTimeStamp t)
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
        int old_flag=this->MatrixPipe->GetUpdateMatrixObserverFlag();
        this->MatrixPipe->UpdateMatrixObserverOff();
        this->MatrixPipe->SetCurrentTime(t);
        matrix->DeepCopy(this->MatrixPipe->GetMatrix());
        // restore right time
        this->MatrixPipe->SetCurrentTime(this->CurrentTime);
        this->MatrixPipe->SetUpdateMatrixObserverFlag(old_flag);

        // THD SAFE implementation
        // create a temporary pipe to interpolate at time t
        /*mflMatrixPipe *newpipe=this->MatrixPipe->MakeACopy();
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
void mflVME::GetPose(double xyz[3],double rxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mflSmartPointer<mflMatrix> mat;
  
  this->GetMatrix(mat,t);
  
  mflTransform::GetOrientation(mat,rxyz);
  
  mflTransform::GetPosition(mat,xyz);
}

//-------------------------------------------------------------------------
void mflVME::GetPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mflTimeStamp t)
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
void mflVME::GetPosition(double &x,double &y,double &z,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double xyz[3];
  this->GetPosition(xyz,t);
  x=xyz[0];
  y=xyz[1];
  z=xyz[2];

}

//-------------------------------------------------------------------------
void mflVME::GetPosition(double xyz[3],mflTimeStamp t)
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
void mflVME::GetOrientation(double &rx,double &ry,double &rz,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double rxyz[3];
  this->GetOrientation(rxyz,t);
  rx=rxyz[0];
  ry=rxyz[1];
  rz=rxyz[2];
}

//-------------------------------------------------------------------------
void mflVME::GetOrientation(double rxyz[3],mflTimeStamp t)
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
void mflVME::GetAbsPose(mflMatrix *matrix,mflTimeStamp t)
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
mflMatrix *mflVME::GetAbsPose()
//-------------------------------------------------------------------------
{
  return this->GetAbsMatrixPipe()->GetMatrix();
}

//-------------------------------------------------------------------------
void mflVME::GetAbsPose(double xyz[3],double rxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mflSmartPointer<mflMatrix> mat;
  this->GetAbsMatrix(mat,t);

  mflTransform::GetPosition(mat,xyz);

  mflTransform::GetOrientation(mat,rxyz);
}

//-------------------------------------------------------------------------
void mflVME::GetAbsPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mflTimeStamp t)
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
void mflVME::GetAbsPosition(double xyz[3],mflTimeStamp t)
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
void mflVME::GetAbsPosition(double &x,double &y,double &z,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double xyz[3];

  this->GetAbsPosition(xyz,t);

  x=xyz[0];
  y=xyz[1];
  z=xyz[2];
}

//-------------------------------------------------------------------------
void mflVME::GetAbsOrientation(double rxyz[3],mflTimeStamp t)
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
void mflVME::GetAbsOrientation(double &rx,double &ry,double &rz,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double rxyz[3];

  this->GetAbsOrientation(rxyz,t);

  rx=rxyz[0];
  ry=rxyz[1];
  rz=rxyz[2];
}

//-------------------------------------------------------------------------
void mflVME::SetAbsPose(double x,double y,double z,double rx,double ry,double rz, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3],trxyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  this->SetAbsPose(txyz,trxyz,t);
}

//-------------------------------------------------------------------------
void mflVME::SetAbsPose(double xyz[3],double rxyz[3], mflTimeStamp t)
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
void mflVME::SetAbsPose(vtkMatrix4x4 *matrix,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?this->CurrentTime:t;

  mflSmartPointer<mflMatrix> mat;
  mat->DeepCopy(matrix);
  mat->SetTimeStamp(t);
  SetAbsPose(mat);
}
//-------------------------------------------------------------------------
void mflVME::SetAbsPose(mflMatrix *matrix)
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
void mflVME::SetAbsPosition(double x,double y,double z, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  this->SetAbsPosition(txyz,t);
}

//-------------------------------------------------------------------------
void mflVME::SetAbsPosition(double xyz[3], mflTimeStamp t)
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
void mflVME::SetAbsOrientation(double rx,double ry,double rz, mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double trxyz[3];
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  this->SetAbsOrientation(trxyz,t);
}

//-------------------------------------------------------------------------
void mflVME::SetAbsOrientation(double rxyz[3], mflTimeStamp t)
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
void mflVME::ApplyAbsTransform(vtkLinearTransform *transform,int premultiply,mflTimeStamp t)
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
void mflVME::ApplyAbsMatrix(vtkMatrix4x4 *matrix,int premultiply,mflTimeStamp t)
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
void mflVME::SetScale(double sx,double sy,double sz,mflTimeStamp t)
{
  t=(t<0)?this->CurrentTime:t;

  mflSmartPointer<mflMatrix> matrix;

  matrix->DeepCopy(this->GetMatrix(t));
  
  mflTransform::Scale(matrix,sx,sy,sz);

  this->MatrixVector->SetMatrix(matrix);
}

//----------------------------------------------------------------------------
void mflVME::SetScale(double sxyz[3],mflTimeStamp t)
{
  this->SetScale(sxyz[0],sxyz[1],sxyz[2]);
}
*/

//----------------------------------------------------------------------------
void mflVME::GetScale(double sxyz[3],mflTimeStamp t)
//-------------------------------------------------------------------------
{
  mflSmartPointer<mflMatrix> mat;
  this->GetMatrix(mat,t);
  mflTransform::GetScale(mat,sxyz);
}

//----------------------------------------------------------------------------
void mflVME::GetScale(double &sx,double &sy,double &sz,mflTimeStamp t)
//-------------------------------------------------------------------------
{
  double sxyz[3];
  this->GetScale(sxyz,t);
  sx=sxyz[0];
  sy=sxyz[1];
  sz=sxyz[2];
}

//-------------------------------------------------------------------------
void mflVME::OutputDataUpdateCallback(void *arg)
//-------------------------------------------------------------------------
{
  // route events toward VME observers
  mflVME *self=(mflVME *)arg;

  if (self->GetDataPipe()) // allocate data pipe if not done yet
  {
    if (self->DataPipe->GetOutput()!=self->CurrentData.GetPointer())
    {
      self->SetCurrentData(self->DataPipe->GetOutput());
    
      // advise observers the output data has changed
      self->InvokeEvent(mflVME::OutputDataChangedEvent);
    }

    self->InvokeEvent(mflVME::OutputDataUpdateEvent);
  }
  else
  {
    vtkErrorWithObjectMacro(self,"Received Update Output data event from NULL data pipe!");
  }
}

//-------------------------------------------------------------------------
void mflVME::MatrixUpdateCallback(void *arg)
//-------------------------------------------------------------------------
{
  // route events toward VME observers
  mflVME *self=(mflVME *)arg;
  self->InvokeEvent(mflVME::MatrixUpdateEvent,self);
}

//-------------------------------------------------------------------------
void mflVME::AbsMatrixUpdateCallback(void *arg)
//-------------------------------------------------------------------------
{
  // route events toward VME observers
  mflVME *self=(mflVME *)arg;
  self->InvokeEvent(mflVME::AbsMatrixUpdateEvent,self);
}

//-------------------------------------------------------------------------
void mflVME::PrintSelf(ostream& os, vtkIndent indent)
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

  mflBounds bounds;
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
  
  os << indent << "VME DataPipe: ";
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
    os << indent << "Current Data: "<< this->CurrentData->GetClassName()<<"("<<this->CurrentData<<")\n";
    this->GetCurrentData()->PrintSelf(os,indent.GetNextIndent());
  }
}

//-------------------------------------------------------------------------
int mflVME::SetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type)
//-------------------------------------------------------------------------
{
  if (AuxRefSys)
  {
    return this->SetAuxiliaryRefSys(AuxRefSys->GetMatrix(),RefSysName,type);
  }

  return VTK_ERROR;
}

//-------------------------------------------------------------------------
int mflVME::SetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type)
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
int mflVME::GetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type)
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
int mflVME::GetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type)
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
			  if (item->GetType() == type)
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
int mflVME::SetMatrixPipe(mflMatrixPipe *mpipe)
//-------------------------------------------------------------------------
{
  if (mpipe!=MatrixPipe)
  {
    if (mpipe==NULL||mpipe->SetVME(this)==VTK_OK)
    { 
      // if we had an observer...
      if (MatrixPipe && MatrixUpdateTag)
      {
        MatrixPipe->RemoveObserver(MatrixUpdateTag);
        MatrixPipe->SetVME(NULL);
        MatrixPipe->SetCurrentTime(CurrentTime);
      }

      vtkSetObjectBodyMacro(MatrixPipe,mflMatrixPipe,mpipe);

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

      this->InvokeEvent(mflVME::MatrixPipeChangedEvent);

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
void mflVME::UpdateCurrentData()
//-------------------------------------------------------------------------
{
  // if output data is available force an update
  if (GetCurrentData()) 
  {
    GetCurrentData()->Update();
  }
}

//-------------------------------------------------------------------------
void mflVME::SetCurrentData(vtkDataSet *data)
//-------------------------------------------------------------------------
{
  if (data!=CurrentData.GetPointer())
  {
    CurrentData=data;
    Modified();
    InvokeEvent(mflVME::OutputDataChangedEvent); // advise observers data pointer has changed
  }
}

//-------------------------------------------------------------------------
void mflVME::SetCrypting(int crypting)
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
int mflVME::GetCrypting()
//-------------------------------------------------------------------------
{
  return RestoreNumericFromTag(GetTagArray(),"MFL_CRYPT_VME",Crypting,-1,0);
}

//-------------------------------------------------------------------------
vtkDataSet *mflVME::GetCurrentData()
//-------------------------------------------------------------------------
{
  if (GetDataPipe()) // allocate data pipe if not done yet
  {
    SetCurrentData(GetDataPipe()->GetOutput());
  }
 
  return CurrentData;
}

//-------------------------------------------------------------------------
int mflVME::SetDataPipe(mflDataPipe *dpipe)
//-------------------------------------------------------------------------
{
  if (dpipe==this->DataPipe)
    return VTK_OK;

  if (dpipe==NULL||dpipe->SetVME(this)==VTK_OK)
  { 
    // if we had an observer...
    if (DataPipe && DataUpdateTag)
    {
      DataPipe->RemoveObserver(DataUpdateTag);
      DataPipe->SetVME(NULL);
    }

    vtkSetObjectBodyMacro(DataPipe,mflDataPipe,dpipe);
    
    if (DataPipe)
    {
      //SetCurrentData(DataPipe->GetOutput());
      DataPipe->SetCurrentTime(CurrentTime);
    }
    else
    {
      SetCurrentData(NULL);
    }

    // must plug the event source after calling UpdateCurrentData, since 
    // UpdateCurrentData calls invoke explicitelly the OutputDataChangedEvent,
    // this avoids a double calling.
    if (DataPipe)
    {
      mflAgent::PlugEventSource(dpipe,OutputDataUpdateCallback,this,mflDataPipe::OutputUpdateEvent);
    }

    // advise listeners the data pipe has changed
    InvokeEvent(mflVME::DataPipeChangedEvent);

    return VTK_OK;
  }
  else
  {
    return VTK_ERROR;
  }
}

#endif