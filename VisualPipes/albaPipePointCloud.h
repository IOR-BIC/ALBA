/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePointCloud
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipePointCloud_H__
#define __albaPipePointCloud_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaPipeGenericPolydata.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkTrivialProducer;

//----------------------------------------------------------------------------
// albaPipePointCloud :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipePointCloud : public albaPipeGenericPolydata
{
public:
	albaTypeMacro(albaPipePointCloud, albaPipeGenericPolydata);

	albaPipePointCloud();
	virtual     ~albaPipePointCloud();

	/** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	albaGUI *CreateGui();

	/** Set the representation Type possibility are Surface, Wireframe, Points */
	virtual void SetRepresentation(REPRESENTATIONS rep);

	/** Update the properties */
	virtual void UpdateProperty(bool fromTag = false);
protected:

	vtkAlgorithmOutput *GetPolyDataOutputPort();
	vtkTrivialProducer* m_TrivialProd;
};
#endif // __albaPipePointCloud_H__
