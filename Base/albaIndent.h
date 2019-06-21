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

#ifndef __albaIndent_h
#define __albaIndent_h

#include "albaConfigure.h"
#include "albaBase.h" 
#include <ostream>

class albaIndent;
ALBA_EXPORT std::ostream& operator<<(std::ostream& os, const albaIndent& o);

/** albaIndent - a simple class to control print indentation.
  albaIndent is used to control indentation during the chaining print 
  process. This way nested objects can correctly indent themselves.*/
class ALBA_EXPORT albaIndent : public albaBase
{
public:
  albaIndent(int ind=0);

  /** return next level of indentetion */
  virtual const char *GetTypeName() {return "albaIndent";};

  /**
    Determine the next indentation level. Keep indenting by two until the 
    max of forty. */
  albaIndent GetNextIndent();

  operator const int() const {return m_Indent;}
  /** Print out the indentation. Basically output a bunch of spaces. */
  friend ALBA_EXPORT std::ostream& operator<<(std::ostream& os, const albaIndent& o);

protected:
  int m_Indent;  
};

#endif

