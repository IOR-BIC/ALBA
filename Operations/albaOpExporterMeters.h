/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMeters
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterMeters_H__
#define __albaOpExporterMeters_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include <fstream>
#include <map>

#include <vnl/vnl_matrix.h>

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_VNL_MATRIX(ALBA_EXPORT, double);
#endif

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaGui;
class albaEvent;


//----------------------------------------------------------------------------
// albaOpExporterMeters :
//----------------------------------------------------------------------------
/** 
    class name: albaOpExporterMeters
   This class provides the functionality for exporting albaVMEWrappedMeter, writing all in a txt file.
*/
class ALBA_EXPORT albaOpExporterMeters: public albaOp
{
public:
  /** object constructor */
  albaOpExporterMeters(const wxString &label = "WrappedMeter");
  /** object destructor */
  ~albaOpExporterMeters(); 
  /** RTTI Macro */
  albaTypeMacro(albaOpExporterMeters, albaOp);
  
  enum ID_METER_EXPORT
  {
    VME_SELECTED_METER = 0,
    VME_CLASSIC_METERS,
    VME_WRAPPED_METERS,
    VME_ALL_METERS,
  };
  /** method for clone object */
  albaOp* Copy();
  /** method for catch the dispatched events */
	void OnEvent(albaEventBase *alba_event);

	/** Builds operation's interface. */
  void OpRun();

  /** execute operation */
  void OpDo();

  /** Set the filename for the .stl to export */
  void SetFileName(const char *file_name) {m_File = file_name;};
  
  /**Public API for exporting*/
  void Export();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

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
  
  
  albaString          m_File;
  std::ofstream      m_OutputFile;
  int                m_ExportRadio;
  albaTimeStamp       m_CurrentTime;
  std::vector<albaTimeStamp> m_Times;

  albaVME            *m_CurrentVme;
  std::vector<albaVME *> m_Meters;
  std::vector< vnl_matrix<double> > m_MetersCoordinatesList;

  int m_SubTreeExportMeter;
};
#endif
