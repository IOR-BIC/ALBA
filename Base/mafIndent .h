/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafIndent\040.h,v $
  Language:  C++
  Date:      $Date: 2004-11-15 08:19:07 $
  Version:   $Revision: 1.1 $
  Authors:   originally based on vtkString (www.vtk.org), rewritten Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafIndent_h
#define __mafIndent_h

#include "mafConfigure.h"
namespace std
{
  class ostream;
}

class mafIndent;
MAF_EXPORT ostream& operator<<(ostream& os, const mafIndent& o);

/** mafIndent - a simple class to control print indentation.
  mafIndent is used to control indentation during the chaining print 
  process. This way nested objects can correctly indent themselves.*/
class MAF_EXPORT mafIndent
{
public:
  mafIndent(int ind=0) {this->Indent=ind;};

  virtual const char *GetClassName() {return "mafIndent";};

  /**
    Determine the next indentation level. Keep indenting by two until the 
    max of forty. */
  mafIndent GetNextIndent();

  //BTX
  /** Print out the indentation. Basically output a bunch of spaces. */
  friend MAF_EXPORT ostream& operator<<(ostream& os, const mafIndent& o);
  //ETX

protected:
  int Indent;  
};

#endif
