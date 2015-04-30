/*=========================================================================

 Program: MAF2
 Module: mafOpImporterLandmark
 Authors: Daniele Giunchi, Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterLandmark_H__
#define __mafOpImporterLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMELandmarkCloud;  
class mafEvent;
//----------------------------------------------------------------------------
// mafOpImporterLandmark :
//----------------------------------------------------------------------------
/** 
class name: mafOpImporterLandmark
this class provides the landmark importing written in the following format:
1) a line with initial # is a comment
2) Before a sequence of landmark it can be a line with "Time XXX" where XXX is a number current of timestep
     After the list of landmark is finished for that time, a new line with Time XXY or similar will follow.
     If there's not time, the cloud is considered time-INVARIANT
3) the line with landmark data are:
     nameOfLandmark x y z
*/
class MAF_EXPORT mafOpImporterLandmark : public mafOp
{
public:
  /** object cosntructor */
	mafOpImporterLandmark(wxString label);
  /** object destructor */
	~mafOpImporterLandmark();
  /** method for clone object */
	mafOp* Copy();
  /** method for catch the dispatched events */
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
  The line with landmark pose can be with any type of char separation (provided it has been selected by the user)
  */
  void ReadWithoutTag();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};


	/** Makes the undo for the operation. */
	//void OpUndo();                       // gia' implementata in mafOp

protected:
  
  enum ID_LANDMARK_IMPORTER
  {
    ID_TYPE_FILE = MINID,
    ID_TYPE_SEPARATION,
    ID_ENABLE_STRING,
    ID_STRING_SEPARATION,
    MINID,
  };

  int m_TypeSeparation;
  int m_EnableString;
  mafString m_StringSeparation;

  wxString m_FileDir;
	wxString m_File;
	mafVMELandmarkCloud  *m_VmeCloud;
  int m_TagFileFlag;

};
#endif
