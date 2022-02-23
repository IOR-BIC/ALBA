/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_Template.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __appInteractor2DMeasure_Template_h
#define __appInteractor2DMeasure_Template_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaInteractor2DMeasure.h"
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkActor;
class vtkActor2D;
class vtkDataSetMapper;
class vtkImageData;
class vtkLookupTable;
class vtkPolyDataMapper2D;
class vtkTransform;
class vtkTransformPolyDataFilter;
class albaVMEImage;

// Class Name: albaInteractor2DMeasure_Template
class ALBA_EXPORT albaInteractor2DMeasure_Template : public albaInteractor2DMeasure
{
public:

	albaTypeMacro(albaInteractor2DMeasure_Template, albaInteractor2DMeasure);

	// MEASURE
	/** Add Measure*/
	void AddMeasure(double * pointUL, double * pointUR, double * pointDR, double * pointDL);
	/** Edit Points*/
	virtual void EditPoints(int index, double * pointUL, double * pointUR, double * pointDR, double * pointDL);
	virtual void EditTextureCoord(int index, double * pointUL, double * pointUR, double * pointDR, double * pointDL);
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point);
	/** Delete the Measure*/
	virtual void RemoveMeasure(int index);
	/** Select a Measure*/
	virtual void SelectMeasure(int index);

	virtual void SetPointSize(double size);
	void SetTexture(vtkImageData *imageData);

	void SetTextureOpacity(double opacity);

	void SetTextureColor(double r, double g, double b);
	
	/** Get measure points*/
	void GetMeasurePoints(int index, double * pointUL, double * pointUR, double * pointDR, double * pointDL);
	void GetCenter(int index, double *center);

	virtual void Show(bool show);
	void ShowPoints(bool show);
	void ShowTexture(bool show);

	void Rotate(double angle, double *origin = NULL);
	void CropImage(albaVMEImage *vmeImage);
	void CropImage(albaVMEImage *vmeImage, double *bounds);

	bool Load(albaVME *input, wxString tag);
	bool Save(albaVME *input, wxString tag);

	vtkTexture* GetTexture(int index) { return m_TexturedQuadVector[index]->GetTexture(); }
	void SetTexture(int index, vtkTexture* texture) { m_TexturedQuadVector[index]->SetTexture(texture); }

	vtkLookupTable* GetLut() { return m_TemplateLookupTable; };

protected:

	albaInteractor2DMeasure_Template();
	virtual ~albaInteractor2DMeasure_Template();

	// Draw Measure
	void MoveMeasure(int index, double * point);
	void DrawNewMeasure(double * wp);

	// RENDERING
	void UpdatePointsActor(double * pointUL, double * pointUR, double * pointDR, double * pointDL);
	void UpdateTexureActor(double * pointUL, double * pointUR, double * pointDR, double * pointDL);

	// UTILS
	virtual void FindAndHighlight(double * point);

	wxString GetConfigDirectory();

	// Persistent Points
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorUL;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorUR;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorDR;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorDL;
	std::vector<albaActor2dStackHelper *> m_PointsStackVectorC;

	// Persistent Texture Quad
	std::vector<vtkPolyData *> m_QuadVector;
	std::vector<vtkActor *> m_TexturedQuadVector;

	vtkLookupTable	*m_TemplateLookupTable;

	double m_Angle;

private:
	albaInteractor2DMeasure_Template(const albaInteractor2DMeasure_Template&);   // Not implemented.
	void operator=(const albaInteractor2DMeasure_Template&);  // Not implemented.
	friend class appInteractor2DMeasure_TemplateTest;
};
#endif
