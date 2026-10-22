/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImageCalibration
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaOpImageCalibration_H__
#define __albaOpImageCalibration_H__

//----------------------------------------------------------------------------
// includes :
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward reference
class albaInteractor2DMeasure;
class albaInteractor2DMeasure_Distance;
class albaInteractor2DMeasure_CenterPoint;

class ALBA_EXPORT albaOpImageCalibration : public albaOp
{
public:
	enum ITERATOR_TYPE
	{
		FREE_LINE = 0,
		HORIZONTAL_LINE,
		VERTICAL_LINE,
		CIRCLE
	};

	enum CALIBRATION_ID
	{
		ID_ITERATOR = MINID,
		ID_UNITS,
		ID_DISTANCE,
		ID_DIAMETER,
		ID_RADIUS
	};

	albaOpImageCalibration(const wxString &label = "Image Spatial Calibration");
	~albaOpImageCalibration();

	albaTypeMacro(albaOpImageCalibration, albaOp);

	albaOp *Copy();
	void OpRun();
	void OnEvent(albaEventBase *alba_event);

protected:
	bool InternalAccept(albaVME *node);
	void OpStop(int result);
	void CreateGui();
	void UpdateGui();
	void SetActiveInteractor();

	void OpDo();
	void OpUndo();


	int m_SelectedIterator;
	int m_Units;

	double m_Distance;
	double m_Diameter;
	double m_Radius;

	double m_InputSpacing[3];
	double m_NewSpacing[3];

	albaInteractor2DMeasure_Distance *m_DistanceInteractor;
	albaInteractor2DMeasure_CenterPoint *m_CenterPointInteractor;
	albaInteractor2DMeasure *m_ActiveInteractor;
};

#endif