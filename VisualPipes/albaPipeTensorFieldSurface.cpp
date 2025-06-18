/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeTensorFieldSurface.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-08-26 14:47:00 $ 
  Version: $Revision: 1.1.2.3 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  modify: Hui Wei (beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"
#include "albaPipeTensorFieldSurface.h"

#include "albaSceneNode.h"
#include "albaPipeFactory.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"
#include "albaGUILutSwatch.h"
#include "albaGUILutPreset.h"
#include "albaVME.h"

#include "vtkALBAAssembly.h"
#include "vtkDataSet.h"
#include "vtkGeometryFilter.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkLookupTable.h"

#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkScalarBarActor.h"
#include "vtkRenderer.h"
#include "vtkMath.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "vtkImageData.h"

//#include "vtkArrayCalculator.h"

#include "albaGUIBusyInfo.h"
#include <float.h>


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeTensorFieldSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeTensorFieldSurface::albaPipeTensorFieldSurface() : albaPipeTensorField()
//----------------------------------------------------------------------------
{     
  m_ColorMappingLUT = NULL;  
  m_ColorMappingMode = 0;  
  m_ShowMapping = 1;  
    
  m_SurfaceActor = NULL;
  m_SurfaceMapper = NULL;
  
  m_MappingActor = NULL; 
}

//----------------------------------------------------------------------------
albaPipeTensorFieldSurface::~albaPipeTensorFieldSurface()
//----------------------------------------------------------------------------
{  
  m_AssemblyFront->RemovePart(m_SurfaceActor);  
  m_RenFront->RemoveActor2D(m_MappingActor);  
  
  vtkDEL(m_MappingActor);
  
  vtkDEL(m_SurfaceActor);
  vtkDEL(m_SurfaceMapper);
  
  vtkDEL(m_ColorMappingLUT);  
}


//----------------------------------------------------------------------------
albaGUI *albaPipeTensorFieldSurface::CreateGui()
//----------------------------------------------------------------------------
{
  int nTensors = GetNumberOfTensors();
  
  m_Gui = new albaGUI(this);
     
  wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );
  if (nTensors == 0)  //no tensor field available, this pipe does not work
    bSizerMain->Add(new wxStaticText( m_Gui, wxID_ANY, _("No tensor field to visualize.")));
  else
  {
    if (nTensors > 1)      
    {
      wxBoxSizer* bSizer1 = new wxBoxSizer( wxHORIZONTAL );
      bSizer1->Add( new wxStaticText( m_Gui, wxID_ANY, _("Tensor Field:"), 
        wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

      wxComboBox* comboField = new wxComboBox( m_Gui, ID_VECTORFIELD, wxEmptyString, 
        wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      comboField->SetToolTip( _("Selects the tensor field to be visualized.") );
      PopulateCombo(comboField, true);  //at least one tensor is available
      bSizer1->Add( comboField, 1, wxALL, 1 );
      bSizerMain->Add( bSizer1, 0, wxEXPAND, 0 );

      //and validator
      comboField->SetValidator(albaGUIValidator(this, ID_VECTORFIELD, comboField, &m_TensorFieldIndex));
    }
    
    wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );
    bSizer2->Add( new wxStaticText( m_Gui, wxID_ANY, _("Color by:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    m_ComboColorBy = new wxComboBox( m_Gui, ID_COLOR_MAPPING_MODE, 
      wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
    UpdateColorByCombo();      
    
    m_ComboColorBy->SetToolTip( 
      _("Specifies how the specified tensor field should be transformed into colors.") );
    bSizer2->Add( m_ComboColorBy, 1, wxALL, 0 );
    bSizerMain->Add( bSizer2, 0, wxEXPAND, 5 );

    //and validator
    m_ComboColorBy->SetValidator(albaGUIValidator(this, ID_COLOR_MAPPING_MODE, m_ComboColorBy, &m_ColorMappingMode));
  
    wxBoxSizer* bSizer3 = new wxBoxSizer( wxHORIZONTAL );    
    bSizer3->Add( new wxStaticText( m_Gui, wxID_ANY, _("LUT:")), 0, wxALL, 5 );

    albaGUILutSwatch* luts2 = new albaGUILutSwatch(m_Gui, ID_COLOR_MAPPING_LUT, 
      wxDefaultPosition, wxSize(140, 18), wxTAB_TRAVERSAL | wxSIMPLE_BORDER );
    luts2->SetLut(m_ColorMappingLUT);
    luts2->SetEditable(true);
    luts2->SetListener(this);
    bSizer3->Add(luts2, 0, 0/*wxEXPAND*/, 5 );
    bSizerMain->Add( bSizer3, 1, wxEXPAND, 5 );

    wxCheckBox* chckShowSFLegend1 = new wxCheckBox( m_Gui, ID_SHOW_COLOR_MAPPING, 
      _("Show Color Mapping"), wxDefaultPosition, wxDefaultSize, 0 );
    chckShowSFLegend1->SetToolTip( _("If checked, mapping is displayed "
      "as a colored bar in the main view.") );
    bSizerMain->Add( chckShowSFLegend1, 0, wxALL|wxEXPAND, 5 );

    //and validator
    chckShowSFLegend1->SetValidator(albaGUIValidator(this, ID_SHOW_COLOR_MAPPING,       
      chckShowSFLegend1, &m_ShowMapping));
  }

  m_Gui->Add(bSizerMain);
  return m_Gui;
}


//----------------------------------------------------------------------------
void albaPipeTensorFieldSurface::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{			
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {	
    if (e->GetId() >= Superclass::ID_LAST && e->GetId() < ID_LAST)
    {
		if (e->GetId() == ID_VECTORFIELD){
			UpdateColorByCombo(); //we need to update list of components
		}	  

      UpdateVTKPipe(); 
    
			GetLogicManager()->CameraUpdate();
      return;
    }     
  }

  //forward it to our listener to deal with it
  albaEventMacro(*alba_event);
}

//------------------------------------------------------------------------
//Constructs VTK pipeline.
/*virtual*/ void albaPipeTensorFieldSurface::CreateVTKPipe()
//------------------------------------------------------------------------
{  
  //build LUT
  m_ColorMappingLUT = vtkLookupTable::New();
  lutPreset(12, m_ColorMappingLUT); //initialize LUT to SAR (it has index 12)  
  m_ColorMappingLUT->Build(); 

  vtkGeometryFilter* filter = vtkGeometryFilter::New();
  filter->SetInput(m_Vme->GetOutput()->GetVTKData());

  m_SurfaceMapper = vtkPolyDataMapper::New();
  m_SurfaceMapper->SetInput(filter->GetOutput());
  m_SurfaceMapper->ImmediateModeRenderingOn();
  m_SurfaceMapper->SetScalarModeToUsePointFieldData();// PointData();
  m_SurfaceMapper->SetColorModeToMapScalars();
  m_SurfaceMapper->SetLookupTable(m_ColorMappingLUT);

  filter->Delete(); //no longer needed

  m_SurfaceActor = vtkActor::New();
  m_SurfaceActor->SetMapper(m_SurfaceMapper);
  m_SurfaceActor->SetPickable(0);   //make it faster

  //scalar field map
  m_MappingActor = vtkScalarBarActor::New();
  m_MappingActor->SetLookupTable(m_SurfaceMapper->GetLookupTable());
  ((vtkActor2D*)m_MappingActor)->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  ((vtkActor2D*)m_MappingActor)->GetPositionCoordinate()->SetValue(0.1,0.01);
  m_MappingActor->SetOrientationToHorizontal();
  m_MappingActor->SetWidth(0.8);
  m_MappingActor->SetHeight(0.12);  
  m_MappingActor->SetLabelFormat("%6.3g");
  m_MappingActor->SetPickable(0);   //make it faster

  m_RenFront->AddActor2D(m_MappingActor);
  m_AssemblyFront->AddPart(m_SurfaceActor); 
}

//------------------------------------------------------------------------
//Updates VTK pipeline (setting radius, etc.). 
/*virtual*/ void albaPipeTensorFieldSurface::UpdateVTKPipe()
//------------------------------------------------------------------------
{
 
  /*vtkFloatArray *tensors ;

  tensors = vtkFloatArray::New() ;
  tensors->SetNumberOfComponents(9) ;
  
  const char* tensor_name = GetTensorFieldName(m_TensorFieldIndex);
  tensors->SetName(tensor_name); //("tensors") ;

  vtkDataArray* da = m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetTensors(tensor_name);
  
  int num =  m_Vme->GetOutput()->GetVTKData()->GetNumberOfPoints();
  for (int i=0;i<num;i++)
  {
	double *tens = da->GetTuple9(i);
	m_covariance[0][0] = tens[0];
	m_covariance[1][0] = tens[1];
	m_covariance[2][0] = tens[2];
	m_covariance[0][1] = tens[3];
	m_covariance[1][1] = tens[4];
	m_covariance[2][1] = tens[5];
	m_covariance[0][2] = tens[6];
	m_covariance[1][2] = tens[7];
	m_covariance[2][2] = tens[8];

	EigenVectors3x3(m_covariance, m_lambda, m_V) ;

	double sd[3] ;
	sd[0] = sqrt(m_lambda[0]) ;
	sd[1] = sqrt(m_lambda[1]) ;
	sd[2] = sqrt(m_lambda[2]) ;

	//double Vmat[9] ;
	MultiplyColumnsByScalars(sd,tens,tens);
	tensors->InsertNextTuple(tens);
  }

  m_Vme->GetOutput()->GetVTKData()->GetPointData()->SetTensors(tensors);
  */

  const char* tensor_name = GetTensorFieldName(m_TensorFieldIndex);

  double sr[2];
  vtkDataArray* da ;
  vtkImageData *orgData =vtkImageData::SafeDownCast(m_Vme->GetOutput()->GetVTKData()) ;  
  da = orgData->GetPointData()->GetTensors(tensor_name);
  
  m_SurfaceMapper->SelectColorArray(tensor_name);

  int idx ;//= m_ColorMappingMode - CMM_COMPONENT1;  //index of component
  
  if (m_ColorMappingMode == CMM_MAGNITUDE )//magnitude
  {      
     idx = -1;
	 m_ColorMappingLUT->SetVectorModeToMagnitude();    
	 da->GetRange(sr, idx);
  }
  else if ( m_ColorMappingMode>CMM_MAGNITUDE && m_ColorMappingMode<CMM_COMPONENT1)    // eigenvalue ¦Ë
  {
	  ComputeEigenvalues(orgData,sr,m_ColorMappingMode-1);
  }
  else //component
  {
    idx = m_ColorMappingMode - CMM_COMPONENT1;
    //some component
    m_ColorMappingLUT->SetVectorModeToComponent();
    m_ColorMappingLUT->SetVectorComponent(idx);    
  }

  //get range for the given component / magnitude
  //RELASE NOTE: GetRange has an undocumented feature to compute
  //magnitude, if the component parameter is negative
  //BUT it has also a BUG, it cannot support more than 3 components
  
  if (m_ColorMappingMode >= CMM_COMPONENT1)
  {
	  if(idx < 3)//CMM_COMPONENT3)
		da->GetRange(sr, idx);
	  else
	  {    
		//we need to compute scalar range (because of a BUG in vtkDataArray)
		sr[0] = DBL_MAX; sr[1] = -DBL_MAX;
	    
		int nCount = da->GetNumberOfTuples();
		for (int i = 0; i < nCount; i++)
		{
		  double value = da->GetComponent(i, idx);
		  if (value < sr[0])
			sr[0] = value;

		  if (value > sr[1])
			sr[1] = value;
		}
	  }
  }
  m_ColorMappingLUT->SetTableRange(sr);
  m_SurfaceMapper->SetScalarRange(sr);
  m_SurfaceMapper->Update();  
  
//  m_MappingActor->SetTitle(scalar_name);
  m_MappingActor->SetVisibility(m_ShowMapping);  
}

//------------------------------------------------------------------------------
// Multiply matrix columns by scalars
// no. of scalars must equal no. of columns (3 or 4)
// This is useful for multiplying column eigenvectors by eigenvalues
void albaPipeTensorFieldSurface::MultiplyColumnsByScalars(const double *s, const double *A, double *B) const
//------------------------------------------------------------------------------
{
	int k = 0 ;
	for (int j = 0 ;  j < 3 ;  j++)
			for (int i = 0 ;  i < 3 ;  i++, k++)
				B[k] = s[j]*A[k] ;
	
}
//------------------------------------------------------------------------------
bool albaPipeTensorFieldSurface::ComputeEigenvalues(vtkImageData* tensorVolume,double sr[2],int mode)
//------------------------------------------------------------------------------
{
	vtkDoubleArray* tensorArray =  vtkDoubleArray::SafeDownCast(tensorVolume->GetPointData()->GetTensors());
	//double sr[2];
	sr[0] = DBL_MAX; sr[1] = -DBL_MAX;
	int counter = 0;
    vtkDataArray* da ;
	vtkFloatArray *scalars ;
	scalars = vtkFloatArray::New() ;
	vtkPointData *allPoints = tensorVolume->GetPointData();
	//double *pCoord;

	if (!tensorArray)
		return false;
	int numTensors = tensorArray->GetNumberOfTuples();

	vtkDoubleArray* eigenvalueArray = vtkDoubleArray::New();
	eigenvalueArray->SetNumberOfComponents(3);//tensorArray->GetNumberOfComponents()
	eigenvalueArray->SetNumberOfTuples(numTensors);

	double Tensor[3][3];  // matrix representing the current tensor
	double Evalues[3];        // array of eigenvalues
	double Evectors[3][3];     // matrix representing the eigenvectors

	for (int i=0; i<numTensors; i++)
	{
		//pCoord = allPoints->GetTuple(i);

		tensorArray->GetTuple(i, (double*)Tensor);
		vtkMath::Diagonalize3x3(Tensor, Evalues, Evectors);// calculate eigenvalues
		eigenvalueArray->SetTuple(i, Evalues);//eigenvalueArray->InsertNextTuple(Evalues);
		
		double tmpScale ;//sqrt(Evalues[0]*Evalues[0]+Evalues[1]*Evalues[1]+Evalues[2]*Evalues[2]);		
		if (mode <3)
		{
			tmpScale = Evalues[mode];
		}
		if (tmpScale < sr[0])
				sr[0] = tmpScale;
		if (tmpScale > sr[1])
				sr[1] = tmpScale;
		scalars->InsertNextTuple((float*)&tmpScale);
	}
	tensorVolume->GetPointData()->SetScalars(scalars);
	da = tensorVolume->GetPointData()->GetScalars();
	
	// set the tensors to the Volume dataset
	tensorVolume->GetPointData()->SetVectors(eigenvalueArray);
	tensorVolume->GetPointData()->SetTensors(eigenvalueArray);
	tensorVolume->GetPointData()->SetScalars(scalars) ;
	tensorVolume->Update();
	eigenvalueArray->Delete();
	return true;
}
//----------------------------------------------------------------------------
// Find eigenvalues and eigenvectors of 3x3 matrix
// eigenvalues are sorted in order of largest to smallest
// eigenvectors are the columns of V[row][col]
// Symmetric matrices only !
void albaPipeTensorFieldSurface:: EigenVectors3x3(double A[3][3], double lambda[3], double V[3][3]) 
//----------------------------------------------------------------------------
{
	vtkMath *mth = vtkMath::New() ;

	// vtk function finds eigenvalues and eigenvectors of a symmetric matrix
	mth->Diagonalize3x3(A, lambda, V) ;

	// sort into order of increasing eigenvalue
	// irritating that the vtk function does not do this
	if (lambda[1] < lambda[2]){
		std::swap(lambda[1], lambda[2]) ;
		std::swap(V[0][1], V[0][2]) ;
		std::swap(V[1][1], V[1][2]) ;
		std::swap(V[2][1], V[2][2]) ;
	}

	if (lambda[0] < lambda[1]){
		std::swap(lambda[0], lambda[1]) ;
		std::swap(V[0][0], V[0][1]) ;
		std::swap(V[1][0], V[1][1]) ;
		std::swap(V[2][0], V[2][1]) ;
	}

	if (lambda[1] < lambda[2]){
		std::swap(lambda[1], lambda[2]) ;
		std::swap(V[0][1], V[0][2]) ;
		std::swap(V[1][1], V[1][2]) ;
		std::swap(V[2][1], V[2][2]) ;
	}
}

//------------------------------------------------------------------------
//Updates the content of m_comboColorBy combobox
//"magnitude" and 0..NumberOfComponents-1 will be listed.
/*virtual*/ void albaPipeTensorFieldSurface::UpdateColorByCombo()
//------------------------------------------------------------------------
{
  int nOldColorMode = m_ColorMappingMode;

  m_ComboColorBy->Clear();
  m_ComboColorBy->Append( _("magnitude") );
  m_ComboColorBy->Append(_("¦Ë0"));
  m_ComboColorBy->Append(_("¦Ë1"));
  m_ComboColorBy->Append(_("¦Ë2"));

  

  if (m_TensorFieldIndex >= 0)
  {
    vtkDataArray* da = m_Vme->GetOutput()->GetVTKData()->
      GetPointData()->GetTensors(GetTensorFieldName(m_TensorFieldIndex));

    if (da != NULL)
    {
      int nComps = da->GetNumberOfComponents();
      for (int i = 0; i < nComps; i++){
        m_ComboColorBy->Append(albaString::Format("%d", i));
      }
    }
  } //end if [valid selection of tensor field]

  int nCount = m_ComboColorBy->GetCount();
  if (nOldColorMode >= nCount)
    nOldColorMode = 0;

  m_ColorMappingMode = nOldColorMode;
  m_ComboColorBy->SetSelection(m_ColorMappingMode);
}