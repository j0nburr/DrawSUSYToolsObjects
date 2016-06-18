// DrawSUSYToolsObjects includes
#include "DrawSUSYToolsObjectsAlg.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODCore/ShallowCopy.h"

#include "xAODEventInfo/EventInfo.h"

//uncomment the line below to use the HistSvc for outputting trees and histograms
//#include "GaudiKernel/ITHistSvc.h"
//#include "TTree.h"
//#include "TH1D.h"

// Helper function to safely retrieve histograms
StatusCode DrawSUSYToolsObjectsAlg::getHist(const std::string& histName, TH1*& histPtr)
{
  histPtr = hist(histName);
  if (!histPtr) {
    ATH_MSG_ERROR( "Failed to retrieve histogram " << histName );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

DrawSUSYToolsObjectsAlg::DrawSUSYToolsObjectsAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ){

  declareProperty("SUSYTools", m_objTool);
  declareProperty("DoPhotonOR", m_doPhotonOR = true);
  declareProperty("DoPhotonMET", m_doPhotonMET = true);
  declareProperty("OutputPhotons", m_outputPhotons = true);
  declareProperty("DoTauOR", m_doTauOR = true);
  declareProperty("DoTauMET", m_doTauMET = true);
  declareProperty("OutputTaus", m_outputTaus = true);
}


DrawSUSYToolsObjectsAlg::~DrawSUSYToolsObjectsAlg() {}


StatusCode DrawSUSYToolsObjectsAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  //
  //This is called once, before the start of the event loop
  //Retrieves of tools you have configured in the joboptions go here
  //

  //HERE IS AN EXAMPLE
  //We will create a histogram and a ttree and register them to the histsvc
  //Remember to uncomment the configuration of the histsvc stream in the joboptions
  //
  //ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
  //TH1D* myHist = new TH1D("myHist","myHist",10,0,10);
  //CHECK( histSvc->regHist("/MYSTREAM/myHist", myHist) ); //registers histogram to output stream (like SetDirectory in EventLoop)
  //TTree* myTree = new TTree("myTree","myTree");
  //CHECK( histSvc->regTree("/MYSTREAM/SubDirectory/myTree", myTree) ); //registers tree to output stream (like SetDirectory in EventLoop) inside a sub-directory

  ATH_CHECK(m_objTool.retrieve() );

  ATH_CHECK( book( TH1F("h_elePt", ";Electron p_{T} [GeV]", 200, 0, 200) ) );
  ATH_CHECK( book( TH1F("h_muPt", ";Muon p_{T} [GeV]", 200, 0, 200) ) );
  ATH_CHECK( book( TH1F("h_jetPt", ";Jet p_{T} [GeV]", 400, 0, 400) ) );
  ATH_CHECK( book( TH1F("h_met", ";E_{T}^{miss} [GeV]", 400, 0, 400) ) );
  if (m_outputPhotons) ATH_CHECK( book( TH1F("h_photonPt", ";Photon p_{T} [GeV]", 200, 0, 200) ) );
  if (m_outputTaus) ATH_CHECK( book( TH1F("h_tauPt", ";Tau p_{T} [GeV]", 400, 0, 400) ) );

  if (m_outputPhotons && !m_doPhotonOR) {
    ATH_MSG_WARNING( "OutputPhotons set to true but DoPhotonOR set to false!" );
    ATH_MSG_WARNING( "Automatically setting DoPhotonOR to true!" );
    m_doPhotonOR = true;
  }
  if (m_outputTaus && !m_doTauOR) {
    ATH_MSG_WARNING( "OutputTaus set to true but DoTauOR set to false!" );
    ATH_MSG_WARNING( "Automatically setting DoTauOR to true!" );
    m_doTauOR = true;
  }

  return StatusCode::SUCCESS;
}

StatusCode DrawSUSYToolsObjectsAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //


  return StatusCode::SUCCESS;
}

StatusCode DrawSUSYToolsObjectsAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed

  xAOD::JetContainer* jets(0);
  xAOD::ShallowAuxContainer* jetsAux(0);
  ATH_CHECK( m_objTool->GetJets(jets, jetsAux, true) );

  xAOD::ElectronContainer* electrons(0);
  xAOD::ShallowAuxContainer* electronsAux(0);
  ATH_CHECK( m_objTool->GetElectrons(electrons, electronsAux, true) );

  xAOD::MuonContainer* muons(0);
  xAOD::ShallowAuxContainer* muonsAux(0);
  ATH_CHECK( m_objTool->GetMuons(muons, muonsAux, true) );

  xAOD::PhotonContainer* photons(0);
  xAOD::ShallowAuxContainer* photonsAux(0);
  if (m_doPhotonOR || m_doPhotonMET || m_outputPhotons) ATH_CHECK( m_objTool->GetPhotons(photons, photonsAux, true) );

  xAOD::TauJetContainer* taus(0);
  xAOD::ShallowAuxContainer* tausAux(0);
  if (m_doTauOR || m_doTauMET || m_outputTaus) ATH_CHECK( m_objTool->GetTaus(taus, tausAux, true) );

  xAOD::MissingETContainer* met = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* metAux = new xAOD::MissingETAuxContainer();
  met->setStore(metAux);
  ATH_CHECK( m_objTool->GetMET(*met, jets, electrons, muons, m_doPhotonMET ? photons : 0, m_doTauMET ? taus : 0 ) );
  ATH_CHECK( evtStore()->record(met, "MyMET") );
  ATH_CHECK( evtStore()->record(metAux, "MyMETAux.") );

  ATH_CHECK( m_objTool->OverlapRemoval(electrons, muons, jets, m_doPhotonOR ? photons : 0, m_doTauMET ? taus : 0) );

  const xAOD::EventInfo* evtInfo(0);
  ATH_CHECK( evtStore()->retrieve(evtInfo, "EventInfo") );
  bool isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  float eventWeight = 1.0;
  if (isMC) eventWeight = evtInfo->mcEventWeight();

  TH1* theHist(0);
  static SG::AuxElement::ConstAccessor<char> cacc_passOR("passOR");

  ATH_CHECK( getHist("h_met", theHist) );
  theHist->Fill( (*met)["FinalTrk"]->met() );

  ATH_CHECK( getHist("h_elePt", theHist) );
  for (auto iele : *electrons) if (cacc_passOR(*iele) ) theHist->Fill(iele->pt() * 0.001, eventWeight);

  ATH_CHECK( getHist("h_muPt", theHist) );
  for (auto imu : *muons) if (cacc_passOR(*imu) ) theHist->Fill(imu->pt() * 0.001, eventWeight);

  ATH_CHECK( getHist("h_jetPt", theHist) );
  for (auto ijet : *jets) if (cacc_passOR(*ijet) ) theHist->Fill(ijet->pt() * 0.001, eventWeight);

  if (m_outputPhotons) {
    ATH_CHECK( getHist("h_photonPt", theHist) );
    for (auto iph : *photons) if (cacc_passOR(*iph) ) theHist->Fill(iph->pt() * 0.001, eventWeight);
  }

  if (m_outputTaus) {
    ATH_CHECK( getHist("h_tauPt", theHist) );
    for (auto itau : *taus) if (cacc_passOR(*itau) ) theHist->Fill(itau->pt() * 0.001, eventWeight);
  }

  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode DrawSUSYToolsObjectsAlg::beginInputFile() { 
  //
  //This method is called at the start of each input file, even if
  //the input file contains no events. Accumulate metadata information here
  //

  //example of retrieval of CutBookkeepers: (remember you will need to include the necessary header files and use statements in requirements file)
  // const xAOD::CutBookkeeperContainer* bks = 0;
  // CHECK( inputMetaStore()->retrieve(bks, "CutBookkeepers") );

  //example of IOVMetaData retrieval (see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_access_file_metadata_in_C)
  //float beamEnergy(0); CHECK( retrieveMetadata("/TagInfo","beam_energy",beamEnergy) );
  //std::vector<float> bunchPattern; CHECK( retrieveMetadata("/Digitiation/Parameters","BeamIntensityPattern",bunchPattern) );



  return StatusCode::SUCCESS;
}


