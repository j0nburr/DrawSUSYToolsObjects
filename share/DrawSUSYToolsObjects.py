#Skeleton joboption for a simple analysis job

# theApp.EvtMax=10                                         #says how many events to run over. Set to -1 for all events

import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of POOL files (e.g. xAODs)
# svcMgr.EventSelector.InputCollections=["/data/atlas/atlasdata3/burr/xAOD/testFiles/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r7380_r6282_tid07626244_00/AOD.07626244._000001.pool.root.1"]   #insert your list of input files here
svcMgr.EventSelector.InputCollections=["/data/atlas/atlasdata3/burr/xAOD/testFiles/mc15cttbar/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r7725_r7676/AOD.07915933._001658.pool.root.1"]   #insert your list of input files here

from PyUtils import AthFile
af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
af.fileinfos    #this is a dict of dicts
isMC = 'IS_SIMULATION' in af.fileinfos['evt_type']
beam_energy = af.fileinfos['beam_energy']
conditions_tag = af.fileinfos['conditions_tag']
if isMC:
    isFullSim = af.fileinfos['metadata']['/Simulation/Parameters']['SimulationFlavour']=='default'

# isData     = 0:  data in ApplySUSYTools

#            = 1:  MC in ApplySUSYTools

# dataSource = 0:  data in ST__SUSYObjDef_xAOD

# dataSource = 1:  fullsim in ST__SUSYObjDef_xAOD

# dataSource = 2:  AtlFastII in ST__SUSYObjDef_xAOD


if isMC:  
    isData = 0
    if isFullSim: 
        dataSource = 1
    else:
        dataSource = 2
else:
    isData = 1
    dataSource = 0


print dataSource

ToolSvc += CfgMgr.TauAnalysisTools__TauSmearingTool("TauSmearingTool",
                                                    SkipTruthMatchCheck = True)


ToolSvc += CfgMgr.ST__SUSYObjDef_xAOD("ObjTool",
                                      ConfigFile = "SUSYTools/SUSYTools_Default.conf",
                                      PRWConfigFiles = ["merged_prw_mc15c.root"],
                                      PRWLumiCalcFiles = ["ilumicalc_histograms_None_276262-284484.root"],
                                      TauSmearingTool = ToolSvc.TauSmearingTool,
                                      DataSource = dataSource)


algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
algseq += CfgMgr.DrawSUSYToolsObjectsAlg("DrawObj",
                                         SUSYTools = ToolSvc.ObjTool,
                                         RootStreamName = "HIST")                                 #adds an instance of your alg to it



##--------------------------------------------------------------------
## This section shows up to set up a HistSvc output stream for outputing histograms and trees
## See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_output_trees_and_histogra for more details and examples

if not hasattr(svcMgr, 'THistSvc'): svcMgr += CfgMgr.THistSvc() #only add the histogram service if not already present (will be the case in this jobo)
svcMgr.THistSvc.Output += ["HIST DATAFILE='hist-out.root' OPT='RECREATE'"] #add an output root file stream

##--------------------------------------------------------------------


##--------------------------------------------------------------------
## The lines below are an example of how to create an output xAOD
## See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_create_an_output_xAOD for more details and examples

#from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
#xaodStream = MSMgr.NewPoolRootStream( "StreamXAOD", "xAOD.out.root" )

##EXAMPLE OF BASIC ADDITION OF EVENT AND METADATA ITEMS
##AddItem and AddMetaDataItem methods accept either string or list of strings
#xaodStream.AddItem( ["xAOD::JetContainer#*","xAOD::JetAuxContainer#*"] ) #Keeps all JetContainers (and their aux stores)
#xaodStream.AddMetaDataItem( ["xAOD::TriggerMenuContainer#*","xAOD::TriggerMenuAuxContainer#*"] )
#ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool("TriggerMenuMetaDataTool") #MetaDataItems needs their corresponding MetaDataTool
#svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ] #Add the tool to the MetaDataSvc to ensure it is loaded

##EXAMPLE OF SLIMMING (keeping parts of the aux store)
#xaodStream.AddItem( ["xAOD::ElectronContainer#Electrons","xAOD::ElectronAuxContainer#ElectronsAux.pt.eta.phi"] ) #example of slimming: only keep pt,eta,phi auxdata of electrons

##EXAMPLE OF SKIMMING (keeping specific events)
#xaodStream.AddAcceptAlgs( "DrawSUSYToolsObjects" ) #will only keep events where 'setFilterPassed(false)' has NOT been called in the given algorithm

##--------------------------------------------------------------------


include("AthAnalysisBaseComps/SuppressLogging.py")              #Optional include to suppress as much athena output as possible. Keep at bottom of joboptions so that it doesn't suppress the logging of the things you have configured above

