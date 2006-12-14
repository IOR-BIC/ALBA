/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGroup.cpp,v $
  Language:  C++
  Date:      $Date: 2006-12-14 09:55:55 $
  Version:   $Revision: 1.5 $
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
#include "mafNode.h"
#include "mafVMEOutputNULL.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEGroup)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEGroup::mafVMEGroup()
//-------------------------------------------------------------------------
{
  // The output is created on demand in GetOutput() to avoid
  // subclasses to have to destroy base class output
}

//-------------------------------------------------------------------------
mafVMEGroup::~mafVMEGroup()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGenericAbstract
}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMEGroup::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
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
char** mafVMEGroup::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEGroup.xpm"
  return mafVMEGroup_xpm;
}
