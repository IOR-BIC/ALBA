# ALBA Framework Examples

This folder contains the example applications for the ALBA Framework. Two demo applications are provided:

- **Alba Viewer**: An application dedicated to importing and visualizing biomedical data (e.g., DICOM files).
- **Alba Master**: A comprehensive, standalone application that extends Alba Viewer's functionalities by offering a wide range of exporters and operations for advanced data management.

## Alba Viewer

![Alba Viewer Splash](AlbaViewer/Installer/Config/AlbaViewerSplash.bmp)

Alba Viewer comes with a variety of pre-configured views including:

- **VTK View** for 3D and surface visualization.
- **Arbitrary Slice View** and **Ortho Slice View** for slicing through data.
- **RX Compound** and **Virtual RX Views** for compound visualizations.
- **Isosurface**, **Slice on Curve**, **Image**, and more.

In addition, it supports several data importers for formats such as DICOM, STL, PLY, VRML, VTK, ITK MetaImage, and others.

## Alba Master

![Alba Master Splash](AlbaMaster/Installer/Config/AlbaMasterSplash.bmp)

Alba Master includes all functionalities of Alba Viewer and extends them with additional features:

- **Exporters**: Tools for exporting data in multiple formats (ALBA, **DICOM**, STL, PLY, VTK, ITK MetaImage, RAW, BMP, Landmark, etc.).
- **Operations**: A comprehensive set of operations for data creation and modification, including volume creation, landmark management, registration, segmentation, and various editing and filtering functions.

These example applications serve as a starting point for exploring the capabilities of the ALBA Framework and can be customized to meet specific biomedical data management needs.

## Getting Started

1. Build the framework following the instructions provided in the main [ALBA Framework README](https://github.com/IOR-BIC/ALBA/blob/master/README.md).
2. Launch Alba Viewer or Alba Master from their respective directories.
3. Refer to the official documentation for detailed instructions and further customization.


*This README is part of the ALBA Framework Examples folder.*
