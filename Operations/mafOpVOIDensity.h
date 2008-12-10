/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpVOIDensity.h,v $
  Language:  C++
  Date:      $Date: 2008-12-10 14:28:55 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Matteo Giacomoni & Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpVOIDensity_H__
#define __mafOpVOIDensity_H__

#include "mafOp.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkDoubleArray;

//----------------------------------------------------------------------------
// mafOpVOIDensity :
//----------------------------------------------------------------------------
class mafOpVOIDensity: public mafOp
{
public:
	mafOpVOIDensity(const wxString &label = "VOIDensity");
	~mafOpVOIDensity(); 
	virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(mafOpVOIDensity, mafOp);

  /** return a copy of itself, this needs to put the operation into the undo stack. */
	mafOp* Copy();
	
	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* Node);

	/** Set the input vme for the operation. */
	void OpRun();

	/** Builds operation's interface. */
	void OpDo();

	/** Execute the operation. */
	void OpUndo();

	/**	Set surface of input in test mode*/
	void SetSurface(mafNode *Surface){m_Surface=Surface;};

	/**	Return the min scalar*/
	double GetMinScalar(){return m_MinScalar;};

	/**	Return the MAX scalar*/
	double GetMaxScalar(){return m_MaxScalar;};

	/**	Return the mean scalar*/
	double GetMeanScalar(){return m_MeanScalar;};
	
	/**	Return the number of scalars*/
	int GetNumberScalars(){return m_NumberOfScalars;};

	/**	Return Standard Deviation*/
	double GetStandardDeviation(){return m_StandardDeviation;};

	/** Extract scalars from input volume that are inside the choosed surface. */
	void ExtractVolumeScalars();

 	static bool OutputSurfaceAccept(mafNode* Node) {return(Node != NULL && (((mafVME *)Node)->GetOutput()->IsA("mafVMEOutputSurface")));};

protected:
  mafNode        *m_Surface;
  vtkDoubleArray *m_VOIScalars;
  mafString       m_NumberOfScalarsString;
  int             m_NumberOfScalars;
  mafString       m_MeanScalarString;
  double          m_MeanScalar;
  mafString       m_MaxScalarString;
  mafString       m_MinScalarString;
  double          m_MaxScalar;
  double          m_MinScalar;
  mafString       m_StandardDeviationString;
  double          m_StandardDeviation;
	wxListBox			 *m_VoxelList;
};
#endif
