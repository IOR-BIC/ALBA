SUBDIRS(exInteraction)
SUBDIRS(exOperations)
SUBDIRS(exVMEMan)
SUBDIRS(exViewVTK)

IF (EXISTS ${MAF_SOURCE_DIR}/Examples/exRuler)
  SUBDIRS(exRuler)
ENDIF (EXISTS ${MAF_SOURCE_DIR}/Examples/exRuler)

IF (EXISTS ${MAF_SOURCE_DIR}/Examples/exHistogram)
  SUBDIRS(exHistogram)
ENDIF (EXISTS ${MAF_SOURCE_DIR}/Examples/exHistogram)

IF (EXISTS ${MAF_SOURCE_DIR}/Examples/exScalars)
  SUBDIRS(exScalars)
ENDIF (EXISTS ${MAF_SOURCE_DIR}/Examples/exScalars)

IF (EXISTS ${MAF_SOURCE_DIR}/Examples/exVectorFlow)
  SUBDIRS(exVectorFlow)
ENDIF (EXISTS ${MAF_SOURCE_DIR}/Examples/exVectorFlow)
