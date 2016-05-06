/*=========================================================================

 Program: MAF2
 Module: mafOpExporterMeters
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExporterMeters_H__
#define __mafOpExporterMeters_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"
#include <fstream>
#include <map>

#include <vnl/vnl_matrix.h>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafGui;
class mafEvent;


//----------------------------------------------------------------------------
// mafOpExporterMeters :
//----------------------------------------------------------------------------
/** 
    class name: mafOpExporterMeters
   This class provides the functionality for exporting mafVMEWrappedMeter, writing all in a txt file.
*/
class MAF_EXPORT mafOpExporterMeters: public mafOp
{
public:
  /** object constructor */
  mafOpExporterMeters(const wxString &label = "WrappedMeter");
  /** object destructor */
  ~mafOpExporterMeters(); 
  /** RTTI Macro */
  mafTypeMacro(mafOpExporterMeters, mafOp);
  
  enum ID_METER_EXPORT
  {
    VME_SELECTED_METER = 0,
    VME_CLASSIC_METERS,
    VME_WRAPPED_METERS,
    VME_ALL_METERS,
  };
  /** method for clone object */
  mafOp* Copy();
  /** method for catch the dispatched events */
	void OnEvent(mafEventBase *maf_event);

 	/** Return true for the acceptable vme type. */
  bool Accept(mafVME*node);

	/** Builds operation's interface. */
  void OpRun();

  /** execute operation */
  void OpDo();

  /** Set the filename for the .stl to export */
  void SetFileName(const char *file_name) {m_File = file_name;};
  
  /**Public API for exporting*/
  void Export();

protected:
  /* Export All meters of an msf in a single ascii file*/
  void ExportTypeOfMeters();

  /* Export All meters of an msf in a single ascii file*/
  void ExportSelectedMeter();

  /* Export single meter in a single ascii file*/
  void ExportMeter();

  /** Export the meter coordinates in file. */
  void ExportClassicMeterCoordinates(int index, int indexTime);

  /** Export the Wrapped meter coordinates in file. */
  void ExportWrappedMeterCoordinates(int index, int indexTime);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  /** Create Gui for Operation*/
  void CreateGui();

  /** Conntrol if the radio selection is valid */
  bool ControlExportChoose();

  /** Write on file all the output, it uses WriteCoordinatesOnFile */
  void WriteOnFile();

  /** Write on file only the coordinates of the file */
  void WriteCoordinatesOnFile(int index);
  
  
  mafString          m_File;
  std::ofstream      m_OutputFile;
  int                m_ExportRadio;
  mafTimeStamp       m_CurrentTime;
  std::vector<mafTimeStamp> m_Times;

  mafVME            *m_CurrentVme;
  std::vector<mafVME *> m_Meters;
  std::vector< vnl_matrix<double> > m_MetersCoordinatesList;

  int m_SubTreeExportMeter;
};
#endif
