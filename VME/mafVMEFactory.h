/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEFactory.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 16:43:20 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEFactory_h
#define __mafVMEFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafNodeFactory.h"

/** Object factory for Nodes.
  To make a new VME available in the MAF it must be plugged inside a factory, in particular
  this factory must be of type mafVMEFactory to be able to retrieve the list of nodes plugged
  in the factory. Also when using mafPlugNode<node_type> the node icon is plugged inside the
  the MAF picture factory. */
class MAF_EXPORT mafVMEFactory : public mafNodeFactory
{
public: 
  mafTypeMacro(mafVMEFactory,mafNodeFactory);
  virtual const char* GetMAFSourceVersion() const;
  virtual const char* GetDescription() const;

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();

  mafVME *CreateVMEInstance(const char *type_name);
  
protected:
  mafVMEFactory();
  ~mafVMEFactory() { }
  
private:
  mafVMEFactory(const mafVMEFactory&);  // Not implemented.
  void operator=(const mafVMEFactory&);  // Not implemented.
};

#endif
