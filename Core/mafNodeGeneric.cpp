/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeGeneric.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:58:27 $
  Version:   $Revision: 1.2 $
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
