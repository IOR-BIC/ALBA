/*=========================================================================
Program:   DentApp
Module:    appOpEmpty.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (https://github.com/IOR-LTM)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpESPCalibration.h"

#include "albaDecl.h"

#include "albaGUI.h"
#include "albaHTMLTemplateParser.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaVMEImage.h"
#include "albaVMEIterator.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEOutput.h"
#include "albaVMESurfaceParametric.h"
#include "albaVect3d.h"
#include "mmaMaterial.h"

#include "vtkAlbaProjectVolume.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "albaTagArray.h"
#include "wx/busyinfo.h"
#include "vtkStructuredPoints.h"

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include "albaXMLString.h"

#define WATER_VALUE 0

#define AIR_VALUE -1000.0
#define TARGET_MAX 1380.0	 //Scale factor 	
#define CUT_OFF_BODY 975.0 //Middle point from wings (700) and Cortical (700)
#define CUT_OFF_WINGS 350.0 //Middle point from outside (150) and wings (700)
#define TAIL_RATIO 14.0/20.0 //Tail on L3 to rearwall ratio
#define MARGIN_REDUCTION 0.70

#define FRONT_BACK_CYL_RADIUS_RATIO 7.5/9.0
#define BACK_CYL_R1_R2_RATIO 9.5/13.0
#define BACK_CYL_MARGIN_REDUCTION 0.94

#define INT_VERT_SPACE 7
#define CYL_SPACE 23
#define CYL_HEIGHT 12
#define BACK_CYL_HEIGHT 6

#define TAILSIZE_MIN 6.0
#define TAILSIZE_MAX 13.0
#define BODY_RADIUS_TAIL_RATIO 1.5


#define CIRCLES_THRESHOLD 0
#define EXPECTED_VALUES {50,100,200,400,800}

#define SIDE_X 0
#define SIDE_Y 1


#define FULL_VERTEBRAL_ZSPACE 100
#define CROPPED_VERTEBRAL_ZSPACE 80
#define FULL_VERTEBRAL_MAX_HSPACE 62
#define FULL_VERTEBRAL_MIN_HSPACE 35
#define FULL_VERTEBRAL_MAX_ERROR 5
#define ZCROP_SPACE 10


//#define CREATE_SAMPLING_CLOUDS



//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpESPCalibration);

//----------------------------------------------------------------------------
albaOpESPCalibration::albaOpESPCalibration(wxString label, bool enable) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_Enabled = enable;
}

//----------------------------------------------------------------------------
albaOpESPCalibration::~albaOpESPCalibration()
{
}

//----------------------------------------------------------------------------
bool albaOpESPCalibration::Accept(albaVME *node)
{
	return node->IsA("albaVMEVolumeGray") && vtkImageData::SafeDownCast(node->GetOutput()->GetVTKData());
}

//----------------------------------------------------------------------------
albaOp* albaOpESPCalibration::Copy()
{
	albaOpESPCalibration *cp = new albaOpESPCalibration(m_Label, m_Enabled);
	return cp;
}
//----------------------------------------------------------------------------
void albaOpESPCalibration::OpRun()
{
	wxBusyInfo wait(_("Extracting calibration info.\nPlease wait..."));
	wxBusyCursor waitCursor;

	m_Volume = albaVMEVolumeGray::SafeDownCast(m_Input);
	
	int result=Calibrate();

	if (result == ALBA_ERROR)
	{
		OpStop(OP_RUN_CANCEL);
	}
	else if (!m_TestMode)
	{
		CreateGui();
	}
}
//----------------------------------------------------------------------------
void albaOpESPCalibration::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	for (int i = 0; i < toHidelist.size(); i++)
		GetLogicManager()->VmeShow(toHidelist[i], false);


	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpESPCalibration::OpDo()
{
	return;
}

//----------------------------------------------------------------------------
int albaOpESPCalibration::Calibrate()
{
	m_Volume->Update();
	m_Volume->GetOutput()->Update();
	vtkImageData *volumeData = vtkImageData::SafeDownCast(m_Volume->GetOutput()->GetVTKData());
	
	double *scalarRange = volumeData->GetScalarRange();
	double *spacing = volumeData->GetSpacing();
	double *origin = volumeData->GetOrigin();
	int *dims = volumeData->GetDimensions();

	vtkALBAProjectVolume *projectFilterX;
	vtkNEW(projectFilterX);
	projectFilterX->SetInput(volumeData);
	projectFilterX->SetProjectionModalityToMax();
	projectFilterX->SetProjectionSideToX();
	projectFilterX->Update();

	albaVMEImage *imageOutputX;
	albaNEW(imageOutputX);
	imageOutputX->SetData((vtkImageData *)projectFilterX->GetOutput(), m_Volume->GetMTime());
	imageOutputX->SetName("outputX");
	imageOutputX->ReparentTo(m_Volume);
	
	vtkALBAProjectVolume *projectFilterY;
	vtkNEW(projectFilterY);
	projectFilterY->SetInput(volumeData);
	projectFilterY->SetProjectionModalityToMax();
	projectFilterY->SetProjectionSideToY();
	projectFilterY->Update();

	albaVMEImage *imageOutputY;
	albaNEW(imageOutputY);
	imageOutputY->SetData((vtkImageData *)projectFilterY->GetOutput(), m_Volume->GetMTime());
	imageOutputY->SetName("outputY");
	imageOutputY->ReparentTo(m_Volume);

	vtkALBAProjectVolume *projectFilterYX;
	vtkNEW(projectFilterYX);
	projectFilterYX->SetInput(projectFilterY->GetOutput());
	projectFilterYX->SetProjectionModalityToMax();
	projectFilterYX->SetProjectionSideToY();
	projectFilterYX->Update();

	albaVMEImage *imageOutputYX;
	albaNEW(imageOutputYX);
	imageOutputYX->SetData((vtkImageData *)projectFilterYX->GetOutput(), m_Volume->GetMTime());
	imageOutputYX->SetName("outputYX");
	imageOutputYX->ReparentTo(m_Volume);

	vtkALBAProjectVolume *projectFilterXY;
	vtkNEW(projectFilterXY);
	projectFilterXY->SetInput(projectFilterX->GetOutput());
	projectFilterXY->SetProjectionModalityToMax();
	projectFilterXY->SetProjectionSideToY();
	projectFilterXY->Update();

	albaVMEImage *imageOutputXY;
	albaNEW(imageOutputXY);
	imageOutputXY->SetData((vtkImageData *)projectFilterXY->GetOutput(), m_Volume->GetMTime());
	imageOutputXY->SetName("outputXY");
	imageOutputXY->ReparentTo(m_Volume);

		vtkALBAProjectVolume *projectFilterYZ;
	vtkNEW(projectFilterYZ);


	//check distance of the min scalar from Air value and Water value to guess if there is some air in the data
	bool hasAir = (abs(scalarRange[0] - AIR_VALUE) < abs(scalarRange[0] - WATER_VALUE));

	//Rescaling cut off zone basing on current CT max value
	double wingsCutOff = CUT_OFF_WINGS*scalarRange[1] / TARGET_MAX;
	double bodyCutOff = CUT_OFF_BODY*scalarRange[1] / TARGET_MAX;

	int xBodyCenter, yBodyCenter, rearWall, bodyRadius;
	double cilCoord[2];
		
	albaLogMessage("Calculate X wings cut off:");
	Range xWingsRange = CaluculateCutOffRange((vtkImageData *)projectFilterYX->GetOutput(), wingsCutOff);
	albaLogMessage("Calculate X body cut off:");
	Range xBodyRange = CaluculateCutOffRange((vtkImageData *)projectFilterYX->GetOutput(), bodyCutOff);

	albaLogMessage("Calculate Y wings cut off:");
	Range yWingsRange = CaluculateCutOffRange((vtkImageData *)projectFilterXY->GetOutput(), wingsCutOff);
	albaLogMessage("Calculate Y body cut off:");
	Range yBodyRange = CaluculateCutOffRange((vtkImageData *)projectFilterXY->GetOutput(), bodyCutOff);

	
	int xWingRSize = xWingsRange.r - xWingsRange.l;
	int xBodyRSize = xBodyRange.r - xBodyRange.l;
	int xRangeDiff = xWingRSize - xBodyRSize;

	int yWingRSize = yWingsRange.r - yWingsRange.l;
	int yBodyRSize = yBodyRange.r - yBodyRange.l;
	int yRangeDiff = yWingRSize - yBodyRSize;

	bool wingsOnX = xRangeDiff > yRangeDiff;
	TailPosition tailPosition;

	if (wingsOnX)
	{
		albaLogMessage("wings on X, xDiff=%d yDiff=%d", xRangeDiff, yRangeDiff);

		if ((abs(xWingRSize*spacing[0] - FULL_VERTEBRAL_MAX_HSPACE) > FULL_VERTEBRAL_MAX_ERROR) || (abs(xBodyRSize*spacing[1] - FULL_VERTEBRAL_MIN_HSPACE) > FULL_VERTEBRAL_MAX_ERROR))
		{
			albaErrorMessage("Error: Cannot estimate Spongious area!");
			return ALBA_ERROR;
		}

		xBodyCenter = (xBodyRange.l + xBodyRange.r) / 2;
		int yDiffLeft = (yBodyRange.l - yWingsRange.l);
		int yDiffRight = (yWingsRange.r - yBodyRange.r);

		if (yDiffLeft > yDiffRight)
		{
			albaLogMessage("Tail on rear"); 
			tailPosition = TAIL_ON_REAR;
			
			albaErrorMessage("Tail on rear this case will be added in the future!");
			return ALBA_ERROR;
		}
		else
		{
			albaLogMessage("Tail on front");
			tailPosition = TAIL_ON_FRONT;

			rearWall = yBodyRange.l + (yBodyRSize*TAIL_RATIO);
			yBodyCenter = (yBodyRange.l + rearWall) / 2;
			bodyRadius = (rearWall - yBodyRange.l) / 2;

			projectFilterY->SetProjectSubRange(true);
			projectFilterY->SetProjectionRange(0, rearWall);
			projectFilterY->Update();
			projectFilterYZ->SetInput(projectFilterY->GetOutput());

		}
	}
	else
	{
		albaLogMessage("wings on Y, xDiff=%d yDiff=%d", xRangeDiff, yRangeDiff);

		if ((abs(xWingRSize*spacing[0] - FULL_VERTEBRAL_MAX_HSPACE) > FULL_VERTEBRAL_MAX_ERROR) || (abs(xBodyRSize*spacing[1] - FULL_VERTEBRAL_MIN_HSPACE) > FULL_VERTEBRAL_MAX_ERROR))
		{
			albaErrorMessage("Error: Cannot estimate Spongious area!");
			return ALBA_ERROR;
		}
		
		yBodyCenter = (yBodyRange.l + yBodyRange.r) / 2;
		int xDiffLeft = (xBodyRange.l - xWingsRange.l);
		int xDiffRight = (xWingsRange.r - xBodyRange.r);

		if (xDiffLeft > xDiffRight)
		{
			albaLogMessage("Tail on left");
			//TODO
			albaErrorMessage("Tail on left this case will be added in the future!");
			return ALBA_ERROR;

		}
		else
		{
			albaLogMessage("Tail on right");

			albaErrorMessage("Tail on right this case will be added in the future!");
			return ALBA_ERROR;
			
			rearWall = xBodyRange.l + (xBodyRSize*TAIL_RATIO);
			xBodyCenter = (xBodyRange.l + rearWall) / 2;
			bodyRadius = (rearWall - xBodyRange.l) / 2;
		}
	}

	projectFilterYZ->SetProjectionModalityToMax();
	projectFilterYZ->SetProjectionSideToX();
	projectFilterYZ->Update();

	albaVMEImage *imageOutputYZ;
	albaNEW(imageOutputYZ);
	imageOutputYZ->SetData((vtkImageData *)projectFilterYZ->GetOutput(), m_Volume->GetMTime());
	imageOutputYZ->SetName("outputYZ");
	imageOutputYZ->ReparentTo(m_Volume);

	albaLogMessage("Calculate Z body cut off:");
	Range zBodyRange = CaluculateCutOffRange((vtkImageData *)projectFilterYZ->GetOutput(), bodyCutOff);
	Range zWingsRange = CaluculateCutOffRange((vtkImageData *)projectFilterYZ->GetOutput(), wingsCutOff);


	double radius = bodyRadius * spacing[0] * MARGIN_REDUCTION;
	double centerX,centerY,centerZ[3];
	double BackCylCenterX, BackCylCenterY, rearRadius1, rearRadius2;

	centerX = origin[0] + (xBodyCenter*spacing[0]);
	centerY = origin[1] + (yBodyCenter*spacing[1]);
	

	int fullVertHeight = (zWingsRange.r - zWingsRange.l)*spacing[2];
	int addZpad = 1; 
	if (abs(fullVertHeight - FULL_VERTEBRAL_ZSPACE) > FULL_VERTEBRAL_MAX_ERROR)
	{
		if (abs(fullVertHeight - CROPPED_VERTEBRAL_ZSPACE) > FULL_VERTEBRAL_MAX_ERROR)
		{
			albaErrorMessage("Error: Cannot estimate vertebral height!");
			return ALBA_ERROR;
		}
		
		addZpad = 0;
	}


	double backCylFullRadius = bodyRadius * spacing[0] * FRONT_BACK_CYL_RADIUS_RATIO;
	double TailCubeCenter[3], TailCubeXlen, TailCubeYLen;
	double zOrigin;

	int upSide = abs(zWingsRange.l-zBodyRange.l)<abs(zWingsRange.r - zBodyRange.r);
	
	if (upSide)
	{

		zOrigin = origin[2] + (zWingsRange.l *spacing[2]);


		centerZ[0] = zOrigin + addZpad*ZCROP_SPACE + CYL_SPACE*2.5 + 2 * INT_VERT_SPACE;
		centerZ[1] = zOrigin + addZpad*ZCROP_SPACE + CYL_SPACE*1.5 + INT_VERT_SPACE;
		centerZ[2] = zOrigin + addZpad*ZCROP_SPACE + CYL_SPACE*0.5;
	}
	else
	{

		zOrigin = origin[2] + (zWingsRange.r *spacing[2]);
		centerZ[0] = zOrigin - (addZpad*ZCROP_SPACE + CYL_SPACE*2.5 + 2 * INT_VERT_SPACE);
		centerZ[1] = zOrigin - (addZpad*ZCROP_SPACE + CYL_SPACE*1.5 + INT_VERT_SPACE);
		centerZ[2] = zOrigin - (addZpad*ZCROP_SPACE + CYL_SPACE*0.5);
	}

	CreateVMECylinder(centerX, centerY, centerZ[0], "CYL L1", CYL_HEIGHT, radius);
	m_SpinalCylinders[0] = CreateCylinder(origin, spacing, centerX, centerY, centerZ[0], radius, CYL_HEIGHT);

	CreateVMECylinder(centerX, centerY, centerZ[1], "CYL L2", CYL_HEIGHT, radius);
	m_SpinalCylinders[1] = CreateCylinder(origin, spacing, centerX, centerY, centerZ[1], radius, CYL_HEIGHT);
	
	CreateVMECylinder(centerX, centerY, centerZ[2], "CYL L3", CYL_HEIGHT, radius);
	m_SpinalCylinders[2] = CreateCylinder(origin, spacing, centerX, centerY, centerZ[2], radius, CYL_HEIGHT);


	if (wingsOnX)
	{
		//back cyl
		BackCylCenterX = centerX;
		BackCylCenterY = origin[1] + rearWall*spacing[1];

		//tail
		TailCubeCenter[1] = BackCylCenterY + (bodyRadius * spacing[0] * BODY_RADIUS_TAIL_RATIO);
		TailCubeCenter[2] = centerZ[2];
		TailCubeCenter[0] = GetTailCenter(volumeData,tailPosition,centerX, TailCubeCenter[1], TailCubeCenter[2]);

		TailCubeXlen = TAILSIZE_MIN;
		TailCubeYLen = TAILSIZE_MAX;

	}
	else
	{
		//back cyl
		BackCylCenterX = origin[0] + rearWall*spacing[0]; 
		BackCylCenterY = centerY;

		//tail
		TailCubeCenter[0] = BackCylCenterX + (bodyRadius * spacing[0] * BODY_RADIUS_TAIL_RATIO);
		TailCubeCenter[2] = centerZ[2];
		TailCubeCenter[1] = GetTailCenter(volumeData, tailPosition, TailCubeCenter[0], centerY, TailCubeCenter[2]);

		TailCubeXlen = TAILSIZE_MAX;
		TailCubeYLen = TAILSIZE_MIN;
	}
	
	double backOutRadius = backCylFullRadius*BACK_CYL_MARGIN_REDUCTION;
	double backInRadius = (backOutRadius * BACK_CYL_R1_R2_RATIO) + (backCylFullRadius-backOutRadius);  //adding margin reduction


	m_BoneCylinders[0] = CreateBoneCylinder(tailPosition,origin, spacing, BackCylCenterX, BackCylCenterY, centerZ[1],backInRadius,backOutRadius, BACK_CYL_HEIGHT);
	CreateVMECylinder(BackCylCenterX, BackCylCenterY, centerZ[1], "Rear CYL L2 out", BACK_CYL_HEIGHT, backOutRadius);
	CreateVMECylinder(BackCylCenterX, BackCylCenterY, centerZ[1], "Rear CYL L2 in", BACK_CYL_HEIGHT, backInRadius);

	m_BoneCylinders[1] = CreateBoneCylinder(tailPosition, origin, spacing, BackCylCenterX, BackCylCenterY, centerZ[2], backInRadius, backOutRadius, BACK_CYL_HEIGHT);
	CreateVMECylinder(BackCylCenterX, BackCylCenterY, centerZ[2], "Rear CYL L3 out", BACK_CYL_HEIGHT, backOutRadius);
	CreateVMECylinder(BackCylCenterX, BackCylCenterY, centerZ[2], "Rear CYL L3 in", BACK_CYL_HEIGHT, backInRadius);

	m_Tail = CreateTail(origin, spacing, TailCubeCenter[0], TailCubeCenter[1], TailCubeCenter[2], TailCubeXlen, TailCubeYLen, CYL_HEIGHT);
	CreateVMEParallelepiped(TailCubeCenter, "Tail", TailCubeXlen, TailCubeYLen, CYL_HEIGHT);


	/*
	albaVMELandmarkCloud *lCloud;
	albaNEW(lCloud);
	lCloud->SetName("PointRef");
	
	
	lCloud->AppendLandmark(centerX, centerY, centerZ[1], "center");
	lCloud->AppendLandmark(centerX, centerY, origin[2] + zWingsRange.l*spacing[2], "zWingL");
	lCloud->AppendLandmark(centerX, centerY, origin[2] + zWingsRange.r*spacing[2], "zWingR");

	
	lCloud->AppendLandmark(origin[0] + (rearWall*spacing[0]), origin[1] + ((dims[1] / 2)*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "wall");
	
	lCloud->AppendLandmark(origin[0] + (xWingsRange.l*spacing[0]), origin[1] + ((dims[1] / 2)*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "xWingL");
	lCloud->AppendLandmark(origin[0] + (xWingsRange.r*spacing[0]), origin[1] + ((dims[1] / 2)*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "xWingR");
	lCloud->AppendLandmark(origin[0] + (xBodyRange.l*spacing[0]), origin[1] + ((dims[1] / 2)*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "xBodyL");
	lCloud->AppendLandmark(origin[0] + (xBodyRange.r*spacing[0]), origin[1] + ((dims[1] / 2)*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "xBodyR");
	
	lCloud->AppendLandmark(origin[0] + ((dims[0] / 2)*spacing[0]), origin[1] + (yWingsRange.l*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "yWingL");
	lCloud->AppendLandmark(origin[0] + ((dims[0] / 2)*spacing[0]), origin[1] + (yWingsRange.r*spacing[1]),  origin[2] + ((dims[2] / 2)*spacing[2]), "yWingR");
	lCloud->AppendLandmark(origin[0] + ((dims[0] / 2)*spacing[0]), origin[1] + (yBodyRange.l*spacing[1]),  origin[2] + ((dims[2] / 2)*spacing[2]), "yBodyL");
	lCloud->AppendLandmark(origin[0] + ((dims[0] / 2)*spacing[0]), origin[1] + (yBodyRange.r*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "yBodyR");


	lCloud->ReparentTo(m_Volume);
	
	*/
		
	CalculateSpinalDensityInfo((vtkImageData *)volumeData);
	CalculateTailDensityInfo((vtkImageData *)volumeData);
	CalculateBoneDensityInfo((vtkImageData *)volumeData);
	

	FitPoints();

	return ALBA_OK;
}

//----------------------------------------------------------------------------
Cylinder albaOpESPCalibration::CreateCylinder(double * origin, double * spacing, double centerX, double centerY, double centerZ, double radius, double cylHeight)
{
	Cylinder cyl;
	cyl.centerX = centerX;
	cyl.centerY = centerY;
	cyl.radius = radius;
	cyl.xRange.l = ((centerX - radius) - origin[0]) / spacing[0];
	cyl.xRange.r = ((centerX + radius) - origin[0]) / spacing[0];
	cyl.yRange.l = ((centerY - radius) - origin[1]) / spacing[1];
	cyl.yRange.r = ((centerY + radius) - origin[1]) / spacing[1];
	cyl.zRange.l = ((centerZ - cylHeight / 2.0) - origin[2]) / spacing[2];
	cyl.zRange.r = ((centerZ + cylHeight / 2.0) - origin[2]) / spacing[2];

	return cyl;
}

//----------------------------------------------------------------------------
BoneCylinder albaOpESPCalibration::CreateBoneCylinder(TailPosition tailPosition, double * origin, double * spacing, double centerX, double centerY, double centerZ, double innerRadius,double OuterRadius, double cylHeight)
{
	BoneCylinder cyl;
	cyl.centerX = centerX;
	cyl.centerY = centerY;
	cyl.InnerRadius = innerRadius;
	cyl.OuterRadius = OuterRadius;
	cyl.xRange.l = ((centerX - OuterRadius) - origin[0]) / spacing[0];
	cyl.xRange.r = ((centerX + OuterRadius) - origin[0]) / spacing[0];
	cyl.yRange.l = ((centerY - OuterRadius) - origin[1]) / spacing[1];
	cyl.yRange.r = ((centerY + OuterRadius) - origin[1]) / spacing[1];
	cyl.zRange.l = ((centerZ - cylHeight / 2.0) - origin[2]) / spacing[2];
	cyl.zRange.r = ((centerZ + cylHeight / 2.0) - origin[2]) / spacing[2];

	//add cut area by excluding area before the rearwall
	if (tailPosition == TAIL_ON_RIGHT)
		cyl.xRange.l = (centerX - origin[0]) / spacing[0];
	else if (tailPosition == TAIL_ON_LEFT)
		cyl.xRange.r = (centerX - origin[0]) / spacing[0];
	else if (tailPosition == TAIL_ON_FRONT)
		cyl.yRange.l = (centerY - origin[1]) / spacing[1];
	else if (tailPosition == TAIL_ON_FRONT)
		cyl.yRange.l = (centerY - origin[1]) / spacing[1];

	return cyl;
}

//----------------------------------------------------------------------------
Tail albaOpESPCalibration::CreateTail(double * origin, double * spacing, double centerX, double centerY, double centerZ, double xSize, double ySize, double zSize)
{
	Tail tail;
	tail.xRange.l = ((centerX - xSize/2.0) - origin[0]) / spacing[0];
	tail.xRange.r = ((centerX + xSize/2.0) - origin[0]) / spacing[0];
	tail.yRange.l = ((centerY - ySize/2.0) - origin[1]) / spacing[1];
	tail.yRange.r = ((centerY + ySize/2.0) - origin[1]) / spacing[1];
	tail.zRange.l = ((centerZ - zSize/2.0) - origin[2]) / spacing[2];
	tail.zRange.r = ((centerZ + zSize/2.0) - origin[2]) / spacing[2];

	return tail;
}

//----------------------------------------------------------------------------
void albaOpESPCalibration::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case ID_SAVE_CALIBRATION:
				SaveCalibration();
				break;

			case wxOK:
				OpStop(OP_RUN_OK);
				break;
			}
		}
		else
		{
			Superclass::OnEvent(alba_event);
		}
	}
}

//----------------------------------------------------------------------------
Range albaOpESPCalibration::CaluculateCutOffRange(vtkImageData *imgData, double cutOff)
{
	vtkPointData 			*inputPd = imgData->GetPointData();
	vtkDataArray 			*inputScalars = inputPd->GetScalars();

	void *inputPointer = inputScalars->GetVoidPointer(0);
	int scalarDim = inputScalars->GetNumberOfTuples();

	double scalarRange[2];
	inputScalars->GetRange(scalarRange);

	switch (inputScalars->GetDataType())
	{
		case VTK_CHAR:
			return CalculateCutOfRange(scalarDim, (char*)inputPointer, cutOff);
			break;
		case VTK_UNSIGNED_CHAR:
			return CalculateCutOfRange(scalarDim, (unsigned char*)inputPointer, cutOff);
			break;
		case VTK_SHORT:
			return CalculateCutOfRange(scalarDim, (short*)inputPointer, cutOff);
			break;
		case VTK_UNSIGNED_SHORT:
			return CalculateCutOfRange(scalarDim, (unsigned short*)inputPointer, cutOff);
			break;
		case VTK_INT:
			return CalculateCutOfRange(scalarDim, (int*)inputPointer, cutOff);
			break;
		case VTK_UNSIGNED_INT:
			return CalculateCutOfRange(scalarDim, (unsigned int*)inputPointer, cutOff);
			break;
		case VTK_FLOAT:
			return CalculateCutOfRange(scalarDim, (float*)inputPointer, cutOff);
			break;
		case VTK_DOUBLE:
			return CalculateCutOfRange(scalarDim, (double*)inputPointer, cutOff);
			break;
		default:
			albaLogMessage("Calibration: Scalar type is not supported");
			Range empty;
			empty.l = empty.r = -1;
			return empty;
	}
}

//----------------------------------------------------------------------------
template<typename DataType>
Range albaOpESPCalibration::CalculateCutOfRange(int scalarSizes, DataType * scalars,double cutOff)
{
	int inCutOff = false;
	int lastInCutOff = false;
	Range range;
	range.l = range.r = -1;
	int left = -1;

	
	lastInCutOff = scalars[0] > cutOff;
	
	for (int i = 1; i < scalarSizes; i++)
	{
		inCutOff = scalars[i] >= cutOff;

		if (inCutOff && lastInCutOff && range.l == -1)
			range.l = i-1;

		if (inCutOff && lastInCutOff)
			range.r = i;

		lastInCutOff = inCutOff;
	}


	albaLogMessage("Cut-off at:%f [%d,%d]",cutOff, range.l,range.r);

	return range;
}


//----------------------------------------------------------------------------
albaVME *albaOpESPCalibration::CreateVMEParallelepiped(double center[3], char * name, double xLen, double yLen, double height)
{
	albaTransform ParalelepipeddBase;
	ParalelepipeddBase.RotateX(90, false);
	ParalelepipeddBase.Translate(center, false);
	ParalelepipeddBase.Update();


	albaVMESurfaceParametric *paralelepiped;
	albaNEW(paralelepiped);
	paralelepiped->SetName(name);
	paralelepiped->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_CUBE);
	paralelepiped->SetCubeXLength(xLen);
	paralelepiped->SetCubeYLength(yLen);
	paralelepiped->SetCubeZLength(height);
	paralelepiped->SetMatrix(ParalelepipeddBase.GetMatrix());

	paralelepiped->ReparentTo(m_Volume);

	GetLogicManager()->VmeShow(paralelepiped, true);

	toHidelist.push_back(paralelepiped);

	return paralelepiped;
}


//----------------------------------------------------------------------------
albaVME *albaOpESPCalibration::CreateVMECylinder(double centerX, double centerY, double centerZ, char * name, double height, double radius)
{
	albaTransform cylinderBase;
	cylinderBase.RotateX(90, false);
	cylinderBase.Translate(centerX, centerY, centerZ, false);
	cylinderBase.Update();


	albaVMESurfaceParametric *cylinder;
	albaNEW(cylinder);
	cylinder->SetName(name);
	cylinder->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_CYLINDER);
	cylinder->SetCylinderHeight(height);
	cylinder->SetCylinderRadius(radius);
	cylinder->SetMatrix(cylinderBase.GetMatrix());

	cylinder->ReparentTo(m_Volume);

	GetLogicManager()->VmeShow(cylinder, true);

	toHidelist.push_back(cylinder);

	return cylinder;
}

//----------------------------------------------------------------------------
bool albaOpESPCalibration::CalculateSpinalDensityInfo(vtkImageData *rg)
{
	double *spacing = rg->GetSpacing();
	double *origin = rg->GetOrigin();

	albaVect3d currentPointVect, center;
	currentPointVect[2] = center[2] = 0;
	int currentPoint[3];

	vtkDataArray* scalars = rg->GetPointData()->GetScalars();

	for (int i = 0; i < 3; i++)
	{
		std::vector<double> values;
		Cylinder cyl = m_SpinalCylinders[i];
		center[0] = cyl.centerX;
		center[1] = cyl.centerY;
		double sumDensity = 0;
		double acc = 0;
		double minValue = VTK_DOUBLE_MAX, maxValue = VTK_DOUBLE_MIN;

#ifdef CREATE_SAMPLING_CLOUDS
		albaVMELandmarkCloud *lCloud;
		albaString tmp;
		tmp.Printf("PointMean Spongious Area:%d", i);
		albaNEW(lCloud);
		lCloud->SetName(tmp);
#endif

		for (int z = cyl.zRange.l; z <= cyl.zRange.r; z++)
			for (int y = cyl.yRange.l; y <= cyl.yRange.r; y++)
				for (int x = cyl.xRange.l; x <= cyl.xRange.r; x++)
				{
					currentPointVect[0] = origin[0] + x*spacing[0];
					currentPointVect[1] = origin[1] + y*spacing[1];

					if (center.Distance(currentPointVect) < cyl.radius)
					{
						currentPoint[0] = x;
						currentPoint[1] = y;
						currentPoint[2] = z;
						vtkIdType pointId = rg->ComputePointId(currentPoint);
						double scalar = scalars->GetTuple1(pointId);
						sumDensity += scalar;
						acc += 1;
						maxValue = MAX(scalar, maxValue);
						minValue = MIN(scalar, minValue);
						values.push_back(scalar);

#ifdef CREATE_SAMPLING_CLOUDS
						if (!(((int)acc) % 15) && (z == cyl.zRange.l || z == cyl.zRange.r)) {
							double pCoord[3];
							pCoord[0] = currentPointVect[0];
							pCoord[1] = currentPointVect[1];
							pCoord[2] = origin[2] + z*spacing[2];

							albaString lName;
							lName.Printf("Point %d ", (int)acc);

							lCloud->AppendLandmark(pCoord[0], pCoord[1], pCoord[2], lName.GetCStr());
						}
#endif
					}
				}
		
		double mean = sumDensity / acc;
		double devSum = 0;
		for (int i = 0; i < acc; i++)
		{
			devSum += (values[i] - mean) * (values[i] - mean);
		}
		double stdDev = sqrt(devSum / acc);
		
		m_AreaInfo[i].mean = mean;
		m_AreaInfo[i].min = minValue;
		m_AreaInfo[i].max = maxValue;
		m_AreaInfo[i].sdtdev = stdDev;
		m_AreaInfo[i].nPoints = acc;
		
		albaLogMessage("\Spinal area L%d:\n Mean:%f\n Min:%f\n Max:%f\n stdDev:%f", i+1, mean, minValue, maxValue, stdDev);

#ifdef CREATE_SAMPLING_CLOUDS
		lCloud->ReparentTo(m_Volume);
#endif
	}



	return true;
}

//----------------------------------------------------------------------------
bool albaOpESPCalibration::CalculateBoneDensityInfo(vtkImageData *rg)
{
	double *spacing = rg->GetSpacing();
	double *origin = rg->GetOrigin();


	albaVect3d currentPointVect, center;
	currentPointVect[2] = center[2] = 0;
	int currentPoint[3];

	vtkDataArray* scalars = rg->GetPointData()->GetScalars();

	double sumDensity = 0, acc = 0;
	double localSumD[2] = { 0,0 }, localAcc[2] = { 0,0 };
	double minValue = VTK_DOUBLE_MAX, maxValue = VTK_DOUBLE_MIN;

	std::vector<double> values;

	for (int i = 0; i < 2; i++)
	{
		BoneCylinder cyl = m_BoneCylinders[i];
		center[0] = cyl.centerX;
		center[1] = cyl.centerY;
		
#ifdef CREATE_SAMPLING_CLOUDS
		albaVMELandmarkCloud *lCloud;
		albaString tmp;
		tmp.Printf("PointMean Cortical Area:%d", i);
		albaNEW(lCloud);
		lCloud->SetName(tmp);
#endif

		for (int z = cyl.zRange.l; z <= cyl.zRange.r; z++)
			for (int y = cyl.yRange.l; y <= cyl.yRange.r; y++)
				for (int x = cyl.xRange.l; x <= cyl.xRange.r; x++)
				{
					currentPointVect[0] = origin[0] + x*spacing[0];
					currentPointVect[1] = origin[1] + y*spacing[1];
					
					double distance = center.Distance(currentPointVect);
					if (distance > cyl.InnerRadius && distance < cyl.OuterRadius)
					{
						currentPoint[0] = x;
						currentPoint[1] = y;
						currentPoint[2] = z;
						vtkIdType pointId = rg->ComputePointId(currentPoint);
						double scalar = scalars->GetTuple1(pointId);
						localSumD[i] += scalar;
						sumDensity += scalar;
						localAcc[i] += 1;
						acc += 1;
						maxValue = MAX(scalar, maxValue);
						minValue = MIN(scalar, minValue);
						values.push_back(scalar);

#ifdef CREATE_SAMPLING_CLOUDS
						if (!(((int)acc) % 15) && (z == cyl.zRange.l || z == cyl.zRange.r)) {
							double pCoord[3];
							pCoord[0] = currentPointVect[0];
							pCoord[1] = currentPointVect[1];
							pCoord[2] = origin[2] + z*spacing[2];

							albaString lName;
							lName.Printf("Point %d ", (int)acc);

							lCloud->AppendLandmark(pCoord[0], pCoord[1], pCoord[2], lName.GetCStr());
						}
#endif
					}
				}

		m_BoneMean[i] = localSumD[i] / localAcc[i];

		albaLogMessage("\nBone area #%d:\n Mean:%f\n ", i, m_BoneMean[i]);

#ifdef CREATE_SAMPLING_CLOUDS
		lCloud->ReparentTo(m_Volume);
#endif
	}

	//TODO ADD check to localmean difference

	double mean = sumDensity / acc;
	double devSum = 0;
	for (int i = 0; i < acc; i++)
	{
		devSum += (values[i] - mean) * (values[i] - mean);
	}
	double stdDev = sqrt(devSum / acc);

	m_AreaInfo[4].mean = mean;
	m_AreaInfo[4].min = minValue;
	m_AreaInfo[4].max = maxValue;
	m_AreaInfo[4].sdtdev = stdDev;
	m_AreaInfo[4].nPoints = acc;

	albaLogMessage("\Bone total area:\n Mean:%f\n Min:%f\n Max:%f\n stdDev:%f", mean, minValue, maxValue, stdDev);



	return true;
}

//----------------------------------------------------------------------------
bool albaOpESPCalibration::CalculateTailDensityInfo(vtkImageData *rg)
{
	double *spacing = rg->GetSpacing();
	double *origin = rg->GetOrigin();

	int currentPoint[3];

	vtkDataArray* scalars = rg->GetPointData()->GetScalars();


	std::vector<double> values;
	double sumDensity = 0;
	double acc = 0;
	double minValue = VTK_DOUBLE_MAX, maxValue = VTK_DOUBLE_MIN;

#ifdef CREATE_SAMPLING_CLOUDS
	albaVMELandmarkCloud *lCloud;
	albaNEW(lCloud);
	lCloud->SetName("Tail Area");
#endif

	for (int z = m_Tail.zRange.l; z <= m_Tail.zRange.r; z++)
		for (int y = m_Tail.yRange.l; y <= m_Tail.yRange.r; y++)
			for (int x = m_Tail.xRange.l; x <= m_Tail.xRange.r; x++)
			{
				
					currentPoint[0] = x;
					currentPoint[1] = y;
					currentPoint[2] = z;
					vtkIdType pointId = rg->ComputePointId(currentPoint);
					double scalar = scalars->GetTuple1(pointId);
					sumDensity += scalar;
					acc += 1;
					maxValue = MAX(scalar, maxValue);
					minValue = MIN(scalar, minValue);
					values.push_back(scalar);

#ifdef CREATE_SAMPLING_CLOUDS
					if (!(((int)acc) % 15) && (z == m_Tail.zRange.l || z == m_Tail.zRange.r)) {
			
						albaString lName;
						double pCoord[3];

						lName.Printf("Point %d ", (int)acc);
						pCoord[0] = origin[0] + z*spacing[0];
						pCoord[1] = origin[1] + z*spacing[1];
						pCoord[2] = origin[2] + z*spacing[2];
						lCloud->AppendLandmark(pCoord[0], pCoord[1], pCoord[2], lName.GetCStr());
					}
#endif
			}

	double mean = sumDensity / acc;
	double devSum = 0;
	for (int i = 0; i < acc; i++)
	{
		devSum += (values[i] - mean) * (values[i] - mean);
	}
	double stdDev = sqrt(devSum / acc);

	m_AreaInfo[3].mean = mean;
	m_AreaInfo[3].min = minValue;
	m_AreaInfo[3].max = maxValue;
	m_AreaInfo[3].sdtdev = stdDev;
	m_AreaInfo[3].nPoints = acc;

	albaLogMessage("\nTail area:\n Mean:%f\n Min:%f\n Max:%f\n stdDev:%f", mean, minValue, maxValue, stdDev);

#ifdef CREATE_SAMPLING_CLOUDS
	lCloud->ReparentTo(m_Volume);
#endif


	return true;
}

//----------------------------------------------------------------------------
double albaOpESPCalibration::GetTailCenter(vtkImageData *rg, TailPosition tailPos, double x, double y, double z)
{
	vtkDataArray* scalars = rg->GetPointData()->GetScalars();

	vtkIdType startCenter = rg->FindPoint(x, y, z);
	double *origin = rg->GetOrigin();
	double *spacing = rg->GetSpacing();

	int *dims = rg->GetDimensions();
	int zPos = startCenter / (dims[0]*dims[1]);
	int yResidue = startCenter - (zPos * dims[0] * dims[1]);
	int yPos = yResidue / dims[0];
	int xPos = yResidue - (yPos * dims[0]);
	bool onTail = true;

	int l, r;
	if (tailPos == TAIL_ON_FRONT || tailPos == TAIL_ON_REAR)
	{
		l = xPos;
		while (onTail)
		{
			vtkIdType currentPoint = zPos * dims[0] * dims[1] + yPos * dims[0] + l;
			onTail = scalars->GetTuple1(currentPoint) > CUT_OFF_WINGS;
			l--;
		}

		onTail = true;
		r = xPos;
		while (onTail)
		{
			vtkIdType currentPoint = zPos * dims[0] * dims[1] + yPos * dims[0] + r;
			onTail = scalars->GetTuple1(currentPoint) > CUT_OFF_WINGS;
			r++;
		}
		
		//TODO ADD CHECK ON SIZE

		double center = (l + r) / 2.0;
		return origin[0] + center*spacing[0];
	
	}
	else
	{
		l = yPos;
		while (onTail)
		{
			vtkIdType currentPoint = zPos * dims[0] * dims[1] + l * dims[0] + xPos;
			onTail = scalars->GetTuple1(currentPoint) > CUT_OFF_WINGS;
			l--;
		}

		onTail = true;
		r = yPos;
		while (onTail)
		{
			vtkIdType currentPoint = zPos * dims[0] * dims[1] + l * dims[0] + xPos;
			onTail = scalars->GetTuple1(currentPoint) > CUT_OFF_WINGS;
			r++;
		}

		//TODO ADD CHECK ON SIZE

		double center = (l + r) / 2.0;
		return origin[1] + center*spacing[1];

	}
}

//----------------------------------------------------------------------------
bool albaOpESPCalibration::FitPoints()
{
	int nPoints = 5;
	double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;

	double expected[5] = EXPECTED_VALUES;

	for (int i = 0; i < nPoints; i++) 
	{
		double x = m_AreaInfo[i].mean;
		double y = expected[i];

		sumX += x;
		sumY += y;
		sumXY += x * y;
		sumX2 += x * x;
	}
	double xMean = sumX / nPoints;
	double yMean = sumY / nPoints;
	double denominator = sumX2 - sumX * xMean;
	// You can tune the eps (1e-7) below for your specific task
	if (std::fabs(denominator) < 1e-7) 
	{
		// Fail: it seems a vertical line
		return false;
	}
	m_Slope = (sumXY - sumX * yMean) / denominator;
	m_Intercept = yMean - m_Slope * xMean;

	//calculate squared residues, their sum etc.
	double yRes = 0;
	double yResSum = 0;
	double res = 0;
	double resSum = 0;
	for (int i = 0; i < nPoints; i++)
	{
		double x = m_AreaInfo[i].mean;
		double y = expected[i];

		//current (y_i - a0 - a1 * x_i)^2
		yRes = pow((y - m_Intercept - (m_Slope * x)), 2);

		//sum of (y_i - a0 - a1 * x_i)^2
		yResSum += yRes;

		//current residue squared (y_i - AVGy)^2
		res = pow(y - yMean, 2);

		//sum of squared residues
		resSum += res;

	}

	//calculate r^2 coefficient of determination
	m_RSquare = (resSum - yResSum) / resSum;

	albaLogMessage("RESULT: \n Slope:%f \n Intercept:%f", m_Slope, m_Intercept);
	return true;
}

//----------------------------------------------------------------------------
void albaOpESPCalibration::CreateGui()
{
	// Interface:
	m_Gui = new albaGUI(this);
	m_Gui->Divider();
	m_Gui->Label("Calibration:", true);
	m_Gui->Label("(mgHA/cm^3 = slope * GV + intercept)");
	m_Gui->Double(-1, "Slope", &m_Slope,VTK_DOUBLE_MIN,VTK_DOUBLE_MAX,4);
	m_Gui->Double(-1, "Intercept", &m_Intercept, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 4);
	m_Gui->Double(-1, "R^2", &m_RSquare, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 4);
	m_Gui->Divider(1);

	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Label("Spongious T1, Area 50:", true);
	m_Gui->Double(-1, "Mean", &m_AreaInfo[0].mean, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "StdDev", &m_AreaInfo[0].sdtdev, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Min", &m_AreaInfo[0].min, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Max", &m_AreaInfo[0].max, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
			
	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Label("Spongious T2, Area 100:", true);
	m_Gui->Double(-1, "Mean", &m_AreaInfo[1].mean, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "StdDev", &m_AreaInfo[1].sdtdev, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Min", &m_AreaInfo[1].min, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Max", &m_AreaInfo[1].max, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);

	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Label("Spongious T3, Area 200:", true);
	m_Gui->Double(-1, "Mean", &m_AreaInfo[2].mean, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "StdDev", &m_AreaInfo[2].sdtdev, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Min", &m_AreaInfo[2].min, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Max", &m_AreaInfo[2].max, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);

	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Label("Spinal Process, Area 400:", true);
	m_Gui->Double(-1, "Mean", &m_AreaInfo[3].mean, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "StdDev", &m_AreaInfo[3].sdtdev, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Min", &m_AreaInfo[3].min, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Max", &m_AreaInfo[3].max, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);


	
	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Label("Cortical Structures, Area 800:",true);
	m_Gui->Double(-1, "L2 Mean", &m_BoneMean[0], VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "L3 Mean", &m_BoneMean[1], VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Label("   Full area:");
	m_Gui->Double(-1, "Mean", &m_AreaInfo[4].mean, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "StdDev", &m_AreaInfo[4].sdtdev, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Min", &m_AreaInfo[4].min, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Max", &m_AreaInfo[4].max, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);


	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->Button(ID_SAVE_CALIBRATION, "Save Calibration");

	m_Gui->Enable(-1, false);

	m_Gui->Divider(1);
	
	m_Gui->Button(wxOK, "Close");


	ShowGui();
}

//----------------------------------------------------------------------------
char ** albaOpESPCalibration::GetIcon()
{
#include "pic/MENU_CALIBRATION_ICON.xpm"
	return MENU_CALIBRATION_ICON_xpm;
}

#define ADD_DICOM_TAG(TAGTOADD) if (m_Input->GetTagArray()->IsTagPresent(TAGTOADD))													\
																{																																						\
																	albaString tmpStr = m_Input->GetTagArray()->GetTag(TAGTOADD)->GetValue();	\
																	dicomTags->setAttribute(albaXMLString(TAGTOADD), albaXMLString(tmpStr));	\
																}

// Create Report
//----------------------------------------------------------------------------
bool albaOpESPCalibration::SaveCalibration() 
	{

		albaString initialFileName;
		initialFileName = albaGetDocumentsDirectory().c_str();
		initialFileName.Append("\\ESPcalibration.xml");

		albaString wildc = "Calibration xml file (*.xml)|*.xml";
		albaString calbrationFilename = albaGetSaveFile(initialFileName.GetCStr(), wildc).c_str();

		//Open the file xml
		try
		{
			XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
		}
		catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
		{
			// Do your failure processing here
			return ALBA_ERROR;
		}

		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc;
		XMLCh tempStr[100];
		XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("LS", tempStr, 99);
		XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation *impl = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(tempStr);
		XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter* theSerializer = ((XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS*)impl)->createDOMWriter();
		theSerializer->setNewLine(albaXMLString("\r"));

		if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true))
			theSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);

		doc = impl->createDocument(NULL, albaXMLString("CALIBRATION"), NULL);

		doc->setEncoding(albaXMLString("UTF-8"));
		doc->setStandalone(true);
		doc->setVersion(albaXMLString("1.0"));

		// extract root element and wrap it with an albaXMLElement object
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
		assert(root);

		// attach version attribute to the root node
		root->setAttribute(albaXMLString("Version"), albaXMLString(albaString(1)));

		try
		{
			// DENSITOMETRIC_CALIBRATION
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *densitometricCalibration = doc->createElement(albaXMLString("DensitometricCalibration"));
			densitometricCalibration->setAttribute(albaXMLString("CalibrationType"), albaXMLString(albaString("ESP_Phantom")));
			densitometricCalibration->setAttribute(albaXMLString("Intercept"), albaXMLString(albaString(m_Intercept)));
			densitometricCalibration->setAttribute(albaXMLString("Slope"), albaXMLString(albaString(m_Slope)));
			densitometricCalibration->setAttribute(albaXMLString("RSquare"), albaXMLString(albaString(m_RSquare)));

			//Area 50
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *area50 = doc->createElement(albaXMLString("SpongiousT1Area50"));
			area50->setAttribute(albaXMLString("Mean"), albaXMLString(albaString(m_AreaInfo[0].mean)));
			area50->setAttribute(albaXMLString("StdDev"), albaXMLString(albaString(m_AreaInfo[0].sdtdev)));
			area50->setAttribute(albaXMLString("Min"), albaXMLString(albaString(m_AreaInfo[0].min)));
			area50->setAttribute(albaXMLString("Max"), albaXMLString(albaString(m_AreaInfo[0].max)));
			densitometricCalibration->appendChild(area50);

			//Area 100
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *area100 = doc->createElement(albaXMLString("SpongiousT2Area100"));
			area100->setAttribute(albaXMLString("Mean"), albaXMLString(albaString(m_AreaInfo[1].mean)));
			area100->setAttribute(albaXMLString("StdDev"), albaXMLString(albaString(m_AreaInfo[1].sdtdev)));
			area100->setAttribute(albaXMLString("Min"), albaXMLString(albaString(m_AreaInfo[1].min)));
			area100->setAttribute(albaXMLString("Max"), albaXMLString(albaString(m_AreaInfo[1].max)));
			densitometricCalibration->appendChild(area100);

			//Area 200
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *area200 = doc->createElement(albaXMLString("SpongiousT2Area200"));
			area200->setAttribute(albaXMLString("Mean"), albaXMLString(albaString(m_AreaInfo[2].mean)));
			area200->setAttribute(albaXMLString("StdDev"), albaXMLString(albaString(m_AreaInfo[2].sdtdev)));
			area200->setAttribute(albaXMLString("Min"), albaXMLString(albaString(m_AreaInfo[2].min)));
			area200->setAttribute(albaXMLString("Max"), albaXMLString(albaString(m_AreaInfo[2].max)));
			densitometricCalibration->appendChild(area200);

			//Area 400
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *area400 = doc->createElement(albaXMLString("SpinalProcessArea400"));
			area400->setAttribute(albaXMLString("Mean"), albaXMLString(albaString(m_AreaInfo[3].mean)));
			area400->setAttribute(albaXMLString("StdDev"), albaXMLString(albaString(m_AreaInfo[3].sdtdev)));
			area400->setAttribute(albaXMLString("Min"), albaXMLString(albaString(m_AreaInfo[3].min)));
			area400->setAttribute(albaXMLString("Max"), albaXMLString(albaString(m_AreaInfo[3].max)));
			densitometricCalibration->appendChild(area400);


			//Area 400
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *area800 = doc->createElement(albaXMLString("SpinalProcessArea400"));
			area800->setAttribute(albaXMLString("MeanL2"), albaXMLString(albaString(m_BoneMean[0])));
			area800->setAttribute(albaXMLString("MeanL3"), albaXMLString(albaString(m_BoneMean[1])));
			area800->setAttribute(albaXMLString("Mean"), albaXMLString(albaString(m_AreaInfo[4].mean)));
			area800->setAttribute(albaXMLString("StdDev"), albaXMLString(albaString(m_AreaInfo[4].sdtdev)));
			area800->setAttribute(albaXMLString("Min"), albaXMLString(albaString(m_AreaInfo[4].min)));
			area800->setAttribute(albaXMLString("Max"), albaXMLString(albaString(m_AreaInfo[4].max)));
			densitometricCalibration->appendChild(area800);

			//Area 400
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *dicomTags = doc->createElement(albaXMLString("DicomTags"));
			ADD_DICOM_TAG("Modality");

			ADD_DICOM_TAG("AcquisitionDate");
			ADD_DICOM_TAG("Manufacturer");
			ADD_DICOM_TAG("InstitutionName");
			ADD_DICOM_TAG("ManufacturersModelName");

			ADD_DICOM_TAG("KVP");
			ADD_DICOM_TAG("XRayTubeCurrent");
			ADD_DICOM_TAG("FocalSpots");
			ADD_DICOM_TAG("FilterType");

			ADD_DICOM_TAG("SliceThickness");
			ADD_DICOM_TAG("TableHeight");
			ADD_DICOM_TAG("ExposureTime");
			ADD_DICOM_TAG("TotalCollimationWidth");
			ADD_DICOM_TAG("SpiralPitchFactor");

			ADD_DICOM_TAG("SpacingBetweenSlices");
			ADD_DICOM_TAG("PixelSpacing");
			ADD_DICOM_TAG("ConvolutionKernel");


			densitometricCalibration->appendChild(dicomTags);

			root->appendChild(densitometricCalibration);
		}
		catch (...)
		{
			return ALBA_ERROR;
		}

		XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget *XMLTarget;
		albaString fileName = calbrationFilename;

		XMLTarget = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget(fileName);

		try
		{
			// do the serialization through DOMWriter::writeNode();
			theSerializer->writeNode(XMLTarget, *doc);
		}
		catch (const XERCES_CPP_NAMESPACE_QUALIFIER  XMLException& toCatch)
		{
			char* message = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.getMessage());
			XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&message);
			return ALBA_ERROR;
		}
		catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch)
		{
			char* message = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.msg);
			XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&message);
			return ALBA_ERROR;
		}
		catch (...) {
			return ALBA_ERROR;
		}

		theSerializer->release();
		cppDEL(XMLTarget);
		doc->release();

		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

		albaLogMessage("New calibration file has been written %s", fileName.GetCStr());

		return ALBA_OK;
	}


