/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaOpExtractImageFromArbitraryView.h
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpExtractImageFromArbitraryView_H__
#define __albaOpExtractImageFromArbitraryView_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaVMEGroup.h"
#include "albaDecl.h"
#include "vtkTransform.h"
#include "vtkImageReslice.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkimageData;

//----------------------------------------------------------------------------
// Class Name: albaOpExtractImageFromArbitraryView
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpExtractImageFromArbitraryView : public albaOp
{
public:
	/** Constructor. */
	albaOpExtractImageFromArbitraryView(wxString label = "Extract Image from View Slice");

	/** Destructor. */
	~albaOpExtractImageFromArbitraryView();

	/** RTTI macro. */
	albaTypeMacro(albaOpExtractImageFromArbitraryView, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);

	enum OP_EXTRACT_IMAGE_ID
	{
		ID_TYPE = MINID,
		ID_AXIS,
		ID_EXTRACT,
		ID_SLICES_LIST,
		ID_SLICES_GROUP,
		ID_RENAME,
		ID_REMOVE,
		ID_RES,
	};

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();

	void UpdateListbox();

	void SelectImageSlice();
	void RenameImageSlice();
	void RemoveImageSlice();
	void ShowImageSlice();

	virtual void ExtractImage();
	virtual vtkImageData *GetSliceImageData();
	virtual void SaveTags(albaVMEImage *image);
	virtual wxString GenerateImageName();

	albaView			*m_View;
	albaVMEGroup	*m_ImageSlicesGroup;
	albaVMEImage  *m_CurrentImage;
	wxListBox			*m_SlicesListBox;

	vtkImageReslice *m_Reslice;

	wxString m_ImageName;
	int m_Axis;
	bool m_ShowInTree;
	int m_Magnification;
	
};
#endif
