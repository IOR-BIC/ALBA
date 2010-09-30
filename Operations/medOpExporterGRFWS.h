/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpExporterGRFWS.h,v $
  Language:  C++
  Date:      $Date: 2010-09-30 07:42:50 $
  Version:   $Revision: 1.1.2.4 $
  Authors:   Simone Brazzale
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpExporterGRFWS_H__
#define __medOpExporterGRFWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafVMEVector.h"
#include "mafVMESurface.h"
#include "mafVMEGroup.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEVector;
class mafVMESurface;
class mafVMEGroup;

//----------------------------------------------------------------------------
// medOpExporterGRFWS :
//----------------------------------------------------------------------------
/** */
class medOpExporterGRFWS : public mafOp
{
public:
	medOpExporterGRFWS(const wxString &label = "GRF Exporter");
	~medOpExporterGRFWS(); 
	
  /** Copy the operation. */
  mafOp* Copy();

  /** Clear */
  void Clear();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node);

	/** Builds operation's interface. */
	void OpRun();

  /** Load VMEs. */
	int LoadVMEs(mafNode* node);

  /** Read the file.
  File format:

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

  this operation does as follows:
  1) Checks for the tag FORCE PLATES in the first line
  2) Sets Frequency to 1 --> This is done since the exporter puts time instead of sample in the file.
  3) Jump line
  3) Writes the corner values
  4) Jump lines
  5) Writes time and COP/REF/FORCE/MOMENT values 
  */
  void Write();

  /** Set the filename for the file to export */
  void SetFileName(const char *file_name){m_File = file_name;};

  /** Set Platforms */
  void SetPlatforms(mafVMESurface* p1,mafVMESurface* p2)
    {m_PlatformLeft = p1; m_PlatformRight = p2;};
  
  /** Set Forces */
  void SetForces(mafVMEVector* f1,mafVMEVector* f2)
    {m_ForceLeft = f1; m_ForceRight = f2;}; 
  
  /** Set Moments */
  void SetMoments(mafVMEVector* m1,mafVMEVector* m2)
    {m_MomentLeft = m1; m_MomentRight = m2;};

private:

  /** Merge time stamps of platforms */
  std::vector<mafTimeStamp> MergeTimeStamps(std::vector<mafTimeStamp> kframes1,std::vector<mafTimeStamp> kframes2);

protected:
  mafVMESurface       *m_PlatformLeft;
  mafVMESurface       *m_PlatformRight;
  mafVMEVector        *m_ForceLeft;
  mafVMEVector        *m_ForceRight;
  mafVMEVector        *m_MomentLeft;
  mafVMEVector        *m_MomentRight;
  mafVMEGroup         *m_Group;

	wxString m_File;
};
#endif
