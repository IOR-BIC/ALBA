/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafIndent.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-06 21:19:38 $
  Version:   $Revision: 1.5 $
  Authors:   originally based on vtkIndent (www.vtk.org), rewritten by Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafIndent.h"
#include <ostream>

#define MAF_STD_INDENT 2
#define MAF_NUMBER_OF_TABS 20

static const char blanks[MAF_NUMBER_OF_TABS*MAF_STD_INDENT+1]="                                        ";

//------------------------------------------------------------------------------
mafIndent::mafIndent(int ind)
//------------------------------------------------------------------------------
{
  m_Indent=ind;
};

//------------------------------------------------------------------------------
// Determine the next indentation level. Keep indenting by two until the 
// max of forty.
mafIndent mafIndent::GetNextIndent()
//------------------------------------------------------------------------------
{
  int indent = m_Indent + 1;
  if ( indent > MAF_NUMBER_OF_TABS )
  {
    indent = MAF_NUMBER_OF_TABS;
  }
  return indent;
}

//------------------------------------------------------------------------------ 
// Print out the indentation. Basically output a bunch of spaces.
std::ostream& operator<<(std::ostream& os, const mafIndent& ind)
//------------------------------------------------------------------------------
{
  os << blanks + (MAF_NUMBER_OF_TABS-ind.m_Indent)*MAF_STD_INDENT ;
  return os;
}

