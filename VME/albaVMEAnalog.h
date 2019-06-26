/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEAnalog
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEAnalog_h
#define __albaVMEAnalog_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVMEScalarMatrix.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMEOutputScalarMatrix;


/** VME child of albaVMEScalarMatrix, for EMG data. Refactor CreateGui and Print methods not to show parameters used for VTK representation */
class ALBA_EXPORT albaVMEAnalog : public albaVMEScalarMatrix
{
public:
  albaTypeMacro(albaVMEAnalog, albaVMEScalarMatrix);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Get the time bounds for the time varying VME based on scalar matrix.*/
  void GetLocalTimeBounds(albaTimeStamp tbounds[2]);

  /** Return true if this VME has more than one time stamp, corresponding to the number of elements in the first row.*/
  bool IsAnimated();

  void GetTimeBounds(albaTimeStamp tbounds[2]); 

  /** Return the list of timestamps for this VME. Timestamps list is 
  obtained by extracting the first row of the scalar matrix.*/
  void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

protected:
  albaVMEAnalog();
  virtual ~albaVMEAnalog();

  /** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

  albaTimeStamp    m_CurrentTime;  ///< the time parameter for generation of the output

};
#endif
