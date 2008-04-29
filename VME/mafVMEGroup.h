/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGroup.h,v $
  Language:  C++
  Date:      $Date: 2008-04-29 10:47:46 $
  Version:   $Revision: 1.6 $
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
#include "mafVME.h"
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
class MAF_EXPORT mafVMEGroup : public mafVME
{
public:
  mafTypeMacro(mafVMEGroup,mafVME);

  /** Copy the contents of another mafVMEProber into this one. */
  /*virtual*/ int DeepCopy(mafNode *a);

  /** Compare with another mafVMEProber. */
  /*virtual*/ bool Equals(mafVME *vme);

  /** Precess events coming from other objects */ 
  /*virtual*/ void OnEvent(mafEventBase *maf_event);

  /** Return the right type of output.*/  
  mafVMEOutputNULL *GetVTKOutput() {return (mafVMEOutputNULL *)GetOutput();}

  /** Return the output. This create the output object on demand. */  
  /*virtual*/ mafVMEOutput *GetOutput();

  /** Set the pose matrix for the mafVMEGroup.*/
  void SetMatrix(const mafMatrix &mat);

  /** Fill the timestamp vector with the timestamp of the pose matrix.*/
  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** Print the information of the pose matrix.*/
  /*virtual*/ void Print(std::ostream& os, const int indent=0);

  /** return icon */
  static char** GetIcon();
  
protected:
  mafVMEGroup();
  virtual ~mafVMEGroup();

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();

  /*virtual*/ int InternalStore(mafStorageElement *parent);
  /*virtual*/ int InternalRestore(mafStorageElement *node);

  mafTransform *m_Transform;

private:
  mafVMEGroup(const mafVMEGroup&); // Not implemented
  void operator=(const mafVMEGroup&); // Not implemented
};
#endif
