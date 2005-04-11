/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMERoot.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 12:59:56 $
  Version:   $Revision: 1.8 $
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
#include "mafEventIO.h"
#include <sstream>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMERoot)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMERoot::mafVMERoot()
//-------------------------------------------------------------------------
{
   m_MaxItemId=-1;
   mafNEW(m_Transform);
   m_Output = mafVMEOutputNULL::New();
   m_Output->SetTransform(m_Transform);
   m_Output->SetVME(this);
   m_AbsMatrixPipe->SetVME(this);
}

//-------------------------------------------------------------------------
mafVMERoot::~mafVMERoot()
//-------------------------------------------------------------------------
{
  mafDEL(m_Output);
  mafDEL(m_Transform);
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
    
    mafVMERoot *vme=mafVMERoot::SafeDownCast(a);
    m_Transform->SetMatrix(vme->m_Transform->GetMatrix());
    m_Transform->SetTimeStamp(vme->m_Transform->GetTimeStamp());

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
void mafVMERoot::SetCurrentTime(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  Superclass::SetCurrentTime(t);
  m_Transform->SetTimeStamp(t);
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
