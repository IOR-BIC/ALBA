/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeRoot.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 12:59:55 $
  Version:   $Revision: 1.7 $
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


#include "mafNodeRoot.h"
#include "mafAttribute.h"
#include "mafIndent.h"
#include "mafStorage.h"
#include "mafEventIO.h"
#include <sstream>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeRoot)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void mafNodeRoot::OnEvent(mafEventBase *e)
//-------------------------------------------------------------------------
{
  if (e->GetChannel()==MCH_UP)
  {
    mafNodeRoot::OnRootEvent(e);
  }
  else
  {
    Superclass::OnEvent(e);
  }
}

//-------------------------------------------------------------------------
void mafNodeRoot::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafNode::Print(os,tabs);
  mafRoot::Print(os,tabs);
}

//-------------------------------------------------------------------------
char** mafNodeRoot::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafNodeRoot.xpm"
  return mafNodeRoot_xpm;
}
