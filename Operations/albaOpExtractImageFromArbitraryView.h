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

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------

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

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(albaVME *node);

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
		ID_RENAME,
		ID_REMOVE,
		ID_RES,
	};

protected:

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();

	void UpdateListbox();

	void SelectImageSlice();
	void RenameImageSlice();
	void RemoveImageSlice();
	void ShowImageSlice();

	void ExtractImage();

	void SaveTags(albaVMEImage * image);

	wxBitmap *GetSliceImage();

	albaView *m_View;
	albaVMEGroup *m_ImageSlicesGroup;

	wxListBox *m_SlicesListBox;

	int m_Axis;
	bool m_ShowInTree;
	bool m_ChooseName;
	bool m_ShowExtractButton;
	bool m_ShowSliceList;
	albaString m_GuiMessage;

	wxString m_ImageName;
	albaVMEImage *m_CurrentImage;
};
#endif
