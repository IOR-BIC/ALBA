/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: mafIndent.cpp,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "mafIndent.h"
#include "mafObjectFactory.h"

#define MAF_STD_INDENT 2
#define MAF_NUMBER_OF_BLANKS 40

static const char blanks[MAF_NUMBER_OF_BLANKS+1]="                                        ";

// Determine the next indentation level. Keep indenting by two until the 
// max of forty.
mafIndent mafIndent::GetNextIndent()
{
  int indent = this->Indent + MAF_STD_INDENT;
  if ( indent > MAF_NUMBER_OF_BLANKS )
    {
    indent = MAF_NUMBER_OF_BLANKS;
    }
  return indent;
}
 
// Print out the indentation. Basically output a bunch of spaces.
ostream& operator<<(ostream& os, const mafIndent& ind)
{
  os << blanks + (MAF_NUMBER_OF_BLANKS-ind.Indent) ;
  return os;
}

