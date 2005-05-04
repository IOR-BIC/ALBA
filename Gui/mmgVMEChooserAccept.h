/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgVMEChooserAccept.h,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:45:20 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

//----------------------------------------------------------------------------
// mmgVmeChooserAccept :
/*
Display a modal dialog filled with a vme tree and
return the choosed vme. The tree is filled from
the vme passed in the constructor with its children.
*/
//----------------------------------------------------------------------------

#ifndef __mmgVmeChooserAccept_H__
#define __mmgVmeChooserAccept_H__

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// mmgVmeChooserAccept :
//----------------------------------------------------------------------------
class mmgVmeChooserAccept
{
public:
	 mmgVmeChooserAccept();
	~mmgVmeChooserAccept();
  
  virtual bool Validate(mafNode *node);
};
#endif