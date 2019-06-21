/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputComputeWrapping
 Authors: Anupam Agrawal and Hui Wei
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputComputeWrapping_h
#define __albaVMEOutputComputeWrapping_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVMEOutputPolyline.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaGUI;

/** 
  class name: albaVMEOutputComputeWrapping
  NULL output for VME node with a VTK image output data.
  albaVMEOutputComputeWrapping is the output produced by a node generating an output
  with a VTK dataset.
*/
class ALBA_EXPORT albaVMEOutputComputeWrapping : public albaVMEOutputPolyline
{
public:
  /** constructor */
  albaVMEOutputComputeWrapping();
  /** destructor */
  virtual ~albaVMEOutputComputeWrapping();

  /** RTTI macro*/
  albaTypeMacro(albaVMEOutputComputeWrapping,albaVMEOutputPolyline);

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

protected:
  /** create the gui of the output panel for this vme output type*/
  albaGUI *CreateGui();

  albaString m_Distance;
  albaString m_Angle;
  std::vector<albaString> m_MiddlePoints;

private:
  /** copy constructor not implemented */
  albaVMEOutputComputeWrapping(const albaVMEOutputComputeWrapping&);
  /** operator = not implemented */
  void operator=(const albaVMEOutputComputeWrapping&); // Not implemented
};

#endif
