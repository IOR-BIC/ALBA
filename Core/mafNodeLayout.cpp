/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeLayout.cpp,v $
  Language:  C++
  Date:      $Date: 2006-12-07 14:38:37 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
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


#include "mafNodeLayout.h"
#include "mmaApplicationLayout.h"
#include "mafIndent.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeLayout)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafNodeLayout::mafNodeLayout()
//-------------------------------------------------------------------------
{
  m_AppLayout = NULL;
}

//-------------------------------------------------------------------------
mmaApplicationLayout *mafNodeLayout::GetLayout()
//-------------------------------------------------------------------------
{
  if (m_AppLayout == NULL)
  {
    m_AppLayout = (mmaApplicationLayout *)GetAttribute("ApplicationLayout");
  }
  return m_AppLayout;
}

//-------------------------------------------------------------------------
void mafNodeLayout::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  mafNode::Print(os,tabs);


}
