/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVME.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-11 10:07:18 $
  Version:   $Revision: 1.7 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVME_cxx
#define __mafVME_cxx

#include "mafVME.h"

#include "mafAbsMatrixPipe.h"
#include "mafMatrixPipe.h"
#include "mafDataPipe.h"
#include "mafEventSource.h"
#include "mafOBB.h"
#include "mafTransform.h"
#include "mmuTimeSet.h"
#include "mafIndent.h"

#include <sstream>
#include <assert.h>

//-------------------------------------------------------------------------
mafVME::mafVME()
//-------------------------------------------------------------------------
{
  // Pipes are dynamically created
  m_MatrixPipe          = NULL; // to be created by subclasses
  m_DataPipe            = NULL; // to be created by subclasses

  cppNEW(m_AbsMatrixPipe);
  m_AbsMatrixPipe->SetVME(this);
  m_AbsMatrixPipe->SetListener(this);

  m_CurrentTime         = 0.0;
  m_Crypting            = -1;
}

//-------------------------------------------------------------------------
mafVME::~mafVME()
//-------------------------------------------------------------------------
{
  // Pipes must be destroyed in the right order
  // to take into consideration dependencies
  cppDEL(m_DataPipe);
  
  m_AbsMatrixPipe->SetVME(NULL);
  cppDEL(m_AbsMatrixPipe);
    
  cppDEL(m_MatrixPipe);
}

//-------------------------------------------------------------------------
mafVME *mafVME::GetParent()
//-------------------------------------------------------------------------
{
  assert(m_Parent->IsA(mafVME::GetStaticTypeId()));
  return (mafVME *)GetParent();
}

//-------------------------------------------------------------------------
int mafVME::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (CanCopy(a))
  {
    mafVME *vme=mafVME::SafeDownCast(a);
    SetMatrixPipe(vme->GetMatrixPipe()?vme->GetMatrixPipe()->MakeACopy():NULL);
    SetDataPipe(vme->GetDataPipe()?vme->GetDataPipe()->MakeACopy():NULL);

    // Runtime properties
    //AutoUpdateAbsMatrix=vme->GetAutoUpdateAbsMatrix();
    SetCurrentTime(vme->GetCurrentTime());

    return MAF_OK;
  }
  else
  {
    mafErrorMacro("Cannot copy VME of type "<<a->GetTypeName()<<" into a VME \
    VME of type "<<GetTypeName());

    return MAF_ERROR;
  }
}

//-------------------------------------------------------------------------
int mafVME::ShallowCopy(mafVME *a)
//-------------------------------------------------------------------------
{  
  // for basic VME ShallowCopy is the same of DeepCopy (no data stored inside)
  return mafVME::DeepCopy(a);
}

//-------------------------------------------------------------------------
// TODO: to be rewritten
void mafVME::SetCurrentTime(mafTimeStamp t)
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
  
  if (m_AbsMatrixPipe)
    m_AbsMatrixPipe->SetCurrentTime(t);

  Modified();

  // TODO: consider if to add a flag to disable event issuing
  //GetEventSource()->InvokeEvent(this,VME_TIME_SET);
  
}

//-------------------------------------------------------------------------
void mafVME::SetTreeTime(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  SetCurrentTime(t);
  //ForwardDownEvent(mafEventBase(this,VME_TIME_SET));
}

//-------------------------------------------------------------------------
int mafVME::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
int mafVME::GetNumberOfLocalTimeStamps()
//-------------------------------------------------------------------------
{
  TimeVector timestamps;
  GetLocalTimeStamps(timestamps);
  return timestamps.size();
}

//-------------------------------------------------------------------------
int mafVME::GetNumberOfTimeStamps()
//-------------------------------------------------------------------------
{
  TimeVector timestamps;
  GetTimeStamps(timestamps);
  return timestamps.size();
}

//-------------------------------------------------------------------------
void mafVME::GetTimeStamps(mafTimeStamp *&kframes)
//-------------------------------------------------------------------------
{
  std::vector<mafTimeStamp> frames;

  GetTimeStamps(frames);

  if(frames.size()>0)
  {
    kframes=new mafTimeStamp[frames.size()];
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
  GetLocalTimeStamps(kframes);
  
  std::vector<mafTimeStamp> subKFrames;

  for (int i=0;i<GetNumberOfChildren();i++)
  {
    GetChild(i)->GetTimeStamps(subKFrames);

    mmuTimeSet::Merge(kframes,subKFrames,kframes);
  }
}

//-------------------------------------------------------------------------
void mafVME::GetAbsTimeStamps(mafTimeStamp *&kframes)
//-------------------------------------------------------------------------
{
  std::vector<mafTimeStamp> frames;

  GetAbsTimeStamps(frames);

  if(frames.size()>0)
  {
    kframes=new mafTimeStamp[frames.size()];
  }
  else
  {
    kframes=NULL;
  }
}

//-------------------------------------------------------------------------
void mafVME::GetAbsTimeStamps(TimeVector &kframes)
//-------------------------------------------------------------------------
{
  GetLocalTimeStamps(kframes);
  
  std::vector<mafTimeStamp> parentKFrames;

  for (mafVME *parent=GetParent();parent;parent=parent->GetParent())
  {

    parent->GetLocalTimeStamps(parentKFrames);

    mmuTimeSet::Merge(kframes,parentKFrames,kframes);
  }
}

//-------------------------------------------------------------------------
bool mafVME::CanReparentTo(mafNode *parent)
//-------------------------------------------------------------------------
{
  return parent==NULL||(parent->IsA(mafVME::GetStaticTypeId())&&||!IsInTree(parent));
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
void mafVME::SetPose(double x,double y,double z,double rx,double ry,double rz, mafTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3],trxyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  SetPose(txyz,trxyz,t);
}

//-------------------------------------------------------------------------
void mafVME::SetPose(double xyz[3],double rxyz[3], mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?t=m_CurrentTime:t;

  mafMatrix matrix;

  mafTransform::SetOrientation(matrix,rxyz);
  mafTransform::SetPosition(matrix,xyz);
  matrix->SetTimeStamp(t);
 
  SetMatrix(&matrix);
}

//----------------------------------------------------------------------------
void mafVME::ApplyMatrix(const mafMatrix &matrix,int premultiply,mafTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;

  mafTransform new_pose;
  mafMatrix pose;
  GetOutput()->GetMatrix(&pose,t);
  new_pose.SetMatrix(&pose);
  new_pose.Concatenate(matrix,premultiply);
  SetMatrix(new_pose.GetMatrix(),t);
}

//-------------------------------------------------------------------------
void mafVME::SetAbsPose(double x,double y,double z,double rx,double ry,double rz, mafTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3],trxyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  SetAbsPose(txyz,trxyz,t);
}

//-------------------------------------------------------------------------
void mafVME::SetAbsPose(double xyz[3],double rxyz[3], mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;
  
  mafMatrix matrix;

  mafTransform::SetOrientation(matrix,rxyz);
  mafTransform::SetPosition(matrix,xyz);
  matrix->SetTimeStamp(t);

  SetAbsPose(matrix);
}

//-------------------------------------------------------------------------
void mafVME::SetAbsMatrix(const mafMatrix &matrix,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;

  mafMatrix mat;
  mat->DeepCopy(matrix);
  mat->SetTimeStamp(t);
  SetAbsPose(mat);
}
//-------------------------------------------------------------------------
void mafVME::SetAbsMatrix(const mafMatrix &matrix)
//-------------------------------------------------------------------------
{
  if (GetParent())
  {
    mafMatrix pmat;
    GetParent()->GetAbsMatrix(pmat,matrix->GetTimeStamp());

    pmat->Invert();

    mafMatrix::Multiply4x4(pmat,matrix,matrix);
  }
  
  SetPose(matrix);
}


//----------------------------------------------------------------------------
void mafVME::ApplyAbsMatrix(mafMatrix *matrix,int premultiply,mafTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;
  mafTransform new_pose;
  mafMatrix pose;
  GetOutput()->GetAbsMatrix(pose,t);
  new_pose->SetMatrix(pose);
  new_pose->Concatenate(matrix,premultiply);
  SetAbsPose(new_pose->GetMatrix(),t);
}

/*
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
*/
//-------------------------------------------------------------------------
void mafVME::Print(std::ostream& os, const int tabs)
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);
  os << indent << "Name: "<<GetName()<<std::endl;

  os << indent << "Number of Children: "<<Children->GetNumberOfItems()<<std::endl;

	os << indent << "Tag Array Contents: \n";
	GetTagArray()->PrintSelf(os,indent.GetNextIndent());

  os << indent << "Current Time: "<<m_CurrentTime<<"\n";

  os << indent << "Output:\n";
  GetOutput()->Print(os,indent.GetNextIndent());

  os << indent << "Matrix Pipe: ";
  if (GetMatrixPipe())
  {
    os << "\n";
    GetMatrixPipe()->Print(os,indent.GetNextIndent());
  }
  else
    os << "NULL\n";
  
  os << indent << "DataPipe: ";
  if (GetDataPipe()) // allocate data pipe if not done yet
  {
    os << "\n";
    GetDataPipe()->Print(os,indent.GetNextIndent());
  }
  else
    os << "NULL\n";
}

/*
//-------------------------------------------------------------------------
int mafVME::SetAuxiliaryRefSys(mafTransform *AuxRefSys, const char *RefSysName, int type)
//-------------------------------------------------------------------------
{
  if (AuxRefSys)
  {
    return SetAuxiliaryRefSys(AuxRefSys->GetMatrix(),RefSysName,type);
  }

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVME::SetAuxiliaryRefSys(mafMatrix *AuxRefSys, const char *RefSysName, int type)
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
	  
	    return MAF_OK;
	  }
  }

	return MAF_ERROR;
}


//-------------------------------------------------------------------------
int mafVME::GetAuxiliaryRefSys(mafTransform *AuxRefSys, const char *RefSysName, int type)
//-------------------------------------------------------------------------
{
  if (AuxRefSys)
  {
    mafMatrix matrix;
    if (GetAuxiliaryRefSys(matrix,RefSysName,type)==0)
    {
      AuxRefSys->SetMatrix(matrix);

      return MAF_OK;
    }
  }

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVME::GetAuxiliaryRefSys(mafMatrix *AuxRefSys, const char *RefSysName, int type)
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

        return MAF_OK;
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
          
					  return MAF_OK;
			  }
		  }
      else if (RefSysName == "Default")
      {
        // if not Default reference system was specified return the Local reference system
        // i.e. the identity!
        AuxRefSys->Identity();
        return MAF_OK;
      }
	  }
  }
  return MAF_ERROR;
}
*/
//-------------------------------------------------------------------------
int mafVME::SetMatrixPipe(mafMatrixPipe *mpipe)
//-------------------------------------------------------------------------
{
  if (mpipe!=m_MatrixPipe)
  {
    if (mpipe==NULL||mpipe->SetVME(this)==MAF_OK)
    { 
      // if we had an observer...
      if (m_MatrixPipe && MatrixUpdateTag)
      {
        m_MatrixPipe->RemoveObserver(MatrixUpdateTag);
        m_MatrixPipe->SetVME(NULL);
        m_MatrixPipe->SetCurrentTime(m_CurrentTime);
      }

      vtkSetObjectBodyMacro(m_MatrixPipe,mafMatrixPipe,mpipe);

      if (mpipe)
      {
        // attach oberver to new pipe (to be substitute with DefaultChannel communication)
        mflAgent::PlugEventSource(mpipe,MatrixUpdateCallback,this,mafMatrixPipe::MatrixUpdateEvent);
        CurrentMatrix=mpipe->GetMatrix();
      }
      else
      {
        // When no Matrix pipe is set, simple provide
        // an identity matrix
        CurrentMatrix=mafSmartPointer<mafMatrix>();
        CurrentMatrix->SetTimeStamp(m_CurrentTime);
      }

      // this forces the the pipe to Update its input and input frame
      if (m_AbsMatrixPipe)
        m_AbsMatrixPipe->SetVME(this);

      InvokeEvent(mafVME::MatrixPipeChangedEvent);

      return MAF_OK;
    }
    else
    {
      return MAF_ERROR;
    }
  }

  return MAF_OK;
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
  Modified();
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
  if (dpipe==m_DataPipe)
    return MAF_OK;

  if (dpipe==NULL||dpipe->SetVME(this)==MAF_OK)
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
    InvokeEvent(mafVME::DataPipeChangedEvent);

    return MAF_OK;
  }
  else
  {
    return MAF_ERROR;
  }
}

//-------------------------------------------------------------------------
void mafVME::OnEvent(mafEventBase *e)
//-------------------------------------------------------------------------
{
  if (e->GetChannel()==MCH_DOWN)
  {
    switch (e->GetId())
    {
    case VME_TIME_SET:
      //SetCurrentTime();
    break;
    }
  }

  Superclass::OnEvent(e);
}

#endif