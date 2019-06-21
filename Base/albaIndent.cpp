/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaIndent
 Authors: originally based on vtkIndent (www.vtk.org), rewritten by Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaIndent.h"
#include <ostream>

#define ALBA_STD_INDENT 2
#define ALBA_NUMBER_OF_TABS 20

static const char blanks[ALBA_NUMBER_OF_TABS*ALBA_STD_INDENT+1]="                                        ";

//------------------------------------------------------------------------------
albaIndent::albaIndent(int ind)
//------------------------------------------------------------------------------
{
  m_Indent=ind;
};

//------------------------------------------------------------------------------
// Determine the next indentation level. Keep indenting by two until the 
// max of forty.
albaIndent albaIndent::GetNextIndent()
//------------------------------------------------------------------------------
{
  int indent = m_Indent + 1;
  if ( indent > ALBA_NUMBER_OF_TABS )
  {
    indent = ALBA_NUMBER_OF_TABS;
  }
  return indent;
}

//------------------------------------------------------------------------------ 
// Print out the indentation. Basically output a bunch of spaces.
std::ostream& operator<<(std::ostream& os, const albaIndent& ind)
//------------------------------------------------------------------------------
{
  os << blanks + (ALBA_NUMBER_OF_TABS-ind.m_Indent)*ALBA_STD_INDENT ;
  return os;
}

