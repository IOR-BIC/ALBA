/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVOIDensity.h,v $
  Language:  C++
  Date:      $Date: 2006-09-01 14:12:10 $
  Version:   $Revision: 1.1 $
  Authors:   Matteo Giacomoni & Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoVOIDensity_H__
#define __mmoVOIDensity_H__

#include "mafVME.h"
#include "mafEvent.h"
#include "mmgVMEChooserAccept.h"
#include "mafVMEVolume.h"
#include "mafVMEOutputSurface.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class vtkDoubleArray;
class mafString;

//----------------------------------------------------------------------------
// mmoVOIDensity :
//----------------------------------------------------------------------------
class mmoVOIDensity: public mafOp
{
public:
	mmoVOIDensity(wxString label);
	~mmoVOIDensity(); 
	virtual void OnEvent(mafEventBase *maf_event);
  
  /** 
  return a copy of itself, this needs to put the operation into the undo stack. */
	mafOp* Copy();
	
	/** 
  Return true for the acceptable vme type. */
	bool Accept(mafNode* Node);

	/** 
  Set the input vme for the operation. */
	void OpRun();

	/** 
  Builds operation's interface. */
	void OpDo();

	/** 
  Execute the operation. */
	void OpUndo();

	/**
	Set surface of input in test mode*/
	void SetSurface(mafNode *Surface){m_Surface=Surface;};

	/**
	Return the min scalar*/
	double GetMinScalar(){return m_MinScalar;};

	/**
	Return the MAX scalar*/
	double GetMaxScalar(){return m_MaxScalar;};

	/**
	Return the mean scalar*/
	double GetMeanScalar(){return m_MeanScalar;};
	
	/**
	Return the number of scalars*/
	int GetNumberScalars(){return m_NumberOfScalars;};

	/**
	Return Standard Deviation*/
	double GetStandardDeviation(){return m_StandardDeviation;};

	/** 
  Extract scalars from input volume that are inside the choosed surface. */
	void ExtractVolumeScalars();

 	class mafVMESurfaceAccept : public mmgVMEChooserAccept
	{
		public:
			
			mafVMESurfaceAccept() {};
		 ~mafVMESurfaceAccept() {};

		bool Validate(mafNode* Node) {return(Node != NULL && Node->IsA("mafVMESurface"));};
	};
  mafVMESurfaceAccept *m_VMESurfaceAccept;

protected:
  /** 
  This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

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
