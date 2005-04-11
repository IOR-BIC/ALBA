/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeGeneric.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 16:40:23 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafNodeGeneric.h"
#include "mafAttribute.h"
#include "mafIndent.h"
#include "mafStorage.h"
#include "mafEventIO.h"
#include <sstream>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeGeneric)
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
char** mafNodeGeneric::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafNodeGeneric.xpm"
  return mafNodeGeneric_xpm;
}
