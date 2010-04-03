/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medDicomCardiacMRIHelper.h,v $
  Language:  C++
  Date:      $Date: 2010-04-03 13:30:55 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __medDicomCardiacMRIHelper_h
#define __medDicomCardiacMRIHelper_h

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include <vector>
#include "mafString.h"

using namespace std;

/** 
Helper class for dicom cardiac images transformations handling: 

This class is the C++/vnl conversion of some matlab code used to handle transformations in 
some dicom cardiac mri images from General Electric and Siemens Machines.

The original script filemapp.m is copied at the end of this file and has been used to validate
the component through these tests cases available in medDicomCardiacMRIHelperTest:

medDicomCardiacMRIHelperTest::TestDicomGeneralElectricPisa
medDicomCardiacMRIHelperTest::TestDicomSiemensNiguarda

Reference to mri data on which this component was tested:
GE PISA (P09.rar)
SIEMENS NIGUARDA (P20.rar)

Sample code:

medDicomCardiacMRIHelper* helper = new medDicomCardiacMRIHelper();

wxString dicomDir = "d:\\dicomTest\\p09\\";
helper->SetInputDicomDirectoryABSPath(dicomDir.c_str());
helper->ParseDicomDirectory();

//query for output variables
cout << helper->GetFileNumberForPlaneIFrameJ();

*/
class medDicomCardiacMRIHelper
{
public:

  medDicomCardiacMRIHelper();

  ~medDicomCardiacMRIHelper();
  
  /** 
  Set dicom input dir*/
  void SetInputDicomDirectoryABSPath(mafString inputDicomDirectoryABSPath);
  mafString GetInputDicomDirectoryABSPath();
  
  /** 
  Perform dicom directory parsing*/
  void ParseDicomDirectory();

  /** 
  Query for dicom dir parsing results: look at the matlab script for further informations.
  This section of the documentation is work in progress since I still have to understand 
  some rotated dicom stuff. For the moment I can only guarantee that the output of this
  component is the same as the matlab one... */

  vector<string> GetDicomLocalFileNamesVector() {return m_DicomLocalFileNamesVector;};
  long int GetTimeFrames() {return m_TimeFrames;};
  int GetPlanesPerFrame() {return m_PlanesPerFrame;};
  vnl_vector<double> GetSpacing() {return m_Spacing;};

  vnl_matrix<double> GetFileNumberForPlaneIFrameJ() {return m_FileNumberForPlaneIFrameJ;};
  vnl_matrix<double> GetFileNumberForPlaneIFrameJIdPlane() {return m_FileNumberForPlaneIFrameJIdPlane;};
 
  vnl_matrix<double> GetPositionSingleFrameIdPlane() {return m_PositionSingleFrameIdPlane;};
  vnl_matrix<double> GetXVersorsSingleFrameIdPlane() {return m_XVersorsSingleFrameIdPlane;};
  vnl_matrix<double> GetYVersorsSingleFrameIdPlane() {return m_YVersorsSingleFrameIdPlane;};
  vnl_matrix<double> GetImageSizeSingleFrameIdPlane() {return m_ImageSizeSingleFrameIdPlane;}; 

  vnl_matrix<double> GetNewPositionSingleFrameIdPlane() {return m_NewPositionSingleFrameIdPlane;};
  vnl_matrix<double> GetNewXVersorsSingleFrameIdPlane() {return m_NewXVersorsSingleFrameIdPlane;};
  vnl_matrix<double> GetNewYVersorsSingleFrameIdPlane() {return m_NewYVersorsSingleFrameIdPlane;}; 
  vnl_matrix<double> GetNewImageSizeSingleFrameIdPlane() {return m_NewImageSizeSingleFrameIdPlane;};

  vnl_matrix<double> GetRotateFlagIdPlane() {return m_RotateFlagIdPlane;};
  vnl_matrix<double> GetFlipLeftRightFlagIdPlane() {return m_FlipLeftRightFlagIdPlane;};
  vnl_matrix<double> GetFlipUpDownFlagIdPlane() {return m_FlipUpDownFlagIdPlane;};

  static void ExtractRows( vnl_matrix<double> &inputMatrix, vector<double> &rowsToExtractVector,
    vnl_matrix<double> &outputMatrix );

  static void ExtractRows( vnl_matrix<double> &inputMatrix,
    vnl_vector<double> &rowsToExtractVector, vnl_matrix<double> &outputMatrix );

  static void RemoveRows( vnl_matrix<double> &inputMatrix, vector<double> &rowsToRemoveVector,
    vnl_matrix<double> &outputMatrix );

private:

  mafString m_InputDicomDirectoryABSPath;

  vector<string> m_DicomLocalFileNamesVector;
  long int m_TimeFrames;
  int m_PlanesPerFrame;

  vnl_vector<double> m_Spacing;

  vnl_matrix<double> m_FileNumberForPlaneIFrameJ;
  vnl_matrix<double> m_FileNumberForPlaneIFrameJIdPlane;
  vnl_matrix<double> m_PositionSingleFrameIdPlane;
  vnl_matrix<double> m_XVersorsSingleFrameIdPlane;
  vnl_matrix<double> m_YVersorsSingleFrameIdPlane;
  vnl_matrix<double> m_ImageSizeSingleFrameIdPlane; 

  vnl_matrix<double> m_NewPositionSingleFrameIdPlane;
  vnl_matrix<double> m_NewXVersorsSingleFrameIdPlane;
  vnl_matrix<double> m_NewYVersorsSingleFrameIdPlane; 
  vnl_matrix<double> m_NewImageSizeSingleFrameIdPlane;

  vnl_matrix<double> m_RotateFlagIdPlane;
  vnl_matrix<double> m_FlipLeftRightFlagIdPlane;
  vnl_matrix<double> m_FlipUpDownFlagIdPlane;

};
 
#endif

// MATLAB SOURCE CODE for filemapp.m;
// Tested with matlab 7.5
// Original author comments are in italian
/*

% script per la mappatura e il calcolo delle traformazioni per allineare le
% immagini CMR
% 
% INPUT directory contente solo le immagini
% 
% OUTPUT 
% 
% mappatura immagini
% - N: numero frame nell'acquisizione
% - numero piani nell'acquisizione
% - filelist: struct array contente nel campo name il nome di ciascuna
%   immagine apprtentente all'acquisizione
% - flag_img: contiene l'indice dell'immagine per ogni coordinata
%   piano-frame (eg: se flag_img(i,j) == num_img, allora
%   filelist(num_img).name è il nome file che contiente l'immagine del piano i nel frame j
% 
% parametri di posizione/orientamento originali da DICOM
% - P patient position (da DICOM)
% - Vx, Vy versori immagine (da DICOM)
% - s dimensione immagine (da DICOM)
% 
% parametri di posizione/orientamento originali da DICOM
% - Pp patient position (da DICOM)
% - Vxx, Vyy versori immagine (da DICOM)
% - ss dimensione immagine (da DICOM)
% 
% parametrui di rotazione da applicare alle immagini acquisite per ottenere
% immagini tra loro coe renti
% - rot: contiene la rotazione da effettuare
% - fliplr_flag: se 1 l'immagine deve essere specchiata in orizzontale
% - fliplr_flag: se 1 l'immagine deve essere specchiata in verticale

clear all
close all

% carico la directory contentente le immagini e creo la list delle immagini
path = uigetdir('Select patient directory');
filelist = dir(path);
filelist(1:2) = [];

% calcolo numero frame N e piani S dal numero totale di file e dal
% dicominfo della prima immagine
info = dicominfo([path '\' filelist(1).name]);
N = info.CardiacNumberOfImages;
S = numel(filelist)/N;
spacing = info.PixelSpacing;

% inizializzazione
% Vx = versori x
% Vy = versory y
% P  = posizione
% s  = image size
% f  = frame
Vx = zeros(N*S,3);
Vy = zeros(N*S,3);
P  = zeros(N*S,3);
s  = zeros(N*S,2);
f  = zeros(N*S,1);

% dall'infodicom di ciascuna immagine recupero le info necessarie
h = waitbar(0,'Mapping DICOM folder ...');
for i = 1:N*S
info = dicominfo([path '\' filelist(i).name]);
P(i,:)  = info.ImagePositionPatient;
Vx(i,:) = info.ImageOrientationPatient(1:3);
Vy(i,:) = info.ImageOrientationPatient(4:6);
s(i,:)  = [info.Columns info.Rows]; 
f(i) = info.InstanceNumber;
waitbar(i/(N*S));
end
close(h);

%% MAPPING PIANO-FRAME

% flag_img(i,j) indica il numero file del piano i, frame j
flag_img = zeros(S,N);

% SIEMENS - NIGUARDA
if strcmp(info.Manufacturer,'SIEMENS')
% indici del primo frame
id_fframe = find(f==1)';
flag_img(:,1) = id_fframe;

% cerco per confronto tutte le immagini appartenenti al piano i-esimo
for i = 1:S
Vtemp  = repmat([Vx(id_fframe(i),:) Vy(id_fframe(i),:)],[N*S 1]);
Vmatch = (Vtemp==[Vx Vy]);
% le immagini sul piano i-esimo hanno Vmatch=6
id_plane = find(sum(Vmatch,2)==6);
% assegno ciascun immagine del piano corrente al frame corrispondente
for k = id_plane'
flag_img(i,f(k)) = k;
end
end
%GE MEDICAL SYSTEMS - PISA    
else 
[dummy id_frameplane] = sort(f);
flag_img = reshape(id_frameplane,[N S]);
flag_img = flag_img';
end

% riduco i vettori Vx, Vy, P e s al singolo frame
Vx = Vx(flag_img(:,1),:);
Vy = Vy(flag_img(:,1),:);
P  = P(flag_img(:,1),:);
s  = s(flag_img(:,1),:);

% plot piani acquisizione
figure, title('Piani posizione originale');
for i = 1:S
p(1,:) = P(i,:);
p(2,:) = P(i,:) + s(i,1)*spacing(1)*Vx(i,:);
p(3,:) = P(i,:) + s(i,1)*spacing(1)*Vx(i,:) + s(i,2)*spacing(2)*Vy(i,:);
p(4,:) = P(i,:) + s(i,2)*spacing(2)*Vy(i,:);
patch(p(:,1),p(:,2),p(:,3),[.8 .8 .8],'FaceAlpha',.8,'EdgeColor',[.6 .6 .6]);
hold on    
plot3(P(i,1),P(i,2),P(i,3),'ro','MarkerFaceColor','r');
text(P(i,1),P(i,2),P(i,3)+25,num2str(i));
% plot direzioni X Y
plot3([p(1,1) p(2,1)],[p(1,2) p(2,2)],[p(1,3) p(2,3)],'.-r');
plot3([p(1,1) p(4,1)],[p(1,2) p(4,2)],[p(1,3) p(4,3)],'.-b');    
end
axis equal 

%% ROTAZIONE E FLIP IMMAGINI

% considero le 4 possibili proiezioni tra le 2 coppie di versori tra 2 piani 
proj = abs([dot(Vx(1,:),Vx(2,:)); dot(Vx(1,:),Vy(2,:)); dot(Vy(1,:),Vx(2,:)); dot(Vy(1,:),Vy(2,:))]);
[dummy, idx_mode] = max(proj);

% salvo in idx e idy i piani in cui ho trovato il versore parallelo
% all'asse di rotazione in Vx o Vy, rispettivamente
if( idx_mode==1 || idx_mode==2)
Vmode = Vx(1,:);
else
Vmode = Vy(1,:);
end

% per ciascun piano verifico quale sia il versore circa parallelo al
% versore Vmode e salvo l'indice in idx o idy
idx = [];
idy = [];
for i = 1:S
proj = abs([dot(Vmode,Vx(i,:)); dot(Vmode,Vy(i,:))]);
[dummy, idx_mode] = max(proj);
if idx_mode == 1
idx(end+1) = i;
else
idy(end+1) = i;
end
end

% inizializzazione delle nuove coordinate ImagePositionPatient e
% ImageOrientationPatient
Pp  = zeros(size(P));
Vxx = zeros(size(Vx));
Vyy = zeros(size(Vy));
ss  = zeros(size(s));

% variabili 
rot = zeros([S 1]);
fliplr_flag = zeros([S 1]);
flipud_flag = zeros([S 1]);

% correzione
for i = idx
ss(i,:) = [s(i,2) s(i,1)];
if Vx(i,1)>0
%         rimg(:,:,i) = imrotate(img,90);
Pp(i,:) = P(i,:) + s(i,1)*spacing(1)*Vx(i,:);
Vxx(i,:) = Vy(i,:);
Vyy(i,:) = -Vx(i,:);
rot(i) = 90;
else
%         rimg(:,:,i) = imrotate(fliplr(img),90);
Pp(i,:) = P(i,:);
Vxx(i,:) = Vy(i,:);
Vyy(i,:) = Vx(i,:);
rot(i) = 90;
fliplr_flag(i) = 1;
end
clear img
end


for i = idy
ss(i,:) = s(i,:);
if Vy(i,1)>0
%         rimg(:,:,i) = flipud(img);
Pp(i,:) = P(i,:) + s(i,2)*spacing(2)*Vy(i,:);
Vxx(i,:) = Vx(i,:);
Vyy(i,:) = -Vy(i,:);
flipud_flag(i) = 1;
else
%         rimg(:,:,i) = img;
Pp(i,:) = P(i,:);
Vxx(i,:) = Vx(i,:);
Vyy(i,:) = Vy(i,:);
end
end

% plot piani acquisizione
figure, title('Piani posizione corretta');
for i = 1:S
p(1,:) = Pp(i,:);
p(2,:) = Pp(i,:) + ss(i,1)*spacing(1)*Vxx(i,:);
p(3,:) = Pp(i,:) + ss(i,1)*spacing(1)*Vxx(i,:) + ss(i,2)*spacing(2)*Vyy(i,:);
p(4,:) = Pp(i,:) + ss(i,2)*spacing(2)*Vyy(i,:);
patch(p(:,1),p(:,2),p(:,3),[.8 .8 .8],'FaceAlpha',.8,'EdgeColor',[.6 .6 .6]);
hold on    
plot3(Pp(i,1),Pp(i,2),Pp(i,3),'ro','MarkerFaceColor','r');
text(Pp(i,1),Pp(i,2),Pp(i,3)+25,num2str(i));
% plot direzioni X Y
plot3([p(1,1) p(2,1)],[p(1,2) p(2,2)],[p(1,3) p(2,3)],'.-r');
plot3([p(1,1) p(4,1)],[p(1,2) p(4,2)],[p(1,3) p(4,3)],'.-b');    
end
axis equal    

%% ORDINAMENTO PIANI

%  stabilisco la relazione tra piani adiacenti 
V = [(1:S)' Vxx];

% scelgo il piano 1 come soluzione tentativo
Vidx(1,:) = V(1,:);
V(1,:) = [];

% calcolo l'angolo piano 1 e gli altri piani
for i = 1:size(V,1)
theta(i) = acos(dot(Vidx(1,2:4),V(i,2:4)));
end

% individuo i piani adiacenti al piano 1, con una tolleranza del 10%
% rispetto all'angolo teorico. Il piano adiacente è un unico quando piano 1
% è un piano estremo
id_theta = find(theta<1.1*(pi/S));
if numel(id_theta)==1
Vidx = [Vidx; V(id_theta,:)];
else
Vidx = [V(id_theta(1),:); Vidx; V(id_theta(2),:)];
end
V(id_theta,:)=[];
clear theta

% individuo i piani adiacenti al primo e ultimo piano in V
while ~isempty(V)
% calcolo l'angolo tra ultimo piano indicizzato e gli altri piani
for i = 1:size(V,1)
theta(i) = acos(dot(Vidx(end,2:4),V(i,2:4)));
end
id_theta = find(theta<1.1*(pi/S));
if ~isempty(id_theta)
Vidx = [Vidx; V(id_theta,:)];
V(id_theta,:)=[];
end
clear theta

% fin qui ok!!!

% calcolo l'angolo tra primo piano indicizzato e gli altri piani
for i = 1:size(V,1)
theta(i) = acos(dot(Vidx(1,2:4),V(i,2:4)));
end
id_theta = find(theta<1.1*(pi/S));
if ~isempty(id_theta)
Vidx = [V(id_theta,:); Vidx];
V(id_theta,:)=[];
end
clear theta
end

% ordino tutte le variabili secondo l'ordine dei piani
id_plane = Vidx(:,1);

% flag_img
flag_img = flag_img(id_plane,:);

% parametri DICOM originali
P  = P(id_plane,:);
Vx = Vx(id_plane,:);
Vy = Vy(id_plane,:);
s  = s(id_plane,:);

% parametri DICOM aggiornati
Pp  = Pp(id_plane,:);
Vxx = Vxx(id_plane,:);
Vyy = Vyy(id_plane,:);
ss  = ss(id_plane,:);

% flag di rotazione e flip
rot = rot(id_plane);
fliplr_flag = fliplr_flag(id_plane);
flipud_flag = flipud_flag(id_plane);


% plot piani acquisizione
figure, title('Piani posizione e ordine corretti');
for i = 1:S
p(1,:) = Pp(i,:);
p(2,:) = Pp(i,:) + ss(i,1)*spacing(1)*Vxx(i,:);
p(3,:) = Pp(i,:) + ss(i,1)*spacing(1)*Vxx(i,:) + ss(i,2)*spacing(2)*Vyy(i,:);
p(4,:) = Pp(i,:) + ss(i,2)*spacing(2)*Vyy(i,:);
patch(p(:,1),p(:,2),p(:,3),[.8 .8 .8],'FaceAlpha',.8,'EdgeColor',[.6 .6 .6]);
hold on    
plot3(Pp(i,1),Pp(i,2),Pp(i,3),'ro','MarkerFaceColor','r');
text(Pp(i,1),Pp(i,2),Pp(i,3)+25,num2str(i));
% plot direzioni X Y
plot3([p(1,1) p(2,1)],[p(1,2) p(2,2)],[p(1,3) p(2,3)],'.-r');
plot3([p(1,1) p(4,1)],[p(1,2) p(4,2)],[p(1,3) p(4,3)],'.-b');    
end
axis equal 


%% SALVATAGGIO VARIABILI DI INTERESSE

% esporto nel file map_DICOM le variabili filelist
save('DICOMap','filelist','N','S','spacing','flag_img','P','Vx','Vy','s','Pp','Vxx','Vyy','ss','rot','fliplr_flag','flipud_flag');

*/
