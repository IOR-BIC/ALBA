/*=========================================================================

 Program: MAF2
 Module: mafVMEGroup
 Authors: Marco Petrone , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class mafGUI;
class mafTransform;

/** mafVMEGroup - a VME use to create hierarchical assemblies of VME's.
  mafVMEGroup is a specialized VME inheriting the VME-Generic features to internally
  store pose matrices, used to create assemblies of VMEs and direct cinematic. 
  @sa mafVME mafMatrixVector */
class MAF_EXPORT mafVMEGroup : public mafVMEGenericAbstract
{
public:
  mafTypeMacro(mafVMEGroup,mafVMEGenericAbstract);

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Return the right type of output.*/  
  mafVMEOutputNULL *GetVTKOutput() {return (mafVMEOutputNULL *)GetOutput();}

  /** Return the output. This create the output object on demand. */  
  virtual mafVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();
  
  /** override superclass */
  void SetMatrix(const mafMatrix &mat);

  /** override superclass */
  void Print(std::ostream& os, const int tabs);

  /** return icon */
  bool Equals(mafVME *vme);

  /** return icon */
  int DeepCopy(mafVME *a);

protected:
  mafVMEGroup();
  virtual ~mafVMEGroup();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  mafTransform *m_Transform;

  int InternalStore(mafStorageElement *parent);

  int InternalRestore(mafStorageElement *node);


private:
  mafVMEGroup(const mafVMEGroup&); // Not implemented
  void operator=(const mafVMEGroup&); // Not implemented
  

  
  
  
};

#endif
