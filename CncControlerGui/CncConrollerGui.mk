##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## release2
ProjectName            :=CncConrollerGui
ConfigurationName      :=release2
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=23/09/2018
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=C:/@Development/Compilers/TDM-GCC-64/bin/g++.exe
SharedObjectLinkerName :=C:/@Development/Compilers/TDM-GCC-64/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG $(PreprocessorSwitch)APP_USE_SPLASH 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="CncConrollerGui.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := $(shell wx-config --rcflags) -D DEBUG 
RcCompilerName         :=C:/@Development/Compilers/TDM-GCC-64/bin/windres.exe
LinkOptions            :=  -mwindows $(shell wx-config --libs)
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)C:\@Development\boost\include\boost-1_64 $(IncludeSwitch). $(IncludeSwitch)C:\@Development\boost_1_65_0 $(IncludeSwitch)C:\@Development\freeglut\include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)wxmsw31u_stc $(LibrarySwitch)wxmsw31u_webview $(LibrarySwitch)wxmsw31u_propgrid $(LibrarySwitch)wxmsw31u_adv $(LibrarySwitch)wxmsw31u_gl $(LibrarySwitch)opengl32 $(LibrarySwitch)glu32 $(LibrarySwitch)freeglut 
ArLibs                 :=  "libwxmsw31u_stc" "libwxmsw31u_webview" "libwxmsw31u_propgrid" "libwxmsw31u_adv" "libwxmsw31u_gl" "opengl32" "glu32" "libfreeglut" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)C:\@Development\boost\lib $(LibraryPathSwitch)C:\@Development\freeglut\lib\x64 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/@Development/Compilers/TDM-GCC-64/bin/ar.exe rcu
CXX      := C:/@Development/Compilers/TDM-GCC-64/bin/g++.exe
CC       := C:/@Development/Compilers/TDM-GCC-64/bin/gcc.exe
CXXFLAGS := -Wno-deprecated-declarations -O3 -std=c++17 -std=c++14 -std=c++11 -Wall $(shell wx-config --cflags) $(Preprocessors)
CFLAGS   :=  -O3 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/@Development/Compilers/TDM-GCC-64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
WXWIN:=C:\@Development\wxWidgets-3.1.0
WXCFG:=gcc_dll\mswu
Objects0=$(IntermediateDirectory)/SvgGeneratorBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/PathGenerators_PGenPolygon.cpp$(ObjectSuffix) $(IntermediateDirectory)/PathGeneratorBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/SvgPathString.cpp$(ObjectSuffix) $(IntermediateDirectory)/CfgGeneralPage.cpp$(ObjectSuffix) $(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(ObjectSuffix) $(IntermediateDirectory)/SVGCurveLib.cpp$(ObjectSuffix) $(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(ObjectSuffix) $(IntermediateDirectory)/SVGNodeParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/SVGFileParser.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(ObjectSuffix) $(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(ObjectSuffix) $(IntermediateDirectory)/SvgEditPopup.cpp$(ObjectSuffix) $(IntermediateDirectory)/SvgPathGroup.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialSimulatorBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialEmulatorNull.cpp$(ObjectSuffix) $(IntermediateDirectory)/OSD_SerialMsw.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncFilePreviewWnd.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncToolMagazine.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPathListManager.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLContextBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncFilePreview.cpp$(ObjectSuffix) $(IntermediateDirectory)/GCodePathHandlerBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncControllerTestSuite.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncNumberFormatter.cpp$(ObjectSuffix) $(IntermediateDirectory)/PathHandlerBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/PathGeneratorFrame.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSpeedView.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncArduino.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/SvgPathFragment.cpp$(ObjectSuffix) $(IntermediateDirectory)/CfgWorkingPage.cpp$(ObjectSuffix) $(IntermediateDirectory)/CfgEnvironmentPage.cpp$(ObjectSuffix) $(IntermediateDirectory)/GCodeFileParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncFileNameService.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncClipperWrapper.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSpeedSimulator.cpp$(ObjectSuffix) $(IntermediateDirectory)/Clipper_clipper.cpp$(ObjectSuffix) 

Objects1=$(IntermediateDirectory)/SvgUnitCalculator.cpp$(ObjectSuffix) $(IntermediateDirectory)/PathGeneratorStore.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CfgSVGOptionPage.cpp$(ObjectSuffix) $(IntermediateDirectory)/OSD_SerialGtk.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLContextPathBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_CncGCodePreview.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialEmulatorFile.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLContextTestCube.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncMotionMonitor.cpp$(ObjectSuffix) $(IntermediateDirectory)/win_resources.rc$(ObjectSuffix) $(IntermediateDirectory)/CncToolStateControl.cpp$(ObjectSuffix) $(IntermediateDirectory)/GCodePathHandlerGL.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncStreamBuffers.cpp$(ObjectSuffix) $(IntermediateDirectory)/GlobalFunctions.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncFileView.cpp$(ObjectSuffix) $(IntermediateDirectory)/GL3DOptions.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialSpyPort.cpp$(ObjectSuffix) $(IntermediateDirectory)/PgPropertyHelpInfos.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncConfigProperty.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialPort.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncCommon.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialSimulatorFacade.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncPerspective.cpp$(ObjectSuffix) $(IntermediateDirectory)/CfgApplicationPage.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncZView.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncVectiesListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxcrafter.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSummaryListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/MainFrame.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/UpdateManagerThread.cpp$(ObjectSuffix) $(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/DataControlModel.cpp$(ObjectSuffix) $(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(ObjectSuffix) $(IntermediateDirectory)/UnitTestFrame.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSpyControl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncConfig.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(ObjectSuffix) $(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/SVGElementConverter.cpp$(ObjectSuffix) $(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(ObjectSuffix) $(IntermediateDirectory)/GCodeField.cpp$(ObjectSuffix) 

Objects2=$(IntermediateDirectory)/SerialSimulatorDevNull.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSetterListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/FileParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/Tests_Test_Base.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialEmulatorSVG.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncToolCorrection.cpp$(ObjectSuffix) $(IntermediateDirectory)/GCodeBlock.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/GCodeCommands.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSvgCurveLib.cpp$(ObjectSuffix) $(IntermediateDirectory)/ManuallyParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLViewPort.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLContextCncPath.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncControl.cpp$(ObjectSuffix) 



Objects=$(Objects0) $(Objects1) $(Objects2) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	@echo $(Objects1) >> $(ObjectsFileList)
	@echo $(Objects2) >> $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@$(MakeDirCommand) "./Release"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Release"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/SvgGeneratorBase.cpp$(ObjectSuffix): SvgGeneratorBase.cpp $(IntermediateDirectory)/SvgGeneratorBase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SvgGeneratorBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SvgGeneratorBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SvgGeneratorBase.cpp$(DependSuffix): SvgGeneratorBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SvgGeneratorBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SvgGeneratorBase.cpp$(DependSuffix) -MM SvgGeneratorBase.cpp

$(IntermediateDirectory)/SvgGeneratorBase.cpp$(PreprocessSuffix): SvgGeneratorBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SvgGeneratorBase.cpp$(PreprocessSuffix) SvgGeneratorBase.cpp

$(IntermediateDirectory)/PathGenerators_PGenPolygon.cpp$(ObjectSuffix): PathGenerators/PGenPolygon.cpp $(IntermediateDirectory)/PathGenerators_PGenPolygon.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/PathGenerators/PGenPolygon.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PathGenerators_PGenPolygon.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PathGenerators_PGenPolygon.cpp$(DependSuffix): PathGenerators/PGenPolygon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PathGenerators_PGenPolygon.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PathGenerators_PGenPolygon.cpp$(DependSuffix) -MM PathGenerators/PGenPolygon.cpp

$(IntermediateDirectory)/PathGenerators_PGenPolygon.cpp$(PreprocessSuffix): PathGenerators/PGenPolygon.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PathGenerators_PGenPolygon.cpp$(PreprocessSuffix) PathGenerators/PGenPolygon.cpp

$(IntermediateDirectory)/PathGeneratorBase.cpp$(ObjectSuffix): PathGeneratorBase.cpp $(IntermediateDirectory)/PathGeneratorBase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/PathGeneratorBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PathGeneratorBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PathGeneratorBase.cpp$(DependSuffix): PathGeneratorBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PathGeneratorBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PathGeneratorBase.cpp$(DependSuffix) -MM PathGeneratorBase.cpp

$(IntermediateDirectory)/PathGeneratorBase.cpp$(PreprocessSuffix): PathGeneratorBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PathGeneratorBase.cpp$(PreprocessSuffix) PathGeneratorBase.cpp

$(IntermediateDirectory)/SvgPathString.cpp$(ObjectSuffix): SvgPathString.cpp $(IntermediateDirectory)/SvgPathString.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SvgPathString.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SvgPathString.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SvgPathString.cpp$(DependSuffix): SvgPathString.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SvgPathString.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SvgPathString.cpp$(DependSuffix) -MM SvgPathString.cpp

$(IntermediateDirectory)/SvgPathString.cpp$(PreprocessSuffix): SvgPathString.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SvgPathString.cpp$(PreprocessSuffix) SvgPathString.cpp

$(IntermediateDirectory)/CfgGeneralPage.cpp$(ObjectSuffix): CfgGeneralPage.cpp $(IntermediateDirectory)/CfgGeneralPage.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CfgGeneralPage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CfgGeneralPage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CfgGeneralPage.cpp$(DependSuffix): CfgGeneralPage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CfgGeneralPage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CfgGeneralPage.cpp$(DependSuffix) -MM CfgGeneralPage.cpp

$(IntermediateDirectory)/CfgGeneralPage.cpp$(PreprocessSuffix): CfgGeneralPage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CfgGeneralPage.cpp$(PreprocessSuffix) CfgGeneralPage.cpp

$(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(ObjectSuffix): CfgGCodeOptionPage.cpp $(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CfgGCodeOptionPage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(DependSuffix): CfgGCodeOptionPage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(DependSuffix) -MM CfgGCodeOptionPage.cpp

$(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(PreprocessSuffix): CfgGCodeOptionPage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(PreprocessSuffix) CfgGCodeOptionPage.cpp

$(IntermediateDirectory)/SVGCurveLib.cpp$(ObjectSuffix): SVGCurveLib.cpp $(IntermediateDirectory)/SVGCurveLib.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGCurveLib.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGCurveLib.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGCurveLib.cpp$(DependSuffix): SVGCurveLib.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGCurveLib.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGCurveLib.cpp$(DependSuffix) -MM SVGCurveLib.cpp

$(IntermediateDirectory)/SVGCurveLib.cpp$(PreprocessSuffix): SVGCurveLib.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGCurveLib.cpp$(PreprocessSuffix) SVGCurveLib.cpp

$(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(ObjectSuffix): SVGPathHandlerCnc.cpp $(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGPathHandlerCnc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(DependSuffix): SVGPathHandlerCnc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(DependSuffix) -MM SVGPathHandlerCnc.cpp

$(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(PreprocessSuffix): SVGPathHandlerCnc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(PreprocessSuffix) SVGPathHandlerCnc.cpp

$(IntermediateDirectory)/SVGNodeParser.cpp$(ObjectSuffix): SVGNodeParser.cpp $(IntermediateDirectory)/SVGNodeParser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGNodeParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGNodeParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGNodeParser.cpp$(DependSuffix): SVGNodeParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGNodeParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGNodeParser.cpp$(DependSuffix) -MM SVGNodeParser.cpp

$(IntermediateDirectory)/SVGNodeParser.cpp$(PreprocessSuffix): SVGNodeParser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGNodeParser.cpp$(PreprocessSuffix) SVGNodeParser.cpp

$(IntermediateDirectory)/SVGFileParser.cpp$(ObjectSuffix): SVGFileParser.cpp $(IntermediateDirectory)/SVGFileParser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGFileParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGFileParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGFileParser.cpp$(DependSuffix): SVGFileParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGFileParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGFileParser.cpp$(DependSuffix) -MM SVGFileParser.cpp

$(IntermediateDirectory)/SVGFileParser.cpp$(PreprocessSuffix): SVGFileParser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGFileParser.cpp$(PreprocessSuffix) SVGFileParser.cpp

$(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(ObjectSuffix): OSD/CncTimeFunctions.cpp $(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/OSD/CncTimeFunctions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(DependSuffix): OSD/CncTimeFunctions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(DependSuffix) -MM OSD/CncTimeFunctions.cpp

$(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(PreprocessSuffix): OSD/CncTimeFunctions.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(PreprocessSuffix) OSD/CncTimeFunctions.cpp

$(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(ObjectSuffix): OSD/CncUsbPortScanner.cpp $(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/OSD/CncUsbPortScanner.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(DependSuffix): OSD/CncUsbPortScanner.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(DependSuffix) -MM OSD/CncUsbPortScanner.cpp

$(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(PreprocessSuffix): OSD/CncUsbPortScanner.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(PreprocessSuffix) OSD/CncUsbPortScanner.cpp

$(IntermediateDirectory)/SvgEditPopup.cpp$(ObjectSuffix): SvgEditPopup.cpp $(IntermediateDirectory)/SvgEditPopup.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SvgEditPopup.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SvgEditPopup.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SvgEditPopup.cpp$(DependSuffix): SvgEditPopup.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SvgEditPopup.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SvgEditPopup.cpp$(DependSuffix) -MM SvgEditPopup.cpp

$(IntermediateDirectory)/SvgEditPopup.cpp$(PreprocessSuffix): SvgEditPopup.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SvgEditPopup.cpp$(PreprocessSuffix) SvgEditPopup.cpp

$(IntermediateDirectory)/SvgPathGroup.cpp$(ObjectSuffix): SvgPathGroup.cpp $(IntermediateDirectory)/SvgPathGroup.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SvgPathGroup.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SvgPathGroup.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SvgPathGroup.cpp$(DependSuffix): SvgPathGroup.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SvgPathGroup.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SvgPathGroup.cpp$(DependSuffix) -MM SvgPathGroup.cpp

$(IntermediateDirectory)/SvgPathGroup.cpp$(PreprocessSuffix): SvgPathGroup.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SvgPathGroup.cpp$(PreprocessSuffix) SvgPathGroup.cpp

$(IntermediateDirectory)/SerialSimulatorBase.cpp$(ObjectSuffix): SerialSimulatorBase.cpp $(IntermediateDirectory)/SerialSimulatorBase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialSimulatorBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialSimulatorBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialSimulatorBase.cpp$(DependSuffix): SerialSimulatorBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialSimulatorBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialSimulatorBase.cpp$(DependSuffix) -MM SerialSimulatorBase.cpp

$(IntermediateDirectory)/SerialSimulatorBase.cpp$(PreprocessSuffix): SerialSimulatorBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialSimulatorBase.cpp$(PreprocessSuffix) SerialSimulatorBase.cpp

$(IntermediateDirectory)/SerialEmulatorNull.cpp$(ObjectSuffix): SerialEmulatorNull.cpp $(IntermediateDirectory)/SerialEmulatorNull.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialEmulatorNull.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialEmulatorNull.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialEmulatorNull.cpp$(DependSuffix): SerialEmulatorNull.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialEmulatorNull.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialEmulatorNull.cpp$(DependSuffix) -MM SerialEmulatorNull.cpp

$(IntermediateDirectory)/SerialEmulatorNull.cpp$(PreprocessSuffix): SerialEmulatorNull.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialEmulatorNull.cpp$(PreprocessSuffix) SerialEmulatorNull.cpp

$(IntermediateDirectory)/OSD_SerialMsw.cpp$(ObjectSuffix): OSD/SerialMsw.cpp $(IntermediateDirectory)/OSD_SerialMsw.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/OSD/SerialMsw.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OSD_SerialMsw.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OSD_SerialMsw.cpp$(DependSuffix): OSD/SerialMsw.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OSD_SerialMsw.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OSD_SerialMsw.cpp$(DependSuffix) -MM OSD/SerialMsw.cpp

$(IntermediateDirectory)/OSD_SerialMsw.cpp$(PreprocessSuffix): OSD/SerialMsw.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OSD_SerialMsw.cpp$(PreprocessSuffix) OSD/SerialMsw.cpp

$(IntermediateDirectory)/CncFilePreviewWnd.cpp$(ObjectSuffix): CncFilePreviewWnd.cpp $(IntermediateDirectory)/CncFilePreviewWnd.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncFilePreviewWnd.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncFilePreviewWnd.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncFilePreviewWnd.cpp$(DependSuffix): CncFilePreviewWnd.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncFilePreviewWnd.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncFilePreviewWnd.cpp$(DependSuffix) -MM CncFilePreviewWnd.cpp

$(IntermediateDirectory)/CncFilePreviewWnd.cpp$(PreprocessSuffix): CncFilePreviewWnd.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncFilePreviewWnd.cpp$(PreprocessSuffix) CncFilePreviewWnd.cpp

$(IntermediateDirectory)/CncToolMagazine.cpp$(ObjectSuffix): CncToolMagazine.cpp $(IntermediateDirectory)/CncToolMagazine.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncToolMagazine.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncToolMagazine.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncToolMagazine.cpp$(DependSuffix): CncToolMagazine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncToolMagazine.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncToolMagazine.cpp$(DependSuffix) -MM CncToolMagazine.cpp

$(IntermediateDirectory)/CncToolMagazine.cpp$(PreprocessSuffix): CncToolMagazine.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncToolMagazine.cpp$(PreprocessSuffix) CncToolMagazine.cpp

$(IntermediateDirectory)/CncPathListManager.cpp$(ObjectSuffix): CncPathListManager.cpp $(IntermediateDirectory)/CncPathListManager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPathListManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPathListManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPathListManager.cpp$(DependSuffix): CncPathListManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPathListManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPathListManager.cpp$(DependSuffix) -MM CncPathListManager.cpp

$(IntermediateDirectory)/CncPathListManager.cpp$(PreprocessSuffix): CncPathListManager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPathListManager.cpp$(PreprocessSuffix) CncPathListManager.cpp

$(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(ObjectSuffix): CncStatisticSummaryListCtrl.cpp $(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncStatisticSummaryListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(DependSuffix): CncStatisticSummaryListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(DependSuffix) -MM CncStatisticSummaryListCtrl.cpp

$(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(PreprocessSuffix): CncStatisticSummaryListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(PreprocessSuffix) CncStatisticSummaryListCtrl.cpp

$(IntermediateDirectory)/3D_GLContextBase.cpp$(ObjectSuffix): 3D/GLContextBase.cpp $(IntermediateDirectory)/3D_GLContextBase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLContextBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLContextBase.cpp$(DependSuffix): 3D/GLContextBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLContextBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLContextBase.cpp$(DependSuffix) -MM 3D/GLContextBase.cpp

$(IntermediateDirectory)/3D_GLContextBase.cpp$(PreprocessSuffix): 3D/GLContextBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLContextBase.cpp$(PreprocessSuffix) 3D/GLContextBase.cpp

$(IntermediateDirectory)/CncFilePreview.cpp$(ObjectSuffix): CncFilePreview.cpp $(IntermediateDirectory)/CncFilePreview.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncFilePreview.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncFilePreview.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncFilePreview.cpp$(DependSuffix): CncFilePreview.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncFilePreview.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncFilePreview.cpp$(DependSuffix) -MM CncFilePreview.cpp

$(IntermediateDirectory)/CncFilePreview.cpp$(PreprocessSuffix): CncFilePreview.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncFilePreview.cpp$(PreprocessSuffix) CncFilePreview.cpp

$(IntermediateDirectory)/GCodePathHandlerBase.cpp$(ObjectSuffix): GCodePathHandlerBase.cpp $(IntermediateDirectory)/GCodePathHandlerBase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodePathHandlerBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodePathHandlerBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodePathHandlerBase.cpp$(DependSuffix): GCodePathHandlerBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodePathHandlerBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodePathHandlerBase.cpp$(DependSuffix) -MM GCodePathHandlerBase.cpp

$(IntermediateDirectory)/GCodePathHandlerBase.cpp$(PreprocessSuffix): GCodePathHandlerBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodePathHandlerBase.cpp$(PreprocessSuffix) GCodePathHandlerBase.cpp

$(IntermediateDirectory)/CncControllerTestSuite.cpp$(ObjectSuffix): CncControllerTestSuite.cpp $(IntermediateDirectory)/CncControllerTestSuite.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncControllerTestSuite.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncControllerTestSuite.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncControllerTestSuite.cpp$(DependSuffix): CncControllerTestSuite.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncControllerTestSuite.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncControllerTestSuite.cpp$(DependSuffix) -MM CncControllerTestSuite.cpp

$(IntermediateDirectory)/CncControllerTestSuite.cpp$(PreprocessSuffix): CncControllerTestSuite.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncControllerTestSuite.cpp$(PreprocessSuffix) CncControllerTestSuite.cpp

$(IntermediateDirectory)/CncNumberFormatter.cpp$(ObjectSuffix): CncNumberFormatter.cpp $(IntermediateDirectory)/CncNumberFormatter.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncNumberFormatter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncNumberFormatter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncNumberFormatter.cpp$(DependSuffix): CncNumberFormatter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncNumberFormatter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncNumberFormatter.cpp$(DependSuffix) -MM CncNumberFormatter.cpp

$(IntermediateDirectory)/CncNumberFormatter.cpp$(PreprocessSuffix): CncNumberFormatter.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncNumberFormatter.cpp$(PreprocessSuffix) CncNumberFormatter.cpp

$(IntermediateDirectory)/PathHandlerBase.cpp$(ObjectSuffix): PathHandlerBase.cpp $(IntermediateDirectory)/PathHandlerBase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/PathHandlerBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PathHandlerBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PathHandlerBase.cpp$(DependSuffix): PathHandlerBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PathHandlerBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PathHandlerBase.cpp$(DependSuffix) -MM PathHandlerBase.cpp

$(IntermediateDirectory)/PathHandlerBase.cpp$(PreprocessSuffix): PathHandlerBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PathHandlerBase.cpp$(PreprocessSuffix) PathHandlerBase.cpp

$(IntermediateDirectory)/PathGeneratorFrame.cpp$(ObjectSuffix): PathGeneratorFrame.cpp $(IntermediateDirectory)/PathGeneratorFrame.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/PathGeneratorFrame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PathGeneratorFrame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PathGeneratorFrame.cpp$(DependSuffix): PathGeneratorFrame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PathGeneratorFrame.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PathGeneratorFrame.cpp$(DependSuffix) -MM PathGeneratorFrame.cpp

$(IntermediateDirectory)/PathGeneratorFrame.cpp$(PreprocessSuffix): PathGeneratorFrame.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PathGeneratorFrame.cpp$(PreprocessSuffix) PathGeneratorFrame.cpp

$(IntermediateDirectory)/CncSpeedView.cpp$(ObjectSuffix): CncSpeedView.cpp $(IntermediateDirectory)/CncSpeedView.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSpeedView.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSpeedView.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSpeedView.cpp$(DependSuffix): CncSpeedView.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSpeedView.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSpeedView.cpp$(DependSuffix) -MM CncSpeedView.cpp

$(IntermediateDirectory)/CncSpeedView.cpp$(PreprocessSuffix): CncSpeedView.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSpeedView.cpp$(PreprocessSuffix) CncSpeedView.cpp

$(IntermediateDirectory)/CncArduino.cpp$(ObjectSuffix): CncArduino.cpp $(IntermediateDirectory)/CncArduino.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncArduino.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncArduino.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncArduino.cpp$(DependSuffix): CncArduino.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncArduino.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncArduino.cpp$(DependSuffix) -MM CncArduino.cpp

$(IntermediateDirectory)/CncArduino.cpp$(PreprocessSuffix): CncArduino.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncArduino.cpp$(PreprocessSuffix) CncArduino.cpp

$(IntermediateDirectory)/SvgPathFragment.cpp$(ObjectSuffix): SvgPathFragment.cpp $(IntermediateDirectory)/SvgPathFragment.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SvgPathFragment.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SvgPathFragment.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SvgPathFragment.cpp$(DependSuffix): SvgPathFragment.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SvgPathFragment.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SvgPathFragment.cpp$(DependSuffix) -MM SvgPathFragment.cpp

$(IntermediateDirectory)/SvgPathFragment.cpp$(PreprocessSuffix): SvgPathFragment.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SvgPathFragment.cpp$(PreprocessSuffix) SvgPathFragment.cpp

$(IntermediateDirectory)/CfgWorkingPage.cpp$(ObjectSuffix): CfgWorkingPage.cpp $(IntermediateDirectory)/CfgWorkingPage.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CfgWorkingPage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CfgWorkingPage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CfgWorkingPage.cpp$(DependSuffix): CfgWorkingPage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CfgWorkingPage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CfgWorkingPage.cpp$(DependSuffix) -MM CfgWorkingPage.cpp

$(IntermediateDirectory)/CfgWorkingPage.cpp$(PreprocessSuffix): CfgWorkingPage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CfgWorkingPage.cpp$(PreprocessSuffix) CfgWorkingPage.cpp

$(IntermediateDirectory)/CfgEnvironmentPage.cpp$(ObjectSuffix): CfgEnvironmentPage.cpp $(IntermediateDirectory)/CfgEnvironmentPage.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CfgEnvironmentPage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CfgEnvironmentPage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CfgEnvironmentPage.cpp$(DependSuffix): CfgEnvironmentPage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CfgEnvironmentPage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CfgEnvironmentPage.cpp$(DependSuffix) -MM CfgEnvironmentPage.cpp

$(IntermediateDirectory)/CfgEnvironmentPage.cpp$(PreprocessSuffix): CfgEnvironmentPage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CfgEnvironmentPage.cpp$(PreprocessSuffix) CfgEnvironmentPage.cpp

$(IntermediateDirectory)/GCodeFileParser.cpp$(ObjectSuffix): GCodeFileParser.cpp $(IntermediateDirectory)/GCodeFileParser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodeFileParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodeFileParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodeFileParser.cpp$(DependSuffix): GCodeFileParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodeFileParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodeFileParser.cpp$(DependSuffix) -MM GCodeFileParser.cpp

$(IntermediateDirectory)/GCodeFileParser.cpp$(PreprocessSuffix): GCodeFileParser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodeFileParser.cpp$(PreprocessSuffix) GCodeFileParser.cpp

$(IntermediateDirectory)/CncFileNameService.cpp$(ObjectSuffix): CncFileNameService.cpp $(IntermediateDirectory)/CncFileNameService.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncFileNameService.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncFileNameService.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncFileNameService.cpp$(DependSuffix): CncFileNameService.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncFileNameService.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncFileNameService.cpp$(DependSuffix) -MM CncFileNameService.cpp

$(IntermediateDirectory)/CncFileNameService.cpp$(PreprocessSuffix): CncFileNameService.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncFileNameService.cpp$(PreprocessSuffix) CncFileNameService.cpp

$(IntermediateDirectory)/CncClipperWrapper.cpp$(ObjectSuffix): CncClipperWrapper.cpp $(IntermediateDirectory)/CncClipperWrapper.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncClipperWrapper.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncClipperWrapper.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncClipperWrapper.cpp$(DependSuffix): CncClipperWrapper.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncClipperWrapper.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncClipperWrapper.cpp$(DependSuffix) -MM CncClipperWrapper.cpp

$(IntermediateDirectory)/CncClipperWrapper.cpp$(PreprocessSuffix): CncClipperWrapper.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncClipperWrapper.cpp$(PreprocessSuffix) CncClipperWrapper.cpp

$(IntermediateDirectory)/CncSpeedSimulator.cpp$(ObjectSuffix): CncSpeedSimulator.cpp $(IntermediateDirectory)/CncSpeedSimulator.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSpeedSimulator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSpeedSimulator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSpeedSimulator.cpp$(DependSuffix): CncSpeedSimulator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSpeedSimulator.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSpeedSimulator.cpp$(DependSuffix) -MM CncSpeedSimulator.cpp

$(IntermediateDirectory)/CncSpeedSimulator.cpp$(PreprocessSuffix): CncSpeedSimulator.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSpeedSimulator.cpp$(PreprocessSuffix) CncSpeedSimulator.cpp

$(IntermediateDirectory)/Clipper_clipper.cpp$(ObjectSuffix): Clipper/clipper.cpp $(IntermediateDirectory)/Clipper_clipper.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/Clipper/clipper.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Clipper_clipper.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Clipper_clipper.cpp$(DependSuffix): Clipper/clipper.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Clipper_clipper.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Clipper_clipper.cpp$(DependSuffix) -MM Clipper/clipper.cpp

$(IntermediateDirectory)/Clipper_clipper.cpp$(PreprocessSuffix): Clipper/clipper.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Clipper_clipper.cpp$(PreprocessSuffix) Clipper/clipper.cpp

$(IntermediateDirectory)/SvgUnitCalculator.cpp$(ObjectSuffix): SvgUnitCalculator.cpp $(IntermediateDirectory)/SvgUnitCalculator.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SvgUnitCalculator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SvgUnitCalculator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SvgUnitCalculator.cpp$(DependSuffix): SvgUnitCalculator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SvgUnitCalculator.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SvgUnitCalculator.cpp$(DependSuffix) -MM SvgUnitCalculator.cpp

$(IntermediateDirectory)/SvgUnitCalculator.cpp$(PreprocessSuffix): SvgUnitCalculator.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SvgUnitCalculator.cpp$(PreprocessSuffix) SvgUnitCalculator.cpp

$(IntermediateDirectory)/PathGeneratorStore.cpp$(ObjectSuffix): PathGeneratorStore.cpp $(IntermediateDirectory)/PathGeneratorStore.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/PathGeneratorStore.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PathGeneratorStore.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PathGeneratorStore.cpp$(DependSuffix): PathGeneratorStore.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PathGeneratorStore.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PathGeneratorStore.cpp$(DependSuffix) -MM PathGeneratorStore.cpp

$(IntermediateDirectory)/PathGeneratorStore.cpp$(PreprocessSuffix): PathGeneratorStore.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PathGeneratorStore.cpp$(PreprocessSuffix) PathGeneratorStore.cpp

$(IntermediateDirectory)/CfgSVGOptionPage.cpp$(ObjectSuffix): CfgSVGOptionPage.cpp $(IntermediateDirectory)/CfgSVGOptionPage.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CfgSVGOptionPage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CfgSVGOptionPage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CfgSVGOptionPage.cpp$(DependSuffix): CfgSVGOptionPage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CfgSVGOptionPage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CfgSVGOptionPage.cpp$(DependSuffix) -MM CfgSVGOptionPage.cpp

$(IntermediateDirectory)/CfgSVGOptionPage.cpp$(PreprocessSuffix): CfgSVGOptionPage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CfgSVGOptionPage.cpp$(PreprocessSuffix) CfgSVGOptionPage.cpp

$(IntermediateDirectory)/OSD_SerialGtk.cpp$(ObjectSuffix): OSD/SerialGtk.cpp $(IntermediateDirectory)/OSD_SerialGtk.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/OSD/SerialGtk.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OSD_SerialGtk.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OSD_SerialGtk.cpp$(DependSuffix): OSD/SerialGtk.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OSD_SerialGtk.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OSD_SerialGtk.cpp$(DependSuffix) -MM OSD/SerialGtk.cpp

$(IntermediateDirectory)/OSD_SerialGtk.cpp$(PreprocessSuffix): OSD/SerialGtk.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OSD_SerialGtk.cpp$(PreprocessSuffix) OSD/SerialGtk.cpp

$(IntermediateDirectory)/3D_GLContextPathBase.cpp$(ObjectSuffix): 3D/GLContextPathBase.cpp $(IntermediateDirectory)/3D_GLContextPathBase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextPathBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLContextPathBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLContextPathBase.cpp$(DependSuffix): 3D/GLContextPathBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLContextPathBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLContextPathBase.cpp$(DependSuffix) -MM 3D/GLContextPathBase.cpp

$(IntermediateDirectory)/3D_GLContextPathBase.cpp$(PreprocessSuffix): 3D/GLContextPathBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLContextPathBase.cpp$(PreprocessSuffix) 3D/GLContextPathBase.cpp

$(IntermediateDirectory)/3D_CncGCodePreview.cpp$(ObjectSuffix): 3D/CncGCodePreview.cpp $(IntermediateDirectory)/3D_CncGCodePreview.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/CncGCodePreview.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_CncGCodePreview.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_CncGCodePreview.cpp$(DependSuffix): 3D/CncGCodePreview.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_CncGCodePreview.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_CncGCodePreview.cpp$(DependSuffix) -MM 3D/CncGCodePreview.cpp

$(IntermediateDirectory)/3D_CncGCodePreview.cpp$(PreprocessSuffix): 3D/CncGCodePreview.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_CncGCodePreview.cpp$(PreprocessSuffix) 3D/CncGCodePreview.cpp

$(IntermediateDirectory)/SerialEmulatorFile.cpp$(ObjectSuffix): SerialEmulatorFile.cpp $(IntermediateDirectory)/SerialEmulatorFile.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialEmulatorFile.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialEmulatorFile.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialEmulatorFile.cpp$(DependSuffix): SerialEmulatorFile.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialEmulatorFile.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialEmulatorFile.cpp$(DependSuffix) -MM SerialEmulatorFile.cpp

$(IntermediateDirectory)/SerialEmulatorFile.cpp$(PreprocessSuffix): SerialEmulatorFile.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialEmulatorFile.cpp$(PreprocessSuffix) SerialEmulatorFile.cpp

$(IntermediateDirectory)/3D_GLContextTestCube.cpp$(ObjectSuffix): 3D/GLContextTestCube.cpp $(IntermediateDirectory)/3D_GLContextTestCube.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextTestCube.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLContextTestCube.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLContextTestCube.cpp$(DependSuffix): 3D/GLContextTestCube.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLContextTestCube.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLContextTestCube.cpp$(DependSuffix) -MM 3D/GLContextTestCube.cpp

$(IntermediateDirectory)/3D_GLContextTestCube.cpp$(PreprocessSuffix): 3D/GLContextTestCube.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLContextTestCube.cpp$(PreprocessSuffix) 3D/GLContextTestCube.cpp

$(IntermediateDirectory)/CncMotionMonitor.cpp$(ObjectSuffix): CncMotionMonitor.cpp $(IntermediateDirectory)/CncMotionMonitor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncMotionMonitor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncMotionMonitor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncMotionMonitor.cpp$(DependSuffix): CncMotionMonitor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncMotionMonitor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncMotionMonitor.cpp$(DependSuffix) -MM CncMotionMonitor.cpp

$(IntermediateDirectory)/CncMotionMonitor.cpp$(PreprocessSuffix): CncMotionMonitor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncMotionMonitor.cpp$(PreprocessSuffix) CncMotionMonitor.cpp

$(IntermediateDirectory)/win_resources.rc$(ObjectSuffix): win_resources.rc
	$(RcCompilerName) -i "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/win_resources.rc" $(RcCmpOptions)   $(ObjectSwitch)$(IntermediateDirectory)/win_resources.rc$(ObjectSuffix) $(RcIncludePath)
$(IntermediateDirectory)/CncToolStateControl.cpp$(ObjectSuffix): CncToolStateControl.cpp $(IntermediateDirectory)/CncToolStateControl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncToolStateControl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncToolStateControl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncToolStateControl.cpp$(DependSuffix): CncToolStateControl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncToolStateControl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncToolStateControl.cpp$(DependSuffix) -MM CncToolStateControl.cpp

$(IntermediateDirectory)/CncToolStateControl.cpp$(PreprocessSuffix): CncToolStateControl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncToolStateControl.cpp$(PreprocessSuffix) CncToolStateControl.cpp

$(IntermediateDirectory)/GCodePathHandlerGL.cpp$(ObjectSuffix): GCodePathHandlerGL.cpp $(IntermediateDirectory)/GCodePathHandlerGL.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodePathHandlerGL.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodePathHandlerGL.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodePathHandlerGL.cpp$(DependSuffix): GCodePathHandlerGL.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodePathHandlerGL.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodePathHandlerGL.cpp$(DependSuffix) -MM GCodePathHandlerGL.cpp

$(IntermediateDirectory)/GCodePathHandlerGL.cpp$(PreprocessSuffix): GCodePathHandlerGL.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodePathHandlerGL.cpp$(PreprocessSuffix) GCodePathHandlerGL.cpp

$(IntermediateDirectory)/CncStreamBuffers.cpp$(ObjectSuffix): CncStreamBuffers.cpp $(IntermediateDirectory)/CncStreamBuffers.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncStreamBuffers.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncStreamBuffers.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncStreamBuffers.cpp$(DependSuffix): CncStreamBuffers.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncStreamBuffers.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncStreamBuffers.cpp$(DependSuffix) -MM CncStreamBuffers.cpp

$(IntermediateDirectory)/CncStreamBuffers.cpp$(PreprocessSuffix): CncStreamBuffers.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncStreamBuffers.cpp$(PreprocessSuffix) CncStreamBuffers.cpp

$(IntermediateDirectory)/GlobalFunctions.cpp$(ObjectSuffix): GlobalFunctions.cpp $(IntermediateDirectory)/GlobalFunctions.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GlobalFunctions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GlobalFunctions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GlobalFunctions.cpp$(DependSuffix): GlobalFunctions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GlobalFunctions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GlobalFunctions.cpp$(DependSuffix) -MM GlobalFunctions.cpp

$(IntermediateDirectory)/GlobalFunctions.cpp$(PreprocessSuffix): GlobalFunctions.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GlobalFunctions.cpp$(PreprocessSuffix) GlobalFunctions.cpp

$(IntermediateDirectory)/CncFileView.cpp$(ObjectSuffix): CncFileView.cpp $(IntermediateDirectory)/CncFileView.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncFileView.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncFileView.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncFileView.cpp$(DependSuffix): CncFileView.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncFileView.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncFileView.cpp$(DependSuffix) -MM CncFileView.cpp

$(IntermediateDirectory)/CncFileView.cpp$(PreprocessSuffix): CncFileView.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncFileView.cpp$(PreprocessSuffix) CncFileView.cpp

$(IntermediateDirectory)/GL3DOptions.cpp$(ObjectSuffix): GL3DOptions.cpp $(IntermediateDirectory)/GL3DOptions.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GL3DOptions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GL3DOptions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GL3DOptions.cpp$(DependSuffix): GL3DOptions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GL3DOptions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GL3DOptions.cpp$(DependSuffix) -MM GL3DOptions.cpp

$(IntermediateDirectory)/GL3DOptions.cpp$(PreprocessSuffix): GL3DOptions.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GL3DOptions.cpp$(PreprocessSuffix) GL3DOptions.cpp

$(IntermediateDirectory)/SerialSpyPort.cpp$(ObjectSuffix): SerialSpyPort.cpp $(IntermediateDirectory)/SerialSpyPort.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialSpyPort.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialSpyPort.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialSpyPort.cpp$(DependSuffix): SerialSpyPort.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialSpyPort.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialSpyPort.cpp$(DependSuffix) -MM SerialSpyPort.cpp

$(IntermediateDirectory)/SerialSpyPort.cpp$(PreprocessSuffix): SerialSpyPort.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialSpyPort.cpp$(PreprocessSuffix) SerialSpyPort.cpp

$(IntermediateDirectory)/PgPropertyHelpInfos.cpp$(ObjectSuffix): PgPropertyHelpInfos.cpp $(IntermediateDirectory)/PgPropertyHelpInfos.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/PgPropertyHelpInfos.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PgPropertyHelpInfos.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PgPropertyHelpInfos.cpp$(DependSuffix): PgPropertyHelpInfos.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PgPropertyHelpInfos.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PgPropertyHelpInfos.cpp$(DependSuffix) -MM PgPropertyHelpInfos.cpp

$(IntermediateDirectory)/PgPropertyHelpInfos.cpp$(PreprocessSuffix): PgPropertyHelpInfos.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PgPropertyHelpInfos.cpp$(PreprocessSuffix) PgPropertyHelpInfos.cpp

$(IntermediateDirectory)/CncConfigProperty.cpp$(ObjectSuffix): CncConfigProperty.cpp $(IntermediateDirectory)/CncConfigProperty.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncConfigProperty.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncConfigProperty.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncConfigProperty.cpp$(DependSuffix): CncConfigProperty.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncConfigProperty.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncConfigProperty.cpp$(DependSuffix) -MM CncConfigProperty.cpp

$(IntermediateDirectory)/CncConfigProperty.cpp$(PreprocessSuffix): CncConfigProperty.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncConfigProperty.cpp$(PreprocessSuffix) CncConfigProperty.cpp

$(IntermediateDirectory)/SerialPort.cpp$(ObjectSuffix): SerialPort.cpp $(IntermediateDirectory)/SerialPort.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialPort.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialPort.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialPort.cpp$(DependSuffix): SerialPort.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialPort.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialPort.cpp$(DependSuffix) -MM SerialPort.cpp

$(IntermediateDirectory)/SerialPort.cpp$(PreprocessSuffix): SerialPort.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialPort.cpp$(PreprocessSuffix) SerialPort.cpp

$(IntermediateDirectory)/CncCommon.cpp$(ObjectSuffix): CncCommon.cpp $(IntermediateDirectory)/CncCommon.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncCommon.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncCommon.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncCommon.cpp$(DependSuffix): CncCommon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncCommon.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncCommon.cpp$(DependSuffix) -MM CncCommon.cpp

$(IntermediateDirectory)/CncCommon.cpp$(PreprocessSuffix): CncCommon.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncCommon.cpp$(PreprocessSuffix) CncCommon.cpp

$(IntermediateDirectory)/SerialSimulatorFacade.cpp$(ObjectSuffix): SerialSimulatorFacade.cpp $(IntermediateDirectory)/SerialSimulatorFacade.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialSimulatorFacade.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialSimulatorFacade.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialSimulatorFacade.cpp$(DependSuffix): SerialSimulatorFacade.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialSimulatorFacade.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialSimulatorFacade.cpp$(DependSuffix) -MM SerialSimulatorFacade.cpp

$(IntermediateDirectory)/SerialSimulatorFacade.cpp$(PreprocessSuffix): SerialSimulatorFacade.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialSimulatorFacade.cpp$(PreprocessSuffix) SerialSimulatorFacade.cpp

$(IntermediateDirectory)/CncPerspective.cpp$(ObjectSuffix): CncPerspective.cpp $(IntermediateDirectory)/CncPerspective.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPerspective.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPerspective.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPerspective.cpp$(DependSuffix): CncPerspective.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPerspective.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPerspective.cpp$(DependSuffix) -MM CncPerspective.cpp

$(IntermediateDirectory)/CncPerspective.cpp$(PreprocessSuffix): CncPerspective.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPerspective.cpp$(PreprocessSuffix) CncPerspective.cpp

$(IntermediateDirectory)/CfgApplicationPage.cpp$(ObjectSuffix): CfgApplicationPage.cpp $(IntermediateDirectory)/CfgApplicationPage.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CfgApplicationPage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CfgApplicationPage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CfgApplicationPage.cpp$(DependSuffix): CfgApplicationPage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CfgApplicationPage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CfgApplicationPage.cpp$(DependSuffix) -MM CfgApplicationPage.cpp

$(IntermediateDirectory)/CfgApplicationPage.cpp$(PreprocessSuffix): CfgApplicationPage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CfgApplicationPage.cpp$(PreprocessSuffix) CfgApplicationPage.cpp

$(IntermediateDirectory)/CncZView.cpp$(ObjectSuffix): CncZView.cpp $(IntermediateDirectory)/CncZView.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncZView.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncZView.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncZView.cpp$(DependSuffix): CncZView.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncZView.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncZView.cpp$(DependSuffix) -MM CncZView.cpp

$(IntermediateDirectory)/CncZView.cpp$(PreprocessSuffix): CncZView.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncZView.cpp$(PreprocessSuffix) CncZView.cpp

$(IntermediateDirectory)/CncVectiesListCtrl.cpp$(ObjectSuffix): CncVectiesListCtrl.cpp $(IntermediateDirectory)/CncVectiesListCtrl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncVectiesListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncVectiesListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncVectiesListCtrl.cpp$(DependSuffix): CncVectiesListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncVectiesListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncVectiesListCtrl.cpp$(DependSuffix) -MM CncVectiesListCtrl.cpp

$(IntermediateDirectory)/CncVectiesListCtrl.cpp$(PreprocessSuffix): CncVectiesListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncVectiesListCtrl.cpp$(PreprocessSuffix) CncVectiesListCtrl.cpp

$(IntermediateDirectory)/wxcrafter.cpp$(ObjectSuffix): wxcrafter.cpp $(IntermediateDirectory)/wxcrafter.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxcrafter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxcrafter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxcrafter.cpp$(DependSuffix): wxcrafter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxcrafter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxcrafter.cpp$(DependSuffix) -MM wxcrafter.cpp

$(IntermediateDirectory)/wxcrafter.cpp$(PreprocessSuffix): wxcrafter.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxcrafter.cpp$(PreprocessSuffix) wxcrafter.cpp

$(IntermediateDirectory)/CncSummaryListCtrl.cpp$(ObjectSuffix): CncSummaryListCtrl.cpp $(IntermediateDirectory)/CncSummaryListCtrl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSummaryListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSummaryListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSummaryListCtrl.cpp$(DependSuffix): CncSummaryListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSummaryListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSummaryListCtrl.cpp$(DependSuffix) -MM CncSummaryListCtrl.cpp

$(IntermediateDirectory)/CncSummaryListCtrl.cpp$(PreprocessSuffix): CncSummaryListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSummaryListCtrl.cpp$(PreprocessSuffix) CncSummaryListCtrl.cpp

$(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(ObjectSuffix): wxcrafter_bitmaps.cpp $(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxcrafter_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(DependSuffix): wxcrafter_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(DependSuffix) -MM wxcrafter_bitmaps.cpp

$(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(PreprocessSuffix): wxcrafter_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(PreprocessSuffix) wxcrafter_bitmaps.cpp

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/MainFrame.cpp$(ObjectSuffix): MainFrame.cpp $(IntermediateDirectory)/MainFrame.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/MainFrame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MainFrame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MainFrame.cpp$(DependSuffix): MainFrame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/MainFrame.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/MainFrame.cpp$(DependSuffix) -MM MainFrame.cpp

$(IntermediateDirectory)/MainFrame.cpp$(PreprocessSuffix): MainFrame.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MainFrame.cpp$(PreprocessSuffix) MainFrame.cpp

$(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(ObjectSuffix): CncPosSpyListCtrl.cpp $(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPosSpyListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(DependSuffix): CncPosSpyListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(DependSuffix) -MM CncPosSpyListCtrl.cpp

$(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(PreprocessSuffix): CncPosSpyListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(PreprocessSuffix) CncPosSpyListCtrl.cpp

$(IntermediateDirectory)/UpdateManagerThread.cpp$(ObjectSuffix): UpdateManagerThread.cpp $(IntermediateDirectory)/UpdateManagerThread.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/UpdateManagerThread.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/UpdateManagerThread.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/UpdateManagerThread.cpp$(DependSuffix): UpdateManagerThread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/UpdateManagerThread.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/UpdateManagerThread.cpp$(DependSuffix) -MM UpdateManagerThread.cpp

$(IntermediateDirectory)/UpdateManagerThread.cpp$(PreprocessSuffix): UpdateManagerThread.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/UpdateManagerThread.cpp$(PreprocessSuffix) UpdateManagerThread.cpp

$(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(ObjectSuffix): Codelite/wxPNGAnimation.cpp $(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/Codelite/wxPNGAnimation.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(DependSuffix): Codelite/wxPNGAnimation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(DependSuffix) -MM Codelite/wxPNGAnimation.cpp

$(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(PreprocessSuffix): Codelite/wxPNGAnimation.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(PreprocessSuffix) Codelite/wxPNGAnimation.cpp

$(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(ObjectSuffix): CncLargeScaleListCtrl.cpp $(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncLargeScaleListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(DependSuffix): CncLargeScaleListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(DependSuffix) -MM CncLargeScaleListCtrl.cpp

$(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(PreprocessSuffix): CncLargeScaleListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(PreprocessSuffix) CncLargeScaleListCtrl.cpp

$(IntermediateDirectory)/DataControlModel.cpp$(ObjectSuffix): DataControlModel.cpp $(IntermediateDirectory)/DataControlModel.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/DataControlModel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/DataControlModel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/DataControlModel.cpp$(DependSuffix): DataControlModel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/DataControlModel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/DataControlModel.cpp$(DependSuffix) -MM DataControlModel.cpp

$(IntermediateDirectory)/DataControlModel.cpp$(PreprocessSuffix): DataControlModel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/DataControlModel.cpp$(PreprocessSuffix) DataControlModel.cpp

$(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(ObjectSuffix): GCodePathHandlerCnc.cpp $(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodePathHandlerCnc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(DependSuffix): GCodePathHandlerCnc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(DependSuffix) -MM GCodePathHandlerCnc.cpp

$(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(PreprocessSuffix): GCodePathHandlerCnc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(PreprocessSuffix) GCodePathHandlerCnc.cpp

$(IntermediateDirectory)/UnitTestFrame.cpp$(ObjectSuffix): UnitTestFrame.cpp $(IntermediateDirectory)/UnitTestFrame.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/UnitTestFrame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/UnitTestFrame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/UnitTestFrame.cpp$(DependSuffix): UnitTestFrame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/UnitTestFrame.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/UnitTestFrame.cpp$(DependSuffix) -MM UnitTestFrame.cpp

$(IntermediateDirectory)/UnitTestFrame.cpp$(PreprocessSuffix): UnitTestFrame.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/UnitTestFrame.cpp$(PreprocessSuffix) UnitTestFrame.cpp

$(IntermediateDirectory)/CncSpyControl.cpp$(ObjectSuffix): CncSpyControl.cpp $(IntermediateDirectory)/CncSpyControl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSpyControl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSpyControl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSpyControl.cpp$(DependSuffix): CncSpyControl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSpyControl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSpyControl.cpp$(DependSuffix) -MM CncSpyControl.cpp

$(IntermediateDirectory)/CncSpyControl.cpp$(PreprocessSuffix): CncSpyControl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSpyControl.cpp$(PreprocessSuffix) CncSpyControl.cpp

$(IntermediateDirectory)/CncConfig.cpp$(ObjectSuffix): CncConfig.cpp $(IntermediateDirectory)/CncConfig.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncConfig.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncConfig.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncConfig.cpp$(DependSuffix): CncConfig.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncConfig.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncConfig.cpp$(DependSuffix) -MM CncConfig.cpp

$(IntermediateDirectory)/CncConfig.cpp$(PreprocessSuffix): CncConfig.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncConfig.cpp$(PreprocessSuffix) CncConfig.cpp

$(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(ObjectSuffix): 3D/GLContextGCodePreview.cpp $(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextGCodePreview.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(DependSuffix): 3D/GLContextGCodePreview.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(DependSuffix) -MM 3D/GLContextGCodePreview.cpp

$(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(PreprocessSuffix): 3D/GLContextGCodePreview.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(PreprocessSuffix) 3D/GLContextGCodePreview.cpp

$(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(ObjectSuffix): ManuallyPathHandlerCnc.cpp $(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/ManuallyPathHandlerCnc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(DependSuffix): ManuallyPathHandlerCnc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(DependSuffix) -MM ManuallyPathHandlerCnc.cpp

$(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(PreprocessSuffix): ManuallyPathHandlerCnc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(PreprocessSuffix) ManuallyPathHandlerCnc.cpp

$(IntermediateDirectory)/SVGElementConverter.cpp$(ObjectSuffix): SVGElementConverter.cpp $(IntermediateDirectory)/SVGElementConverter.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGElementConverter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGElementConverter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGElementConverter.cpp$(DependSuffix): SVGElementConverter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGElementConverter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGElementConverter.cpp$(DependSuffix) -MM SVGElementConverter.cpp

$(IntermediateDirectory)/SVGElementConverter.cpp$(PreprocessSuffix): SVGElementConverter.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGElementConverter.cpp$(PreprocessSuffix) SVGElementConverter.cpp

$(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(ObjectSuffix): OSD/CncAsyncKeyboardState.cpp $(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/OSD/CncAsyncKeyboardState.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(DependSuffix): OSD/CncAsyncKeyboardState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(DependSuffix) -MM OSD/CncAsyncKeyboardState.cpp

$(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(PreprocessSuffix): OSD/CncAsyncKeyboardState.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(PreprocessSuffix) OSD/CncAsyncKeyboardState.cpp

$(IntermediateDirectory)/GCodeField.cpp$(ObjectSuffix): GCodeField.cpp $(IntermediateDirectory)/GCodeField.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodeField.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodeField.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodeField.cpp$(DependSuffix): GCodeField.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodeField.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodeField.cpp$(DependSuffix) -MM GCodeField.cpp

$(IntermediateDirectory)/GCodeField.cpp$(PreprocessSuffix): GCodeField.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodeField.cpp$(PreprocessSuffix) GCodeField.cpp

$(IntermediateDirectory)/SerialSimulatorDevNull.cpp$(ObjectSuffix): SerialSimulatorDevNull.cpp $(IntermediateDirectory)/SerialSimulatorDevNull.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialSimulatorDevNull.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialSimulatorDevNull.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialSimulatorDevNull.cpp$(DependSuffix): SerialSimulatorDevNull.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialSimulatorDevNull.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialSimulatorDevNull.cpp$(DependSuffix) -MM SerialSimulatorDevNull.cpp

$(IntermediateDirectory)/SerialSimulatorDevNull.cpp$(PreprocessSuffix): SerialSimulatorDevNull.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialSimulatorDevNull.cpp$(PreprocessSuffix) SerialSimulatorDevNull.cpp

$(IntermediateDirectory)/CncSetterListCtrl.cpp$(ObjectSuffix): CncSetterListCtrl.cpp $(IntermediateDirectory)/CncSetterListCtrl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSetterListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSetterListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSetterListCtrl.cpp$(DependSuffix): CncSetterListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSetterListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSetterListCtrl.cpp$(DependSuffix) -MM CncSetterListCtrl.cpp

$(IntermediateDirectory)/CncSetterListCtrl.cpp$(PreprocessSuffix): CncSetterListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSetterListCtrl.cpp$(PreprocessSuffix) CncSetterListCtrl.cpp

$(IntermediateDirectory)/FileParser.cpp$(ObjectSuffix): FileParser.cpp $(IntermediateDirectory)/FileParser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/FileParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FileParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/FileParser.cpp$(DependSuffix): FileParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/FileParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/FileParser.cpp$(DependSuffix) -MM FileParser.cpp

$(IntermediateDirectory)/FileParser.cpp$(PreprocessSuffix): FileParser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/FileParser.cpp$(PreprocessSuffix) FileParser.cpp

$(IntermediateDirectory)/Tests_Test_Base.cpp$(ObjectSuffix): Tests/Test_Base.cpp $(IntermediateDirectory)/Tests_Test_Base.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/Tests/Test_Base.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Tests_Test_Base.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Tests_Test_Base.cpp$(DependSuffix): Tests/Test_Base.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Tests_Test_Base.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Tests_Test_Base.cpp$(DependSuffix) -MM Tests/Test_Base.cpp

$(IntermediateDirectory)/Tests_Test_Base.cpp$(PreprocessSuffix): Tests/Test_Base.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Tests_Test_Base.cpp$(PreprocessSuffix) Tests/Test_Base.cpp

$(IntermediateDirectory)/SerialEmulatorSVG.cpp$(ObjectSuffix): SerialEmulatorSVG.cpp $(IntermediateDirectory)/SerialEmulatorSVG.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialEmulatorSVG.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialEmulatorSVG.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialEmulatorSVG.cpp$(DependSuffix): SerialEmulatorSVG.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialEmulatorSVG.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialEmulatorSVG.cpp$(DependSuffix) -MM SerialEmulatorSVG.cpp

$(IntermediateDirectory)/SerialEmulatorSVG.cpp$(PreprocessSuffix): SerialEmulatorSVG.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialEmulatorSVG.cpp$(PreprocessSuffix) SerialEmulatorSVG.cpp

$(IntermediateDirectory)/CncToolCorrection.cpp$(ObjectSuffix): CncToolCorrection.cpp $(IntermediateDirectory)/CncToolCorrection.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncToolCorrection.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncToolCorrection.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncToolCorrection.cpp$(DependSuffix): CncToolCorrection.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncToolCorrection.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncToolCorrection.cpp$(DependSuffix) -MM CncToolCorrection.cpp

$(IntermediateDirectory)/CncToolCorrection.cpp$(PreprocessSuffix): CncToolCorrection.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncToolCorrection.cpp$(PreprocessSuffix) CncToolCorrection.cpp

$(IntermediateDirectory)/GCodeBlock.cpp$(ObjectSuffix): GCodeBlock.cpp $(IntermediateDirectory)/GCodeBlock.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodeBlock.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodeBlock.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodeBlock.cpp$(DependSuffix): GCodeBlock.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodeBlock.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodeBlock.cpp$(DependSuffix) -MM GCodeBlock.cpp

$(IntermediateDirectory)/GCodeBlock.cpp$(PreprocessSuffix): GCodeBlock.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodeBlock.cpp$(PreprocessSuffix) GCodeBlock.cpp

$(IntermediateDirectory)/GCodeCommands.cpp$(ObjectSuffix): GCodeCommands.cpp $(IntermediateDirectory)/GCodeCommands.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodeCommands.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodeCommands.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodeCommands.cpp$(DependSuffix): GCodeCommands.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodeCommands.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodeCommands.cpp$(DependSuffix) -MM GCodeCommands.cpp

$(IntermediateDirectory)/GCodeCommands.cpp$(PreprocessSuffix): GCodeCommands.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodeCommands.cpp$(PreprocessSuffix) GCodeCommands.cpp

$(IntermediateDirectory)/CncSvgCurveLib.cpp$(ObjectSuffix): CncSvgCurveLib.cpp $(IntermediateDirectory)/CncSvgCurveLib.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSvgCurveLib.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSvgCurveLib.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSvgCurveLib.cpp$(DependSuffix): CncSvgCurveLib.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSvgCurveLib.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSvgCurveLib.cpp$(DependSuffix) -MM CncSvgCurveLib.cpp

$(IntermediateDirectory)/CncSvgCurveLib.cpp$(PreprocessSuffix): CncSvgCurveLib.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSvgCurveLib.cpp$(PreprocessSuffix) CncSvgCurveLib.cpp

$(IntermediateDirectory)/ManuallyParser.cpp$(ObjectSuffix): ManuallyParser.cpp $(IntermediateDirectory)/ManuallyParser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/ManuallyParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ManuallyParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ManuallyParser.cpp$(DependSuffix): ManuallyParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ManuallyParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ManuallyParser.cpp$(DependSuffix) -MM ManuallyParser.cpp

$(IntermediateDirectory)/ManuallyParser.cpp$(PreprocessSuffix): ManuallyParser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ManuallyParser.cpp$(PreprocessSuffix) ManuallyParser.cpp

$(IntermediateDirectory)/3D_GLViewPort.cpp$(ObjectSuffix): 3D/GLViewPort.cpp $(IntermediateDirectory)/3D_GLViewPort.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLViewPort.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLViewPort.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLViewPort.cpp$(DependSuffix): 3D/GLViewPort.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLViewPort.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLViewPort.cpp$(DependSuffix) -MM 3D/GLViewPort.cpp

$(IntermediateDirectory)/3D_GLViewPort.cpp$(PreprocessSuffix): 3D/GLViewPort.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLViewPort.cpp$(PreprocessSuffix) 3D/GLViewPort.cpp

$(IntermediateDirectory)/3D_GLContextCncPath.cpp$(ObjectSuffix): 3D/GLContextCncPath.cpp $(IntermediateDirectory)/3D_GLContextCncPath.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextCncPath.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLContextCncPath.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLContextCncPath.cpp$(DependSuffix): 3D/GLContextCncPath.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLContextCncPath.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLContextCncPath.cpp$(DependSuffix) -MM 3D/GLContextCncPath.cpp

$(IntermediateDirectory)/3D_GLContextCncPath.cpp$(PreprocessSuffix): 3D/GLContextCncPath.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLContextCncPath.cpp$(PreprocessSuffix) 3D/GLContextCncPath.cpp

$(IntermediateDirectory)/CncControl.cpp$(ObjectSuffix): CncControl.cpp $(IntermediateDirectory)/CncControl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncControl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncControl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncControl.cpp$(DependSuffix): CncControl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncControl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncControl.cpp$(DependSuffix) -MM CncControl.cpp

$(IntermediateDirectory)/CncControl.cpp$(PreprocessSuffix): CncControl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncControl.cpp$(PreprocessSuffix) CncControl.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Release/


