/*=========================================================================

 Program: MAF2
 Module: mafOpApplyTrajectory
 Authors: Roberto Mucci, Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpApplyTrajectory_H__
#define __mafOpApplyTrajectory_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// mafOpApplyTrajectory :
//----------------------------------------------------------------------------
/** Import an ACII file containig roto-tranlation information and apply a trajectory to a static VME. 
    Modified 30/11/2010 by Simone Brazzale:
    Added possibility to apply roto-translation from a time-varying VME to a static VME */

class MAF_EXPORT mafOpApplyTrajectory : public mafOp
{
public:
  //----------------------------------------------------------------------------
  // Constants:
  //----------------------------------------------------------------------------
  enum GUI_IDS
  {
    ID_OPEN_FILE = MINID,
    ID_SELECT_VME,
	ID_HELP,
  };

  /** Contructor. */
	mafOpApplyTrajectory(const wxString &label = "Apply trajectory");

  /** Desctructor. */
	~mafOpApplyTrajectory(); 

  /** type macro for RTTI and instance creation*/
  mafTypeMacro(mafOpApplyTrajectory, mafOp);

  /** Copy the operation. */
	mafOp* Copy();

  /** Return true for the vme to which the trajectories should be applied. */
  bool Accept(mafVME* vme);

  /** Returns true for the vme from which the trajectories should be red. (Callback function) */
  static bool AcceptInputVME(mafVME* node);

	/** Builds operation's interface. */
	void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Undo operation. */
  void OpUndo(); 

  /** Stop the operation. */
  void OpStop(int result);

  /** Wait for events */
  void OnEvent(mafEventBase *maf_event);

  /** Create GUI. */
  void CreateGui();

  /** Read the file.
  the format of the file admits some specifics.
  1) Ignore the first line
  2) the first element of each line is Time, then x y z rx ry rz 
  3) the separator is a tab or a blank space
  */
  int Read();

  /** Apply transform from a time-varying VME */
  int ApplyTrajectoriesFromVME();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};

  /** Set the VME from which apply the transformation */
  void SetVME(mafVME* vme){m_VME = vme;};

protected:

  mafMatrix *m_OriginalMatrix;
  mafString m_FileDir;
	mafString m_File;
  mafVME* m_VME;
};
#endif
