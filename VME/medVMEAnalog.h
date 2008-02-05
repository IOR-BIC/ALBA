/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEAnalog.h,v $
  Language:  C++
  Date:      $Date: 2008-02-05 11:31:00 $
  Version:   $Revision: 1.2 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medVMEAnalog_h
#define __medVMEAnalog_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEScalarMatrix.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMEOutputScalarMatrix;


/** VME child of mafVMEScalarMatrix, for EMG data. Refactor CreateGui and Print methods not to show parameters used for VTK representation */
class MAF_EXPORT medVMEAnalog : public mafVMEScalarMatrix
{
public:
  mafTypeMacro(medVMEAnalog, mafVMEScalarMatrix);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Get the time bounds for the time varying VME based on scalar matrix.*/
  void GetLocalTimeBounds(mafTimeStamp tbounds[2]);

  /** Return true if this VME has more than one time stamp, corresponding to the number of elements in the first row.*/
  bool IsAnimated();

  void GetTimeBounds(mafTimeStamp tbounds[2]); 

  /** Return the list of timestamps for this VME. Timestamps list is 
  obtained by extracting the first row of the scalar matrix.*/
  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

protected:
  medVMEAnalog();
  virtual ~medVMEAnalog();

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();

  mafTimeStamp    m_CurrentTime;  ///< the time parameter for generation of the output

};
#endif
