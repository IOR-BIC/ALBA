/*=========================================================================

 Program: MAF2
 Module: mafDataPipeCustom
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafDataPipeCustom_h
#define __mafDataPipeCustom_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDataPipeInterpolator.h"
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

  /** update bounds of the data pipe copying from VTK filter output bounds */
  virtual void UpdateBounds();

  /** sets the first input of the datapipe */
  void SetInput(vtkDataSet *input_dataset);

  /** sets the N-th input of the datapipe */
  void SetNthInput(int n, vtkDataSet *input_dataset);

protected:
  mafDataPipeCustom();
  virtual ~mafDataPipeCustom();

  vtkMAFDataPipe *m_VTKDataPipe; ///< VTK pipeline bridge component

private:
  mafDataPipeCustom(const mafDataPipeCustom&); // Not implemented
  void operator=(const mafDataPipeCustom&); // Not implemented  
};

#endif /* __mafDataPipeCustom_h */
 
