/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpApplyTrajectory.h,v $
  Language:  C++
  Date:      $Date: 2009-05-18 14:49:32 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpApplyTrajectory_H__
#define __mafOpApplyTrajectory_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// mafOpApplyTrajectory :
//----------------------------------------------------------------------------
/** Import an ACII file containig roto-tranlation information and apply a trajectory to a static VME. */
class mafOpApplyTrajectory : public mafOp
{
public:
  /** Contructor. */
	mafOpApplyTrajectory(const wxString &label = "Apply trajectory");

  /** Desctructor. */
	~mafOpApplyTrajectory(); 

  /** type macro for RTTI and instance creation*/
  mafTypeMacro(mafOpApplyTrajectory, mafOp);

  /** Copy the operation. */
	mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme);

	/** Builds operation's interface. */
	void OpRun();

  /** Undo operation. */
  void OpUndo(); 

  /** Read the file.
  the format of the file admits some specifics.
  1) Ignore the first line
  2) the first element of each line is Time, then x y z rx ry rz 
  3) the separator is a tab or a blank space
  */
  int Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};

protected:

  mafMatrix *m_OriginalMatrix;
  wxString m_FileDir;
	wxString m_File;
};
#endif
