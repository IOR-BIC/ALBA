/*=========================================================================

 Program: MAF2
 Module: mafOpImporterGRFWS
 Authors: Roberto Mucci, Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterGRFWS_H__
#define __mafOpImporterGRFWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEVector;
class mafVMESurface;
class mafVMEGroup;


/** 
class name: mafOpImporterGRFWS
Importer for ground reactions force, used in motion analysis
*/
class MAF_EXPORT mafOpImporterGRFWS : public mafOp
{
public:
  /** constructor */
	mafOpImporterGRFWS(const wxString &label = "GRF Importer");
  /** destructor */
	~mafOpImporterGRFWS(); 

  /** apply the undo pattern to the operation, retrieving the previous state*/
  void mafOpImporterGRFWS::OpUndo();

  /** Copy the operation. */
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

  /** Read the file.  
  the format of the file admits some specifics.
  1) The first line contains the tag FORCE PLATES
  2) The second line contains the Frequency
  3) The third line contains the tag for the 4 plates corners
  4) The fourth and fifth line contains the 12 corners values
  5) The sixth line is a blank line
  6) The seventh line contains the tag for plates
  7) The eighth line contains the tag for COP,REF,FORCE,MOMENT
  8) The ninth line contains the units
  5) The first element of each line is the sample, then 24 values
  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};

protected:

  /* Read force plate */ 
  void ReadForcePlates();

  /* Read a single vector */
  void ReadSingleVector();

  mafVMESurface       *m_PlatformLeft;
  mafVMESurface       *m_PlatformRight;
  mafVMEVector        *m_ForceLeft;
  mafVMEVector        *m_ForceRight;
  mafVMEVector        *m_MomentLeft;
  mafVMEVector        *m_MomentRight;
  mafVMEGroup         *m_Group;

  wxString             m_FileDir;
	wxString             m_File;

};
#endif
