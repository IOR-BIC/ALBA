/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterGRFWS
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterGRFWS_H__
#define __albaOpExporterGRFWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "albaVMEVector.h"
#include "albaVMESurface.h"
#include "albaVMEGroup.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEVector;
class albaVMESurface;
class albaVMEGroup;

//----------------------------------------------------------------------------
// typedef declarations :
//----------------------------------------------------------------------------

const double MAX_DOUBLE = 1.e30;
typedef struct {
  int m_SwitchX;
  int m_SwitchY;
  int m_SwitchZ;
  double m_Bounds[6];
  double m_OldDistance[3];
  int m_WasDecreasing[3];
  int m_IsDecreasing[3];
  } medGRFVector;

/** 
class name: albaOpExporterGRFWS
Exporterr for ground reactions force, used in motion analysis
*/
class ALBA_EXPORT albaOpExporterGRFWS : public albaOp
{
public:
	albaOpExporterGRFWS(const wxString &label = "GRF Exporter");
	~albaOpExporterGRFWS(); 
	
  /** Copy the operation. */
  albaOp* Copy();

  /** Clear */
  void Clear();

	/** Builds operation's interface. */
	void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Stop the operation. */
  void OpStop(int result);

  /** Wait for events */
  void OnEvent(albaEventBase *alba_event);

  /** Create GUI. */
  void CreateGui();

  /** Load VMEs. */
	int LoadVMEs(albaVME* node);

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
  */
  void Write();
  
  /*Try a method to run the Operation faster.
    In order to avoid loading all VTK data for all frames, which should drastically increase the time required by the operation,
    the WriteFast method retrieves the Vector bounds and guesses the position of the vector by using its bounds.
    To check if the guessed position and the real one are effectively the same, this method adjust manually the first frame and then
    switch the axis of the vector if the bounds coordinates are going to change direction (this means the vector is passing through
    its origin respect to that axis).
    THIS METHOD IS NOT SURED TO WORK WITH ALL INPUTS, BUT JUST WHICH THAT ONES WHICH VECTORS DO NOT CHANGE DIRECTIONS TOO QUICKLY!
    */
  void WriteFast();

  /* Write single vector */ 
  void WriteSingleVector();
  void WriteSingleVectorFast();

  /** Set the filename for the file to export */
  void SetFileName(const char *file_name);

  /** Set Platforms */
  void SetPlatforms(albaVMESurface* p1,albaVMESurface* p2)
    {m_PlatformLeft = p1; m_PlatformRight = p2;};
  
  /** Set Forces */
  void SetForces(albaVMEVector* f1,albaVMEVector* f2)
    {m_ForceLeft = f1; m_ForceRight = f2;}; 
  
  /** Set Moments */
  void SetMoments(albaVMEVector* m1,albaVMEVector* m2)
    {m_MomentLeft = m1; m_MomentRight = m2;};

  /* Remove temp files */
  void RemoveTempFiles();

protected:

  enum ID_TRESHOLDS
	{
		ID_FL = MINID,
		ID_ML,
		ID_FR,
    ID_MR,
    ID_RES,
    ID_FAST,
    MINID
	};
 
	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /* Calculate proposed tresholds */
  void CalculateTresholds();

  /* Calculate a single treshold for a single vector */
  double CalculateTreshold(albaVMEVector* v);
  
  /** Merge time stamps of platforms */
  std::vector<albaTimeStamp> MergeTimeStamps(std::vector<albaTimeStamp> kframes1,std::vector<albaTimeStamp> kframes2);

  /** Check consistency with vector direction:
      CheckVectorToSwitch(  frame = number of frame
                        coor1_ID = index of the first point coordinate to check consistency
                        coor2_ID = index of the second point coordinate to check consistency
                        coor3_ID = index of the third point coordinate to check consistency
                        v = vector structure with all its information
                        original_pos = position of the real point (at frame 0)
                        tr = treshold value id
                        vVME = VME associated with the vector
                        pointID = id of the original point in the VTK data with which we must compare the guessed value
                        time = timestamp 
  */
  void CheckVectorToSwitch(int frame, int coor1_ID, int coor2_ID, int coor3_ID, medGRFVector* v, double* original_pos, int tr = -1, albaVMEVector* vVME = NULL, int pointID = 0, albaTimeStamp time = 0);

  albaVMESurface       *m_PlatformLeft;
  albaVMESurface       *m_PlatformRight;
  albaVMEVector        *m_ForceLeft;
  albaVMEVector        *m_ForceRight;
  albaVMEVector        *m_MomentLeft;
  albaVMEVector        *m_MomentRight;
  albaVMEGroup         *m_Group;

  std::vector<double> m_Treshold;
  int m_FastMethod;
  int m_Resolution;

  albaGUI* m_AdvanceSettings;

	wxString m_File;
  wxString m_File_temp1;
  wxString m_File_temp2;
  wxString m_File_temp3;
  wxString m_File_temp4;
};
#endif
