/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkTriangleQualityRatio.h,v $
Language:  C++
Date:      $Date: 2007-04-05 10:27:54 $
Version:   $Revision: 1.1 $
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

#ifndef __vtkTriangleQualityRatio_h
#define __vtkTriangleQualityRatio_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "vtkObject.h"
#include "vtkMAFConfigure.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;

//----------------------------------------------------------------------------
// vtkTriangleQualityRatio :
//----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkTriangleQualityRatio : public vtkObject 
{

public:

	static vtkTriangleQualityRatio *New();

	vtkTypeMacro(vtkTriangleQualityRatio,vtkObject);

	// To get average value
	double GetMeanRatio() {return this->m_MeanRatio;};

	// To get the max value
	double GetMaxRatio() {return this->m_MaxRatio;};

	// To get the min value
	double GetMinRatio() {return this->m_MinRatio;};

	// Set a PolyData as input
	void SetInput(vtkPolyData *UserSetInput) {this->m_Input = UserSetInput;};  

	void Update();

	vtkPolyData* GetOutput(){return m_Output;};

protected:
	vtkTriangleQualityRatio();
	~vtkTriangleQualityRatio();

	double m_MeanRatio;
	double m_MaxRatio;
	double m_MinRatio;

	vtkPolyData *m_Input;
	vtkPolyData *m_Output;
};

#endif