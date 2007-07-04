/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEEmg.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-04 13:44:37 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
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


#include "medVMEEmg.h"
#include "mmgGui.h"

#include "mafDataVector.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMEEmg)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
medVMEEmg::medVMEEmg()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
medVMEEmg::~medVMEEmg()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
mmgGui* medVMEEmg::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
 
  return m_Gui;
}

//-----------------------------------------------------------------------
void medVMEEmg::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}