/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafIndent.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-30 14:16:58 $
  Version:   $Revision: 1.2 $
  Authors:   originally based on vtkIndent (www.vtk.org), rewritten by Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafIndent.h"
#include <ostream>

#define MAF_STD_INDENT 2
#define MAF_NUMBER_OF_BLANKS 40

static const char blanks[MAF_NUMBER_OF_BLANKS+1]="                                        ";

//------------------------------------------------------------------------------
mafIndent::mafIndent(int ind)
//------------------------------------------------------------------------------
{
  m_Indent=ind*MAF_STD_INDENT;
};

//------------------------------------------------------------------------------
// Determine the next indentation level. Keep indenting by two until the 
// max of forty.
mafIndent mafIndent::GetNextIndent()
//------------------------------------------------------------------------------
{
  int indent = m_Indent + MAF_STD_INDENT;
  if ( indent > MAF_NUMBER_OF_BLANKS )
  {
    indent = MAF_NUMBER_OF_BLANKS;
  }
  return indent;
}

//------------------------------------------------------------------------------ 
// Print out the indentation. Basically output a bunch of spaces.
std::ostream& operator<<(std::ostream& os, const mafIndent& ind)
//------------------------------------------------------------------------------
{
  os << blanks + (MAF_NUMBER_OF_BLANKS-ind.m_Indent) ;
  return os;
}

