/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGroup.h,v $
  Language:  C++
  Date:      $Date: 2005-06-21 09:48:19 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEGroup_h
#define __mafVMEGroup_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGenericAbstract.h"
#include "mafVMEOutputNULL.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmgGui;

/** mafVMEGroup - a VME use to create hierarchical assemblies of VME's.
  mafVMEGroup is a specialized VME inheriting the VME-Generic features to internally
  store pose matrices, used to create assemblies of VMEs and direct cinematic. 
  @sa mafVME mafMatrixVector */
class MAF_EXPORT mafVMEGroup : public mafVMEGenericAbstract
{
public:
  mafTypeMacro(mafVMEGroup,mafVMEGenericAbstract);

  /** Return the right type of output.*/  
  mafVMEOutputNULL *GetVTKOutput() {return (mafVMEOutputNULL *)GetOutput();}

  /** Return the output. This create the output object on demand. */  
  virtual mafVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();
  
protected:
  mafVMEGroup();
  virtual ~mafVMEGroup();

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();

private:
  mafVMEGroup(const mafVMEGroup&); // Not implemented
  void operator=(const mafVMEGroup&); // Not implemented
};

#endif
