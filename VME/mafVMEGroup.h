/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGroup.h,v $
  Language:  C++
  Date:      $Date: 2008-07-16 11:25:56 $
  Version:   $Revision: 1.8 $
  Authors:   Marco Petrone , Stefano Perticoni
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
  int DeepCopy(mafNode *a);

protected:
  mafVMEGroup();
  virtual ~mafVMEGroup();

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();

  mafTransform *m_Transform;

  int InternalStore(mafStorageElement *parent);

  int InternalRestore(mafStorageElement *node);


private:
  mafVMEGroup(const mafVMEGroup&); // Not implemented
  void operator=(const mafVMEGroup&); // Not implemented
  

  
  
  
};

#endif
