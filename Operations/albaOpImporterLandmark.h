/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterLandmark
 Authors: Daniele Giunchi, Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterLandmark_H__
#define __albaOpImporterLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMELandmarkCloud;  
class albaEvent;
//----------------------------------------------------------------------------
// albaOpImporterLandmark :
//----------------------------------------------------------------------------
/** 
class name: albaOpImporterLandmark
this class provides the landmark importing written in the following format:
1) a line with initial # is a comment
2) Before a sequence of landmark it can be a line with "Time XXX" where XXX is a number current of timestep
	After the list of landmark is finished for that time, a new line with Time XXY or similar will follow.
	If there's not time, the cloud is considered time-INVARIANT
3) if m_OnlyCoordinates is true the line with landmark data are:
	nameOfLandmark x y z
	else
	x y z
*/
class ALBA_EXPORT albaOpImporterLandmark : public albaOp
{
public:
  /** object constructor */
	albaOpImporterLandmark(wxString label);
  /** object destructor */
	~albaOpImporterLandmark();
  /** method for clone object */
	albaOp* Copy();
  /** method for catch the dispatched events */
  virtual void OnEvent(albaEventBase *alba_event);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop	(int result);

  /** Read the file  */ 
  void Read();

	/** Converts the line */
	void ConvertLine(char *line, int count, char separator, wxString &name, double &x, double &y, double &z);

	/** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};
	
	/** Returns OnlyCoordinates mode */
	int GetOnlyCoordinates() const { return m_OnlyCoordinates; }

	/** Sets OnlyCoordinates mode */
	void SetOnlyCoordinates(int onlyCoordinates) { m_OnlyCoordinates = onlyCoordinates; }
		
	/** Returns TypeSeparation */
	int GetTypeSeparation() const { return m_TypeSeparation; }

	/** Sets TypeSeparation values are 0 = comma, 1 = space, 2 = semicolon, 3 = tab */
	void SetTypeSeparation(int typeSeparation) { m_TypeSeparation = typeSeparation; }

protected:
  
  enum ID_LANDMARK_IMPORTER
  {
    ID_TYPE_FILE = MINID,
    ID_TYPE_SEPARATION,
    MINID,
  };

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  int m_TypeSeparation;
  albaVMELandmarkCloud  *m_VmeCloud;
	wxString m_File;
  int m_OnlyCoordinates;

};
#endif
