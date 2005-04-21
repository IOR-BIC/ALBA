/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataPipeCustom.h,v $
  Language:  C++
  Date:      $Date: 2005-04-21 14:05:14 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafDataPipeCustom_h
#define __mafDataPipeCustom_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDataInterpolator.h"
//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class vtkMAFDataPipe;

/** a data pipe which simply forwards VTK update events to the VME.
  This data pipe creates an internal vtkSource objects and links to it to 
  receive update events and forward them to the VME.
  @sa vtkMAFDataPipe
*/
class MAF_EXPORT mafDataPipeCustom : public mafDataPipe
{
public:
  mafTypeMacro(mafDataPipeCustom,mafDataPipe);

  /** process events coming from vtkMAFDataPipe bridge component */
  void OnEvent(mafEventBase *e);

  /** return the VTK dataset generated as output to this data pipe */
  virtual vtkDataSet *GetVTKData();

  /** return the bridge object between VTK datapipe and MAF update mechanism */
  vtkMAFDataPipe *GetVTKDataPipe();

  /** update the data pipe output */
  virtual void Update();

protected:
  mafDataPipeCustom();
  virtual ~mafDataPipeCustom();

  vtkMAFDataPipe *m_VTKDataPipe; ///< VTK pipeline bridge component

private:
  mafDataPipeCustom(const mafDataPipeCustom&); // Not implemented
  void operator=(const mafDataPipeCustom&); // Not implemented  
};

#endif /* __mafDataPipeCustom_h */
 
