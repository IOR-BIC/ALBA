/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEAdvancedProber.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-25 16:03:21 $
  Version:   $Revision: 1.18 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEAdvancedProber.h"
#include "mmgGui.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafStorageElement.h"
#include "mafDataPipeCustom.h"
#include "mafVMEOutputSurface.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mmgListCtrl.h"

#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafMatrix.h"
#include "mafAbsMatrixPipe.h"
#include "mafVMEPolylineSpline.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEImage.h"
#include "mafVMESurface.h"
#include "mafVMEPolyline.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFDataPipe.h"
#include "vtkPolyData.h"
#include "vtkDistanceFilter.h"
#include "vtkTransformPolyDataFilter.h"

#include "vtkCellArray.h"
#include "vtkCardinalSpline.h"
#include "vtkPointData.h"

#include "vtkDistanceFilter.h"
#include "vtkProbeFilter.h"

#include "vtkImageData.h"
#include "vtkDoubleArray.h"
#include "vtkPlaneSource.h"
#include "vtkImageMapToColors.h"
#include "vtkLookupTable.h"

#include "vtkMath.h"
#include "itkLaplacianSharpeningImageFilter.h"

#include "vtkImageCast.h"
#include "itkVTKImageToImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include <algorithm>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEAdvancedProber)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEAdvancedProber::mafVMEAdvancedProber()
//-------------------------------------------------------------------------
{
	m_ListBox = NULL;
  m_SamplingCoefficient = 1.0;
	//m_SplineCoefficient = 2;
  m_Image = NULL;
  m_IMTC = NULL;
  m_Points = NULL;
  m_Plane = NULL;
  
  m_Transform = NULL;
  m_ControlPoints = NULL;


  m_AdditionalProfileNumber = 0; 
  m_ProfileDistance = 0.0;
  
  mafNEW(m_Transform);
  mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);
	//GetOutput()->Update();

  vtkNEW(m_Image);
  vtkNEW(m_ScalarImage);
  vtkNEW(m_ImageFiltered);
  vtkNEW(m_IMTC);
  vtkNEW(m_Points);
  vtkNEW(m_Plane);
	vtkNEW(m_Lut);

  vtkNEW(m_ControlPoints);


  //vtkNEW(append);

	GetMaterial()->m_MaterialType = mmaMaterial::USE_TEXTURE;
	GetMaterial()->m_TextureMappingMode = mmaMaterial::PLANE_MAPPING;

  this->GetTagArray()->SetTag(mafTagItem("MFL_SURFACE_PANORAMIC", "idem"));

  DependsOnLinkedNodeOn();
  
	// attach a data pipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  dpipe->SetInput(m_Plane->GetOutput()); //<***********************m_AdvanceProbeFilter
  SetDataPipe(dpipe);

	m_Fixed = 0;
  m_AutomaticCalculum = 0;
  m_EnableSharpening  = 0;
}

//-------------------------------------------------------------------------
mafVMEAdvancedProber::~mafVMEAdvancedProber()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);

	vtkDEL(m_Lut);
  vtkDEL(m_Image);
  vtkDEL(m_ScalarImage);
  vtkDEL(m_ImageFiltered);
  vtkDEL(m_IMTC);
  vtkDEL(m_Points);
  vtkDEL(m_Plane);

  vtkDEL(m_ControlPoints);


  SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEAdvancedProber::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
  }
  return material;
}

//-------------------------------------------------------------------------
int mafVMEAdvancedProber::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mafVMEAdvancedProber::InternalUpdate() //Multi
//-----------------------------------------------------------------------
{
  //wxBusyCursor wait;
  //if(m_AutomaticCalculum == 0) return;
	ForwardUpEvent(&mafEvent(this,VME_SYNC_WITH_SPLINE_LINK, m_ProfileDistance*m_AdditionalProfileNumber));

  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink());
  mafVMEPolylineSpline *vme;
  
  int counter = 0;
  std::vector<vtkPolyData *> profilesOrdered;
	profilesOrdered.clear();
  
  for (mafLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); i++)
  {
    mafString name = i->first;
    if(i->first.Equals("Volume")) continue;
    if(i->second.m_Node == NULL)
    {
      mafString message;
      message = mafString(i->first);
      message += _("doesn't exist");
      mafLogMessage(message);
      return;
    }

    vme = (mafVMEPolylineSpline *)i->second.m_Node;
    vme->Update();

		//If Profile has only 1 points or less panoramic shouldn't be created
		/*if(vme->GetOutput()->GetVTKData()->GetNumberOfPoints()<=1)
			return;*/

    ////transform
    ////this part set the transform for every polydata inside a node, using the pose of the node
    vtkMAFSmartPointer<vtkTransform> transform;
    transform->SetMatrix(vme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
    transform->Update();

    vtkMAFSmartPointer<vtkTransformPolyDataFilter> tpdf;
    tpdf->SetInput(((vtkPolyData *)vme->GetOutput()->GetVTKData()));
    tpdf->SetTransform(transform);
    tpdf->Update();
    ////
    // creation of ordered vector of profiles
    if(profilesOrdered.size() == 0)
    {
       profilesOrdered.push_back(vtkPolyData::New());
       profilesOrdered[profilesOrdered.size()-1]->DeepCopy(tpdf->GetOutput());
    }
    else
    {
    	double z;
    	z = tpdf->GetOutput()->GetPoint(0)[2];
    	
    	for(int k=0 ; k< profilesOrdered.size(); k++)
      {
        double zc;	
        zc = profilesOrdered[k]->GetPoint(0)[2];
        if(z < zc)
        {
          vtkPolyData *poly;
          vtkNEW(poly);
          poly->DeepCopy(tpdf->GetOutput());
          profilesOrdered.insert(profilesOrdered.begin()+k, poly);
          k = profilesOrdered.size();
        }
        else if(k == profilesOrdered.size()-1)
        {
          vtkPolyData *poly;
          vtkNEW(poly);
          poly->DeepCopy(tpdf->GetOutput());
          profilesOrdered.push_back(poly);
          k = profilesOrdered.size();
        }
      }
    }
    counter++;
  }
  if(!CheckUpdatePanoramic(vme)) return;
	if (vol == NULL || counter == 0) 
	{ 
		int dimensions[3];
		double spacing = 1;

		dimensions[0] = 1;
		dimensions[1] = 1;
		dimensions[2] = 1;

		m_Image->SetDimensions(dimensions);
		m_Image->SetSpacing(spacing , spacing , spacing);

		m_Image->UpdateData();

		m_IMTC->SetInput(m_Image);
		m_IMTC->Update();

		((mafVMEOutputSurface *) GetOutput())->SetTexture(m_IMTC->GetOutput());
		GetMaterial()->SetMaterialTexture(((mafVMEOutputSurface *) GetOutput())->GetTexture());
		return;
	}

  ForwardUpEvent(&mafEvent(this,PROGRESSBAR_SHOW));
  vol->GetOutput()->Update();

  // control passed 
  std::vector<double> lengths;
  for(int j=0 ; j<profilesOrdered.size(); j++)
  {
    double distance = 0;
    
    for(int i=0; i< profilesOrdered[j]->GetNumberOfPoints()-1; i++)
    {
      double p1[3], p2[3];
      profilesOrdered[j]->GetPoint(i,p1);
      profilesOrdered[j]->GetPoint(i+1,p2);
        
      distance += sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
    }
    lengths.push_back(distance);
    //mafLogMessage(wxString::Format(_("%f"),distance));
  }
  
  int index = -1;
  double maxDis = 0.0;
  for(int in = 0; in < lengths.size(); in++)
  {
    if(maxDis < lengths[in])
    {
      index = in;
      maxDis = lengths[in];
    }
  }
	if (index==-1)
		return;

  //mafLogMessage(index);

  // ALGORITHM

  //set spacing 
   double spacing; 
   spacing = m_SamplingCoefficient  * ((vtkImageData *)(vol)->GetOutput()->GetVTKData())->GetSpacing()[0];

   m_Points->Reset();

   double b[6];
   (vol)->GetOutput()->GetVTKData()->GetBounds(b);

	 //cleaned point list
	 vtkMAFSmartPointer<vtkPoints> pts;
   
	 for(int i = 0; i< profilesOrdered[index]->GetNumberOfPoints(); i++)
	 {
     pts->InsertNextPoint(profilesOrdered[index]->GetPoint(i));
	 }

	 vtkMAFSmartPointer<vtkCellArray> lineCells;
	 lineCells->InsertNextCell(pts->GetNumberOfPoints());
	 for (int i = 0; i < pts->GetNumberOfPoints(); i ++)
		 lineCells->InsertCellPoint(i);       
  

   double difference = 0;
   for (int i=0; i< pts->GetNumberOfPoints()-1; i++)
   {   
     double p1[3], p2[3];
     pts->GetPoint(i,p1);
     pts->GetPoint(i+1,p2);

     if (difference != 0)
     {
      double length;
      length = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
      p1[0] = (difference/length) * (p2[0] - p1[0]) + p1[0];
      p1[1] = (difference/length) * (p2[1] - p1[1]) + p1[1];
     }

     double length;
     length = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));

     double point[3] = {0,0,0};
     long counter = 0;
     while(counter * spacing <= length)
     {
        point[0] = (counter * spacing/length) * (p2[0] - p1[0]) + p1[0];
        point[1] = (counter * spacing/length) * (p2[1] - p1[1]) + p1[1];
        point[2] = b[4];

        m_Points->InsertNextPoint(point);
        counter++;
     }
     if(counter * spacing > length)
     {
        difference = counter * spacing - length;
     }

   }


   
   vtkMAFSmartPointer<vtkPoints> tempPoints;
   tempPoints->DeepCopy(m_Points);
   int firstDimension = tempPoints->GetNumberOfPoints();
   
   int numberOfIteration;
   //how many iteration?
   double depth;
   depth = fabs((b[5] - b[4]));
   numberOfIteration = depth / spacing;

   if(counter == 1)
   {
     //simple extrusion along z

     for(int j=0 ; j< numberOfIteration; j++)
     {
       for(int i=0; i < tempPoints->GetNumberOfPoints(); i++)
       {
         double pt[3];
         tempPoints->GetPoint(i, pt);
        
         pt[2] = pt[2] + spacing;
         tempPoints->SetPoint(i, pt);
        
         m_Points->InsertNextPoint(pt);
       }
     }
   }
   else // more than 1
   {
     double n = m_Points->GetNumberOfPoints();
     m_Points->Reset();
     numberOfIteration = 0;
     std::vector<vtkPoints *> pointsVector;
		 int numberMin = n-1; // minimum number of points in case of roundness error
     for(int k=0 ; k< profilesOrdered.size()-1; k++)
     {
  
        pointsVector.push_back(vtkPoints::New());
        pointsVector.push_back(vtkPoints::New());
        /*
        
          1) sample ordered list [k] e [k+1]
          //////////////////////////////////////////
        */
          j=k;
          
          while(j < k + 2)
          {
            
	          vtkMAFSmartPointer<vtkPoints> pts;      
	          for(int i = 0; i< profilesOrdered[j]->GetNumberOfPoints(); i++)
	          {
              pts->InsertNextPoint(profilesOrdered[j]->GetPoint(i));
	          }
            vtkMAFSmartPointer<vtkCellArray> lineCells;
	          lineCells->InsertNextCell(pts->GetNumberOfPoints());
	          for (int i = 0; i < pts->GetNumberOfPoints(); i ++)
		          lineCells->InsertCellPoint(i);       

            double pt[3];
            profilesOrdered[j]->GetPoint(0, pt);

            double sum = 0;
            for(int i=0; i < pts->GetNumberOfPoints()-1; i++)
            {
              double pt[3],pt2[3];
              pts->GetPoint(i, pt);
              pts->GetPoint(i+1, pt2);

              sum += sqrt(vtkMath::Distance2BetweenPoints(pt,pt2));
            
            }

            double variableSpacing = sum/n;

            for (int i=0; i< pts->GetNumberOfPoints()-1; i++)
            {   
              double p1[3], p2[3];
              pts->GetPoint(i,p1);
              pts->GetPoint(i+1,p2);

              if (difference != 0)
              {
                double length;
                length = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
                p1[0] = (difference/length) * (p2[0] - p1[0]) + p1[0];
                p1[1] = (difference/length) * (p2[1] - p1[1]) + p1[1];
              }

              double length;
              length = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));

              double point[3] = {0,0,0};
              long counter = 0;
              while(counter * variableSpacing <= length)
              {
                  point[0] = (counter * variableSpacing/length) * (p2[0] - p1[0]) + p1[0];
                  point[1] = (counter * variableSpacing/length) * (p2[1] - p1[1]) + p1[1];
                  point[2] = pt[2];

                  pointsVector[j]->InsertNextPoint(point);
                  counter++;
              }
              if(counter * variableSpacing > length)
              {
                  difference = counter * variableSpacing - length;
              }              
            } 

            if(numberMin > pointsVector[j]->GetNumberOfPoints())
              numberMin = pointsVector[j]->GetNumberOfPoints();

            	j++;
          }
   
         
        /*
          2) search vector point to point
          3) modules calculum
          //////////////////////////////////////////
        */
          //pointsVector[pointsVector.size()-1];
          //pointsVector[pointsVector.size()-2];
          
          std::vector<double> modules;   
          for(int h = 0; h < numberMin; h++)
          {
            double pL[3], pH[3];
            pointsVector[k]->GetPoint(h , pL);
            pointsVector[k+1]->GetPoint(h , pH);

            modules.push_back(sqrt(vtkMath::Distance2BetweenPoints(pL,pH)));
          }

          /*  
              4) search maximum module value
              5) sample with the right spacing variableSpac/length (x-y) and spacing in z
              
              6) fill a vector with the profile inside the two used
              //////////////////////////////////////////
          */

              int index = -1;
              double maxMod = 0.0;
              for(int in = 0; in < modules.size(); in++)
              {
                if(maxMod < modules[in])
                {
                  index = in;
                  maxMod = modules[in];
                }
              }
          
              
              double pLL[3], pHH[3];
              pointsVector[k]->GetPoint(index,pLL);
              pointsVector[k+1]->GetPoint(index,pHH);
              int nForModules = maxMod / spacing;
              numberOfIteration += nForModules;

              std::vector<vtkPoints *> pointsModuleAlongZ;
              for (int i=0; i< modules.size(); i++)
              {   
                pointsModuleAlongZ.push_back(vtkPoints::New());

                double p1[3], p2[3];
                pointsVector[k]->GetPoint(i,p1);
                pointsVector[k+1]->GetPoint(i,p2);


                double length;
                length = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
                
                double variableSpac = length / nForModules;

                double point[3] = {0,0,0};
                long counter = 0;
                while(counter <= nForModules)
                {
                    point[0] = (counter * variableSpac/length) * (p2[0] - p1[0]) + p1[0];
                    point[1] = (counter * variableSpac/length) * (p2[1] - p1[1]) + p1[1];
                    point[2] = (counter * spacing/maxMod) * (pHH[2] - pLL[2]) + pLL[2];

                    pointsModuleAlongZ[i]->InsertNextPoint(point);
                    counter++;
                }
                
              }

              std::vector<vtkPoints *> pointsModuleAlongXY;
              for(int d = 0 ; d < nForModules; d++)
              {
                pointsModuleAlongXY.push_back(vtkPoints::New());

                for(int y = 0; y< pointsModuleAlongZ.size(); y++) 
                {
                  double pp[3];
                  pointsModuleAlongZ[y]->GetPoint(d,pp);
                  pointsModuleAlongXY[d]->InsertNextPoint(pp);
                }
              }

        /*
          7) fill m_Points
        //////////////////////////////////////////
        */

          for(int i= 0 ; i< pointsModuleAlongXY.size() ; i++)
          {
            for(int j=0; j< pointsModuleAlongXY[i]->GetNumberOfPoints(); j++)
            {
              double ppp[3];
              pointsModuleAlongXY[i]->GetPoint(j,ppp);
              m_Points->InsertNextPoint(ppp);
            }
          }

          

     }

     firstDimension = m_Points->GetNumberOfPoints() / numberOfIteration;
     for(int u = 0; u< pointsVector.size(); u++)
       vtkDEL(pointsVector[u]);
     pointsVector.clear();
   }
   
  /*
            vtkMAFSmartPointer<vtkPolyData> pl;
            pl->SetPoints(m_Points);
            pl->Update();

            vtkMAFSmartPointer<vtkDelaunay2D> del;
            del->SetInput(pl);
            del->SetTolerance(2);
            del->SetAlpha(5);
            del->Update();

            append->AddInput(del->GetOutput());
            append->Update();
   */       


   // here I must have m_Points that contains all points of the prober profile
   //input -> m_Points

   for(int i= -m_AdditionalProfileNumber; i <= m_AdditionalProfileNumber ; i++)
   {
     vtkPoints *temporaryPoints = vtkPoints::New();
     temporaryPoints->DeepCopy(m_Points);

     for(long int j = 0; j < temporaryPoints->GetNumberOfPoints(); j++)
     {
       if(  ((j+1) % firstDimension) == 0) // control if it's last point of profile.
       {          
         double tempPoint1[3];
         double tempPoint2[3];
         temporaryPoints->GetPoint(j-1, tempPoint1);
         temporaryPoints->GetPoint(j, tempPoint2);

         //search the versor
         double versor[3];
         versor[0] = tempPoint2[0] - tempPoint1[0];
         versor[1] = tempPoint2[1] - tempPoint1[1];
         versor[2] = tempPoint2[2] - tempPoint1[2];

         double zAxis[3] = {0,0,1};

         //vectorial product beetween my versor and zAxis
         double perpendicular[3];
         double *u , *v;
         u = versor;
         v = zAxis;

         vtkMath::Cross(versor,zAxis,perpendicular);

         double tempPoint3[3];
         // sum this versor to my second point
         tempPoint3[0] = tempPoint2[0] + perpendicular[0];
         tempPoint3[1] = tempPoint2[1] + perpendicular[1];
         tempPoint3[2] = tempPoint2[2] + perpendicular[2];

         //calculate the distance between point1 and 3
         double distance23 = 0;
         distance23 = sqrt(vtkMath::Distance2BetweenPoints(tempPoint2, tempPoint3));

         //now I can calculate the the coordinate of the point, distanciated by the step
         double newPoint[3];
         newPoint[0] = ((i * m_ProfileDistance)/distance23) * (tempPoint3[0] - tempPoint2[0]) + tempPoint2[0];
         newPoint[1] = ((i * m_ProfileDistance)/distance23) * (tempPoint3[1] - tempPoint2[1]) + tempPoint2[1];
         newPoint[2] = ((i * m_ProfileDistance)/distance23) * (tempPoint3[2] - tempPoint2[2]) + tempPoint2[2];

         temporaryPoints->SetPoint(j, newPoint);
       }
       else
       {
         double tempPoint1[3];
         double tempPoint2[3];
         temporaryPoints->GetPoint(j, tempPoint1);
         temporaryPoints->GetPoint(j+1, tempPoint2);

         //search the versor
         double versor[3];
         versor[0] = tempPoint2[0] - tempPoint1[0];
         versor[1] = tempPoint2[1] - tempPoint1[1];
         versor[2] = tempPoint2[2] - tempPoint1[2];

         double zAxis[3] = {0,0,1};

         //vectorial product beetween my versor and zAxis
         double perpendicular[3];
         double *u , *v;
         u = versor;
         v = zAxis;

         vtkMath::Cross(versor,zAxis,perpendicular);

         /*perpendicular[0] = (u[1] * v[2] - u[2] * v[1]); 
         perpendicular[1] = (u[0] * v[2] - u[2] * v[0]);
         perpendicular[2] = (u[0] * v[1] - u[1] * v[0]);
*/
         double tempPoint3[3];
         // sum this versor to my first point
         tempPoint3[0] = tempPoint1[0] + perpendicular[0];
         tempPoint3[1] = tempPoint1[1] + perpendicular[1];
         tempPoint3[2] = tempPoint1[2] + perpendicular[2];

         //calculate the distance between point1 and 3
         double distance13 = 0;
         distance13 = sqrt(vtkMath::Distance2BetweenPoints(tempPoint1, tempPoint3));

         //now I can calculate the the coordinate of the point, distanciated by the step
         double newPoint[3];
         newPoint[0] = ((i * m_ProfileDistance)/distance13) * (tempPoint3[0] - tempPoint1[0]) + tempPoint1[0];
         newPoint[1] = ((i * m_ProfileDistance)/distance13) * (tempPoint3[1] - tempPoint1[1]) + tempPoint1[1];
         newPoint[2] = ((i * m_ProfileDistance)/distance13) * (tempPoint3[2] - tempPoint1[2]) + tempPoint1[2];

         temporaryPoints->SetPoint(j, newPoint);
       }
     }

     m_PointsVector.push_back(temporaryPoints);    
   }

   for(int i = 0; i< m_PointsVector.size(); i++)
   {
     m_PolyDataVector.push_back(vtkPolyData::New());
     m_PolyDataVector[i]->SetPoints(m_PointsVector[i]);
     m_PolyDataVector[i]->Update();
   }

   for(int i = 0; i< m_PolyDataVector.size(); i++)
   {
     m_DistanceFilterVector.push_back(vtkProbeFilter::New());
     m_DistanceFilterVector[i]->SetSource(vol->GetOutput()->GetVTKData());
     //m_DistanceFilterVector[i]->SetFilterModeToDensity();
     m_DistanceFilterVector[i]->SetInput(m_PolyDataVector[i]);
     m_DistanceFilterVector[i]->Update();
		 long percent;
		 percent = (i * 100) / m_PolyDataVector.size();
		 ForwardUpEvent(&mafEvent(this,PROGRESSBAR_SET_VALUE, percent));
   }


   m_Image->SetOrigin(((vtkImageData *)((mafVMEVolumeGray *)vol)->GetOutput()->GetVTKData())->GetOrigin());
   
   int dimensions[3];
   dimensions[0] = firstDimension;
   dimensions[1] = 1;
   dimensions[2] = numberOfIteration+1;

   m_Image->SetDimensions(dimensions);
   m_Image->SetSpacing(spacing , spacing , spacing);
   m_Image->AllocateScalars();

   m_Image->AllocateScalars();
   m_Image->AllocateScalars();
   m_Image->AllocateScalars();
	 m_Image->Update();

	 vtkMAFSmartPointer<vtkDoubleArray> x;
	 vtkMAFSmartPointer<vtkDoubleArray> y;
	 vtkMAFSmartPointer<vtkDoubleArray> z;

	 for(int i = 0 ; i < m_Points->GetNumberOfPoints() ; i++)
	 {
		 x->InsertNextTuple1(0.0);
		 y->InsertNextTuple1(0.0);
		 z->InsertNextTuple1(0.0);
	 }

	 x->SetName("XCoords");
	 y->SetName("YCoords");
	 z->SetName("ZCoords");

	 m_Image->GetPointData()->AddArray(x);
	 m_Image->GetPointData()->AddArray(y);
	 m_Image->GetPointData()->AddArray(z);

   /*for(int i=0 ; i<m_Image->GetNumberOfPoints(); i++)
			m_Image->GetPointData()->GetScalars()->SetTuple4(i, 0,0,0,0);*/
   m_Image->UpdateData();
   
   //int numberPoints = prob->GetOutput()->GetNumberOfPoints();

   for(int i=0 ; i< m_DistanceFilterVector[0]->GetOutput()->GetNumberOfPoints(); i++)
   {
     double value = 0;
     for(int j=0; j< m_DistanceFilterVector.size(); j++)
     {
       double v = 0;

       double tmppt[3];
       m_DistanceFilterVector[j]->GetOutput()->GetPoint(i, tmppt);
        
       v = m_DistanceFilterVector[j]->GetOutput()->GetPointData()->GetScalars()->GetTuple1(i);

       if(tmppt[0] < b[0] || tmppt[0] > b[1] || tmppt[1] < b[2] || tmppt[1] > b[3])
         v = 0;

       value += v;
     }
     m_Image->GetPointData()->GetScalars()->SetTuple1(i,value);
   }
    
   
   // additional array for x,y,z coordinates
	 /*vtkMAFSmartPointer<vtkDoubleArray> x;
	 vtkMAFSmartPointer<vtkDoubleArray> y;
	 vtkMAFSmartPointer<vtkDoubleArray> z;*/
	 x->Reset();
	 y->Reset();
	 z->Reset();
			
	 for(int i = 0 ; i < m_Points->GetNumberOfPoints() ; i++)
	 {
		 x->InsertNextTuple1(m_Points->GetPoint(i)[0]);
		 y->InsertNextTuple1(m_Points->GetPoint(i)[1]);
		 z->InsertNextTuple1(m_Points->GetPoint(i)[2]);
	 }
	 /*x->SetName("XCoords");
	 y->SetName("YCoords");
	 z->SetName("ZCoords");*/
			
	 m_Image->GetPointData()->AddArray(x);
	 m_Image->GetPointData()->AddArray(y);
	 m_Image->GetPointData()->AddArray(z);

   m_Image->UpdateData();

   
   /*m_Plane->SetNormal(0,0,-1);
   m_Plane->SetOrigin(b[0],b[2],b[4]);
   m_Plane->SetPoint1(b[0] + spacing * firstDimension,b[2],b[4]);
   m_Plane->SetPoint2(b[0],b[2]+ spacing * (numberOfIteration+1),b[4] );*/

	 m_Plane->SetNormal(0,-1,0);
	 m_Plane->SetOrigin(b[0],b[2],b[4]);
	 m_Plane->SetPoint1(b[0] + spacing * firstDimension,b[2],b[4]);
	 m_Plane->SetPoint2(b[0],b[2],b[4] + spacing * (numberOfIteration+1) );

   //m_Plane->SetResolution(firstDimension,numberOfIteration+1);
   m_Plane->SetResolution(1,1); //with this modification increase the speed
   m_Plane->Update();

   mmaVolumeMaterial *vol_material;
   mafNEW(vol_material);
   vol_material->DeepCopy(((mafVMEVolumeGray *)vol)->GetMaterial());
   vol_material->UpdateProp();
   m_Lut = vol_material->m_ColorLut;
   
   double range[2];
   // unsharp filtering

   m_ScalarImage->DeepCopy(m_Image); // copy the image and clean the coord arrays
   m_ScalarImage->GetPointData()->RemoveArray("XCoords");
   m_ScalarImage->GetPointData()->RemoveArray("YCoords");
   m_ScalarImage->GetPointData()->RemoveArray("ZCoords");


	if (m_EnableSharpening)
	{
		UnsharpImage();
	}
	else
	{
		m_ImageFiltered->DeepCopy(m_ScalarImage);
	}
	m_ImageFiltered->GetPointData()->AddArray(x);
	m_ImageFiltered->GetPointData()->AddArray(y);
	m_ImageFiltered->GetPointData()->AddArray(z);
   //
   m_ImageFiltered->GetScalarRange(range);

	 double lowHigh[2];
	 vol_material->m_ColorLut->GetTableRange(lowHigh);

	 double wholeScalarRangeVol[2];
	 ((mafVMEVolumeGray *)vol)->GetOutput()->GetVTKData()->GetScalarRange(wholeScalarRangeVol);

	 double wholeScalarRangeAdv[2];
	 m_ImageFiltered->GetScalarRange(wholeScalarRangeAdv);

	 double advLow,advHigh;
	 advLow = wholeScalarRangeAdv[0] + ((wholeScalarRangeAdv[1] - wholeScalarRangeAdv[0])/(wholeScalarRangeVol[1] - wholeScalarRangeVol[0])) * (lowHigh[0] - wholeScalarRangeVol[0]);
	 advHigh = wholeScalarRangeAdv[1] + ((wholeScalarRangeAdv[1] - wholeScalarRangeAdv[0])/(wholeScalarRangeVol[1] - wholeScalarRangeVol[0])) * (lowHigh[1] - wholeScalarRangeVol[1]);
 
   m_Lut->SetRange(advLow,advHigh);
   m_Lut->Build();

   //m_Surface->SetData(m_Plane->GetOutput(), 0);
   //m_Surface->Update();
   m_Plane->GetOutput()->Update();

   m_ImageFiltered->GetPointData()->GetArray(0)->SetLookupTable(m_Lut);
   m_ImageFiltered->UpdateData();

   
   m_IMTC->SetInput(m_ImageFiltered);
   m_IMTC->SetLookupTable(m_Lut);
   m_IMTC->Update();

	 ((mafVMEOutputSurface *) GetOutput())->SetTexture(m_IMTC->GetOutput());
   GetMaterial()->SetMaterialTexture(((mafVMEOutputSurface *) GetOutput())->GetTexture());

   //control for update
   m_ControlPoints->DeepCopy(vtkPolyData::SafeDownCast(vme->GetPolylineLink()->GetOutput()->GetVTKData())->GetPoints());
   m_ControlSamplingCoefficient = m_SamplingCoefficient;
   m_ControlProfileDistance = m_ProfileDistance;
   m_ControlAdditionalProfileNumber = m_AdditionalProfileNumber;
	 
   for(int i=0; i< m_PointsVector.size();i++)
   {
     vtkDEL(m_PointsVector[i]);
   }
   m_PointsVector.clear();

   for(int i=0; i< m_PolyDataVector.size();i++)
   {
     vtkDEL(m_PolyDataVector[i]);
   }
   m_PolyDataVector.clear();

   for(int i=0; i< m_DistanceFilterVector.size();i++)
   {
     vtkDEL(m_DistanceFilterVector[i]);
   }
   m_DistanceFilterVector.clear();
  
   for(int i=0; i< profilesOrdered.size();i++)
   {
     vtkDEL(profilesOrdered[i]);
   }
   profilesOrdered.clear();

   mafDEL(vol_material);

	 ForwardUpEvent(&mafEvent(this,PROGRESSBAR_HIDE));

}

//-----------------------------------------------------------------------
/*void mafVMEAdvancedProber::InternalPreUpdateOld() //Single
//-----------------------------------------------------------------------
{
  wxBusyCursor wait;
  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink()); 
  mafVMEPolyline *vme;
  
  int counter = 0;
  for (mafLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); i++)
  {
    if(i->first.Equals(L"Volume")) continue;
    if(i->second.m_Node == NULL)
    {
      mafString message;
      message = mafString(i->first);
      message += _("doesn't exist");
      mafLogMessage(message);
      return;
    }
    
    vme = (mafVMEPolyline *)i->second.m_Node;
    vme->Update();

    ////transform
    ////this part set the transform for every polydata inside a node, using the pose of the node
    vtkMAFSmartPointer<vtkTransform> transform;
    transform->SetMatrix(vme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
    transform->Update();

    vtkMAFSmartPointer<vtkTransformPolyDataFilter> tpdf;
    tpdf->SetInput(((vtkPolyData *)vme->GetOutput()->GetVTKData()));
    tpdf->SetTransform(transform);
    tpdf->Update();
    
    counter++;
  }

  if (vol == NULL || counter == 0) 
  { 
    return;
  }
  
  vol->GetOutput()->Update();
  
  // algorithm *********************************************
  //     *************************************************

  //set spacing 
   double spacing; 
   spacing = m_SamplingCoefficient  * ((vtkImageData *)(vol)->GetOutput()->GetVTKData())->GetSpacing()[0];

   m_Points->Reset();

   double b[6];
   vol->GetOutput()->GetVTKData()->GetBounds(b);

   double difference = 0;

	 //cleaned point list
	 vtkMAFSmartPointer<vtkPoints> pts;
   long numberOfPoints = vme->GetOutput()->GetVTKData()->GetNumberOfPoints();
	 for(int i = 0; i< numberOfPoints; i++)
	 {
     pts->InsertNextPoint(vme->GetOutput()->GetVTKData()->GetPoint(i));
	 }

	 vtkMAFSmartPointer<vtkCellArray> lineCells;
	 lineCells->InsertNextCell(numberOfPoints);
	 for (int i = 0; i < numberOfPoints; i ++)
		 lineCells->InsertCellPoint(i);       
	 

	 vtkMAFSmartPointer<vtkCardinalSpline> splineX;
	 vtkMAFSmartPointer<vtkCardinalSpline> splineY;
	 //vtkMAFSmartPointer<vtkCardinalSpline> splineZ;

	 for(int i=0 ; i<numberOfPoints; i++)
	 {
		 //mafLogMessage(wxString::Format(_("old %d : %f %f %f"), i, pts->GetPoint(i)[0],pts->GetPoint(i)[1],pts->GetPoint(i)[2] ));
		 splineX->AddPoint(i, pts->GetPoint(i)[0]);
		 splineY->AddPoint(i, pts->GetPoint(i)[1]);
		 //splineZ->AddPoint(i, pts->GetPoint(i)[2]);
	 }

	 vtkMAFSmartPointer<vtkPoints> ptsSplined;
	 for(int i=0 ; i<(numberOfPoints * m_SplineCoefficient); i++)
	 {		 
		 double t;
	   t = ( pts->GetNumberOfPoints() - 1.0 ) / ( pts->GetNumberOfPoints()*m_SplineCoefficient - 1.0 ) * i;
		 ptsSplined->InsertPoint(i , splineX->Evaluate(t), splineY->Evaluate(t), 0.0);
		 
	 }
  

   int numberOfSplinedPoints = ptsSplined->GetNumberOfPoints();
   for (int i=0; i< numberOfSplinedPoints-1; i++)
   {   
     double p1[3], p2[3];
     ptsSplined->GetPoint(i,p1);
     ptsSplined->GetPoint(i+1,p2);

     if (difference != 0)
     {
      double length;
      length = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
      p1[0] = (difference/length) * (p2[0] - p1[0]) + p1[0];
      p1[1] = (difference/length) * (p2[1] - p1[1]) + p1[1];
     }

     double length;
     length = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));

     double point[3] = {0,0,0};
     long counter = 0;
     while(counter * spacing <= length)
     {
        point[0] = (counter * spacing/length) * (p2[0] - p1[0]) + p1[0];
        point[1] = (counter * spacing/length) * (p2[1] - p1[1]) + p1[1];
        point[2] = b[4];

        m_Points->InsertNextPoint(point);
        counter++;
     }
     if(counter * spacing > length)
     {
        difference = counter * spacing - length;
     }

   }

   //extrusion along z
   vtkMAFSmartPointer<vtkPoints> tempPoints;
   tempPoints->DeepCopy(m_Points);

   //how many iteration?
   double depth;

   depth = fabs((b[5] - b[4]));

   int numberOfIteration;
   numberOfIteration = depth / spacing;
   
   int numberOfTempPoints = tempPoints->GetNumberOfPoints();
   for(int j=0 ; j< numberOfIteration; j++)
   {
    for(int i=0; i < numberOfTempPoints; i++)
    {
      double pt[3];
      tempPoints->GetPoint(i, pt);
      
      pt[2] = pt[2] + spacing;
      tempPoints->SetPoint(i, pt);
      
      m_Points->InsertNextPoint(pt);
    }
   }
    
   //input -> m_Points <-
   for(int i= -m_AdditionalProfileNumber; i <= m_AdditionalProfileNumber ; i++)
   {
     vtkPoints *temporaryPoints = vtkPoints::New();
     temporaryPoints->DeepCopy(m_Points);

     int num1 = temporaryPoints->GetNumberOfPoints();
     int num2 = tempPoints->GetNumberOfPoints();
     for(long int j = 0; j < num1; j++)
     {
       if(  ((j+1) % num2) == 0) // control if it's last point of profile.
       {          
         double tempPoint1[3];
         double tempPoint2[3];
         temporaryPoints->GetPoint(j-1, tempPoint1);
         temporaryPoints->GetPoint(j, tempPoint2);

         //search the versor
         double versor[3];
         versor[0] = tempPoint2[0] - tempPoint1[0];
         versor[1] = tempPoint2[1] - tempPoint1[1];
         versor[2] = tempPoint2[2] - tempPoint1[2];

         double zAxis[3] = {0,0,1};

         //vectorial product beetween my versor and zAxis
         double perpendicular[3];
         double *u , *v;
         u = versor;
         v = zAxis;

         vtkMath::Cross(versor,zAxis,perpendicular);

         double tempPoint3[3];
         // sum this versor to my second point
         tempPoint3[0] = tempPoint2[0] + perpendicular[0];
         tempPoint3[1] = tempPoint2[1] + perpendicular[1];
         tempPoint3[2] = tempPoint2[2] + perpendicular[2];

         //calculate the distance between point1 and 3
         double distance23 = 0;
         distance23 = sqrt(vtkMath::Distance2BetweenPoints(tempPoint2, tempPoint3));

         //now I can calculate the the coordinate of the point, distanciated by the step
         double newPoint[3];
         newPoint[0] = ((i * m_ProfileDistance)/distance23) * (tempPoint3[0] - tempPoint2[0]) + tempPoint2[0];
         newPoint[1] = ((i * m_ProfileDistance)/distance23) * (tempPoint3[1] - tempPoint2[1]) + tempPoint2[1];
         newPoint[2] = ((i * m_ProfileDistance)/distance23) * (tempPoint3[2] - tempPoint2[2]) + tempPoint2[2];

         temporaryPoints->SetPoint(j, newPoint);
       }
       else
       {
         double tempPoint1[3];
         double tempPoint2[3];
         temporaryPoints->GetPoint(j, tempPoint1);
         temporaryPoints->GetPoint(j+1, tempPoint2);

         //search the versor
         double versor[3];
         versor[0] = tempPoint2[0] - tempPoint1[0];
         versor[1] = tempPoint2[1] - tempPoint1[1];
         versor[2] = tempPoint2[2] - tempPoint1[2];

         double zAxis[3] = {0,0,1};

         //vectorial product beetween my versor and zAxis
         double perpendicular[3];
         double *u , *v;
         u = versor;
         v = zAxis;

         vtkMath::Cross(versor,zAxis,perpendicular);

         //perpendicular[0] = (u[1] * v[2] - u[2] * v[1]); 
         //perpendicular[1] = (u[0] * v[2] - u[2] * v[0]);
         //perpendicular[2] = (u[0] * v[1] - u[1] * v[0]);

         double tempPoint3[3];
         // sum this versor to my first point
         tempPoint3[0] = tempPoint1[0] + perpendicular[0];
         tempPoint3[1] = tempPoint1[1] + perpendicular[1];
         tempPoint3[2] = tempPoint1[2] + perpendicular[2];

         //calculate the distance between point1 and 3
         double distance13 = 0;
         distance13 = sqrt(vtkMath::Distance2BetweenPoints(tempPoint1, tempPoint3));

         //now I can calculate the the coordinate of the point, distanciated by the step
         double newPoint[3];
         newPoint[0] = ((i * m_ProfileDistance)/distance13) * (tempPoint3[0] - tempPoint1[0]) + tempPoint1[0];
         newPoint[1] = ((i * m_ProfileDistance)/distance13) * (tempPoint3[1] - tempPoint1[1]) + tempPoint1[1];
         newPoint[2] = ((i * m_ProfileDistance)/distance13) * (tempPoint3[2] - tempPoint1[2]) + tempPoint1[2];

         temporaryPoints->SetPoint(j, newPoint);
       }
     }

     //controllino sui bounds...se va oltre azzera il punto.

     m_PointsVector.push_back(temporaryPoints);    
   }


   int numberInPointsVector = m_PointsVector.size();
   for(int i = 0; i< numberInPointsVector; i++)
   {
     m_PolyDataVector.push_back(vtkPolyData::New());
     m_PolyDataVector[i]->SetPoints(m_PointsVector[i]);
     m_PolyDataVector[i]->Update();
   }

   for(int i = 0; i< m_PolyDataVector.size(); i++)
   {
     m_DistanceFilterVector.push_back(vtkProbeFilter::New());
     m_DistanceFilterVector[i]->SetSource(vol->GetOutput()->GetVTKData());
     //m_DistanceFilterVector[i]->SetFilterModeToDensity();
     m_DistanceFilterVector[i]->SetInput(m_PolyDataVector[i]);
     m_DistanceFilterVector[i]->Update();
   }


   
   
   int dimensions[3];
   dimensions[0] = tempPoints->GetNumberOfPoints();
   dimensions[1] = 1;
   dimensions[2] = numberOfIteration+1;

   m_Image->SetDimensions(dimensions);
   m_Image->SetSpacing(spacing , 0 , spacing);
   m_Image->AllocateScalars();

   m_Image->AllocateScalars();
   m_Image->AllocateScalars();
   m_Image->AllocateScalars();
   m_Image->UpdateData();
   
   //int numberPoints = prob->GetOutput()->GetNumberOfPoints();
   int numberOfPointsInDistanceFilter = m_DistanceFilterVector[0]->GetOutput()->GetNumberOfPoints();
   for(int i=0 ; i< numberOfPointsInDistanceFilter; i++)
   {
     double value = 0;
     for(int j=0; j< m_DistanceFilterVector.size(); j++)
     {
       double v = 0;

       double tmppt[3];
       m_DistanceFilterVector[j]->GetOutput()->GetPoint(i, tmppt);
        
       v = m_DistanceFilterVector[j]->GetOutput()->GetPointData()->GetScalars()->GetTuple1(i);

       if(tmppt[0] < b[0] || tmppt[0] > b[1] || tmppt[1] < b[2] || tmppt[1] > b[3])
         v = 0;

       value += v;
     }
     m_Image->GetPointData()->GetScalars()->SetTuple1(i,value);
   }
    
   
   // additional array for x,y,z coordinates
	 vtkMAFSmartPointer<vtkDoubleArray> x;
	 vtkMAFSmartPointer<vtkDoubleArray> y;
	 vtkMAFSmartPointer<vtkDoubleArray> z;
			
   int numberOfPoints1 = m_Points->GetNumberOfPoints();
	 for(int i = 0 ; i < numberOfPoints1; i++)
	 {
		 x->InsertNextTuple1(m_Points->GetPoint(i)[0]);
		 y->InsertNextTuple1(m_Points->GetPoint(i)[1]);
		 z->InsertNextTuple1(m_Points->GetPoint(i)[2]);
	 }
	 x->SetName("XCoords");
	 y->SetName("YCoords");
	 z->SetName("ZCoords");
			
   vtkPointData *pointdata;
   pointdata = m_Image->GetPointData();
	 pointdata->AddArray(x);
	 pointdata->AddArray(y);
	 pointdata->AddArray(z);

   m_Image->UpdateData();

   
   m_Plane->SetNormal(0,-1,0);
   m_Plane->SetOrigin(b[0],b[2],b[4]);
   m_Plane->SetPoint1(b[0] + spacing * tempPoints->GetNumberOfPoints(),b[2],b[4]);
   m_Plane->SetPoint2(b[0],b[2],b[5]);

   m_Plane->SetResolution(tempPoints->GetNumberOfPoints(),numberOfIteration+1);
   m_Plane->Update();

   mmaVolumeMaterial *vol_material;
   mafNEW(vol_material);
   vol_material->DeepCopy(((mafVMEVolumeGray *)vol)->GetMaterial());
   vol_material->UpdateProp();
   vtkLookupTable *lut = vol_material->m_ColorLut;

   
   double range[2];
   m_Image->GetScalarRange(range);
   lut->SetRange(range);
	 lut->Build();

//   m_Surface->SetData(m_Plane->GetOutput(), 0);
//   m_Surface->Update();

   m_Plane->GetOutput()->Update();

   m_Image->GetPointData()->GetArray(0)->SetLookupTable(lut);
   m_Image->UpdateData();

   
   m_IMTC->SetInput(m_Image);
   m_IMTC->SetLookupTable(lut);
   m_IMTC->Update();

   ((mafVMEOutputSurface *) GetOutput())->SetTexture(m_IMTC->GetOutput());
   GetMaterial()->SetMaterialTexture(((mafVMEOutputSurface *) GetOutput())->GetTexture());

   for(int i=0; i< m_PointsVector.size();i++)
   {
     vtkDEL(m_PointsVector[i]);
   }
   m_PointsVector.clear();

   for(int i=0; i< m_PolyDataVector.size();i++)
   {
     vtkDEL(m_PolyDataVector[i]);
   }
   m_PolyDataVector.clear();

   for(int i=0; i< m_DistanceFilterVector.size();i++)
   {
     vtkDEL(m_DistanceFilterVector[i]);
   }
   m_DistanceFilterVector.clear();
  
   mafDEL(vol_material);
}*/

//-----------------------------------------------------------------------
void mafVMEAdvancedProber::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	
}

//-------------------------------------------------------------------------
int mafVMEAdvancedProber::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEAdvancedProber *advancedProber = mafVMEAdvancedProber::SafeDownCast(a);
    
		for (mafLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); i++)
		{
			this->SetLink(i->first, i->second.m_Node);
		}

    m_Transform->SetMatrix(advancedProber->m_Transform->GetMatrix());
    
    m_VolumeName      = advancedProber->m_VolumeName;
		
		/*for(int i=0; i<advancedProber->m_ProfilesNameList.size();i++)
		{
		  m_ProfilesNameList.push_back(advancedProber->m_ProfilesNameList[i]);  
		}*/
		m_SamplingCoefficient = advancedProber->m_SamplingCoefficient;
		m_AdditionalProfileNumber = advancedProber->m_AdditionalProfileNumber;
		m_ProfileDistance = advancedProber->m_ProfileDistance;
		
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
void mafVMEAdvancedProber::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void mafVMEAdvancedProber::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-----------------------------------------------------------------------
mafNode *mafVMEAdvancedProber::GetVolumeLink()
//-----------------------------------------------------------------------
{
  return GetLink("Volume");
}
//-----------------------------------------------------------------------
mafNode::mafLinksMap *mafVMEAdvancedProber::GetPolylineLink()
//-----------------------------------------------------------------------
{
  // da ritornare la link maps
  return GetLinks();
}
//-----------------------------------------------------------------------
void mafVMEAdvancedProber::SetVolumeLink(mafNode *volume)
//-----------------------------------------------------------------------
{
  SetLink("Volume", volume);
  Modified();
}
//-----------------------------------------------------------------------
void mafVMEAdvancedProber::SetPolylineLink(mafNode *polyline)
//-----------------------------------------------------------------------
{
  SetLink(polyline->GetName(), polyline);
  Modified();
}
//-----------------------------------------------------------------------
int mafVMEAdvancedProber::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
		parent->StoreInteger("Fixed", m_Fixed);
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMEAdvancedProber::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
			node->RestoreInteger("Fixed", m_Fixed);
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}

/*-------------------------------------------------------------------------
void mafVMEAdvancedProber::SetMode(int mode)
//-------------------------------------------------------------------------
{
  if (mode != mafVMEAdvancedProber::DENSITY_MODE && mode != mafVMEAdvancedProber::DISTANCE_MODE)
  {
    mafErrorMacro("trying to set the map mode to invalid type: allowed types are DENSITY_MODE & DISTANCE_MODE");
    return;
  }

  m_ProberMode = mode;
  GetTagArray()->SetTag(mafTagItem("MFL_MAP_MODE",m_ProberMode));
  this->Modified();
}
/-------------------------------------------------------------------------
int mafVMEAdvancedProber::GetMode()
//-------------------------------------------------------------------------
{
  return (int)mafRestoreNumericFromTag(GetTagArray(), "MFL_MAP_MODE",m_ProberMode,-1,0);
}*/
//-------------------------------------------------------------------------
mmgGui* mafVMEAdvancedProber::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink());
  m_VolumeName = vol ? vol->GetName() : "none";
  //m_Gui->Button(ID_VOLUME_LINK,&m_VolumeName,_("Volume"), _("Select the volume to be probed"));


  /*m_ListBox = new mmgListCtrl(m_Gui, ID_LISTBOX);
  m_ListBox->SetListener(this);
  m_ListBox->Reset();
	m_ListBox->SetSize(wxSize(140,80));
  m_ListBox->SetColumnLabel(0, _("Profile"));*/
  //m_ListBox->SetColumnLabel(1, _("Slice"));

	m_ListBox=m_Gui->ListBox(ID_LISTBOX);

  //m_ListBox->SetColumnSize(0, 160);
  
  //m_ListBox->SetColumnSize(1, 40);

  for (mafLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); ++i)
  {
    if(i->first.Equals("Volume")) continue;

    /*double temp = 0;
    for(int j=0; j<m_IdListBox.size(); j++)
    {
      if(m_IdListBox[j] > temp) temp = m_IdListBox[j];
    }

    int id = temp + 1;
    m_IdListBox.push_back(id);*/
 
    //m_ProfilesNameList.push_back(i->second.m_Node->GetName());
    m_Gui->Update();

		m_ListBox->Append(i->second.m_Node->GetName());
    //m_ListBox->AddItem(m_IdListBox[m_IdListBox.size()-1], i->second.m_Node->GetName());
    //m_ListBox->Show();
  }

  //m_Gui->Add(m_ListBox);
  
  m_Gui->Button(ID_ADD_PROFILE, _("Add"), _("") ,_(""));
	m_Gui->Enable(ID_ADD_PROFILE,m_Fixed==0);
  //m_Gui->Button(ID_REMOVE_PROFILE, _("Remove"), _("") ,_(""));
  m_Gui->Divider(2);

  m_Gui->Double(ID_SAMPLING, _("Sample"),  &m_SamplingCoefficient, 1 , 5);
	//m_Gui->Integer(ID_SPLINE, _("Spline"),  &m_SplineCoefficient, 1 , 20);

  m_Gui->Label(_("Projection Panoramic Parameters"));
  m_Gui->Integer(ID_PROFILE_NUMBER,_("Number"),&m_AdditionalProfileNumber,0,1000);
  m_Gui->Double(ID_PROFILE_DISTANCE,_("Step"),&m_ProfileDistance,0.0);
  /*m_ProberMode = GetMode();
  wxString prober_mode[2] = {_("density"), _("distance")};
  m_Gui->Combo(ID_MODALITY,_("modality"), &m_ProberMode, 2, prober_mode);*/

  m_Gui->Divider();
  // none event to process:
 
  m_Gui->Bool(NULL,"Sharpening",&m_EnableSharpening,0,"This option adjust the image, but will make the application slower");
  m_Gui->Label("");
  m_Gui->Button(ID_UPDATE,_("Update"),_(""));
	m_Gui->Label("");
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEAdvancedProber::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_VOLUME_LINK:
      {
        mafString title = _("Choose volume vme");
        e->SetId(VME_CHOOSE);
        e->SetArg((long)&mafVMEAdvancedProber::VolumeAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
        if (n != NULL)
        {
          SetVolumeLink(n);
          m_VolumeName = n->GetName();
          m_Gui->Update();
									
					ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
        }
      }
      break;
      //case ID_MODALITY:
      //  SetMode(m_ProberMode);
      //break;
      case ID_ADD_PROFILE:
      {
				/*if(m_ListBox->GetCount()!=0)
				{
					wxMessageBox("Current max profile number is one!");
					return;
				}*/
        mafString title = _("Choose surface vme");
        e->SetId(VME_CHOOSE);
        e->SetArg((long)&mafVMEAdvancedProber::PolylineAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
        
        if (n == NULL) return;

        //wxString nameProfile ="";
        //mafString idNumber = wxString::Format(_("%d"),id);
        //nameProfile = n->GetName();
        
				if(wxNOT_FOUND != m_ListBox->FindString(n->GetName()))
				{
					wxMessageBox(_("Warning: you're introducing a profile\nwith the same name of a selected one.\n\
              Modify it and readd it"));
            return;
				}

        /*for(int i=0; i<m_ListBox->;i++)
        {
          if(m_ProfilesNameList[i] == nameProfile)
          {
            wxMessageBox(L"Warning: you're introducing a profile\nwith the same name of a selected one.\n\
              Modify it and readd it"); 
            return;
          }
        }*/

        /*if(m_ProfilesNameList.size() >= 2)
        {
            wxMessageBox(L"Warning: Supported 2 profiles"); 
            return;
        }*/

        /*double temp = 0;
        for(int i=0; i<m_IdListBox.size(); i++)
        {
          if(m_IdListBox[i] > temp) temp = m_IdListBox[i];
        }

        int id = temp + 1;
        m_IdListBox.push_back(id);*/
   
        SetPolylineLink(n);

        //nameProfile += idNumber;
        //m_ProfilesNameList.push_back(nameProfile);
//        m_PolylineList.push_back(n);

				//int a=m_ListBox->Append(wxString::Format("%s",n->GetName()));
				mafString t;
        t = n->GetName();
				m_ListBox->Append(_(t));
        //m_ListBox->AddItem(m_IdListBox[m_IdListBox.size()-1], nameProfile.GetCStr());
        m_ListBox->Update();
				m_Gui->Update();
				Modified();
				//InternalUpdate();

				ForwardUpEvent(&mafEvent(this,CAMERA_RESET));
				ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));

				m_Fixed = 1;
				m_Gui->Enable(ID_ADD_PROFILE,m_Fixed==0);
				m_Gui->Update();
      }
      break;
      case ID_REMOVE_PROFILE:
      { 
				if(m_ListBox->GetCount()!=0)
				{
					wxString name = m_ListBox->GetStringSelection();
					RemoveLink(name);
					m_ListBox->Delete(m_ListBox->FindString(m_ListBox->GetStringSelection()));

					Modified();
					m_Gui->Update();
					ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
				}
      }
      break;
      case ID_SAMPLING:
			case ID_SPLINE:
      case ID_PROFILE_NUMBER:
      case ID_PROFILE_DISTANCE:
      break;
      case ID_UPDATE:
        if (m_EnableSharpening)
		    {
			  
			  // unsharp filtering

			  m_ScalarImage->DeepCopy(m_Image); // copy the image and clean the coord arrays
			  vtkDoubleArray *x, *y, *z;
			  vtkNEW(x);
			  vtkNEW(y);
			  vtkNEW(z);

			  x->DeepCopy(m_ScalarImage->GetPointData()->GetArray("XCoords"));
			  y->DeepCopy(m_ScalarImage->GetPointData()->GetArray("YCoords"));
			  z->DeepCopy(m_ScalarImage->GetPointData()->GetArray("ZCoords"));
			  m_ScalarImage->GetPointData()->RemoveArray("XCoords");
			  m_ScalarImage->GetPointData()->RemoveArray("YCoords");
			  m_ScalarImage->GetPointData()->RemoveArray("ZCoords");

			  UnsharpImage();

			  //
			  m_ImageFiltered->GetPointData()->AddArray(x);
			  m_ImageFiltered->GetPointData()->AddArray(y);
			  m_ImageFiltered->GetPointData()->AddArray(z);
			  //m_ImageFiltered->GetScalarRange(range);
				mmaVolumeMaterial *vol_material;
				mafNEW(vol_material);
				vol_material->DeepCopy(((mafVMEVolumeGray *)this->GetParent())->GetMaterial());
				vol_material->UpdateProp();
				double lowHigh[2];
				vol_material->m_ColorLut->GetTableRange(lowHigh);

				double wholeScalarRangeVol[2];
				((mafVMEVolumeGray *)this->GetParent())->GetOutput()->GetVTKData()->GetScalarRange(wholeScalarRangeVol);

				double wholeScalarRangeAdv[2];
				m_ImageFiltered->GetScalarRange(wholeScalarRangeAdv);

				double advLow,advHigh;
				advLow = wholeScalarRangeAdv[0] + ((wholeScalarRangeAdv[1] - wholeScalarRangeAdv[0])/(wholeScalarRangeVol[1] - wholeScalarRangeVol[0])) * (lowHigh[0] - wholeScalarRangeVol[0]);
				advHigh = wholeScalarRangeAdv[1] + ((wholeScalarRangeAdv[1] - wholeScalarRangeAdv[0])/(wholeScalarRangeVol[1] - wholeScalarRangeVol[0])) * (lowHigh[1] - wholeScalarRangeVol[1]);

			  m_Lut->SetRange(advLow,advHigh);
			  m_Lut->Build();

			  m_Plane->GetOutput()->Update();

			  m_Image->GetPointData()->GetArray(0)->SetLookupTable(m_Lut);
			  m_Image->UpdateData();

				
			  m_IMTC->SetInput(m_ImageFiltered);
			  m_IMTC->SetLookupTable(m_Lut);
			  m_IMTC->Update();

			  ((mafVMEOutputSurface *) GetOutput())->SetTexture(m_IMTC->GetOutput());
			  GetMaterial()->SetMaterialTexture(((mafVMEOutputSurface *) GetOutput())->GetTexture());
			  vtkDEL(x);
			  vtkDEL(y);
			  vtkDEL(z);

			 
		  }
		  else
		  {
			  double range[2];
				mmaVolumeMaterial *vol_material;
				mafNEW(vol_material);
				vol_material->DeepCopy(((mafVMEVolumeGray *)this->GetParent())->GetMaterial());
				vol_material->UpdateProp();
				double lowHigh[2];
				vol_material->m_ColorLut->GetTableRange(lowHigh);

				double wholeScalarRangeVol[2];
				((mafVMEVolumeGray *)this->GetParent())->GetOutput()->GetVTKData()->GetScalarRange(wholeScalarRangeVol);

				double wholeScalarRangeAdv[2];
				m_Image->GetScalarRange(wholeScalarRangeAdv);

				double advLow,advHigh;
				advLow = wholeScalarRangeAdv[0] + ((wholeScalarRangeAdv[1] - wholeScalarRangeAdv[0])/(wholeScalarRangeVol[1] - wholeScalarRangeVol[0])) * (lowHigh[0] - wholeScalarRangeVol[0]);
				advHigh = wholeScalarRangeAdv[1] + ((wholeScalarRangeAdv[1] - wholeScalarRangeAdv[0])/(wholeScalarRangeVol[1] - wholeScalarRangeVol[0])) * (lowHigh[1] - wholeScalarRangeVol[1]);

				m_Lut->SetRange(advLow,advHigh);
			  m_Lut->Build();

			  m_Plane->GetOutput()->Update();

			  m_Image->GetPointData()->GetArray(0)->SetLookupTable(m_Lut);
			  m_Image->UpdateData();


			  m_IMTC->SetInput(m_Image);
			  m_IMTC->SetLookupTable(m_Lut);
			  m_IMTC->Update();

			  ((mafVMEOutputSurface *) GetOutput())->SetTexture(m_IMTC->GetOutput());
			  GetMaterial()->SetMaterialTexture(((mafVMEOutputSurface *) GetOutput())->GetTexture());

		  }
        //InternalUpdate();
        Modified();
        ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
      break;
      default:
      mafNode::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
char** mafVMEAdvancedProber::GetIcon() 
//-------------------------------------------------------------------------
{
	#include "mafVMEAdvancedProber.xpm"
  return mafVMEAdvancedProber_xpm;
}
//-------------------------------------------------------------------------
bool mafVMEAdvancedProber::CheckUpdatePanoramic(mafVMEPolylineSpline *vme) 
//-------------------------------------------------------------------------
{
  if(m_AutomaticCalculum) return true;
  vtkPoints *tempPoints;

  if( m_SamplingCoefficient != m_ControlSamplingCoefficient ||
      m_ProfileDistance     != m_ControlProfileDistance     ||
      m_AdditionalProfileNumber != m_ControlAdditionalProfileNumber //||
	   )
  {
      return true;
  }

  if(vme && vme->GetPolylineLink())
    tempPoints = vtkPolyData::SafeDownCast(vme->GetPolylineLink()->GetOutput()->GetVTKData())->GetPoints();
  else
    return true;

  if(m_ControlPoints->GetNumberOfPoints() == 0 || m_ControlPoints->GetNumberOfPoints() != tempPoints->GetNumberOfPoints()) return true;

  bool result = false;
  if(m_ControlPoints && tempPoints)
  {
    for(int i=0; i<tempPoints->GetNumberOfPoints(); i++)
    {
      double controlOrig[3], controlNew[3];
      tempPoints->GetPoint(i,controlNew);
      m_ControlPoints->GetPoint(i,controlOrig);

      if(
          controlNew[0] ==  controlOrig[0] &&
          controlNew[1] ==  controlOrig[1] &&
          controlNew[2] ==  controlOrig[2]
        ) ;
      else
      {
        result = true;
        break;
      }
        
    }
  }


  

  return result;
}

//-----------------------------------------------
void mafVMEAdvancedProber::UnsharpImage()
//-----------------------------------------------
{


	typedef  double InputPixelType;
	typedef  itk::Image< InputPixelType, 3 > InputImageType;
	typedef  double OutputPixelType;
	typedef  itk::Image< OutputPixelType, 3 > OutputImageType; 
	typedef  itk::LaplacianSharpeningImageFilter< InputImageType, OutputImageType > MyLaplacianFilter;

	MyLaplacianFilter::Pointer spLaplFilt = MyLaplacianFilter::New();
	//c->SetInput

	

	typedef itk::VTKImageToImageFilter< OutputImageType > ConvertervtkTOitk;
	ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New(); // i prefer the smart pointer...

	vtkTOitk->SetInput( m_ScalarImage );
	
	try
	{
		vtkTOitk->Update();
	}

	catch (itk::ExceptionObject &err)
	{
		wxMessageBox("An error has been occurred during vtk to itk conversion");
		m_ImageFiltered->DeepCopy(vtkImageData::SafeDownCast(m_ScalarImage));
		return;
	}
	spLaplFilt->SetInput(vtkTOitk->GetOutput());

	typedef itk::RescaleIntensityImageFilter<OutputImageType, OutputImageType> RescaleFilter;



	//Setting the IO
	RescaleFilter::Pointer rescale = RescaleFilter::New();

	//Setting the ITK pipeline filter



	// Rescale and cast to unsigned char
	rescale->SetInput( spLaplFilt->GetOutput() );

	rescale->SetOutputMinimum(   0 );
	rescale->SetOutputMaximum( 65536 ); //16 bit..


		// i prefer the smart pointer...->Update();
	try
	{
		rescale->Update();
	}
	catch (itk::ExceptionObject &err)
	{
		m_ImageFiltered->DeepCopy(vtkImageData::SafeDownCast(m_ScalarImage));
		wxMessageBox("An error has been occurred during unsharp computation");
		return;
	}

	typedef itk::ImageToVTKImageFilter< OutputImageType > ConverteritkTOvtk;
	ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New(); // i prefer the smart pointer...
	itkTOvtk->SetInput( rescale->GetOutput() );
	itkTOvtk->Update();
	m_ImageFiltered->DeepCopy(vtkImageData::SafeDownCast(itkTOvtk->GetOutput()));

	//set the output of the filter


}