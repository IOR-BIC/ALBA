/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputComputeWrapping
 Authors: Anupam Agrawal and Hui Wei
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEOutputComputeWrapping_h
#define __mafVMEOutputComputeWrapping_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafVMEOutputPolyline.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafGUI;

/** 
  class name: mafVMEOutputComputeWrapping
  NULL output for VME node with a VTK image output data.
  mafVMEOutputComputeWrapping is the output produced by a node generating an output
  with a VTK dataset.
*/
class MAF_EXPORT mafVMEOutputComputeWrapping : public mafVMEOutputPolyline
{
public:
  /** constructor */
  mafVMEOutputComputeWrapping();
  /** destructor */
  virtual ~mafVMEOutputComputeWrapping();

  /** RTTI macro*/
  mafTypeMacro(mafVMEOutputComputeWrapping,mafVMEOutputPolyline);

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

protected:
  /** create the gui of the output panel for this vme output type*/
  mafGUI *CreateGui();

  mafString m_Distance;
  mafString m_Angle;
  std::vector<mafString> m_MiddlePoints;

private:
  /** copy constructor not implemented */
  mafVMEOutputComputeWrapping(const mafVMEOutputComputeWrapping&);
  /** operator = not implemented */
  void operator=(const mafVMEOutputComputeWrapping&); // Not implemented
};

#endif
