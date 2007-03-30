/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafICPUtility.h,v $
  Language:  C++
  Date:      $Date: 2007-03-30 10:52:50 $
  Version:   $Revision: 1.4 $
  Authors:   Andrea Cutti porting Matteo Giacomoni    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

// .NAME mafICPUtility - Implementation of the ICP algorithm utility.
// .SECTION Description

#ifndef __mafICPUtility_h
#define __mafICPUtility_h

//system includes
#include <fstream>

#include "vtkMAFConfigure.h"

#include "vtkObject.h"

//vnl include
#include <algorithm>
#include <vcl_utility.h>
#include <vcl_map.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_determinant.h>

//----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT mafICPUtility : public vtkObject
//----------------------------------------------------------------------------
{
public:
  static mafICPUtility *New();

  vtkTypeRevisionMacro(mafICPUtility,vtkObject);

  struct RegResult{
		vnl_matrix_fixed<double,3,3> R;
		vnl_vector_fixed<double,3> t;

		vnl_vector_fixed<double,3> th;
		vnl_matrix_fixed<double,4,4> pose;

		double var;
		double err;
		double test;
		vnl_matrix_fixed<double,6,6> cov_matrix;
  };

  //method that allow to make the registration
  //vtkTransform *RegisterClouds(mflVME *source, mflVME *target, float varThreshold, int swap, int maxCycleNumber, wxString knownMinLocalFile = "");
  inline vnl_matrix<double> PolyData2VnlFilter(vtkPolyData* Shape);
  inline void vtk2vnl(vtkPolyData* Shape, double* MatrixData);
  inline RegResult SVD_Registration(vnl_matrix<double>& MS, vnl_matrix<double>& DS);
  inline void Vnl2PolyDataFilter(vnl_matrix<double> M, vtkPolyData* Shape);
  inline void rows_randperm(vnl_matrix<double>& M);
  inline vnl_matrix_fixed<double,3,3> theta2R(const vnl_vector_fixed<double,3>& th);
  inline vnl_matrix_fixed<double,4,4> R_t_2_Tr(const vnl_matrix_fixed<double,3,3>& R,const vnl_vector_fixed<double,3>& t);
  inline void findPointLocator(vtkPoints *MS_vtkpoints, vtkPointLocator *pointLocator, double* DS_data, int DScols, vnl_matrix<double> &closest_pts);
  inline RegResult ICP_vtkPointLocator(vnl_matrix<double>& MS, vnl_matrix<double> & DS, int cycle_lim);
  inline RegResult ICP_vtkPointLocator(vnl_matrix<double>& MS, vnl_matrix<double> & DS, vcl_ifstream& ml_read_file, float var_threshold, int cycle_lim);
  inline RegResult ICP_vtkPointLocator(vnl_matrix<double>& MS, vnl_matrix<double> & DS, vcl_multimap<double,int>& mm, const char* ml_filepath, float var_threshold, int cycle_lim);
  inline vnl_matrix_fixed<double,3,3> rpy2R(const vnl_vector_fixed<double,3> v);
  inline vnl_matrix_fixed<double,3,3> rpy2R(const double* row);
  inline vnl_matrix<double> rpyOnShape(const vnl_matrix<double>& Shape, const double* row);
  inline vnl_vector_fixed<double,3> centroid(const vnl_matrix<double> & S);
  inline vnl_vector<double> center(const vnl_matrix<double>& S);
  inline void neg_translation(vnl_matrix<double>& S, const vnl_vector<double> c);
  inline void pos_translation(vnl_matrix<double>& S, const vnl_vector<double> c);
  inline void Rotation_classif(vcl_multimap<double,int>& mm,const vnl_matrix<double>& DS, vnl_matrix<double>& MS);
  inline RegResult FFRegistration( vnl_matrix<double>& MS, vnl_matrix<double>& DS, const char* knownMinLocalFile, float VarianceThrshold, int MaximumNumberOfIterations);
  inline void Target_on_Source(vnl_matrix_fixed<double,3,3>& R, vnl_vector_fixed<double,3>& p);

  inline RegResult StandardRegistration( vnl_matrix<double>& MS, vnl_matrix<double>& DS, float Convergence);

  mafString RotationFixedFile;
  mafString RotationFixedIndexFile;
  mafString KnownMinLocalPositionFile;
  bool KnownMinLocalFlag;
  bool RotationFixedFlag;

protected:
  mafICPUtility():KnownMinLocalFlag(false),RotationFixedFlag(false){};
  ~mafICPUtility() {};
};

vtkCxxRevisionMacro(mafICPUtility, "$Revision: 1.4 $");
  vtkStandardNewMacro(mafICPUtility);
//----------------------------------------------------------------------------
inline vnl_matrix<double> mafICPUtility::PolyData2VnlFilter(vtkPolyData* Shape)
//----------------------------------------------------------------------------
{
	double* MatrixData=new double[(Shape->GetNumberOfPoints())*3];
	vtk2vnl(Shape, MatrixData);
	vnl_matrix<double> M(MatrixData, Shape->GetNumberOfPoints() , 3);
	delete[] MatrixData;
	return M;
}
//----------------------------------------------------------------------------
inline void mafICPUtility::vtk2vnl(vtkPolyData* Shape, double* MatrixData)
//----------------------------------------------------------------------------
{
	double coord[3];
	int n_pts=Shape->GetNumberOfPoints();

	for (int i=0;i<n_pts;i++)
	{
		Shape->GetPoint(i, coord);
		for(int j=0; j<3; j++)
		{
			(*MatrixData)=coord[j]; 
			MatrixData++;
		}
	}
}
//----------------------------------------------------------------------------
inline mafICPUtility::RegResult mafICPUtility::SVD_Registration(vnl_matrix<double>& MS, vnl_matrix<double>& DS)
//----------------------------------------------------------------------------
{
	////vcl_cout << DS << '\n';

	vnl_vector_fixed<double,3> MS_centroid=centroid(MS);
	vnl_vector_fixed<double,3> DS_centroid=centroid(DS);

	//vcl_cout << "Dentro SVD il centroide di MS vale: " << MS_centroid << '\n';
	//vcl_cout << "Dentro SVD il centroide di DS vale: " << DS_centroid << '\n';

	neg_translation(MS, MS_centroid);
	neg_translation(DS, DS_centroid);

	vnl_matrix<double> MSt=MS.transpose();
	vnl_matrix_fixed<double,3,3> Z=MSt*DS; //MS deve essere 3xn mentre DS una nx3
	
	vnl_svd<double> s_v_d(Z);
	
	vnl_matrix<double> V=s_v_d.V();
	V.inplace_transpose();
	
	vnl_vector_fixed<double,3> temp(1,1,vnl_determinant(s_v_d.U()*V));
	vnl_diag_matrix<double> diag(temp); 

	mafICPUtility::RegResult registrazione_S_V_D;
	registrazione_S_V_D.R=s_v_d.U()*diag*V;
	registrazione_S_V_D.t=MS_centroid-(registrazione_S_V_D.R)*DS_centroid;
	
	//vcl_cout << "Entro SVD R finale vale: \n" << registrazione_S_V_D.R << '\n';
	//vcl_cout << "Entro SVD t finale vale: \n" << registrazione_S_V_D.t << '\n';

	pos_translation(MS,MS_centroid); //Se le matrici sono passate per riferimento questo serve! se no commentare
	pos_translation(DS,DS_centroid); //Se le matrici sono passate per riferimento questo serve! se no commentare
	
	return registrazione_S_V_D;
}
//----------------------------------------------------------------------------
inline void mafICPUtility::Vnl2PolyDataFilter(vnl_matrix<double> M, vtkPolyData* Shape)
//----------------------------------------------------------------------------
{
	vtkPoints *points = vtkPoints::New();
	for (int i=0; i<M.rows(); i++) points->InsertPoint(i,M[i]);
	Shape->SetPoints(points);
	points->Delete();
}
//----------------------------------------------------------------------------
inline void mafICPUtility::rows_randperm(vnl_matrix<double>& M)
//----------------------------------------------------------------------------
{
	const int nrows=M.rows();
	const int ncols=M.columns();

	vnl_vector<int> index(nrows);
	for(int i=0; i< nrows; i++) index.put(i,i);
	std::random_shuffle(index.begin(), index.end());
	
	vnl_matrix<double> temp(nrows, ncols);
	for(int i=0;i<M.rows(); i++) temp.set_row(i,M[index[i]]);
	M=temp;
}
//----------------------------------------------------------------------------
inline vnl_matrix_fixed<double,3,3> mafICPUtility::theta2R(const vnl_vector_fixed<double,3>& th)
//----------------------------------------------------------------------------
{
	double fi=th.magnitude();
	double sinc=sin(fi)/fi; //sin accept rediants, such as MATLAB
	double cosc=(1-cos(fi))/(fi*fi);
		
	vnl_matrix_fixed<double,3,3> A;
		A(0,0)=0;      A(0,1)=-th(2)*sinc;  A(0,2)=th(1)*sinc;
		A(1,0)=th(2)*sinc;  A(1,1)=0;       A(1,2)=-th(0)*sinc;
		A(2,0)=-th(1)*sinc; A(2,1)=th(0)*sinc;   A(2,2)=0;
	
	vnl_diag_matrix<double> first_term(3, cos(fi));

	vnl_matrix_fixed<double,3,3> third_term;
		for(int i=0; i<3; i++) third_term.set_row(i, cosc*th(i)*th);

		vnl_matrix_fixed<double,3,3> R=first_term+A.as_matrix()+third_term.as_matrix();
	return R;
}
//----------------------------------------------------------------------------
inline vnl_matrix_fixed<double,4,4> mafICPUtility::R_t_2_Tr(const vnl_matrix_fixed<double,3,3>& R,const vnl_vector_fixed<double,3>& t)
//----------------------------------------------------------------------------
{
	vnl_matrix_fixed<double,4,4> Tr;
	Tr.update(R);
	double last_row[]={0,0,0,1};
	Tr.set_column(3,t);
	Tr.set_row(3,last_row);
	return Tr;
}
//----------------------------------------------------------------------------
inline void mafICPUtility::findPointLocator(vtkPoints *MS_vtkpoints, vtkPointLocator *pointLocator, double* DS_data, int DScols, vnl_matrix<double> &closest_pts)
//----------------------------------------------------------------------------
{
	//NB: DS è una 3 x n
	vtkIdType ptId;
	double query_DS[3]; 
	double cl_pt[3];

	for (int k=0; k< DScols; k++)
	{
		for (int i=0; i < 3; i++) query_DS[i]=DS_data[i*DScols+k];//  [k*3+i]riempio query_DS con il punto di DS
		ptId = pointLocator->FindClosestPoint(query_DS);
				
		MS_vtkpoints-> GetPoint(ptId, cl_pt);
		closest_pts.set_column(k,cl_pt);
	}
}
//----------------------------------------------------------------------------
inline mafICPUtility::RegResult mafICPUtility::ICP_vtkPointLocator(vnl_matrix<double>& MS, vnl_matrix<double> & DS, int cycle_lim)
//----------------------------------------------------------------------------
{
	//-----------Contruzione Point Locator----------------------
	vtkPoints *MS_vtkpoints = vtkPoints::New();
	MS_vtkpoints->SetNumberOfPoints(MS.rows());
	for (int i = 0; i < MS.rows(); i++) {
		MS_vtkpoints->InsertPoint(i, MS(i,0), MS(i,1), MS(i,2));
	}
	
	vtkPolyData *MS_poly = vtkPolyData::New();
	MS_poly->SetPoints(MS_vtkpoints);
	MS_poly->Modified();

	//vcl_cout << "Start building PointLocator" <<'\n';
	vtkPointLocator *p_locator = vtkPointLocator::New();
	p_locator->SetDataSet(MS_poly);
	p_locator->BuildLocator();
	//vcl_cout << "Finish building PointLocator" <<'\n';
	//-----------------------------------------------------------
		
	DS.inplace_transpose(); //Dato che di DS non me ne faccio niente dritto, transpongo il  sorgente

	const int DSrows = DS.rows();		//ora le righe sono 3...
	const int DScols = DS.columns();	//...e le colonne n

	double costov=1E+6;
	double coston=1E+5;

	double b=2E-5;
	const double a=1+1E-5;

	vnl_vector_fixed<double,3> th(1E-5,1E-5,1E-5);
	vnl_vector_fixed<double,3> t(1E-5,1E-5,1E-5);

	double test=1;			double test_lim=1E-5;//orig:1E-5
	double err=100;			double err_lim=1E-2;//orig:1E-2
	double var_err=1E+3;	double var_err_lim=1E-12;
	int cycle=0;			//int cycle_lim=28;//Usualmente 28 NOW PASSED FROM THE USER INTERFACE
		
	//Initialization of partial result needed to compute dy
	vnl_matrix<double> dy(DSrows, DScols);
	vnl_vector_fixed<double,3> th_a;					//es: th.*[a 1 1]
	vnl_vector_fixed<double,3> t_a;						//es: t.*[a 1 1]
	vnl_matrix<double> R_DS(DSrows, DScols);			//es: theta2R(th)*DS
	vnl_matrix<double> translation(DSrows, DScols);		//es: t*ones(1,n)
	vnl_matrix<double> DS_move(DSrows, DScols);			//es: theta2R(th)*DS+t*ones(1,n)
	vnl_matrix<double> S(DSrows*DScols, 6);
	vnl_matrix<double> St(6,DSrows*DScols);
	
	vnl_matrix<double> closest_pts(DSrows, DScols);		
	vnl_matrix<double> em(DSrows, DScols);		
	vnl_vector<double> e(DSrows*DScols);		
	vnl_vector_fixed<double,6> dv;

	while(test>test_lim && err>err_lim && var_err > var_err_lim && cycle < cycle_lim){
		cycle++;
		//vcl_cout << "ciclo numero: \n" << cycle << '\n';
		if(cycle>8) b=1E-5; //era 8 metto 1

		//Caricamente strutture parziali
		R_DS=theta2R(th)*DS;
		for (int i=0; i< DScols; i++) translation.set_column(i,t);
		DS_move=R_DS+translation;


		for(int j=0; j<3; j++){
			th_a=th;
			th_a.put(j, th[j]*a);
			

			dy=( (theta2R(th_a)*DS)+translation-DS_move ) / ( th[j]*b );
			S.set_column(j,dy.data_block());
		}

		for(int j=0; j<3; j++){
			t_a=t;
			t_a.put(j, t[j]*a);
			
			for (int i=0; i< DScols; i++) translation.set_column(i,t_a);

			dy=( R_DS+translation-DS_move ) / ( t[j]*b );
			S.set_column(j+3,dy.data_block());
		}

		findPointLocator(MS_vtkpoints, p_locator, DS_move.data_block(), DScols, closest_pts);

		em=closest_pts-DS_move;
		e.set(em.data_block());
		
		//Aggiorno condizioni di ciclo
		coston=e.squared_magnitude();
		test=abs(coston-costov)/costov;
		costov=coston;
		err = sqrt( coston / (3*DScols) );
		var_err = coston / ((3*DScols)-6);

		//Aggiorno i parametri di trasformazione
		St=S.transpose();
		vnl_matrix_inverse<double> inv_St_S(St*S);
		vnl_matrix<double> dv_mat_temp=inv_St_S*St;
		dv=dv_mat_temp*e;
		th +=dv.extract(3,0);
		t +=dv.extract(3,3);
	}

	DS=DS_move;
	DS.inplace_transpose();

	mafICPUtility::RegResult res;
	res.R=theta2R(th);
	res.t=t;
	res.err= err;
	res.var = var_err;
	res.test=test;
	//vcl_cout << "var finale vale: "<< res.var << var_err << '\n';
	//vcl_cout << "test finale vale: " << res.test << test << '\n';
	//vcl_cout << "err finale vale: " << res.err << err << '\n';
	//vcl_cout << "th vale: \n" << th << '\n';
	//vcl_cout << "t vale: \n" << t << '\n';
	vnl_matrix_inverse<double> inv_St_S(St*S);
	vnl_matrix_fixed<double,6,6> covariance_mat_temp=inv_St_S;
	res.cov_matrix=var_err*covariance_mat_temp;
	//vcl_cout << res.cov_matrix <<'\n';
	
	MS_vtkpoints->Delete();
	MS_poly->Delete();
	p_locator->Delete();	
	return res;
}
//----------------------------------------------------------------------------
inline vnl_matrix_fixed<double,3,3> mafICPUtility::rpy2R(const vnl_vector_fixed<double,3> v)
//----------------------------------------------------------------------------
{
	double pi=3.14159265358979323846;
	double xrad=(v[0]*pi)/180;
	double yrad=(v[1]*pi)/180;
	double zrad=(v[2]*pi)/180;

	vnl_matrix_fixed<double,3,3> rx,ry,rz,R;
	rx(0,0)=1 ;rx(0,1)=0 ;rx(0,2)=0 ;
	rx(1,0)=0 ;rx(1,1)=cos(xrad) ;rx(1,2)=-sin(xrad) ;
	rx(2,0)=0 ;rx(2,1)=sin(xrad) ;rx(2,2)=cos(xrad) ;
	
	ry(0,0)=cos(yrad) ;ry(0,1)=0 ;ry(0,2)=sin(yrad) ;
	ry(1,0)=0 ;ry(1,1)=1 ;ry(1,2)=0 ;
	ry(2,0)=-sin(yrad) ;ry(2,1)=0 ;ry(2,2)=cos(yrad) ;
	
	rz(0,0)=cos(zrad) ;rz(0,1)=-sin(zrad) ;rz(0,2)=0 ;
	rz(1,0)=sin(zrad) ;rz(1,1)=cos(zrad) ;rz(1,2)=0 ;
	rz(2,0)=0 ;rz(2,1)=0 ;rz(2,2)=1 ;

	R=rz*ry*rx;
	
	return R;
}
//----------------------------------------------------------------------------
inline vnl_matrix_fixed<double,3,3> mafICPUtility::rpy2R(const double* row)
//----------------------------------------------------------------------------
{
	double pi=3.14159265358979323846;
	double xrad=(row[0]*pi)/180;
	double yrad=(row[1]*pi)/180;
	double zrad=(row[2]*pi)/180;

	vnl_matrix_fixed<double,3,3> rx,ry,rz,R;
	rx(0,0)=1 ;rx(0,1)=0 ;rx(0,2)=0 ;
	rx(1,0)=0 ;rx(1,1)=cos(xrad) ;rx(1,2)=-sin(xrad) ;
	rx(2,0)=0 ;rx(2,1)=sin(xrad) ;rx(2,2)=cos(xrad) ;
	
	ry(0,0)=cos(yrad) ;ry(0,1)=0 ;ry(0,2)=sin(yrad) ;
	ry(1,0)=0 ;ry(1,1)=1 ;ry(1,2)=0 ;
	ry(2,0)=-sin(yrad) ;ry(2,1)=0 ;ry(2,2)=cos(yrad) ;
	
	rz(0,0)=cos(zrad) ;rz(0,1)=-sin(zrad) ;rz(0,2)=0 ;
	rz(1,0)=sin(zrad) ;rz(1,1)=cos(zrad) ;rz(1,2)=0 ;
	rz(2,0)=0 ;rz(2,1)=0 ;rz(2,2)=1 ;

	R=rz*ry*rx;
	
	return R;
}
//----------------------------------------------------------------------------
inline vnl_matrix<double> mafICPUtility::rpyOnShape(const vnl_matrix<double>& Shape, const double* row)
//----------------------------------------------------------------------------
{
//NB Shape DEVE essere già 3xn!!!
		
	double pi=3.14159265358979323846;
	double xrad=(row[0]*pi)/180;
	double yrad=(row[1]*pi)/180;
	double zrad=(row[2]*pi)/180;

	vnl_matrix_fixed<double,3,3> rx,ry,rz,R;
	rx(0,0)=1 ;rx(0,1)=0 ;rx(0,2)=0 ;
	rx(1,0)=0 ;rx(1,1)=cos(xrad) ;rx(1,2)=-sin(xrad) ;
	rx(2,0)=0 ;rx(2,1)=sin(xrad) ;rx(2,2)=cos(xrad) ;
	
	ry(0,0)=cos(yrad) ;ry(0,1)=0 ;ry(0,2)=sin(yrad) ;
	ry(1,0)=0 ;ry(1,1)=1 ;ry(1,2)=0 ;
	ry(2,0)=-sin(yrad) ;ry(2,1)=0 ;ry(2,2)=cos(yrad) ;
	
	rz(0,0)=cos(zrad) ;rz(0,1)=-sin(zrad) ;rz(0,2)=0 ;
	rz(1,0)=sin(zrad) ;rz(1,1)=cos(zrad) ;rz(1,2)=0 ;
	rz(2,0)=0 ;rz(2,1)=0 ;rz(2,2)=1 ;

	R=rz*ry*rx;
	
	vnl_matrix<double> Shape_move=R*Shape;
	return Shape_move;
}
//----------------------------------------------------------------------------
inline mafICPUtility::RegResult mafICPUtility::ICP_vtkPointLocator(vnl_matrix<double>& MS, vnl_matrix<double> & DS, vcl_ifstream& ml_read_file, float var_threshold, int cycle_lim)
//----------------------------------------------------------------------------
{
	//-----------Contruzione Point Locator----------------------
	vtkPoints *MS_vtkpoints = vtkPoints::New();
	MS_vtkpoints->SetNumberOfPoints(MS.rows());
	for (int i = 0; i < MS.rows(); i++) {
		MS_vtkpoints->InsertPoint(i, MS(i,0), MS(i,1), MS(i,2));
	}
	
	vtkPolyData *MS_poly = vtkPolyData::New();
	MS_poly->SetPoints(MS_vtkpoints);
	MS_poly->Modified();

	//vcl_cout << "Start building PointLocator" <<'\n';
	vtkPointLocator *p_locator = vtkPointLocator::New();
	p_locator->SetDataSet(MS_poly);
	p_locator->BuildLocator();
	//vcl_cout << "Finish building PointLocator" <<'\n';
	//-----------------------------------------------------------
	
	DS.inplace_transpose(); //Dato che di DS non me ne faccio niente dritto, transpongo il  sorgente
	
	//------------------------------------------------
	vnl_matrix<double> Min;
	vnl_matrix<double> DS_R(DS.rows(),DS.columns());//3xn
	
	Min.read_ascii(ml_read_file);
	if(Min.cols() != 3 && Min.rows() < 1) 
	{
		vtkWarningMacro("Error in of known min positions file:\n inexistent rotations or erroneous");
		MS_vtkpoints->Delete(); //FONDAMENTALE!!!!!!
		MS_poly->Delete();
		p_locator->Delete();
		mafICPUtility::RegResult dummy_result;
		dummy_result.var=100000;
		return dummy_result;
	}
	const int DSrows = DS.rows();		//ora le righe sono 3...
	const int DScols = DS.columns();	//...e le colonne n
	
	double costov=1E+6;
	double coston=1E+5;
	
	double b=2E-5;
	const double a=1+1E-5;//era 1+1E-5
	
	vnl_vector_fixed<double,3> th(1E-5,1E-5,1E-5);
	vnl_vector_fixed<double,3> t(1E-5,1E-5,1E-5);

	vnl_vector_fixed<double,3> th_ref(1E-5,1E-5,1E-5);
	vnl_vector_fixed<double,3> t_ref(1E-5,1E-5,1E-5);
	
	double test=1;			double test_lim=1E-5;
	double err=100;			double err_lim=1E-2;
	double var_err=1E+3;	double var_err_lim=1E-12;
	int cycle=0;			//int cycle_lim=28;NOW GIVEN from the User Interface
		
	//Initialization of partial result needed to compute dy
	vnl_matrix<double> DS_move(DSrows, DScols);			//es: theta2R(th)*DS+t*ones(1,n)
	
	//-------
	vnl_matrix<double> dy(DSrows, DScols);
	vnl_vector_fixed<double,3> th_a;					//es: th.*[a 1 1]
	vnl_vector_fixed<double,3> t_a;						//es: t.*[a 1 1]
	vnl_matrix<double> R_DS(DSrows, DScols);			//es: theta2R(th)*DS
	vnl_matrix<double> translation(DSrows, DScols);		//es: t*ones(1,n)
	vnl_matrix<double> S(DSrows*DScols, 6);
	vnl_matrix<double> St(6,DSrows*DScols);
	
	vnl_matrix<double> closest_pts(DSrows, DScols);		
	vnl_matrix<double> em(DSrows, DScols);		
	vnl_vector<double> e(DSrows*DScols);		
	vnl_vector_fixed<double,6> dv;
//-----------------------------------------------------------------

	int ind=0; //Necessario se, fuori dal for, voglio capire qual'è stata l'ultima RAR applicata

	for(ind=0; ind<Min.rows() && var_err >var_threshold; ind++){
		DS_R=rpyOnShape(DS, Min[ind]);//3xn
		
		b=2E-5;

		costov=1E+6;
		coston=1E+5;
		//vcl_cout << "---------------NUOVO CICLO------------\n"; 
		
		th=th_ref;
		t=t_ref;
		
		test=1;			
		err=100;		
		var_err=1E+3;	
		cycle=0;		

		while( (test>test_lim) && (err>err_lim) && (var_err > var_err_lim) && (cycle < cycle_lim)){
			cycle++;
			//vcl_cout << "ciclo numero: \n" << cycle << '\n';
				
			if(cycle>18) b=1E-5; 

			//Caricamente strutture parziali
			R_DS=theta2R(th)*DS_R;
			for (int i=0; i< DScols; i++) translation.set_column(i,t);
			DS_move=R_DS+translation;

			for(int j=0; j<3; j++){
				th_a=th;
				th_a.put(j, th[j]*a);

				dy=( (theta2R(th_a)*DS_R)+translation-DS_move ) / ( th[j]*b );
				S.set_column(j,dy.data_block());
			}

			for(int j=0; j<3; j++){
				t_a=t;
				t_a.put(j, t[j]*a);
				
				for (int i=0; i< DScols; i++) translation.set_column(i,t_a);

				dy=( R_DS+translation-DS_move ) / ( t[j]*b );
				S.set_column(j+3,dy.data_block());
			}

			findPointLocator(MS_vtkpoints, p_locator, DS_move.data_block(), DScols, closest_pts);

			em=closest_pts-DS_move;
			e.set(em.data_block());
			
			//Aggiorno condizioni di ciclo
			coston=e.squared_magnitude();
			test=abs(coston-costov)/costov;
			costov=coston;
			err = sqrt( coston / (3*DScols) );
			var_err = coston / ((3*DScols)-6);

			//Aggiorno i parametri di trasformazione
			St=S.transpose();
			vnl_matrix_inverse<double> inv_St_S(St*S);
			vnl_matrix<double> dv_mat_temp=inv_St_S*St;
			dv=dv_mat_temp*e;
			th +=dv.extract(3,0);
			t +=dv.extract(3,3);
		}//end_while
//		vcl_cout << "err finale vale: \n" << err << '\n';
//		vcl_cout << "var finale vale: \n" << var_err << '\n';
//		vcl_cout << "Ind Vale: " << ind << '\n';
	}//end_for

	DS=DS_move;
	DS.inplace_transpose();

	//Composizione della RAR e della matrice di rotazione ottenuta da ICP
	vnl_matrix_fixed<double,3,3> R_rar, R_icp, R_reg;
	//vcl_cout << "Ind vale: " << ind << '\n';
	//for(int p=0;p<3;p++) //vcl_cout << Min[ind-1][p] << '\t';
	//vcl_cout << '\n';
	R_rar=rpy2R(Min[ind-1]);
	R_icp=theta2R(th);
	R_reg=R_icp*R_rar;

	mafICPUtility::RegResult res;
	res.R=R_reg;
	res.t=t;
//	vcl_cout << "Matrice di rotazione:\n" << res.R << '\n';
//	vcl_cout << "Vettore traslazione:\n" << res.t << '\n';
	res.err= err;
	res.var = var_err;
	res.test=test;
//	vcl_cout << "test finale vale: " << test << '\n';
//	vcl_cout << "err finale vale: " << err << '\n';
//	vcl_cout << "var finale vale: " << var_err << '\n';
	//vcl_cout << "th vale: \n" << th << '\n';
	//vcl_cout << "t vale: \n" << t << '\n';
	vnl_matrix_inverse<double> inv_St_S(St*S);
	vnl_matrix_fixed<double,6,6> covariance_mat_temp=inv_St_S;
	res.cov_matrix=var_err*covariance_mat_temp;
//	vcl_cout << res.cov_matrix <<'\n';
	MS_vtkpoints->Delete();
	MS_poly->Delete();
	p_locator->Delete();
	return res;
}
//----------------------------------------------------------------------------
inline vnl_vector_fixed<double,3> mafICPUtility::centroid(const vnl_matrix<double> & S)
//----------------------------------------------------------------------------
{
	vnl_vector_fixed<double,3> c(0.);
	
	for(int i=0; i<S.rows(); i++){
		c[0]+=S(i,0);
		c[1]+=S(i,1);
		c[2]+=S(i,2);
	}
	c/=S.rows();	
	
	return c;
}
//----------------------------------------------------------------------------
inline vnl_vector<double> mafICPUtility::center(const vnl_matrix<double>& S)
//----------------------------------------------------------------------------
{
	vnl_vector<double> aux(S.rows());
	vnl_vector<double> c(3);

	for(int i=0; i<3; i++){
		aux=S.get_column(i);
		c.put(i, (aux.max_value()+aux.min_value())/2 );
	}
	
	return c;
}
//----------------------------------------------------------------------------
inline void mafICPUtility::neg_translation(vnl_matrix<double>& S, const vnl_vector<double> c)
//----------------------------------------------------------------------------
{
	vnl_matrix<double> aux(S.rows(),S.columns());
	for(int i=0; i<aux.rows(); i++){
		aux.set_row(i,c);
	}

	S -= aux;
}
//----------------------------------------------------------------------------
inline void mafICPUtility::pos_translation(vnl_matrix<double>& S, const vnl_vector<double> c)
//----------------------------------------------------------------------------
{
	vnl_matrix<double> aux(S.rows(),S.columns());
	for(int i=0; i<aux.rows(); i++){
		aux.set_row(i,c);
	}

	S += aux;
}
//----------------------------------------------------------------------------
inline void mafICPUtility::Rotation_classif(vcl_multimap<double,int>& mm,const vnl_matrix<double>& DS, vnl_matrix<double>& MS)
//----------------------------------------------------------------------------
{
	//-----------Contruzione Point Locator----------------------
		vtkPoints *MS_vtkpoints = vtkPoints::New();
		MS_vtkpoints->SetNumberOfPoints(MS.rows());
		for (int i = 0; i < MS.rows(); i++) {
			MS_vtkpoints->InsertPoint(i, MS(i,0), MS(i,1), MS(i,2));
		}
		
		vtkPolyData *MS_poly = vtkPolyData::New();
		MS_poly->SetPoints(MS_vtkpoints);
		MS_poly->Modified();

		//vcl_cout << "Start building PointLocator" <<'\n';
		vtkPointLocator *p_locator = vtkPointLocator::New();
		p_locator->SetDataSet(MS_poly);
		p_locator->BuildLocator();
		//vcl_cout << "Finish building PointLocator" <<'\n';
	//-----------------------------------------------------------
	
	vnl_matrix<double> DS_temp(DS); //PER COPIA
	DS_temp.inplace_transpose(); //adesso DS_temp è 3xn
	const int DS_temp_rows=DS_temp.rows();
	const int DS_temp_cols=DS_temp.columns();

	vnl_vector<int> index;
	vcl_ifstream ml_rot_fixed(this->RotationFixedIndexFile.GetCStr(), std::ios::in);//Changed from "Rot_fixed_index.txt"
	index.read_ascii(ml_rot_fixed);
	//vcl_cout << "There are  " << index.size() << "  to be tested\n";

	vnl_vector_fixed<double,3> x45(45,0,0);
	vnl_vector_fixed<double,3> y45(0,45,0);
	vnl_vector_fixed<double,3> z45(0,0,45);
	
	vnl_matrix_fixed<double,3,3> Rx=rpy2R(x45);
	vnl_matrix_fixed<double,3,3> Ry=rpy2R(y45);
	vnl_matrix_fixed<double,3,3> Rz=rpy2R(z45);

	vnl_matrix<double> closest_pts(DS_temp_rows, DS_temp_cols);		
	vnl_matrix<double> em(DS_temp_rows, DS_temp_cols);		
	vnl_vector<double> e(DS_temp_rows*DS_temp_cols);	
	
	double costo;
	//vcl_cout << "Inizio classificazione\n";

	for(int i=0; i<index.size();i++){
		//vcl_cout << "i vale " << i << '\n';
		//wxLogMessage(wxString::Format("%d",i));
		switch(index[i]){
		case 1:
			DS_temp=Rz*DS_temp;
			findPointLocator(MS_vtkpoints, p_locator, DS_temp.data_block(), DS_temp_cols, closest_pts);
			em=closest_pts-DS_temp;
			e.set(em.data_block());
			costo=e.squared_magnitude();
			mm.insert(std::make_pair(costo,i));
			break;
		case 2:
			DS_temp=Ry*DS_temp;
			findPointLocator(MS_vtkpoints, p_locator, DS_temp.data_block(), DS_temp_cols, closest_pts);
			em=closest_pts-DS_temp;
			e.set(em.data_block());
			costo=e.squared_magnitude();
			mm.insert(std::make_pair(costo,i));
			break;
		case 3:
			DS_temp=Rx*DS_temp;
			findPointLocator(MS_vtkpoints, p_locator, DS_temp.data_block(), DS_temp_cols, closest_pts);
			em=closest_pts-DS_temp;
			e.set(em.data_block());
			costo=e.squared_magnitude();
			mm.insert(std::make_pair(costo,i));
			break;
		case 11:
			DS_temp=Rz*DS_temp;
			break;
		case 22:
			DS_temp=Ry*DS_temp;
			break;
		}
	}
	MS_vtkpoints->Delete();
	MS_poly->Delete();
	p_locator->Delete();
}
//----------------------------------------------------------------------------
inline mafICPUtility::RegResult mafICPUtility::ICP_vtkPointLocator(vnl_matrix<double>& MS, vnl_matrix<double> & DS, vcl_multimap<double,int>& mm, const char* ml_filepath, float var_threshold, int cycle_lim)
//----------------------------------------------------------------------------
{
	//-----------Contruzione Point Locator----------------------
	vtkPoints *MS_vtkpoints = vtkPoints::New();
	MS_vtkpoints->SetNumberOfPoints(MS.rows());
	for (int i = 0; i < MS.rows(); i++) 
	{
		MS_vtkpoints->InsertPoint(i, MS(i,0), MS(i,1), MS(i,2));
	}
		
	vtkPolyData *MS_poly = vtkPolyData::New();
	MS_poly->SetPoints(MS_vtkpoints);
	MS_poly->Modified();

	//vcl_cout << "Start building PointLocator" <<'\n';
	vtkPointLocator *p_locator = vtkPointLocator::New();
	p_locator->SetDataSet(MS_poly);
	p_locator->BuildLocator();
	//vcl_cout << "Finish building PointLocator" <<'\n';
	//-----------------------------------------------------------
	
	DS.inplace_transpose(); //Dato che di DS non me ne faccio niente dritto, transpongo il  sorgente
	
	//------------------------------------------------
		
	vcl_multimap<double, int>::iterator mm_it;
	
	vnl_matrix<double> Min;
	vnl_matrix<double> DS_R(DS.rows(),DS.columns());//3xn
	
	vcl_ifstream ml_rot_fixed(this->RotationFixedFile.GetCStr(), std::ios::in); //Changed from Rotation_fixed.txt
	if(ml_rot_fixed.is_open() == 0) 
	{
		//vcl_cout << "File rotazioni RAR2 inesistente: impossibile proseguire" <<'\n';
		exit(0);
	}
	
	Min.read_ascii(ml_rot_fixed);
	
	if(Min.cols() !=3 && Min.rows()<1) 
	{
		//vcl_cout << "Errore nel file per RAR2:\n rotazioni RAR2 inesistenti o errone\n IMPOSSIBILE PROSEGUIRE" <<'\n';
		exit(1);
	}

	//double var_threshold=3; NOW GIVEN from the User Interface

	//--------------------------------------------------

	const int DSrows = DS.rows();		//ora le righe sono 3...
	const int DScols = DS.columns();	//...e le colonne n

	//vcl_cout << "Righe di DS: " << DSrows <<'\n' ;
	//vcl_cout << "Colonne di DS: " << DScols <<'\n' ;

	double costov=1E+6;
	double coston=1E+5;

	double b=2E-5;
	const double a=1+1E-5;

	vnl_vector_fixed<double,3> th(1E-5,1E-5,1E-5);
	vnl_vector_fixed<double,3> t(1E-5,1E-5,1E-5);

	vnl_vector_fixed<double,3> th_ref(1E-5,1E-5,1E-5);
	vnl_vector_fixed<double,3> t_ref(1E-5,1E-5,1E-5);

	double test=1;			double test_lim=1E-5;
	double err=100;			double err_lim=1E-2;
	double var_err=1E+3;	double var_err_lim=1E-12;
	int cycle=0;			//int cycle_lim=28;NOW GIVEN from the User Interface
		
	vnl_matrix<double> DS_move(DSrows, DScols);			//es: theta2R(th)*DS+t*ones(1,n)
	//----------
	vnl_matrix<double> dy(DSrows, DScols);
	vnl_vector_fixed<double,3> th_a;					//es: th.*[a 1 1]
	vnl_vector_fixed<double,3> t_a;						//es: t.*[a 1 1]
	vnl_matrix<double> R_DS(DSrows, DScols);			//es: theta2R(th)*DS
	vnl_matrix<double> translation(DSrows, DScols);		//es: t*ones(1,n)
	vnl_matrix<double> S(DSrows*DScols, 6);
	vnl_matrix<double> St(6,DSrows*DScols);
	
	vnl_matrix<double> closest_pts(DSrows, DScols);		
	vnl_matrix<double> em(DSrows, DScols);		
	vnl_vector<double> e(DSrows*DScols);		
	vnl_vector_fixed<double,6> dv;
	//----------
	vcl_ofstream ml_write_file(ml_filepath, std::ios::app);
//	vcl_ofstream ver_index("verifica_indici.txt", std::ios::out);	

	//-----------------------------------------------------------------
	for(mm_it=mm.begin(); (mm_it !=mm.end()) && (var_err >var_threshold); mm_it++){
	
		DS_R=rpyOnShape(DS, Min[mm_it->second]);//3xn
	
		//ver_index << mm_it->second << "    " << Min.get_row(mm_it->second) << '\n';

		//vcl_cout << "---------------NUOVO CICLO------------\n"; 
		b=2E-5;

		costov=1E+6;
		coston=1E+5;
		
		th=th_ref;
		t=t_ref;
		
		test=1;			
		err=100;		
		var_err=1E+3;	
		cycle=0;		

		while(test>test_lim && err>err_lim && var_err > var_err_lim && cycle < cycle_lim){
			cycle++;
			//vcl_cout << "ciclo numero: \n" << cycle << '\n';
			if(cycle>18) b=1E-5;

			//Caricamente strutture parziali
			R_DS=theta2R(th)*DS_R;
			for (int i=0; i< DScols; i++) translation.set_column(i,t);
			DS_move=R_DS+translation;

			for(int j=0; j<3; j++)
			{
				th_a=th;
				th_a.put(j, th[j]*a);
			
				dy=( (theta2R(th_a)*DS_R)+translation-DS_move ) / ( th[j]*b );
				S.set_column(j,dy.data_block());
			}

			for(int j=0; j<3; j++)
			{
				t_a=t;
				t_a.put(j, t[j]*a);
				
				for (int i=0; i< DScols; i++) translation.set_column(i,t_a);

				dy=( R_DS+translation-DS_move ) / ( t[j]*b );
				S.set_column(j+3,dy.data_block());
			}

			findPointLocator(MS_vtkpoints, p_locator, DS_move.data_block(), DScols, closest_pts);

			em=closest_pts-DS_move;
			e.set(em.data_block());
				
			//Aggiorno condizioni di ciclo
			coston=e.squared_magnitude();
			test=abs(coston-costov)/costov;
			costov=coston;
			err = sqrt( coston / (3*DScols) );
			var_err = coston / ((3*DScols)-6);
			//vcl_cout << "var_err vale: \n" << var_err << '\n';

			//Aggiorno i parametri di trasformazione
			St=S.transpose();
			vnl_matrix_inverse<double> inv_St_S(St*S);
			vnl_matrix<double> dv_mat_temp=inv_St_S*St;
			dv=dv_mat_temp*e;
			th +=dv.extract(3,0);
			t +=dv.extract(3,3);
		}//end_while
		//vcl_cout << "var_err vale: " << var_err << '\n';
	}//end_for

	DS=DS_move;
	DS.inplace_transpose();

	int id_RAR2_fin=(--mm_it)->second;

	ml_write_file << '\n' << Min.get_row(id_RAR2_fin);
	//Composizione della RAR e della matrice di rotazione ottenuta da ICP

	vnl_matrix_fixed<double,3,3> R_rar, R_icp, R_reg;

	R_rar=rpy2R(Min[id_RAR2_fin]);
	R_icp=theta2R(th);
	R_reg=R_icp*R_rar;

	mafICPUtility::RegResult res;
	res.R=R_reg;
	res.t=t;
	res.err= err;
	res.var = var_err;
	res.test=test;
	//vcl_cout << "var vale: " << var_err << '\n';
	//vcl_cout << "err vale: " << err << '\n';
	//vcl_cout << "test vale: " << test << '\n';
	//vcl_cout << "th vale: \n" << th << '\n';
	//vcl_cout << "t vale: \n" << t << '\n';
	vnl_matrix_inverse<double> inv_St_S(St*S);
	vnl_matrix_fixed<double,6,6> covariance_mat_temp=inv_St_S;
	res.cov_matrix=var_err*covariance_mat_temp;
	//vcl_cout << res.cov_matrix <<'\n';
	
	MS_vtkpoints->Delete();
	MS_poly->Delete();
	p_locator->Delete();
	return res;
}


//----------------------------------------------------------------------------
inline mafICPUtility::RegResult mafICPUtility::FFRegistration( vnl_matrix<double>& MS, vnl_matrix<double>& DS, const char* knownMinLocalFile, float VarianceThrshold, int MaximumNumberOfIterations)
//----------------------------------------------------------------------------
{
	vnl_vector<double> MS_centroid=centroid(MS);
	vnl_vector<double> DS_centroid=centroid(DS);
	//vcl_cout << "centro di MS:" << MS_center << '\n';
	//vcl_cout << "centro di DS:" << DS_center << '\n';

	neg_translation(MS, MS_centroid);
	neg_translation(DS, DS_centroid);
	
	mafICPUtility::RegResult res=ICP_vtkPointLocator(MS, DS, MaximumNumberOfIterations);	//Copy by reference; finally DS is the registered one
	
	if (res.var < VarianceThrshold)
	{
		vnl_vector_fixed<double,3> ttot;
		ttot=res.t-res.R*DS_centroid+MS_centroid;
		res.t=ttot;
	}
	else
	{
		mafICPUtility::RegResult res2;
		res2.var=VarianceThrshold+1;

		vnl_matrix<double> DS_ml(DS);
		
		vcl_ifstream ml_read_file(knownMinLocalFile, std::ios::in);
		//if(ml_read_file.is_open() != 0)
		if(KnownMinLocalFlag)
		{	
			res2=ICP_vtkPointLocator(MS, DS, ml_read_file, VarianceThrshold, MaximumNumberOfIterations);
		}
		//else vcl_cout << "The execution of RAR1 is impossible: known min local file inexistent;\n" << vcl_endl;
		
		//vcl_cout << res2.var <<'\n';
		
		if (res2.var > VarianceThrshold && 	RotationFixedFlag)
		{
			//vcl_cout << res2.var <<'\n';
			vcl_multimap<double, int> mm;
			Rotation_classif(mm,DS_ml,MS);
			res2 = ICP_vtkPointLocator(MS, DS_ml, mm, knownMinLocalFile, VarianceThrshold, MaximumNumberOfIterations);
			DS=DS_ml;	
		}
		
		vnl_matrix_fixed<double,3,3> Rtot;
		vnl_vector_fixed<double,3> ttot,tfin;
		Rtot=res2.R * res.R;
		ttot=res2.R * res.t + res2.t;
		tfin=ttot-Rtot*DS_centroid+MS_centroid;
		res.R=Rtot;
		res.t=tfin;
		//Cov, var_err and err in res2 are also copied into res
		res.var=res2.var;
		res.err=res2.err;
		res.test=res2.test;
		res.cov_matrix=res2.cov_matrix;
	}
	
	pos_translation(DS, MS_centroid);
	pos_translation(MS, MS_centroid); //Aggiunta, in modo che l'MS in input alla funzione non subisca variazioni
	
	return res;
}

//----------------------------------------------------------------------------
inline mafICPUtility::RegResult mafICPUtility::StandardRegistration( vnl_matrix<double>& MS, vnl_matrix<double>& DS, float Convergence)
//----------------------------------------------------------------------------
{
	//HO BISOGNO DI FARE LA TRASPOSTA DELLE MATRICI TARGET(MS) E SOURCE(DS)
	//REGISTRO LA SOURCE(DS) SUL TARGET(MS)
	//APPLICO LA ROTAZIONE E LA TRASLAZIONE ALLA SOURCE(DS)

	
	//-----------Contruzione Point Locator----------------------
  int i;
	vtkPoints *MS_vtkpoints = vtkPoints::New();
	MS_vtkpoints->SetNumberOfPoints(MS.rows());
	for (i = 0; i < MS.rows(); i++) 
  {
		MS_vtkpoints->InsertPoint(i, MS(i,0), MS(i,1), MS(i,2));
	}
	
	vtkPolyData *MS_poly = vtkPolyData::New();
	MS_poly->SetPoints(MS_vtkpoints);
	MS_poly->Modified();

	//vcl_cout << "Start building PointLocator" <<'\n';
	vtkPointLocator *p_locator = vtkPointLocator::New();
	p_locator->SetDataSet(MS_poly);
	p_locator->BuildLocator();
	//vcl_cout << "Finish building PointLocator" <<'\n';
	//-----------------------------------------------------------
	
	//modified by STEFY 5-12-2003(begin)
	if(DS.rows() != 3)
		DS = DS.transpose(); //tranpost of the source 
		
	if(MS.rows() != 3)
		MS = MS.transpose(); //transpost of the target 
	//modified by STEFY 5-12-2003(end)
	
	const int DSrows = DS.rows();		//source row = 3  
	const int DScols = DS.columns();	//source column = n

	const int MSrows = MS.rows();		//target row = 3  
	const int MScols = MS.columns();	//target column = m

	double costov=1E+6;
	double coston=1E+5;

	const double a=1+1E-5;

	vnl_vector_fixed<double,3> th(1E-5,1E-5,1E-5);
	vnl_vector_fixed<double,3> t(1E-5,1E-5,1E-5);

	int cycle=0;
  float test=1;	
	float test_lim = Convergence;
		
	double err=100;			
		
	//Initialization of partial results 
	vnl_matrix<double> dy(DSrows, DScols);	

	
	vnl_vector_fixed<double,3> th_a;					//es: th.*[a 1 1]
	vnl_vector_fixed<double,3> t_a;						//es: t.*[a 1 1]
	vnl_matrix<double> R_DS(DSrows, DScols);			//es: theta2R(th)*DS

	vnl_matrix<double> translation(DSrows, DScols);		//es: t*ones(1,n)

	vnl_matrix<double> DS_move(DSrows, DScols);			//es: theta2R(th)*DS+t*ones(1,n)
	vnl_matrix<double> S(DSrows*DScols, 6);
	vnl_matrix<double> St(6,DSrows*DScols);
	
	vnl_matrix<double> closest_pts(DSrows, DScols);		
	vnl_matrix<double> em(DSrows, DScols);		
	vnl_vector<double> e(DSrows*DScols);		
	vnl_vector_fixed<double,6> dv;
	
	double b = 1E-5;
  int j;

	while(test > test_lim)
  {
		R_DS=theta2R(th)*DS;

		for (i = 0; i < DScols; i++) 
      translation.set_column(i, t);
		
    DS_move = R_DS + translation;
	
		for(j = 0; j < 3; j++)
    {
			th_a = th;
			th_a.put(j, th[j]*a);
			dy = ((theta2R(th_a) * DS) + translation - DS_move) / (th[j] * b);
			S.set_column(j,dy.data_block());
		}

		for(j = 0; j < 3; j++)
    {
			t_a=t;
			t_a.put(j, t[j]*a);
			for (i = 0; i < DScols; i++) 
        translation.set_column(i,t_a);
			dy = (R_DS + translation - DS_move) / (t[j] * b);
			S.set_column(j + 3, dy.data_block());
		}
		findPointLocator(MS_vtkpoints, p_locator, DS_move.data_block(), DScols, closest_pts);

		em = closest_pts - DS_move;
		e.set(em.data_block());
		
		//Updating transformation parameters:
		St = S.transpose();
		vnl_matrix_inverse<double> inv_St_S(St * S);
		vnl_matrix<double> dv_mat_temp = inv_St_S * St;
		dv = dv_mat_temp * e;
		th += dv.extract(3, 0);
		t += dv.extract(3, 3);
			
		//Updating cycle-ending variables:
		coston = e.squared_magnitude();
		test = abs(coston-costov)/costov;
		costov = coston;

		err = sqrt(coston / (3 * DScols));
		cycle++;
	}

	mafICPUtility::RegResult res;
	res.R = theta2R(th);
	res.t = t;
	res.err = err;
	res.test = test;
	res.th = th;

	vnl_matrix_fixed<double,4,4> pos_mat;
	for (i = 0; i<3; i++)
	{
		for (j = 0; j<3; j++)
		{
		  pos_mat.put(i, j, res.R.get(i, j));
	  }
  }

	pos_mat.put(0, 3, t.get(0));
	pos_mat.put(1, 3, t.get(1));
	pos_mat.put(2, 3, t.get(2));

	pos_mat.put(3, 0, 0);
	pos_mat.put(3, 1, 0);
	pos_mat.put(3, 2, 0);
	pos_mat.put(3, 3, 1);
		
	res.pose = pos_mat;

	MS_vtkpoints->Delete();
	MS_poly->Delete();
	p_locator->Delete();
	
	return res;
}
//----------------------------------------------------------------------------
inline void mafICPUtility::Target_on_Source(vnl_matrix_fixed<double,3,3>& R, vnl_vector_fixed<double,3>& p)
//----------------------------------------------------------------------------
{
	vnl_matrix_fixed<double,3,3> Rtemp = R.transpose();
	R = Rtemp;

	vnl_vector_fixed<double,3> ptemp = -R * p;

	//R.inplace_transpose();
	p = ptemp;
}
#endif