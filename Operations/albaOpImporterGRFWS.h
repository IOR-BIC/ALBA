/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterGRFWS
 Authors: Roberto Mucci, Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterGRFWS_H__
#define __albaOpImporterGRFWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEVector;
class albaVMESurface;
class albaVMEGroup;

/** 
class name: albaOpImporterGRFWS
Importer for ground reactions force, used in motion analysis
*/
class ALBA_EXPORT albaOpImporterGRFWS : public albaOp
{
public:
  /** constructor */
	albaOpImporterGRFWS(const wxString &label = "GRF Importer");
  /** destructor */
	~albaOpImporterGRFWS(); 

  /** apply the undo pattern to the operation, retrieving the previous state*/
  void albaOpImporterGRFWS::OpUndo();

  /** Copy the operation. */
	albaOp* Copy();

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

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  /* Read force plate */ 
  void ReadForcePlates();

  /* Read a single vector */
  void ReadSingleVector();

  albaVMESurface       *m_PlatformLeft;
  albaVMESurface       *m_PlatformRight;
  albaVMEVector        *m_ForceLeft;
  albaVMEVector        *m_ForceRight;
  albaVMEVector        *m_MomentLeft;
  albaVMEVector        *m_MomentRight;
  albaVMEGroup         *m_Group;

  wxString             m_FileDir;
	wxString             m_File;

};
#endif
