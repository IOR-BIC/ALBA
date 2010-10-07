/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpExporterGRFWS.h,v $
  Language:  C++
  Date:      $Date: 2010-10-07 10:02:42 $
  Version:   $Revision: 1.1.2.8 $
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

  /** Execute the operation. */
  void OpDo();

  /** Stop the operation. */
  void OpStop(int result);

  /** Wait for events */
  void OnEvent(mafEventBase *maf_event);

  /** Create GUI. */
  void CreateGui();

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

  /** Set the filename for the file to export */
  void SetFileName(const char *file_name);

  /** Set Platforms */
  void SetPlatforms(mafVMESurface* p1,mafVMESurface* p2)
    {m_PlatformLeft = p1; m_PlatformRight = p2;};
  
  /** Set Forces */
  void SetForces(mafVMEVector* f1,mafVMEVector* f2)
    {m_ForceLeft = f1; m_ForceRight = f2;}; 
  
  /** Set Moments */
  void SetMoments(mafVMEVector* m1,mafVMEVector* m2)
    {m_MomentLeft = m1; m_MomentRight = m2;};

  /* Remove temp files */
  void RemoveTempFiles();

protected:
 
  /** Merge time stamps of platforms */
  std::vector<mafTimeStamp> MergeTimeStamps(std::vector<mafTimeStamp> kframes1,std::vector<mafTimeStamp> kframes2);

  /** Check consistency with vector direction:
      CheckVectorToSwitch(  frame = number of frame
                        coor1_ID = index of the first point coordinate to check consistency
                        coor2_ID = index of the second point coordinate to check consistency
                        coor3_ID = index of the third point coordinate to check consistency
                        v = vector structure with all its information
                        original_pos = position of the real point (at frame 0)
                        vVME = VME associated with the vector
                        pointID = id of the original point in the VTK data with which we must compare the guessed value
                        time = timestamp 
  */
  void CheckVectorToSwitch(int frame, int coor1_ID, int coor2_ID, int coor3_ID, medGRFVector* v, double* original_pos, mafVMEVector* vVME = NULL, int pointID = 0, mafTimeStamp time = 0);

  mafVMESurface       *m_PlatformLeft;
  mafVMESurface       *m_PlatformRight;
  mafVMEVector        *m_ForceLeft;
  mafVMEVector        *m_ForceRight;
  mafVMEVector        *m_MomentLeft;
  mafVMEVector        *m_MomentRight;
  mafVMEGroup         *m_Group;

  double m_Treshold;
  int m_FastMethod;

	wxString m_File;
  wxString m_File_temp1;
  wxString m_File_temp2;
  wxString m_File_temp3;
  wxString m_File_temp4;
};
#endif
