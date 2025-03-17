/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeCustom
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaDataPipeCustom_h
#define __albaDataPipeCustom_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDataPipeInterpolator.h"
//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class vtkALBADataPipe;
class vtkAlgorithmOutput;

/** a data pipe which simply forwards VTK update events to the VME.
  This data pipe creates an internal vtkSource objects and links to it to 
  receive update events and forward them to the VME.
  @sa vtkALBADataPipe
*/
class ALBA_EXPORT albaDataPipeCustom : public albaDataPipe
{
public:
  albaTypeMacro(albaDataPipeCustom,albaDataPipe);

  /** process events coming from vtkALBADataPipe bridge component */
  void OnEvent(albaEventBase *e);

  /** return the VTK dataset generated as output to this data pipe */
  virtual vtkDataSet *GetVTKData();

  /** return the bridge object between VTK datapipe and ALBA update mechanism */
  vtkALBADataPipe *GetVTKDataPipe();

  /** update the data pipe output */
  virtual void Update();

  /** update bounds of the data pipe copying from VTK filter output bounds */
  virtual void UpdateBounds();

  /** sets the first input of the datapipe */
  void SetInput(vtkDataSet *input_dataset);

  /** sets the N-th input of the datapipe */
  void SetNthInput(int n, vtkDataSet *input_dataset);

	/** sets the first input connection of the datapipe */
	void SetInputConnection(vtkAlgorithmOutput *input);

	/** sets the N-th input connection of the datapipe */
	void SetInputConnection(int n, vtkAlgorithmOutput *input);

protected:
  albaDataPipeCustom();
  virtual ~albaDataPipeCustom();

  vtkALBADataPipe *m_VTKDataPipe; ///< VTK pipeline bridge component

private:
  albaDataPipeCustom(const albaDataPipeCustom&); // Not implemented
  void operator=(const albaDataPipeCustom&); // Not implemented  
};

#endif /* __albaDataPipeCustom_h */
 
