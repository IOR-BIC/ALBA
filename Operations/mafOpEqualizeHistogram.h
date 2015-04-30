/*=========================================================================

 Program: MAF2
 Module: mafOpEqualizeHistogram
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpEqualizeHistogram_H__
#define __mafOpEqualizeHistogram_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEVolumeGray;
class mafGUIDialog;
class mafGUIHistogramWidget;

/** 
class name : mafOpEqualizeHistogram
*/
class MAF_EXPORT mafOpEqualizeHistogram: public mafOp
{
public:

  /** constructor. */
  mafOpEqualizeHistogram(const wxString &label = "Equalize Histogram");
  /** destructor. */
  ~mafOpEqualizeHistogram(); 

  /** Precess events coming from other objects */
  /*virtual*/ void OnEvent(mafEventBase *maf_event);

  /** RTTI macro */
  mafTypeMacro(mafOpEqualizeHistogram, mafOp);

  /*virtual*/ mafOp* Copy();

  /** Return true for the acceptable vme type. */
  /*virtual*/ bool Accept(mafNode *node);

  /** Builds operation's interface. */
  /*virtual*/ void OpRun();

  /** Execute the operation. */
  /*virtual*/ void OpDo();

  /** Makes the undo for the operation. */
  /*virtual*/ void OpUndo();

protected:

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  /*virtual*/ void OpStop(int result);

  /** Perform the equalization of the histogram */
  void Algorithm();

  /** Create the operation gui */
  void CreateGui();

  void CreateHistogramDialog();
  void DeleteHistogramDialog();

  double m_Alpha;
  double m_Beta;
  int m_Radius[3];

  mafVMEVolumeGray *m_VolumeInput;
  mafVMEVolumeGray *m_VolumeOutput;

  mafGUIDialog *m_Dialog;
  mafGUIHistogramWidget *m_Histogram;


};
#endif
