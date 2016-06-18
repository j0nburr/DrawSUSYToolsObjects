#ifndef DRAWSUSYTOOLSOBJECTS_DRAWSUSYTOOLSOBJECTSALG_H
#define DRAWSUSYTOOLSOBJECTS_DRAWSUSYTOOLSOBJECTSALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "SUSYTools/ISUSYObjDef_xAODTool.h"
#include "AsgTools/ToolHandle.h"


class DrawSUSYToolsObjectsAlg: public ::AthAnalysisAlgorithm { 
 public: 
  DrawSUSYToolsObjectsAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~DrawSUSYToolsObjectsAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  virtual StatusCode beginInputFile();

  StatusCode getHist(const std::string& histName, TH1*& histPtr);

 private: 
  ToolHandle<ST::ISUSYObjDef_xAODTool> m_objTool;

  bool m_doPhotonOR;
  bool m_doPhotonMET;
  bool m_outputPhotons;

  bool m_doTauOR;
  bool m_doTauMET;
  bool m_outputTaus;
}; 

#endif //> !DRAWSUSYTOOLSOBJECTS_DRAWSUSYTOOLSOBJECTSALG_H
