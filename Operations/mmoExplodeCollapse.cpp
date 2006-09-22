/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoExplodeCollapse.cpp,v $
Language:  C++
Date:      $Date: 2006-09-22 10:11:57 $
Version:   $Revision: 1.2 $
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

#include "mmoExplodeCollapse.h"
#include "mafDecl.h"

#include "mafVMELandmarkCloud.h" 
#include "mafVMELandmark.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoExplodeCollapse);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoExplodeCollapse::mmoExplodeCollapse(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mmoExplodeCollapse::~mmoExplodeCollapse()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmoExplodeCollapse::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMELandmarkCloud));
}
//----------------------------------------------------------------------------
mafOp* mmoExplodeCollapse::Copy()   
//----------------------------------------------------------------------------
{
  mmoExplodeCollapse *cp = new mmoExplodeCollapse(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
void mmoExplodeCollapse::OpRun()   
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,OP_RUN_OK)); 
}

//----------------------------------------------------------------------------
void mmoExplodeCollapse::OpDo()
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
void mmoExplodeCollapse::OpUndo()
//----------------------------------------------------------------------------
{
  OpDo();
}
