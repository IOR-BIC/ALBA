/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMERoot.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:59:49 $
  Version:   $Revision: 1.11 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "mafVMERoot.h"
#include "mafAbsMatrixPipe.h"
#include "mafAttribute.h"
#include "mafTransform.h"
#include "mafVMEOutputNULL.h"
#include "mafIndent.h"
#include "mafStorage.h"
#include "mafStorageElement.h"
#include "mafEventIO.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMERoot)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMERoot::mafVMERoot()
//-------------------------------------------------------------------------
{
   m_MaxItemId=-1;
   mafNEW(m_Transform);
   mafVMEOutputNULL *output=mafVMEOutputNULL::New(); // an output with no data
   output->SetTransform(m_Transform); // force my transform in the output
   SetOutput(output);
}

//-------------------------------------------------------------------------
mafVMERoot::~mafVMERoot()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  SetOutput(NULL);
}

//-------------------------------------------------------------------------
bool mafVMERoot::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  { 
    return m_Transform->GetMatrix()==((mafVMERoot *)vme)->m_Transform->GetMatrix();
  }
  return false;
}

//-------------------------------------------------------------------------
int mafVMERoot::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMERoot *vme_root=mafVMERoot::SafeDownCast(a);
    m_Transform->SetMatrix(vme_root->m_Transform->GetMatrix());
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
void mafVMERoot::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void mafVMERoot::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamp
}

//-------------------------------------------------------------------------
void mafVMERoot::OnEvent(mafEventBase *e)
//-------------------------------------------------------------------------
{
  if (e->GetChannel()==MCH_UP)
  {
    switch (e->GetId())
    {
    case VME_GET_NEWITEM_ID:
      if (mafEventIO *event_io=mafEventIO::SafeDownCast(e))
      {
        event_io->SetItemId(mafVMERoot::SafeDownCast(GetRoot())->GetNextItemId()); // retrieve and return an item ID
      }
    break;
    default:
      mafRoot::OnRootEvent(e);
    };
  }
  else
  {
    Superclass::OnEvent(e);
  }
}

//-------------------------------------------------------------------------
void mafVMERoot::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafNode::Print(os,tabs);
  mafRoot::Print(os,tabs);
  os << mafIndent(tabs) << "MaxItemId: " << m_MaxItemId << "\n";
}
//-------------------------------------------------------------------------
char** mafVMERoot::GetIcon() 
//-------------------------------------------------------------------------
{
#include "mafVMERoot.xpm"
  return mafVMERoot_xpm;
}

//-------------------------------------------------------------------------
int mafVMERoot::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMERoot::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}
