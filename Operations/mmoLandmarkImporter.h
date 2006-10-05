/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoLandmarkImporter.h,v $
  Language:  C++
  Date:      $Date: 2006-10-05 08:44:58 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoLandmarkImporter_H__
#define __mmoLandmarkImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMELandmarkCloud;  
class mafEvent;
//----------------------------------------------------------------------------
// mmoLandmarkImporter :
//----------------------------------------------------------------------------
/** */
class mmoLandmarkImporter : public mafOp
{
public:
	mmoLandmarkImporter(wxString label);
	~mmoLandmarkImporter(); 
	mafOp* Copy();
  virtual void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop	(int result);

  /** Read the file.
  the format of the file admits some speficics.
  1) a line with initial # is a comment
  2) Before a sequence of landmark it can be a line with "Time XXX" where XXX is a number current of timestep
     After the list of landmark is finished for that time, a new line with Time XXY or similar will follow.
     If there's not time, the cloud is considered time-INVARIANT
  3) the line with landmark data are:
     nameOfLandmark x y z
  */
  void Read();

  /** Read the file.
  the format of the file admits some speficics.
  1) the line with landmark data are:
     nameOfLandmark x y z
  */
  void ReadWithoutTag();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};


	/** Makes the undo for the operation. */
	//void OpUndo();                       // gia' implementata in mafOp

protected:
  wxString m_FileDir;
	wxString m_File;
	mafVMELandmarkCloud  *m_VmeCloud;
  int m_TagFileFlag;

};
#endif
