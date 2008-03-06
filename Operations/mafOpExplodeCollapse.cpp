/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpExplodeCollapse.cpp,v $
Language:  C++
Date:      $Date: 2008-03-06 11:55:06 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpExplodeCollapse.h"
#include "mafDecl.h"

#include "mafVMELandmarkCloud.h" 
#include "mafVMELandmark.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExplodeCollapse);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExplodeCollapse::mafOpExplodeCollapse(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpExplodeCollapse::~mafOpExplodeCollapse()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOpExplodeCollapse::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMELandmarkCloud));
}
//----------------------------------------------------------------------------
mafOp* mafOpExplodeCollapse::Copy()   
//----------------------------------------------------------------------------
{
  mafOpExplodeCollapse *cp = new mafOpExplodeCollapse(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
void mafOpExplodeCollapse::OpRun()   
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,OP_RUN_OK)); 
}

//----------------------------------------------------------------------------
void mafOpExplodeCollapse::OpDo()
//----------------------------------------------------------------------------
{
  mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Input);

  if (cloud->IsOpen())
  {
	  cloud->Close(); 
	}
  else
  {
    cloud->Open();  
  }
	mafEventMacro(mafEvent(this,VME_MODIFIED,cloud)); //update the icon in the tree
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpExplodeCollapse::OpUndo()
//----------------------------------------------------------------------------
{
  OpDo();
}
