/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafIndent.h,v $
  Language:  C++
  Date:      $Date: 2005-01-15 19:23:54 $
  Version:   $Revision: 1.2 $
  Authors:   originally based on vtkIndent (www.vtk.org), rewritten by Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafIndent_h
#define __mafIndent_h

#include "mafConfigure.h"
#include "mafBase.h" 
#include <ostream>

class mafIndent;
MAF_EXPORT std::ostream& operator<<(std::ostream& os, const mafIndent& o);

/** mafIndent - a simple class to control print indentation.
  mafIndent is used to control indentation during the chaining print 
  process. This way nested objects can correctly indent themselves.*/
class MAF_EXPORT mafIndent : public mafBase
{
public:
  mafIndent(int ind=0);

  /** return next level of indentetion */
  virtual const char *GetTypeName() {return "mafIndent";};

  /**
    Determine the next indentation level. Keep indenting by two until the 
    max of forty. */
  mafIndent GetNextIndent();

  operator const int() const {return m_Indent;}
  /** Print out the indentation. Basically output a bunch of spaces. */
  friend MAF_EXPORT std::ostream& operator<<(std::ostream& os, const mafIndent& o);

protected:
  int m_Indent;  
};

#endif

