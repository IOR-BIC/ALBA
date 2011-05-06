/*=========================================================================
Program:   MED
Module:    $RCSfile: medOpMergeDicomSeries.h,v $
Language:  C++
Date:      $Date: 2011-05-06 10:12:48 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi
==========================================================================
Copyright (c) 2009
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#include "mafOp.h"
#include "vtkDirectory.h"

//----------------------------------------------------------------------------
// medOpMergeDicomSeries :
//----------------------------------------------------------------------------
/** 
@ToDo
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

  /** @ToDo */
  virtual bool Accept(mafNode *node){return true;};

protected:

  mafString m_DicomDirectoryABSFileName;
  vtkDirectory  *m_DICOMDirectoryReader;

  bool RanameSeriesAndManufacturer(const char *dicomDirABSPath);
};