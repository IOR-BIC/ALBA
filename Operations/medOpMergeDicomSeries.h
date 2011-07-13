/*=========================================================================
Program:   MED
Module:    $RCSfile: medOpMergeDicomSeries.h,v $
Language:  C++
Date:      $Date: 2011-07-13 12:53:06 $
Version:   $Revision: 1.1.2.3 $
Authors:   Alberto Losi
==========================================================================
Copyright (c) 2009
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#include "mafOp.h"
#include "vtkDirectory.h"

//----------------------------------------------------------------------------
// medOpMergeDicomSeries : Merge dicom series located in the same folder
//----------------------------------------------------------------------------
/** 
Merge dicom series located in the same folder; this operation was implemented to correctly import and visualize data located in 
"\\HD01\Public\Dati\Dicom Regression\NIG004_MAGLU_SA" and "\\HD01\Public\Dati\Dicom Regression\NIG009-PAVVI_SA"
WARNING: This operation will be removed and integrated in the importer dicom operation as an optiona functionality
*/
class medOpMergeDicomSeries : public mafOp
{
public:
	/** constructor */
  medOpMergeDicomSeries(wxString label = "Merge DICOM Series");

  /** destructor */
  ~medOpMergeDicomSeries();

	/** RTTI macro */
	mafTypeMacro(medOpMergeDicomSeries, mafOp);
	
	/** Copy. */
	mafOp* Copy();

	/** Builds operation's interface calling CreateGui() method. */
	virtual void OpRun();

  /** turn true for the acceptable vme type. */
  virtual bool Accept(mafNode *node){return true;};

protected:

  mafString m_DicomDirectoryABSFileName;
  vtkDirectory  *m_DICOMDirectoryReader;
  int m_DicomSeriesInstanceUID;

  bool RanameSeriesAndManufacturer(const char *dicomDirABSPath, int dicomSeriesUID);
};