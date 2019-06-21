/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGroup
 Authors: Marco Petrone , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEGroup_h
#define __albaVMEGroup_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEGenericAbstract.h"
#include "albaVMEOutputNULL.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaGUI;
class albaTransform;

/** albaVMEGroup - a VME use to create hierarchical assemblies of VME's.
  albaVMEGroup is a specialized VME inheriting the VME-Generic features to internally
  store pose matrices, used to create assemblies of VMEs and direct cinematic. 
  @sa albaVME albaMatrixVector */
class ALBA_EXPORT albaVMEGroup : public albaVMEGenericAbstract
{
public:
  albaTypeMacro(albaVMEGroup,albaVMEGenericAbstract);

  /** Precess events coming from other objects */ 
  virtual void OnEvent(albaEventBase *alba_event);

  /** Return the right type of output.*/  
  albaVMEOutputNULL *GetVTKOutput() {return (albaVMEOutputNULL *)GetOutput();}

  /** Return the output. This create the output object on demand. */  
  virtual albaVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();
  
  /** override superclass */
  void SetMatrix(const albaMatrix &mat);

  /** override superclass */
  void Print(std::ostream& os, const int tabs);

  /** return icon */
  bool Equals(albaVME *vme);

  /** return icon */
  int DeepCopy(albaVME *a);

protected:
  albaVMEGroup();
  virtual ~albaVMEGroup();

  /** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

  albaTransform *m_Transform;

  int InternalStore(albaStorageElement *parent);

  int InternalRestore(albaStorageElement *node);


private:
  albaVMEGroup(const albaVMEGroup&); // Not implemented
  void operator=(const albaVMEGroup&); // Not implemented
  

  
  
  
};

#endif
