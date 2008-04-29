/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGroup.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-29 10:47:46 $
  Version:   $Revision: 1.9 $
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


#include "mafVMEGroup.h"
#include "mmgGui.h"

#include "mafIndent.h"
#include "mafTransform.h"
#include "mafNode.h"
#include "mafVMEOutputNULL.h"
#include "mafStorageElement.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEGroup)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEGroup::mafVMEGroup()
//-------------------------------------------------------------------------
{
  // The output is created on demand in GetOutput() to avoid
  // subclasses to have to destroy base class output
  mafNEW(m_Transform);
}

//-------------------------------------------------------------------------
mafVMEGroup::~mafVMEGroup()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGenericAbstract
}
//-------------------------------------------------------------------------
int mafVMEGroup::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{
  if (Superclass::DeepCopy(a) == MAF_OK)
  {
    mafVMEGroup *group = mafVMEGroup::SafeDownCast(a);
    m_Transform->SetMatrix(group->m_Transform->GetMatrix());
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMEGroup::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((mafVMEGroup *)vme)->m_Transform->GetMatrix();
  }
  return ret;
}
//-------------------------------------------------------------------------
void mafVMEGroup::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
}

//-------------------------------------------------------------------------
int mafVMEGroup::InternalStore(mafStorageElement *parent)
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
int mafVMEGroup::InternalRestore(mafStorageElement *node)
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
//-------------------------------------------------------------------------
void mafVMEGroup::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();
  kframes.push_back(m_Transform->GetTimeStamp());
}
//-------------------------------------------------------------------------
mafVMEOutput *mafVMEGroup::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output == NULL)
  {
    SetOutput(mafVMEOutputNULL::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
mmgGui* mafVMEGroup::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->Divider();
	return m_Gui;
}

//-------------------------------------------------------------------------
void mafVMEGroup::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  if (maf_event->GetChannel()==MCH_UP)
  {
    switch (maf_event->GetId())
    {
      case NODE_ATTACHED_TO_TREE:
      {
        mafNode *n = mafNode::SafeDownCast((mafObject *)maf_event->GetSender());
        if (n)
        {
          mafNode *parent = n->GetParent();
          if (parent == this)
          {
            //mafMessage("Ask for shared GUI!!");
          }
        }
        Superclass::OnEvent(maf_event);
      }
      break;
      case NODE_DETACHED_FROM_TREE:
      {
        mafNode *n = mafNode::SafeDownCast((mafObject *)maf_event->GetSender());
        if (n)
        {
          mafNode *parent = n->GetParent();
          if (parent == this)
          {
            //mafMessage("Remove shared GUI!!");
          }
        }
        Superclass::OnEvent(maf_event);
      }
      break;
      default:
        Superclass::OnEvent(maf_event);//ForwardUpEvent(maf_event);
    }
  }
  else if (maf_event->GetChannel()==MCH_DOWN)
  {
    ForwardDownEvent(maf_event);
    return;
  }
}
//----------------------------------------------------------------------------
void mafVMEGroup::Print(std::ostream& os, const int indent)
//----------------------------------------------------------------------------
{
  Superclass::Print(os,indent);

  mafIndent the_indent(indent);
  os << the_indent << "Matrix:" << std::endl;
  m_Transform->Print(os,the_indent.GetNextIndent());
}

//-------------------------------------------------------------------------
char** mafVMEGroup::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEGroup.xpm"
  return mafVMEGroup_xpm;
}
