/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGroup
 Authors: Marco Petrone , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaVMEGroup.h"
#include "albaGUI.h"

#include "albaTransform.h"
#include "albaMatrixVector.h"
#include "albaVME.h"
#include "albaVMEOutputNULL.h"

const int DEBUG_MODE = false;

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEGroup)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEGroup::albaVMEGroup()
//-------------------------------------------------------------------------
{
  // The output is created on demand in GetOutput() to avoid
  // subclasses to have to destroy base class output
  albaNEW(m_Transform);
  m_MatrixVector->SetMatrix(m_Transform->GetMatrix());

  albaVMEOutputNULL *output=albaVMEOutputNULL::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);
}

//-------------------------------------------------------------------------
albaVMEGroup::~albaVMEGroup()
//-------------------------------------------------------------------------
{
  albaDEL(m_Transform);
  
  SetOutput(NULL);

  // data pipe destroyed in albaVME
  // data vector destroyed in albaVMEGenericAbstract
}

//-------------------------------------------------------------------------
albaVMEOutput *albaVMEGroup::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputNULL::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
albaGUI* albaVMEGroup::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->Divider();
	return m_Gui;
}

//-------------------------------------------------------------------------
void albaVMEGroup::OnEvent(albaEventBase *alba_event)
//-------------------------------------------------------------------------
{
  if (alba_event->GetChannel()==MCH_UP)
  {
    switch (alba_event->GetId())
    {
      case NODE_ATTACHED_TO_TREE:
      {
        albaVME *n = albaVME::SafeDownCast((albaObject *)alba_event->GetSender());
        if (n)
        {
          albaVME *parent = n->GetParent();
          if (parent == this)
          {
            //albaMessage("Ask for shared GUI!!");
          }
        }
        Superclass::OnEvent(alba_event);
      }
      break;
      case NODE_DETACHED_FROM_TREE:
      {
        albaVME *n = albaVME::SafeDownCast((albaObject *)alba_event->GetSender());
        if (n)
        {
          albaVME *parent = n->GetParent();
          if (parent == this)
          {
            //albaMessage("Remove shared GUI!!");
          }
        }
        Superclass::OnEvent(alba_event);
      }
      break;
      default:
        Superclass::OnEvent(alba_event);//ForwardUpEvent(alba_event);
    }
  }
  else if (alba_event->GetChannel()==MCH_DOWN)
  {
    ForwardDownEvent(alba_event);
    return;
  }
  else if (alba_event->GetChannel() == MCH_NODE)
  {
	  Superclass::OnEvent(alba_event);
  }
}

//-------------------------------------------------------------------------
char** albaVMEGroup::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "albaVMEGroup.xpm"
  return albaVMEGroup_xpm;
}

//-------------------------------------------------------------------------
void albaVMEGroup::SetMatrix(const albaMatrix &mat)
//-------------------------------------------------------------------------
{
  Superclass::SetMatrix(mat);
  m_Transform->SetMatrix(mat);
}

//-----------------------------------------------------------------------
int albaVMEGroup::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{ 
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Storing matrix:"  << std::endl;
    m_Transform->Print(stringStream);
    albaLogMessage(stringStream.str().c_str());
  }

  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return ALBA_OK;
  }
  return ALBA_ERROR;
  
  
}

//-----------------------------------------------------------------------
int albaVMEGroup::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==ALBA_OK)
  {
    albaMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==ALBA_OK)
    {

      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "Restoring group matrix:"  << std::endl;
        matrix.Print(stringStream);
        albaLogMessage(stringStream.str().c_str());
      }

      this->SetMatrix(matrix);
      return ALBA_OK;
    }
    else
    {
      // code handling for old msf without group pose matrix serialization
      this->SetMatrix(matrix);
      if (DEBUG_MODE)
        {
          std::ostringstream stringStream;
          stringStream << "BEWARE!!! Opening an old MSF without group matrix serialized:\
restoring group matrix as:"  << std::endl;
          matrix.Print(stringStream);
          stringStream << "Please report any problem with old MSF containing groups!!!"  << std::endl;
          albaLogMessage(stringStream.str().c_str());
        }
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}

//-----------------------------------------------------------------------
void albaVMEGroup::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);

  albaMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}

//-------------------------------------------------------------------------
bool albaVMEGroup::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    return ret = (m_Transform->GetMatrix()==((albaVMEGroup *)vme)->m_Transform->GetMatrix());
  }
  return ret;
}

//-------------------------------------------------------------------------
int albaVMEGroup::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMEGroup *group = albaVMEGroup::SafeDownCast(a);
    
    m_Transform->SetMatrix(group->m_Transform->GetMatrix());
    
    return ALBA_OK;
  }  

  return ALBA_ERROR;
}
