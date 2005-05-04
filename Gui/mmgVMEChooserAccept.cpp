/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgVMEChooserAccept.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:45:19 $
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

#include "mmgVmeChooserAccept.h"
#include "mafNode.h"

//----------------------------------------------------------------------------
mmgVmeChooserAccept::mmgVmeChooserAccept()
//----------------------------------------------------------------------------
{  
}
//----------------------------------------------------------------------------
mmgVmeChooserAccept::~mmgVmeChooserAccept()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmgVmeChooserAccept::Validate(mafNode *node)
//----------------------------------------------------------------------------
{
	return node != NULL;
}