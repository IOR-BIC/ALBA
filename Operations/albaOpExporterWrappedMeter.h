/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterWrappedMeter_H__
#define __albaOpExporterWrappedMeter_H__

#include "albaDefines.h"
#include "albaOp.h"
#include <vnl/vnl_matrix.h>
#include "wx/busyinfo.h"
#include "albaVMEIterator.h"
#include <fstream>

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_VNL_MATRIX(ALBA_EXPORT, double);
#endif

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEWrappedMeter;
class albaGui;
class albaEvent;
//----------------------------------------------------------------------------
// albaOpExporterWrappedMeter :
//----------------------------------------------------------------------------
/** 
class name: albaOpExporterWrappedMeter
   This class provides the functionality for exporting albaVMEWrappedMeter, writing all in a txt file.
*/
class ALBA_EXPORT albaOpExporterWrappedMeter: public albaOp
{
public:
  /** object constructor */
  albaOpExporterWrappedMeter(const wxString &label = "ActionLine");
  /** object destructor */  
  ~albaOpExporterWrappedMeter(); 

  std::vector<albaVME *> m_Meters;
  std::vector< vnl_matrix<double> > m_MetersCoordinatesList ;
  albaTimeStamp       m_CurrentTime;
  std::vector<albaTimeStamp> m_Times ;
  albaVME            *m_CurrentVme ;
  std::ofstream      m_OutputFile;
  std::vector<int>  m_KeyNumList;

  /** RTTI macro*/
  albaTypeMacro(albaOpExporterWrappedMeter, albaOp);
  /** method for clone object */
  albaOp* Copy();
  /** method for catch the dispatched events */
	void OnEvent(albaEventBase *alba_event);

	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .stl to export */
  void SetFileName(const char *file_name) {m_File = file_name;};
  
  /** Export the Wrapped meter coordinates in file. */
  void ExportWrappedMeterCoordinates(int index, int indexTime);
  /** test method copy from test albaOpExporterWrappedMeterTest**/
  void Test() ;

  /** specific exporter for wrapped meters */
  void ExportWrappedMeter();
  /** generic function for handling export*/
  void Export();
  /** write the data stream to a file */
  void WriteOnFile();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);
  
  albaString           m_File;
};
#endif
