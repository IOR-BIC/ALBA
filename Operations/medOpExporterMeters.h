/*=========================================================================

 Program: MAF2Medical
 Module: medOpExporterMeters
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpExporterMeters_H__
#define __medOpExporterMeters_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medOperationsDefines.h"
#include "mafOp.h"
#include <fstream>
#include <map>

#include <vnl/vnl_matrix.h>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafGui;
class mafEvent;


//----------------------------------------------------------------------------
// medOpExporterMeters :
//----------------------------------------------------------------------------
/** 
    class name: medOpExporterMeters
   This class provides the functionality for exporting medVMEWrappedMeter, writing all in a txt file.
*/
class MED_OPERATION_EXPORT medOpExporterMeters: public mafOp
{
public:
  /** object constructor */
  medOpExporterMeters(const wxString &label = "WrappedMeter");
  /** object destructor */
  ~medOpExporterMeters(); 
  /** RTTI Macro */
  mafTypeMacro(medOpExporterMeters, mafOp);
  
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
  bool Accept(mafNode *node);

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

  mafNode            *m_CurrentVme;
  std::vector<mafNode *> m_Meters;
  std::vector< vnl_matrix<double> > m_MetersCoordinatesList;

  int m_SubTreeExportMeter;
};
#endif
