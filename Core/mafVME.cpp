/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVME.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-11 15:46:25 $
  Version:   $Revision: 1.8 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVME_cxx
#define __mafVME_cxx

#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafAbsMatrixPipe.h"
#include "mafMatrixPipe.h"
#include "mafDataPipe.h"
#include "mafEventSource.h"
#include "mafTagArray.h"
#include "mafOBB.h"
#include "mafTransform.h"
#include "mmuTimeSet.h"
#include "mafIndent.h"
#include "mafDecl.h"

#include <sstream>
#include <assert.h>

//-------------------------------------------------------------------------
mafVME::mafVME()
//-------------------------------------------------------------------------
{
  m_AbsMatrixPipe=mafAbsMatrixPipe::New();
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
  m_DataPipe=NULL;
  
  m_AbsMatrixPipe->SetVME(NULL);
  m_AbsMatrixPipe=NULL;
    
  m_MatrixPipe=NULL;
}

//-------------------------------------------------------------------------
mafVME *mafVME::GetParent() const
//-------------------------------------------------------------------------
{
  assert(m_Parent->IsA(typeid(mafVME)));
  return (mafVME *)Superclass::GetParent();
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
    if (mafVME *vme=mafVME::SafeDownCast(GetChild(i)))
    {
      vme->GetTimeStamps(subKFrames);
    }

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

  for (mafVME *parent=mafVME::SafeDownCast(GetParent());parent;parent=mafVME::SafeDownCast(parent->GetParent()))
  {
    parent->GetLocalTimeStamps(parentKFrames);

    mmuTimeSet::Merge(kframes,parentKFrames,kframes);
  }
}

//-------------------------------------------------------------------------
bool mafVME::CanReparentTo(mafNode *parent)
//-------------------------------------------------------------------------
{
  return (parent == NULL)|| (parent->IsA(typeid(mafVME)) && !IsInTree(parent));
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
  matrix.SetTimeStamp(t);
 
  SetMatrix(matrix);
}

//----------------------------------------------------------------------------
void mafVME::ApplyMatrix(const mafMatrix &matrix,int premultiply,mafTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;

  mafTransform new_pose;
  mafMatrix pose;
  GetOutput()->GetMatrix(pose,t);
  new_pose.SetMatrix(pose);
  new_pose.Concatenate(matrix,premultiply);
  new_pose.SetTimeStamp(t);
  SetMatrix(new_pose.GetMatrix());
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
  matrix.SetTimeStamp(t);

  SetAbsMatrix(matrix);
}

//-------------------------------------------------------------------------
void mafVME::SetAbsMatrix(const mafMatrix &matrix,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;

  mafMatrix mat=matrix;
  mat.SetTimeStamp(t);
  SetAbsMatrix(mat);
}
//-------------------------------------------------------------------------
void mafVME::SetAbsMatrix(const mafMatrix &matrix)
//-------------------------------------------------------------------------
{
  if (GetParent())
  {
    mafMatrix pmat;
    GetParent()->GetOutput()->GetAbsMatrix(pmat,matrix.GetTimeStamp());

    pmat.Invert();

    mafMatrix::Multiply4x4(pmat,matrix,pmat);
    SetMatrix(pmat);
    return;
  }
  
  SetMatrix(matrix);
}


//----------------------------------------------------------------------------
void mafVME::ApplyAbsMatrix(const mafMatrix &matrix,int premultiply,mafTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;
  mafTransform new_pose;
  mafMatrix pose;
  GetOutput()->GetAbsMatrix(pose,t);
  new_pose.SetMatrix(pose);
  new_pose.Concatenate(matrix,premultiply);
  new_pose.SetTimeStamp(t);
  SetAbsMatrix(new_pose.GetMatrix());
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
void mafVME::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  
  mafIndent indent(tabs);

  os << indent << "Current Time: "<<m_CurrentTime<<"\n";

  os << indent << "Output:\n";
  m_Output->Print(os,indent.GetNextIndent());

  os << indent << "Matrix Pipe: ";
  if (m_MatrixPipe)
  {
    os << "\n";
    m_MatrixPipe->Print(os,indent.GetNextIndent());
  }
  else
    os << "NULL\n";
  
  os << indent << "DataPipe: ";
  if (m_DataPipe) // allocate data pipe if not done yet
  {
    os << "\n";
    m_DataPipe->Print(os,indent.GetNextIndent());
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
      if (m_MatrixPipe)
      {
        // detach the old pipe
        m_MatrixPipe->SetListener(NULL);
        m_MatrixPipe->SetVME(NULL);
        m_MatrixPipe->SetCurrentTime(m_CurrentTime);
      }
      
      m_MatrixPipe = mpipe;

      if (mpipe)
      {
        // attach as observer to the new pipe
        mpipe->SetListener(this);
        mpipe->SetVME(this);
        // set the output matrix to pipe output matrix
        GetOutput()->m_Matrix=mpipe->GetMatrixPointer();
      }
      else
      {
        // When no Matrix pipe is set, simply provide
        // an identity matrix as output matrix
        GetOutput()->m_Matrix=mafSmartPointer<mafMatrix>();
        GetOutput()->m_Matrix->SetTimeStamp(m_CurrentTime);
      }

      // this forces the the pipe to Update its input and input frame
      if (m_AbsMatrixPipe)
        m_AbsMatrixPipe->SetVME(this);

      GetEventSource()->InvokeEvent(this,VME_MATRIX_CHANGED);

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
void mafVME::Update()
//-------------------------------------------------------------------------
{
  // to be written  
}

//-------------------------------------------------------------------------
void mafVME::SetCrypting(int crypting)
//-------------------------------------------------------------------------
{
  if(crypting > 0)
    m_Crypting = 1;
  else
    m_Crypting = 0;

  GetTagArray()->SetTag(mmuTagItem("MAF_CRYPT_VME",m_Crypting));
  Modified();
}

//-------------------------------------------------------------------------
int mafVME::GetCrypting()
//-------------------------------------------------------------------------
{
  return mafRestoreNumericFromTag(GetTagArray(),"MAF_CRYPT_VME",m_Crypting,-1,0);
}

//-------------------------------------------------------------------------
int mafVME::SetDataPipe(mafDataPipe *dpipe)
//-------------------------------------------------------------------------
{
  if (dpipe==m_DataPipe.GetPointer())
    return MAF_OK;

  if (dpipe==NULL||dpipe->SetVME(this)==MAF_OK)
  { 
    // if we had an observer...
    if (m_DataPipe)
    {
      // detach the old pipe
      m_DataPipe->SetListener(NULL);
      m_DataPipe->SetVME(NULL);
    }

    m_DataPipe = dpipe;
    
    if (m_DataPipe)
    {
      m_DataPipe->SetCurrentTime(m_CurrentTime);
      m_DataPipe->SetListener(this);
    }

    // must plug the event source after calling UpdateCurrentData, since 
    // UpdateCurrentData calls invoke explicitly the OutputDataChangedEvent,
    // this avoids a double calling.
    //if (m_DataPipe)
    //{
    //  mflAgent::PlugEventSource(dpipe,OutputDataUpdateCallback,this,mafDataPipe::OutputUpdateEvent);
    //}

    // advise listeners the data pipe has changed
    GetEventSource()->InvokeEvent(this,VME_OUTPUT_DATA_CHANGED);

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