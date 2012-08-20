/*=========================================================================

 Program: MAF2Medical
 Module: medVMEOutputComputeWrapping
 Authors: Anupam Agrawal and Hui Wei
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __medVMEOutputComputeWrapping_h
#define __medVMEOutputComputeWrapping_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medVMEDefines.h"
#include "mafVMEOutputPolyline.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafGUI;

/** 
  class name: medVMEOutputComputeWrapping
  NULL output for VME node with a VTK image output data.
  medVMEOutputComputeWrapping is the output produced by a node generating an output
  with a VTK dataset.
*/
class MED_VME_EXPORT medVMEOutputComputeWrapping : public mafVMEOutputPolyline
{
public:
  /** constructor */
  medVMEOutputComputeWrapping();
  /** destructor */
  virtual ~medVMEOutputComputeWrapping();

  /** RTTI macro*/
  mafTypeMacro(medVMEOutputComputeWrapping,mafVMEOutputPolyline);

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
  medVMEOutputComputeWrapping(const medVMEOutputComputeWrapping&);
  /** operator = not implemented */
  void operator=(const medVMEOutputComputeWrapping&); // Not implemented
};

#endif
