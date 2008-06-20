/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMeshQuality.h,v $
Language:  C++
Date:      $Date: 2008-06-20 15:25:16 $
Version:   $Revision: 1.2 $
Authors:   Matteo Giacomoni - Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#ifndef __medOpMeshQuality_H__
#define __medOpMeshQuality_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class vtkMeshQuality;
class mmgDialog;
class mafRWI;
class mmiCameraMove;
class vtkPolyDataMapper;
class vtkActor;
class vtkScalarBarActor;
class vtkTriangleQualityRatio;
class vtkFeatureEdges;
class vtkTubeFilter;

//----------------------------------------------------------------------------
// medOpMeshQuality :
//----------------------------------------------------------------------------
/** */
class medOpMeshQuality: public mafOp
{
public:
	medOpMeshQuality(const wxString &label = "Check Mesh Quality");
	~medOpMeshQuality(); 

	virtual void OnEvent(mafEventBase *maf_event);

	mafTypeMacro(medOpMeshQuality, mafOp);

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	void CreateOpDialog();
	void DeleteOpDialog();
	void CreatePolydataPipeline();

	mmgDialog								*m_Dialog;
	mafRWI									*m_Rwi;
	mmiCameraMove						*m_Picker;
	vtkPolyDataMapper				*m_Mapper;
	vtkActor								*m_Actor;
	vtkScalarBarActor				*m_BarActor;

	wxStaticText						*m_LabelAverageAspectRatio;
	wxStaticText						*m_LabelMaxAspectRatio;
	wxStaticText						*m_LabelMinAspectRatio;

	vtkTriangleQualityRatio	*m_CheckMeshQuality;

  vtkFeatureEdges *m_FeatureEdgeFilter;
  vtkPolyDataMapper *m_MapperFeatureEdge;
  vtkTubeFilter *m_TubeFilter;
  vtkActor *m_ActorFeatureEdge;

  double m_Angle;

};
#endif
