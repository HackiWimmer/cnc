##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=CncControllerGui
ConfigurationName      :=Release
WorkspaceConfiguration :=Release
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui
IntermediateDirectory  :=$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=05/05/2022
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=C:/msys64/mingw64/bin/g++.exe
SharedObjectLinkerName :=C:/msys64/mingw64/bin/g++.exe -shared -fPIC
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
OutputDirectory        :=$(IntermediateDirectory)
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG $(PreprocessorSwitch)APP_USE_SPLASH $(PreprocessorSwitch)BOOST_STACKTRACE_USE_BACKTRACE $(PreprocessorSwitch)wxUSE_DPI_AWARE_MANIFEST=2 $(PreprocessorSwitch)wxUSE_DEBUGREPORT=1 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="CncControllerGui.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := $(shell wx-config --rcflags ) -D NDEBUG 
RcCompilerName         :=C:/msys64/mingw64/bin/windres.exe
LinkOptions            :=  $(shell wx-config --libs) -mwindows
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)C:/@Development/boost_1_79_0 $(IncludeSwitch)C:/msys64/mingw64/include $(IncludeSwitch)C:/@Development/wxsvg-1.5.23/include $(IncludeSwitch)C:/@Development/opencv/build/include $(IncludeSwitch)C:/@Development/openvg 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)wxmsw31u_stc $(LibrarySwitch)wxmsw31u_webview $(LibrarySwitch)wxmsw31u_propgrid $(LibrarySwitch)wxmsw31u_adv $(LibrarySwitch)wxmsw31u_gl $(LibrarySwitch)wxmsw31u_aui $(LibrarySwitch)wxsvg $(LibrarySwitch)cairo $(LibrarySwitch)avutil $(LibrarySwitch)avformat $(LibrarySwitch)avcodec $(LibrarySwitch)xvidcore $(LibrarySwitch)expat $(LibrarySwitch)exif $(LibrarySwitch)swscale $(LibrarySwitch)backtrace $(LibrarySwitch)dl $(LibrarySwitch)dbgeng $(LibrarySwitch)opengl32 $(LibrarySwitch)strmiids $(LibrarySwitch)glu32 $(LibrarySwitch)glew32 $(LibrarySwitch)ole32 $(LibrarySwitch)oleaut32 $(LibrarySwitch)freeglut $(LibrarySwitch)Xinput9_1_0 $(LibrarySwitch)opencv_core451 $(LibrarySwitch)opencv_video451 $(LibrarySwitch)opencv_videoio451 $(LibrarySwitch)opencv_highgui451 
ArLibs                 :=  "libwxmsw31u_stc" "libwxmsw31u_webview" "libwxmsw31u_propgrid" "libwxmsw31u_adv" "libwxmsw31u_gl" "libwxmsw31u_aui" "libwxsvg" "libcairo" "libavutil" "libavformat" "libavcodec" "libxvidcore" "libexpat" "libexif" "libswscale" "backtrace" "libdl" "dbgeng" "opengl32" "strmiids" "glu32" "glew32" "ole32" "oleaut32" "freeglut" "Xinput9_1_0" "libopencv_core451" "libopencv_video451" "libopencv_videoio451" "libopencv_highgui451" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)C:/@Development/wxWidgets-3.1.6/lib/gcc_dll $(LibraryPathSwitch)C:/msys64/mingw64/lib $(LibraryPathSwitch)C:/@Development/opencv/mingwbuild/lib 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := C:/msys64/mingw64/bin/ar.exe rcu
CXX      := C:/msys64/mingw64/bin/g++.exe
CC       := C:/msys64/mingw64/bin/gcc.exe
CXXFLAGS := -Wno-deprecated-declarations -g1 -fpic -O3 -std=c++2a -Wall $(shell wx-config --cflags) $(Preprocessors)
CFLAGS   :=  -O3 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/msys64/mingw64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/CncToolMagazine.cpp$(ObjectSuffix) $(IntermediateDirectory)/CfgWorkingPage.cpp$(ObjectSuffix) $(IntermediateDirectory)/CfgGeneralPage.cpp$(ObjectSuffix) $(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(ObjectSuffix) $(IntermediateDirectory)/CfgEnvironmentPage.cpp$(ObjectSuffix) $(IntermediateDirectory)/CfgApplicationPage.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncTsTplProcessing.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncOSDConfigList.cpp$(ObjectSuffix) $(IntermediateDirectory)/SvgUnitCalculator.cpp$(ObjectSuffix) $(IntermediateDirectory)/SVGElementConverter.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/SvgEditPopup.cpp$(ObjectSuffix) $(IntermediateDirectory)/SvgViewBox.cpp$(ObjectSuffix) $(IntermediateDirectory)/SvgColourScheme.cpp$(ObjectSuffix) $(IntermediateDirectory)/SVGUserAgent.cpp$(ObjectSuffix) $(IntermediateDirectory)/SVGUserAgentInfo.cpp$(ObjectSuffix) $(IntermediateDirectory)/SvgCncContext.cpp$(ObjectSuffix) $(IntermediateDirectory)/SvgTransformMatrix.cpp$(ObjectSuffix) $(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(ObjectSuffix) $(IntermediateDirectory)/SVGPathHandlerBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/SVGFileParser.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/SVGParserBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncFileView.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncZView.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncStreamBuffers.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterPodiumManagement_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/GCodeCommands.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_CncGLCanvas.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialCircularBuffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncControl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncFilePreview.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncReferencePosition.cpp$(ObjectSuffix) $(IntermediateDirectory)/ArduinoEnvWrapper.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncMouseRemoteControl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncGamepadDirectionPanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncNumberFormatter.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncNavigatorPanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncOSEnvironmentDialog.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSpeedMonitor.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncAverage.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncTemplateObserver.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/SerialSpyPort.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncMotionVertexTrace.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncOpenGLContextObserver.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncMonitorReplayPane.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncLastProcessingTimestampSummary.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncAutoProgressDialog.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSecureSlidepadDialog.cpp$(ObjectSuffix) $(IntermediateDirectory)/BinaryPathHandlerCnc.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPosition.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncUserEvents.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncToolStateControl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSpeedPlayground.cpp$(ObjectSuffix) 

Objects1=$(IntermediateDirectory)/CncSpeedAccelerationDiagram.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncBaseEditor.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncFloatingPointValidator.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncManuallyMoveCoordinates.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncFilePreviewWnd.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncArduino.cpp$(ObjectSuffix) $(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncCommon.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncPositionStorage.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncCircularBuffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/WireEndPoint.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSpeedSlider.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncAutoFreezer.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPolarRegionDetector.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncClipperLib.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialThread.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncStringLogger.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterGamePort_bitmaps.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncSecureRotateModelPanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPerspective.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterSpeedMonitor.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPreprocessor.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialEmulatorSvgStreamer.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncTextCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncLoggerView.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncMonitorSplitterWindow.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncMessageListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncConnectProgress.cpp$(ObjectSuffix) $(IntermediateDirectory)/ArduinoData.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncCurveLib.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncLoggerListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/win_resources.rc$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterUnitTest_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncAnchorPosListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncArduinoPinsListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSecureGesturesPanel.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/GL3DOptionPane.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncInfoBar.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncTemplateContextSummaryPanel.cpp$(ObjectSuffix) 

Objects2=$(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncUnitTests.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSerialSpyPanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncProcessingInfo.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncOutboundEditor.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSecureCtrlPanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_CncGCodePreview.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncBinaryTemplateStreamer.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncMoveSequence.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterToolMagazine_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncStatisticsPane.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterPathGenerator.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterMotionMonitor.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSetterListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPathListInterface.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncFileNameService.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSecureSlidepad.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncSecureManuallyMovePanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialEndPoint.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncConfigProperties.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSerialSpyListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncTemplateContextSummaryDialog.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncFileDialog.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLLabelCluser.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterArduinoEnv_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncReferenceEvaluation.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialEmulatorNull.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/GamepadThread.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncVideoCapturePanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncBoundarySpace.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSourceEditor.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncFlowPositionConverter.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSecureScrollButton.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLGuidePath.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncTouchBlockDetector.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSvgControl.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterTemplateObserver.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/wxCrafterContextObserver_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLContextCncPathBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_CncMetricRulerSetup.cpp$(ObjectSuffix) $(IntermediateDirectory)/Clipper_clipper.cpp$(ObjectSuffix) $(IntermediateDirectory)/GL3DDrawPane.cpp$(ObjectSuffix) 

Objects3=$(IntermediateDirectory)/CncArduinoEnvironment.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPodiumManagement.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterPodiumManagement.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncCommandDecoder.cpp$(ObjectSuffix) $(IntermediateDirectory)/Tests_Test_Base.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/3D_GLContextBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncMotionMonitor.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialPort.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSummaryListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSecureNumpad.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPathListManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPathListEntry.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncAnchorPosition.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncGamePad.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncUnitCalculator.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncLoggerProxy.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncUsbConnectionObserver.cpp$(ObjectSuffix) $(IntermediateDirectory)/Codelite_cl_aui_dock_art.cpp$(ObjectSuffix) $(IntermediateDirectory)/GamepadEvent.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncGamepadSpy.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncVector.cpp$(ObjectSuffix) $(IntermediateDirectory)/DataControlModel.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterSecurePanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterSerialSpy.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/wxCrafterSerialSpy_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncMotionMonitorVertexTrace.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterLoggerPanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLCommon.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncTraceCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterLoggerPanel_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterMessageDialog.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncMoveSequenceListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterLCDPanel_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLContextTestCube.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/wxCrafterLCDPanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterArduinoEnv.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterSecurePanel_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterMotionMonitor_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncConfig.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterFileView_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterFileView.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLViewPort.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterToolMagazine.cpp$(ObjectSuffix) $(IntermediateDirectory)/ContextInterface.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/wxCrafterBaseDialogs_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterBaseDialogs.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterReferencePosition_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterSpeedMonitor_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/CfgSVGOptionPage.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterUnitTest.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterTemplateObserver_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterPathGenerator_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncGamepadCommandHistory.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncLCDPositionPanel.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/wxCrafterMessageDialog_bitmaps.cpp$(ObjectSuffix) 

Objects4=$(IntermediateDirectory)/wxCrafterMainFrame.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterImages_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterImages.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterGamePort.cpp$(ObjectSuffix) $(IntermediateDirectory)/MainFrameProxy.cpp$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterMainFrame_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/MainFrame.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncGamepadControllerState.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/StdStreamRedirector.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLHelperModel.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncSecureNumpadDialog.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialEmulatorBinaryStreamer.cpp$(ObjectSuffix) $(IntermediateDirectory)/sha1.cpp$(ObjectSuffix) $(IntermediateDirectory)/OSD_CncCameraHelper.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncGamepadGtk.cpp$(ObjectSuffix) $(IntermediateDirectory)/UnitTestFrame.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncGamepadFilter.cpp$(ObjectSuffix) $(IntermediateDirectory)/GlobalFunctions.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncContext.cpp$(ObjectSuffix) $(IntermediateDirectory)/HexDecoder.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPodiumMgmtMovement.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPathListRunner.cpp$(ObjectSuffix) $(IntermediateDirectory)/WireCircularBuffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCode_lcdwindow.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPathListInterfaceCnc.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLOpenGLPathBufferStore.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncTemplateContext.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncStartPositionResolver.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/CncSecurePortListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncFileViewLists.cpp$(ObjectSuffix) $(IntermediateDirectory)/ManuallyParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncContextListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncGCodeSequenceListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPathListEntryListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterReferencePosition.cpp$(ObjectSuffix) $(IntermediateDirectory)/BinaryPathHandlerBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/CircularBuffer.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/BinaryFileParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncGamePadMsw.cpp$(ObjectSuffix) $(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(ObjectSuffix) $(IntermediateDirectory)/GCodeFileParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/GCodePathHandlerBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/GCodeBlock.cpp$(ObjectSuffix) 

Objects5=$(IntermediateDirectory)/GCodeField.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncOpenGLContextObserverListCtrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLLineCluster.cpp$(ObjectSuffix) $(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/PathGeneratorFrame.cpp$(ObjectSuffix) $(IntermediateDirectory)/GCodePathHandlerGL.cpp$(ObjectSuffix) $(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLCncPathData.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncPositionStorageView.cpp$(ObjectSuffix) $(IntermediateDirectory)/3D_GLContextCncPath.cpp$(ObjectSuffix) $(IntermediateDirectory)/BaseEndPoint.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialAdminChannel.cpp$(ObjectSuffix) $(IntermediateDirectory)/OSD_SerialMsw.cpp$(ObjectSuffix) $(IntermediateDirectory)/OSD_SerialGtk.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/SerialThreadStub.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncMessageDialog.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialEmulatorGCodeStreamer.cpp$(ObjectSuffix) $(IntermediateDirectory)/SerialEmulatorTextStreamer.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncMillingSoundController.cpp$(ObjectSuffix) $(IntermediateDirectory)/CncExternalViewBox.cpp$(ObjectSuffix) $(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(ObjectSuffix) $(IntermediateDirectory)/FileParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/PathHandlerBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxCrafterContextObserver.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/SVGNodeParser.cpp$(ObjectSuffix) 



Objects=$(Objects0) $(Objects1) $(Objects2) $(Objects3) $(Objects4) $(Objects5) 

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
	@echo $(Objects3) >> $(ObjectsFileList)
	@echo $(Objects4) >> $(ObjectsFileList)
	@echo $(Objects5) >> $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

PostBuild:
	@echo Executing Post Build commands ...
	copy "C:\@Development\@Projekte\c++\CNCGuiController\CncControlerGui\Release\CncControllerGui.exe" "t:\"
	@echo Done

MakeIntermediateDirs:
	@$(MakeDirCommand) "$(ConfigurationName)"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "$(ConfigurationName)"

PreBuild:
	@echo Executing Pre Build commands ...
	g++ -v
	echo C:\@Development\wxWidgets-3.1.6
	where g++
	g++ -v
	echo C:\@Development\wxWidgets-3.1.6
	where g++
	@echo Done


##
## Objects
##
$(IntermediateDirectory)/CncToolMagazine.cpp$(ObjectSuffix): CncToolMagazine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncToolMagazine.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncToolMagazine.cpp$(DependSuffix) -MM CncToolMagazine.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncToolMagazine.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncToolMagazine.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncToolMagazine.cpp$(PreprocessSuffix): CncToolMagazine.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncToolMagazine.cpp$(PreprocessSuffix) CncToolMagazine.cpp

$(IntermediateDirectory)/CfgWorkingPage.cpp$(ObjectSuffix): CfgWorkingPage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CfgWorkingPage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CfgWorkingPage.cpp$(DependSuffix) -MM CfgWorkingPage.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CfgWorkingPage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CfgWorkingPage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CfgWorkingPage.cpp$(PreprocessSuffix): CfgWorkingPage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CfgWorkingPage.cpp$(PreprocessSuffix) CfgWorkingPage.cpp

$(IntermediateDirectory)/CfgGeneralPage.cpp$(ObjectSuffix): CfgGeneralPage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CfgGeneralPage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CfgGeneralPage.cpp$(DependSuffix) -MM CfgGeneralPage.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CfgGeneralPage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CfgGeneralPage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CfgGeneralPage.cpp$(PreprocessSuffix): CfgGeneralPage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CfgGeneralPage.cpp$(PreprocessSuffix) CfgGeneralPage.cpp

$(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(ObjectSuffix): CfgGCodeOptionPage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(DependSuffix) -MM CfgGCodeOptionPage.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CfgGCodeOptionPage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(PreprocessSuffix): CfgGCodeOptionPage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CfgGCodeOptionPage.cpp$(PreprocessSuffix) CfgGCodeOptionPage.cpp

$(IntermediateDirectory)/CfgEnvironmentPage.cpp$(ObjectSuffix): CfgEnvironmentPage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CfgEnvironmentPage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CfgEnvironmentPage.cpp$(DependSuffix) -MM CfgEnvironmentPage.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CfgEnvironmentPage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CfgEnvironmentPage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CfgEnvironmentPage.cpp$(PreprocessSuffix): CfgEnvironmentPage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CfgEnvironmentPage.cpp$(PreprocessSuffix) CfgEnvironmentPage.cpp

$(IntermediateDirectory)/CfgApplicationPage.cpp$(ObjectSuffix): CfgApplicationPage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CfgApplicationPage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CfgApplicationPage.cpp$(DependSuffix) -MM CfgApplicationPage.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CfgApplicationPage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CfgApplicationPage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CfgApplicationPage.cpp$(PreprocessSuffix): CfgApplicationPage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CfgApplicationPage.cpp$(PreprocessSuffix) CfgApplicationPage.cpp

$(IntermediateDirectory)/CncTsTplProcessing.cpp$(ObjectSuffix): CncTsTplProcessing.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncTsTplProcessing.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncTsTplProcessing.cpp$(DependSuffix) -MM CncTsTplProcessing.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncTsTplProcessing.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncTsTplProcessing.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncTsTplProcessing.cpp$(PreprocessSuffix): CncTsTplProcessing.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncTsTplProcessing.cpp$(PreprocessSuffix) CncTsTplProcessing.cpp

$(IntermediateDirectory)/CncOSDConfigList.cpp$(ObjectSuffix): CncOSDConfigList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncOSDConfigList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncOSDConfigList.cpp$(DependSuffix) -MM CncOSDConfigList.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncOSDConfigList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncOSDConfigList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncOSDConfigList.cpp$(PreprocessSuffix): CncOSDConfigList.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncOSDConfigList.cpp$(PreprocessSuffix) CncOSDConfigList.cpp

$(IntermediateDirectory)/SvgUnitCalculator.cpp$(ObjectSuffix): SvgUnitCalculator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SvgUnitCalculator.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SvgUnitCalculator.cpp$(DependSuffix) -MM SvgUnitCalculator.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SvgUnitCalculator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SvgUnitCalculator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SvgUnitCalculator.cpp$(PreprocessSuffix): SvgUnitCalculator.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SvgUnitCalculator.cpp$(PreprocessSuffix) SvgUnitCalculator.cpp

$(IntermediateDirectory)/SVGElementConverter.cpp$(ObjectSuffix): SVGElementConverter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGElementConverter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGElementConverter.cpp$(DependSuffix) -MM SVGElementConverter.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGElementConverter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGElementConverter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGElementConverter.cpp$(PreprocessSuffix): SVGElementConverter.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGElementConverter.cpp$(PreprocessSuffix) SVGElementConverter.cpp

$(IntermediateDirectory)/SvgEditPopup.cpp$(ObjectSuffix): SvgEditPopup.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SvgEditPopup.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SvgEditPopup.cpp$(DependSuffix) -MM SvgEditPopup.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SvgEditPopup.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SvgEditPopup.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SvgEditPopup.cpp$(PreprocessSuffix): SvgEditPopup.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SvgEditPopup.cpp$(PreprocessSuffix) SvgEditPopup.cpp

$(IntermediateDirectory)/SvgViewBox.cpp$(ObjectSuffix): SvgViewBox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SvgViewBox.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SvgViewBox.cpp$(DependSuffix) -MM SvgViewBox.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SvgViewBox.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SvgViewBox.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SvgViewBox.cpp$(PreprocessSuffix): SvgViewBox.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SvgViewBox.cpp$(PreprocessSuffix) SvgViewBox.cpp

$(IntermediateDirectory)/SvgColourScheme.cpp$(ObjectSuffix): SvgColourScheme.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SvgColourScheme.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SvgColourScheme.cpp$(DependSuffix) -MM SvgColourScheme.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SvgColourScheme.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SvgColourScheme.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SvgColourScheme.cpp$(PreprocessSuffix): SvgColourScheme.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SvgColourScheme.cpp$(PreprocessSuffix) SvgColourScheme.cpp

$(IntermediateDirectory)/SVGUserAgent.cpp$(ObjectSuffix): SVGUserAgent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGUserAgent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGUserAgent.cpp$(DependSuffix) -MM SVGUserAgent.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGUserAgent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGUserAgent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGUserAgent.cpp$(PreprocessSuffix): SVGUserAgent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGUserAgent.cpp$(PreprocessSuffix) SVGUserAgent.cpp

$(IntermediateDirectory)/SVGUserAgentInfo.cpp$(ObjectSuffix): SVGUserAgentInfo.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGUserAgentInfo.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGUserAgentInfo.cpp$(DependSuffix) -MM SVGUserAgentInfo.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGUserAgentInfo.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGUserAgentInfo.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGUserAgentInfo.cpp$(PreprocessSuffix): SVGUserAgentInfo.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGUserAgentInfo.cpp$(PreprocessSuffix) SVGUserAgentInfo.cpp

$(IntermediateDirectory)/SvgCncContext.cpp$(ObjectSuffix): SvgCncContext.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SvgCncContext.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SvgCncContext.cpp$(DependSuffix) -MM SvgCncContext.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SvgCncContext.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SvgCncContext.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SvgCncContext.cpp$(PreprocessSuffix): SvgCncContext.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SvgCncContext.cpp$(PreprocessSuffix) SvgCncContext.cpp

$(IntermediateDirectory)/SvgTransformMatrix.cpp$(ObjectSuffix): SvgTransformMatrix.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SvgTransformMatrix.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SvgTransformMatrix.cpp$(DependSuffix) -MM SvgTransformMatrix.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SvgTransformMatrix.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SvgTransformMatrix.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SvgTransformMatrix.cpp$(PreprocessSuffix): SvgTransformMatrix.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SvgTransformMatrix.cpp$(PreprocessSuffix) SvgTransformMatrix.cpp

$(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(ObjectSuffix): SVGPathHandlerCnc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(DependSuffix) -MM SVGPathHandlerCnc.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGPathHandlerCnc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(PreprocessSuffix): SVGPathHandlerCnc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGPathHandlerCnc.cpp$(PreprocessSuffix) SVGPathHandlerCnc.cpp

$(IntermediateDirectory)/SVGPathHandlerBase.cpp$(ObjectSuffix): SVGPathHandlerBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGPathHandlerBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGPathHandlerBase.cpp$(DependSuffix) -MM SVGPathHandlerBase.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGPathHandlerBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGPathHandlerBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGPathHandlerBase.cpp$(PreprocessSuffix): SVGPathHandlerBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGPathHandlerBase.cpp$(PreprocessSuffix) SVGPathHandlerBase.cpp

$(IntermediateDirectory)/SVGFileParser.cpp$(ObjectSuffix): SVGFileParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGFileParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGFileParser.cpp$(DependSuffix) -MM SVGFileParser.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGFileParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGFileParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGFileParser.cpp$(PreprocessSuffix): SVGFileParser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGFileParser.cpp$(PreprocessSuffix) SVGFileParser.cpp

$(IntermediateDirectory)/SVGParserBase.cpp$(ObjectSuffix): SVGParserBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGParserBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGParserBase.cpp$(DependSuffix) -MM SVGParserBase.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGParserBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGParserBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGParserBase.cpp$(PreprocessSuffix): SVGParserBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGParserBase.cpp$(PreprocessSuffix) SVGParserBase.cpp

$(IntermediateDirectory)/CncFileView.cpp$(ObjectSuffix): CncFileView.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncFileView.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncFileView.cpp$(DependSuffix) -MM CncFileView.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncFileView.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncFileView.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncFileView.cpp$(PreprocessSuffix): CncFileView.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncFileView.cpp$(PreprocessSuffix) CncFileView.cpp

$(IntermediateDirectory)/CncZView.cpp$(ObjectSuffix): CncZView.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncZView.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncZView.cpp$(DependSuffix) -MM CncZView.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncZView.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncZView.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncZView.cpp$(PreprocessSuffix): CncZView.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncZView.cpp$(PreprocessSuffix) CncZView.cpp

$(IntermediateDirectory)/CncStreamBuffers.cpp$(ObjectSuffix): CncStreamBuffers.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncStreamBuffers.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncStreamBuffers.cpp$(DependSuffix) -MM CncStreamBuffers.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncStreamBuffers.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncStreamBuffers.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncStreamBuffers.cpp$(PreprocessSuffix): CncStreamBuffers.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncStreamBuffers.cpp$(PreprocessSuffix) CncStreamBuffers.cpp

$(IntermediateDirectory)/wxCrafterPodiumManagement_bitmaps.cpp$(ObjectSuffix): wxCrafterPodiumManagement_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterPodiumManagement_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterPodiumManagement_bitmaps.cpp$(DependSuffix) -MM wxCrafterPodiumManagement_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterPodiumManagement_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterPodiumManagement_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterPodiumManagement_bitmaps.cpp$(PreprocessSuffix): wxCrafterPodiumManagement_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterPodiumManagement_bitmaps.cpp$(PreprocessSuffix) wxCrafterPodiumManagement_bitmaps.cpp

$(IntermediateDirectory)/GCodeCommands.cpp$(ObjectSuffix): GCodeCommands.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodeCommands.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodeCommands.cpp$(DependSuffix) -MM GCodeCommands.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodeCommands.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodeCommands.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodeCommands.cpp$(PreprocessSuffix): GCodeCommands.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodeCommands.cpp$(PreprocessSuffix) GCodeCommands.cpp

$(IntermediateDirectory)/3D_CncGLCanvas.cpp$(ObjectSuffix): 3D/CncGLCanvas.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_CncGLCanvas.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_CncGLCanvas.cpp$(DependSuffix) -MM 3D/CncGLCanvas.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/CncGLCanvas.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_CncGLCanvas.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_CncGLCanvas.cpp$(PreprocessSuffix): 3D/CncGLCanvas.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_CncGLCanvas.cpp$(PreprocessSuffix) 3D/CncGLCanvas.cpp

$(IntermediateDirectory)/SerialCircularBuffer.cpp$(ObjectSuffix): SerialCircularBuffer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialCircularBuffer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialCircularBuffer.cpp$(DependSuffix) -MM SerialCircularBuffer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialCircularBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialCircularBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialCircularBuffer.cpp$(PreprocessSuffix): SerialCircularBuffer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialCircularBuffer.cpp$(PreprocessSuffix) SerialCircularBuffer.cpp

$(IntermediateDirectory)/CncControl.cpp$(ObjectSuffix): CncControl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncControl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncControl.cpp$(DependSuffix) -MM CncControl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncControl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncControl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncControl.cpp$(PreprocessSuffix): CncControl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncControl.cpp$(PreprocessSuffix) CncControl.cpp

$(IntermediateDirectory)/CncFilePreview.cpp$(ObjectSuffix): CncFilePreview.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncFilePreview.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncFilePreview.cpp$(DependSuffix) -MM CncFilePreview.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncFilePreview.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncFilePreview.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncFilePreview.cpp$(PreprocessSuffix): CncFilePreview.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncFilePreview.cpp$(PreprocessSuffix) CncFilePreview.cpp

$(IntermediateDirectory)/CncReferencePosition.cpp$(ObjectSuffix): CncReferencePosition.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncReferencePosition.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncReferencePosition.cpp$(DependSuffix) -MM CncReferencePosition.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncReferencePosition.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncReferencePosition.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncReferencePosition.cpp$(PreprocessSuffix): CncReferencePosition.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncReferencePosition.cpp$(PreprocessSuffix) CncReferencePosition.cpp

$(IntermediateDirectory)/ArduinoEnvWrapper.cpp$(ObjectSuffix): ArduinoEnvWrapper.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ArduinoEnvWrapper.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ArduinoEnvWrapper.cpp$(DependSuffix) -MM ArduinoEnvWrapper.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/ArduinoEnvWrapper.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ArduinoEnvWrapper.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ArduinoEnvWrapper.cpp$(PreprocessSuffix): ArduinoEnvWrapper.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ArduinoEnvWrapper.cpp$(PreprocessSuffix) ArduinoEnvWrapper.cpp

$(IntermediateDirectory)/CncMouseRemoteControl.cpp$(ObjectSuffix): CncMouseRemoteControl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncMouseRemoteControl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncMouseRemoteControl.cpp$(DependSuffix) -MM CncMouseRemoteControl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncMouseRemoteControl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncMouseRemoteControl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncMouseRemoteControl.cpp$(PreprocessSuffix): CncMouseRemoteControl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncMouseRemoteControl.cpp$(PreprocessSuffix) CncMouseRemoteControl.cpp

$(IntermediateDirectory)/CncGamepadDirectionPanel.cpp$(ObjectSuffix): CncGamepadDirectionPanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncGamepadDirectionPanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncGamepadDirectionPanel.cpp$(DependSuffix) -MM CncGamepadDirectionPanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncGamepadDirectionPanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncGamepadDirectionPanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncGamepadDirectionPanel.cpp$(PreprocessSuffix): CncGamepadDirectionPanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncGamepadDirectionPanel.cpp$(PreprocessSuffix) CncGamepadDirectionPanel.cpp

$(IntermediateDirectory)/CncNumberFormatter.cpp$(ObjectSuffix): CncNumberFormatter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncNumberFormatter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncNumberFormatter.cpp$(DependSuffix) -MM CncNumberFormatter.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncNumberFormatter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncNumberFormatter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncNumberFormatter.cpp$(PreprocessSuffix): CncNumberFormatter.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncNumberFormatter.cpp$(PreprocessSuffix) CncNumberFormatter.cpp

$(IntermediateDirectory)/CncNavigatorPanel.cpp$(ObjectSuffix): CncNavigatorPanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncNavigatorPanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncNavigatorPanel.cpp$(DependSuffix) -MM CncNavigatorPanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncNavigatorPanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncNavigatorPanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncNavigatorPanel.cpp$(PreprocessSuffix): CncNavigatorPanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncNavigatorPanel.cpp$(PreprocessSuffix) CncNavigatorPanel.cpp

$(IntermediateDirectory)/CncOSEnvironmentDialog.cpp$(ObjectSuffix): CncOSEnvironmentDialog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncOSEnvironmentDialog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncOSEnvironmentDialog.cpp$(DependSuffix) -MM CncOSEnvironmentDialog.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncOSEnvironmentDialog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncOSEnvironmentDialog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncOSEnvironmentDialog.cpp$(PreprocessSuffix): CncOSEnvironmentDialog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncOSEnvironmentDialog.cpp$(PreprocessSuffix) CncOSEnvironmentDialog.cpp

$(IntermediateDirectory)/CncSpeedMonitor.cpp$(ObjectSuffix): CncSpeedMonitor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSpeedMonitor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSpeedMonitor.cpp$(DependSuffix) -MM CncSpeedMonitor.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSpeedMonitor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSpeedMonitor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSpeedMonitor.cpp$(PreprocessSuffix): CncSpeedMonitor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSpeedMonitor.cpp$(PreprocessSuffix) CncSpeedMonitor.cpp

$(IntermediateDirectory)/CncAverage.cpp$(ObjectSuffix): CncAverage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncAverage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncAverage.cpp$(DependSuffix) -MM CncAverage.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncAverage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncAverage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncAverage.cpp$(PreprocessSuffix): CncAverage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncAverage.cpp$(PreprocessSuffix) CncAverage.cpp

$(IntermediateDirectory)/CncTemplateObserver.cpp$(ObjectSuffix): CncTemplateObserver.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncTemplateObserver.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncTemplateObserver.cpp$(DependSuffix) -MM CncTemplateObserver.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncTemplateObserver.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncTemplateObserver.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncTemplateObserver.cpp$(PreprocessSuffix): CncTemplateObserver.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncTemplateObserver.cpp$(PreprocessSuffix) CncTemplateObserver.cpp

$(IntermediateDirectory)/SerialSpyPort.cpp$(ObjectSuffix): SerialSpyPort.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialSpyPort.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialSpyPort.cpp$(DependSuffix) -MM SerialSpyPort.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialSpyPort.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialSpyPort.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialSpyPort.cpp$(PreprocessSuffix): SerialSpyPort.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialSpyPort.cpp$(PreprocessSuffix) SerialSpyPort.cpp

$(IntermediateDirectory)/CncMotionVertexTrace.cpp$(ObjectSuffix): CncMotionVertexTrace.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncMotionVertexTrace.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncMotionVertexTrace.cpp$(DependSuffix) -MM CncMotionVertexTrace.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncMotionVertexTrace.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncMotionVertexTrace.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncMotionVertexTrace.cpp$(PreprocessSuffix): CncMotionVertexTrace.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncMotionVertexTrace.cpp$(PreprocessSuffix) CncMotionVertexTrace.cpp

$(IntermediateDirectory)/CncOpenGLContextObserver.cpp$(ObjectSuffix): CncOpenGLContextObserver.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncOpenGLContextObserver.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncOpenGLContextObserver.cpp$(DependSuffix) -MM CncOpenGLContextObserver.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncOpenGLContextObserver.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncOpenGLContextObserver.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncOpenGLContextObserver.cpp$(PreprocessSuffix): CncOpenGLContextObserver.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncOpenGLContextObserver.cpp$(PreprocessSuffix) CncOpenGLContextObserver.cpp

$(IntermediateDirectory)/CncMonitorReplayPane.cpp$(ObjectSuffix): CncMonitorReplayPane.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncMonitorReplayPane.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncMonitorReplayPane.cpp$(DependSuffix) -MM CncMonitorReplayPane.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncMonitorReplayPane.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncMonitorReplayPane.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncMonitorReplayPane.cpp$(PreprocessSuffix): CncMonitorReplayPane.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncMonitorReplayPane.cpp$(PreprocessSuffix) CncMonitorReplayPane.cpp

$(IntermediateDirectory)/CncLastProcessingTimestampSummary.cpp$(ObjectSuffix): CncLastProcessingTimestampSummary.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncLastProcessingTimestampSummary.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncLastProcessingTimestampSummary.cpp$(DependSuffix) -MM CncLastProcessingTimestampSummary.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncLastProcessingTimestampSummary.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncLastProcessingTimestampSummary.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncLastProcessingTimestampSummary.cpp$(PreprocessSuffix): CncLastProcessingTimestampSummary.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncLastProcessingTimestampSummary.cpp$(PreprocessSuffix) CncLastProcessingTimestampSummary.cpp

$(IntermediateDirectory)/CncAutoProgressDialog.cpp$(ObjectSuffix): CncAutoProgressDialog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncAutoProgressDialog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncAutoProgressDialog.cpp$(DependSuffix) -MM CncAutoProgressDialog.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncAutoProgressDialog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncAutoProgressDialog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncAutoProgressDialog.cpp$(PreprocessSuffix): CncAutoProgressDialog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncAutoProgressDialog.cpp$(PreprocessSuffix) CncAutoProgressDialog.cpp

$(IntermediateDirectory)/CncSecureSlidepadDialog.cpp$(ObjectSuffix): CncSecureSlidepadDialog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSecureSlidepadDialog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSecureSlidepadDialog.cpp$(DependSuffix) -MM CncSecureSlidepadDialog.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSecureSlidepadDialog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSecureSlidepadDialog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSecureSlidepadDialog.cpp$(PreprocessSuffix): CncSecureSlidepadDialog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSecureSlidepadDialog.cpp$(PreprocessSuffix) CncSecureSlidepadDialog.cpp

$(IntermediateDirectory)/BinaryPathHandlerCnc.cpp$(ObjectSuffix): BinaryPathHandlerCnc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/BinaryPathHandlerCnc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/BinaryPathHandlerCnc.cpp$(DependSuffix) -MM BinaryPathHandlerCnc.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/BinaryPathHandlerCnc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/BinaryPathHandlerCnc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/BinaryPathHandlerCnc.cpp$(PreprocessSuffix): BinaryPathHandlerCnc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/BinaryPathHandlerCnc.cpp$(PreprocessSuffix) BinaryPathHandlerCnc.cpp

$(IntermediateDirectory)/CncPosition.cpp$(ObjectSuffix): CncPosition.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPosition.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPosition.cpp$(DependSuffix) -MM CncPosition.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPosition.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPosition.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPosition.cpp$(PreprocessSuffix): CncPosition.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPosition.cpp$(PreprocessSuffix) CncPosition.cpp

$(IntermediateDirectory)/CncUserEvents.cpp$(ObjectSuffix): CncUserEvents.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncUserEvents.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncUserEvents.cpp$(DependSuffix) -MM CncUserEvents.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncUserEvents.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncUserEvents.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncUserEvents.cpp$(PreprocessSuffix): CncUserEvents.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncUserEvents.cpp$(PreprocessSuffix) CncUserEvents.cpp

$(IntermediateDirectory)/CncToolStateControl.cpp$(ObjectSuffix): CncToolStateControl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncToolStateControl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncToolStateControl.cpp$(DependSuffix) -MM CncToolStateControl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncToolStateControl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncToolStateControl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncToolStateControl.cpp$(PreprocessSuffix): CncToolStateControl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncToolStateControl.cpp$(PreprocessSuffix) CncToolStateControl.cpp

$(IntermediateDirectory)/CncSpeedPlayground.cpp$(ObjectSuffix): CncSpeedPlayground.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSpeedPlayground.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSpeedPlayground.cpp$(DependSuffix) -MM CncSpeedPlayground.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSpeedPlayground.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSpeedPlayground.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSpeedPlayground.cpp$(PreprocessSuffix): CncSpeedPlayground.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSpeedPlayground.cpp$(PreprocessSuffix) CncSpeedPlayground.cpp

$(IntermediateDirectory)/CncSpeedAccelerationDiagram.cpp$(ObjectSuffix): CncSpeedAccelerationDiagram.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSpeedAccelerationDiagram.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSpeedAccelerationDiagram.cpp$(DependSuffix) -MM CncSpeedAccelerationDiagram.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSpeedAccelerationDiagram.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSpeedAccelerationDiagram.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSpeedAccelerationDiagram.cpp$(PreprocessSuffix): CncSpeedAccelerationDiagram.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSpeedAccelerationDiagram.cpp$(PreprocessSuffix) CncSpeedAccelerationDiagram.cpp

$(IntermediateDirectory)/CncBaseEditor.cpp$(ObjectSuffix): CncBaseEditor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncBaseEditor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncBaseEditor.cpp$(DependSuffix) -MM CncBaseEditor.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncBaseEditor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncBaseEditor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncBaseEditor.cpp$(PreprocessSuffix): CncBaseEditor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncBaseEditor.cpp$(PreprocessSuffix) CncBaseEditor.cpp

$(IntermediateDirectory)/CncFloatingPointValidator.cpp$(ObjectSuffix): CncFloatingPointValidator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncFloatingPointValidator.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncFloatingPointValidator.cpp$(DependSuffix) -MM CncFloatingPointValidator.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncFloatingPointValidator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncFloatingPointValidator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncFloatingPointValidator.cpp$(PreprocessSuffix): CncFloatingPointValidator.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncFloatingPointValidator.cpp$(PreprocessSuffix) CncFloatingPointValidator.cpp

$(IntermediateDirectory)/CncManuallyMoveCoordinates.cpp$(ObjectSuffix): CncManuallyMoveCoordinates.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncManuallyMoveCoordinates.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncManuallyMoveCoordinates.cpp$(DependSuffix) -MM CncManuallyMoveCoordinates.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncManuallyMoveCoordinates.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncManuallyMoveCoordinates.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncManuallyMoveCoordinates.cpp$(PreprocessSuffix): CncManuallyMoveCoordinates.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncManuallyMoveCoordinates.cpp$(PreprocessSuffix) CncManuallyMoveCoordinates.cpp

$(IntermediateDirectory)/CncFilePreviewWnd.cpp$(ObjectSuffix): CncFilePreviewWnd.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncFilePreviewWnd.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncFilePreviewWnd.cpp$(DependSuffix) -MM CncFilePreviewWnd.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncFilePreviewWnd.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncFilePreviewWnd.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncFilePreviewWnd.cpp$(PreprocessSuffix): CncFilePreviewWnd.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncFilePreviewWnd.cpp$(PreprocessSuffix) CncFilePreviewWnd.cpp

$(IntermediateDirectory)/CncArduino.cpp$(ObjectSuffix): CncArduino.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncArduino.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncArduino.cpp$(DependSuffix) -MM CncArduino.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncArduino.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncArduino.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncArduino.cpp$(PreprocessSuffix): CncArduino.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncArduino.cpp$(PreprocessSuffix) CncArduino.cpp

$(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(ObjectSuffix): Codelite/wxPNGAnimation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(DependSuffix) -MM Codelite/wxPNGAnimation.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/Codelite/wxPNGAnimation.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(PreprocessSuffix): Codelite/wxPNGAnimation.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Codelite_wxPNGAnimation.cpp$(PreprocessSuffix) Codelite/wxPNGAnimation.cpp

$(IntermediateDirectory)/CncCommon.cpp$(ObjectSuffix): CncCommon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncCommon.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncCommon.cpp$(DependSuffix) -MM CncCommon.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncCommon.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncCommon.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncCommon.cpp$(PreprocessSuffix): CncCommon.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncCommon.cpp$(PreprocessSuffix) CncCommon.cpp

$(IntermediateDirectory)/CncPositionStorage.cpp$(ObjectSuffix): CncPositionStorage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPositionStorage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPositionStorage.cpp$(DependSuffix) -MM CncPositionStorage.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPositionStorage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPositionStorage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPositionStorage.cpp$(PreprocessSuffix): CncPositionStorage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPositionStorage.cpp$(PreprocessSuffix) CncPositionStorage.cpp

$(IntermediateDirectory)/CncCircularBuffer.cpp$(ObjectSuffix): CncCircularBuffer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncCircularBuffer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncCircularBuffer.cpp$(DependSuffix) -MM CncCircularBuffer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncCircularBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncCircularBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncCircularBuffer.cpp$(PreprocessSuffix): CncCircularBuffer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncCircularBuffer.cpp$(PreprocessSuffix) CncCircularBuffer.cpp

$(IntermediateDirectory)/WireEndPoint.cpp$(ObjectSuffix): WireEndPoint.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/WireEndPoint.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/WireEndPoint.cpp$(DependSuffix) -MM WireEndPoint.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/WireEndPoint.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/WireEndPoint.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/WireEndPoint.cpp$(PreprocessSuffix): WireEndPoint.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/WireEndPoint.cpp$(PreprocessSuffix) WireEndPoint.cpp

$(IntermediateDirectory)/CncSpeedSlider.cpp$(ObjectSuffix): CncSpeedSlider.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSpeedSlider.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSpeedSlider.cpp$(DependSuffix) -MM CncSpeedSlider.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSpeedSlider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSpeedSlider.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSpeedSlider.cpp$(PreprocessSuffix): CncSpeedSlider.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSpeedSlider.cpp$(PreprocessSuffix) CncSpeedSlider.cpp

$(IntermediateDirectory)/CncAutoFreezer.cpp$(ObjectSuffix): CncAutoFreezer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncAutoFreezer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncAutoFreezer.cpp$(DependSuffix) -MM CncAutoFreezer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncAutoFreezer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncAutoFreezer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncAutoFreezer.cpp$(PreprocessSuffix): CncAutoFreezer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncAutoFreezer.cpp$(PreprocessSuffix) CncAutoFreezer.cpp

$(IntermediateDirectory)/CncPolarRegionDetector.cpp$(ObjectSuffix): CncPolarRegionDetector.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPolarRegionDetector.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPolarRegionDetector.cpp$(DependSuffix) -MM CncPolarRegionDetector.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPolarRegionDetector.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPolarRegionDetector.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPolarRegionDetector.cpp$(PreprocessSuffix): CncPolarRegionDetector.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPolarRegionDetector.cpp$(PreprocessSuffix) CncPolarRegionDetector.cpp

$(IntermediateDirectory)/CncClipperLib.cpp$(ObjectSuffix): CncClipperLib.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncClipperLib.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncClipperLib.cpp$(DependSuffix) -MM CncClipperLib.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncClipperLib.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncClipperLib.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncClipperLib.cpp$(PreprocessSuffix): CncClipperLib.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncClipperLib.cpp$(PreprocessSuffix) CncClipperLib.cpp

$(IntermediateDirectory)/SerialThread.cpp$(ObjectSuffix): SerialThread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialThread.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialThread.cpp$(DependSuffix) -MM SerialThread.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialThread.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialThread.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialThread.cpp$(PreprocessSuffix): SerialThread.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialThread.cpp$(PreprocessSuffix) SerialThread.cpp

$(IntermediateDirectory)/CncStringLogger.cpp$(ObjectSuffix): CncStringLogger.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncStringLogger.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncStringLogger.cpp$(DependSuffix) -MM CncStringLogger.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncStringLogger.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncStringLogger.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncStringLogger.cpp$(PreprocessSuffix): CncStringLogger.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncStringLogger.cpp$(PreprocessSuffix) CncStringLogger.cpp

$(IntermediateDirectory)/wxCrafterGamePort_bitmaps.cpp$(ObjectSuffix): wxCrafterGamePort_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterGamePort_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterGamePort_bitmaps.cpp$(DependSuffix) -MM wxCrafterGamePort_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterGamePort_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterGamePort_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterGamePort_bitmaps.cpp$(PreprocessSuffix): wxCrafterGamePort_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterGamePort_bitmaps.cpp$(PreprocessSuffix) wxCrafterGamePort_bitmaps.cpp

$(IntermediateDirectory)/CncSecureRotateModelPanel.cpp$(ObjectSuffix): CncSecureRotateModelPanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSecureRotateModelPanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSecureRotateModelPanel.cpp$(DependSuffix) -MM CncSecureRotateModelPanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSecureRotateModelPanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSecureRotateModelPanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSecureRotateModelPanel.cpp$(PreprocessSuffix): CncSecureRotateModelPanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSecureRotateModelPanel.cpp$(PreprocessSuffix) CncSecureRotateModelPanel.cpp

$(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(ObjectSuffix): CncPosSpyListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(DependSuffix) -MM CncPosSpyListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPosSpyListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(PreprocessSuffix): CncPosSpyListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPosSpyListCtrl.cpp$(PreprocessSuffix) CncPosSpyListCtrl.cpp

$(IntermediateDirectory)/CncPerspective.cpp$(ObjectSuffix): CncPerspective.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPerspective.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPerspective.cpp$(DependSuffix) -MM CncPerspective.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPerspective.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPerspective.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPerspective.cpp$(PreprocessSuffix): CncPerspective.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPerspective.cpp$(PreprocessSuffix) CncPerspective.cpp

$(IntermediateDirectory)/wxCrafterSpeedMonitor.cpp$(ObjectSuffix): wxCrafterSpeedMonitor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterSpeedMonitor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterSpeedMonitor.cpp$(DependSuffix) -MM wxCrafterSpeedMonitor.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterSpeedMonitor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterSpeedMonitor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterSpeedMonitor.cpp$(PreprocessSuffix): wxCrafterSpeedMonitor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterSpeedMonitor.cpp$(PreprocessSuffix) wxCrafterSpeedMonitor.cpp

$(IntermediateDirectory)/CncPreprocessor.cpp$(ObjectSuffix): CncPreprocessor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPreprocessor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPreprocessor.cpp$(DependSuffix) -MM CncPreprocessor.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPreprocessor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPreprocessor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPreprocessor.cpp$(PreprocessSuffix): CncPreprocessor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPreprocessor.cpp$(PreprocessSuffix) CncPreprocessor.cpp

$(IntermediateDirectory)/SerialEmulatorSvgStreamer.cpp$(ObjectSuffix): SerialEmulatorSvgStreamer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialEmulatorSvgStreamer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialEmulatorSvgStreamer.cpp$(DependSuffix) -MM SerialEmulatorSvgStreamer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialEmulatorSvgStreamer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialEmulatorSvgStreamer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialEmulatorSvgStreamer.cpp$(PreprocessSuffix): SerialEmulatorSvgStreamer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialEmulatorSvgStreamer.cpp$(PreprocessSuffix) SerialEmulatorSvgStreamer.cpp

$(IntermediateDirectory)/CncTextCtrl.cpp$(ObjectSuffix): CncTextCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncTextCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncTextCtrl.cpp$(DependSuffix) -MM CncTextCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncTextCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncTextCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncTextCtrl.cpp$(PreprocessSuffix): CncTextCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncTextCtrl.cpp$(PreprocessSuffix) CncTextCtrl.cpp

$(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(ObjectSuffix): 3D/GLContextGCodePreview.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(DependSuffix) -MM 3D/GLContextGCodePreview.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextGCodePreview.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(PreprocessSuffix): 3D/GLContextGCodePreview.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLContextGCodePreview.cpp$(PreprocessSuffix) 3D/GLContextGCodePreview.cpp

$(IntermediateDirectory)/CncLoggerView.cpp$(ObjectSuffix): CncLoggerView.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncLoggerView.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncLoggerView.cpp$(DependSuffix) -MM CncLoggerView.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncLoggerView.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncLoggerView.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncLoggerView.cpp$(PreprocessSuffix): CncLoggerView.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncLoggerView.cpp$(PreprocessSuffix) CncLoggerView.cpp

$(IntermediateDirectory)/CncMonitorSplitterWindow.cpp$(ObjectSuffix): CncMonitorSplitterWindow.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncMonitorSplitterWindow.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncMonitorSplitterWindow.cpp$(DependSuffix) -MM CncMonitorSplitterWindow.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncMonitorSplitterWindow.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncMonitorSplitterWindow.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncMonitorSplitterWindow.cpp$(PreprocessSuffix): CncMonitorSplitterWindow.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncMonitorSplitterWindow.cpp$(PreprocessSuffix) CncMonitorSplitterWindow.cpp

$(IntermediateDirectory)/CncMessageListCtrl.cpp$(ObjectSuffix): CncMessageListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncMessageListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncMessageListCtrl.cpp$(DependSuffix) -MM CncMessageListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncMessageListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncMessageListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncMessageListCtrl.cpp$(PreprocessSuffix): CncMessageListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncMessageListCtrl.cpp$(PreprocessSuffix) CncMessageListCtrl.cpp

$(IntermediateDirectory)/CncConnectProgress.cpp$(ObjectSuffix): CncConnectProgress.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncConnectProgress.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncConnectProgress.cpp$(DependSuffix) -MM CncConnectProgress.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncConnectProgress.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncConnectProgress.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncConnectProgress.cpp$(PreprocessSuffix): CncConnectProgress.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncConnectProgress.cpp$(PreprocessSuffix) CncConnectProgress.cpp

$(IntermediateDirectory)/ArduinoData.cpp$(ObjectSuffix): ArduinoData.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ArduinoData.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ArduinoData.cpp$(DependSuffix) -MM ArduinoData.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/ArduinoData.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ArduinoData.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ArduinoData.cpp$(PreprocessSuffix): ArduinoData.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ArduinoData.cpp$(PreprocessSuffix) ArduinoData.cpp

$(IntermediateDirectory)/CncCurveLib.cpp$(ObjectSuffix): CncCurveLib.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncCurveLib.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncCurveLib.cpp$(DependSuffix) -MM CncCurveLib.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncCurveLib.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncCurveLib.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncCurveLib.cpp$(PreprocessSuffix): CncCurveLib.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncCurveLib.cpp$(PreprocessSuffix) CncCurveLib.cpp

$(IntermediateDirectory)/CncLoggerListCtrl.cpp$(ObjectSuffix): CncLoggerListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncLoggerListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncLoggerListCtrl.cpp$(DependSuffix) -MM CncLoggerListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncLoggerListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncLoggerListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncLoggerListCtrl.cpp$(PreprocessSuffix): CncLoggerListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncLoggerListCtrl.cpp$(PreprocessSuffix) CncLoggerListCtrl.cpp

$(IntermediateDirectory)/win_resources.rc$(ObjectSuffix): win_resources.rc
	$(RcCompilerName) -i "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/win_resources.rc" $(RcCmpOptions)   $(ObjectSwitch)$(IntermediateDirectory)/win_resources.rc$(ObjectSuffix) $(RcIncludePath)
$(IntermediateDirectory)/wxCrafterUnitTest_bitmaps.cpp$(ObjectSuffix): wxCrafterUnitTest_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterUnitTest_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterUnitTest_bitmaps.cpp$(DependSuffix) -MM wxCrafterUnitTest_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterUnitTest_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterUnitTest_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterUnitTest_bitmaps.cpp$(PreprocessSuffix): wxCrafterUnitTest_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterUnitTest_bitmaps.cpp$(PreprocessSuffix) wxCrafterUnitTest_bitmaps.cpp

$(IntermediateDirectory)/CncAnchorPosListCtrl.cpp$(ObjectSuffix): CncAnchorPosListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncAnchorPosListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncAnchorPosListCtrl.cpp$(DependSuffix) -MM CncAnchorPosListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncAnchorPosListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncAnchorPosListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncAnchorPosListCtrl.cpp$(PreprocessSuffix): CncAnchorPosListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncAnchorPosListCtrl.cpp$(PreprocessSuffix) CncAnchorPosListCtrl.cpp

$(IntermediateDirectory)/CncArduinoPinsListCtrl.cpp$(ObjectSuffix): CncArduinoPinsListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncArduinoPinsListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncArduinoPinsListCtrl.cpp$(DependSuffix) -MM CncArduinoPinsListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncArduinoPinsListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncArduinoPinsListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncArduinoPinsListCtrl.cpp$(PreprocessSuffix): CncArduinoPinsListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncArduinoPinsListCtrl.cpp$(PreprocessSuffix) CncArduinoPinsListCtrl.cpp

$(IntermediateDirectory)/CncSecureGesturesPanel.cpp$(ObjectSuffix): CncSecureGesturesPanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSecureGesturesPanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSecureGesturesPanel.cpp$(DependSuffix) -MM CncSecureGesturesPanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSecureGesturesPanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSecureGesturesPanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSecureGesturesPanel.cpp$(PreprocessSuffix): CncSecureGesturesPanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSecureGesturesPanel.cpp$(PreprocessSuffix) CncSecureGesturesPanel.cpp

$(IntermediateDirectory)/GL3DOptionPane.cpp$(ObjectSuffix): GL3DOptionPane.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GL3DOptionPane.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GL3DOptionPane.cpp$(DependSuffix) -MM GL3DOptionPane.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GL3DOptionPane.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GL3DOptionPane.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GL3DOptionPane.cpp$(PreprocessSuffix): GL3DOptionPane.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GL3DOptionPane.cpp$(PreprocessSuffix) GL3DOptionPane.cpp

$(IntermediateDirectory)/CncInfoBar.cpp$(ObjectSuffix): CncInfoBar.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncInfoBar.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncInfoBar.cpp$(DependSuffix) -MM CncInfoBar.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncInfoBar.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncInfoBar.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncInfoBar.cpp$(PreprocessSuffix): CncInfoBar.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncInfoBar.cpp$(PreprocessSuffix) CncInfoBar.cpp

$(IntermediateDirectory)/CncTemplateContextSummaryPanel.cpp$(ObjectSuffix): CncTemplateContextSummaryPanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncTemplateContextSummaryPanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncTemplateContextSummaryPanel.cpp$(DependSuffix) -MM CncTemplateContextSummaryPanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncTemplateContextSummaryPanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncTemplateContextSummaryPanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncTemplateContextSummaryPanel.cpp$(PreprocessSuffix): CncTemplateContextSummaryPanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncTemplateContextSummaryPanel.cpp$(PreprocessSuffix) CncTemplateContextSummaryPanel.cpp

$(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(ObjectSuffix): OSD/CncUsbPortScanner.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(DependSuffix) -MM OSD/CncUsbPortScanner.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/OSD/CncUsbPortScanner.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(PreprocessSuffix): OSD/CncUsbPortScanner.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OSD_CncUsbPortScanner.cpp$(PreprocessSuffix) OSD/CncUsbPortScanner.cpp

$(IntermediateDirectory)/CncUnitTests.cpp$(ObjectSuffix): CncUnitTests.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncUnitTests.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncUnitTests.cpp$(DependSuffix) -MM CncUnitTests.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncUnitTests.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncUnitTests.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncUnitTests.cpp$(PreprocessSuffix): CncUnitTests.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncUnitTests.cpp$(PreprocessSuffix) CncUnitTests.cpp

$(IntermediateDirectory)/CncSerialSpyPanel.cpp$(ObjectSuffix): CncSerialSpyPanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSerialSpyPanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSerialSpyPanel.cpp$(DependSuffix) -MM CncSerialSpyPanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSerialSpyPanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSerialSpyPanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSerialSpyPanel.cpp$(PreprocessSuffix): CncSerialSpyPanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSerialSpyPanel.cpp$(PreprocessSuffix) CncSerialSpyPanel.cpp

$(IntermediateDirectory)/CncProcessingInfo.cpp$(ObjectSuffix): CncProcessingInfo.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncProcessingInfo.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncProcessingInfo.cpp$(DependSuffix) -MM CncProcessingInfo.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncProcessingInfo.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncProcessingInfo.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncProcessingInfo.cpp$(PreprocessSuffix): CncProcessingInfo.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncProcessingInfo.cpp$(PreprocessSuffix) CncProcessingInfo.cpp

$(IntermediateDirectory)/CncOutboundEditor.cpp$(ObjectSuffix): CncOutboundEditor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncOutboundEditor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncOutboundEditor.cpp$(DependSuffix) -MM CncOutboundEditor.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncOutboundEditor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncOutboundEditor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncOutboundEditor.cpp$(PreprocessSuffix): CncOutboundEditor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncOutboundEditor.cpp$(PreprocessSuffix) CncOutboundEditor.cpp

$(IntermediateDirectory)/CncSecureCtrlPanel.cpp$(ObjectSuffix): CncSecureCtrlPanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSecureCtrlPanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSecureCtrlPanel.cpp$(DependSuffix) -MM CncSecureCtrlPanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSecureCtrlPanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSecureCtrlPanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSecureCtrlPanel.cpp$(PreprocessSuffix): CncSecureCtrlPanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSecureCtrlPanel.cpp$(PreprocessSuffix) CncSecureCtrlPanel.cpp

$(IntermediateDirectory)/3D_CncGCodePreview.cpp$(ObjectSuffix): 3D/CncGCodePreview.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_CncGCodePreview.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_CncGCodePreview.cpp$(DependSuffix) -MM 3D/CncGCodePreview.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/CncGCodePreview.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_CncGCodePreview.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_CncGCodePreview.cpp$(PreprocessSuffix): 3D/CncGCodePreview.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_CncGCodePreview.cpp$(PreprocessSuffix) 3D/CncGCodePreview.cpp

$(IntermediateDirectory)/CncBinaryTemplateStreamer.cpp$(ObjectSuffix): CncBinaryTemplateStreamer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncBinaryTemplateStreamer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncBinaryTemplateStreamer.cpp$(DependSuffix) -MM CncBinaryTemplateStreamer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncBinaryTemplateStreamer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncBinaryTemplateStreamer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncBinaryTemplateStreamer.cpp$(PreprocessSuffix): CncBinaryTemplateStreamer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncBinaryTemplateStreamer.cpp$(PreprocessSuffix) CncBinaryTemplateStreamer.cpp

$(IntermediateDirectory)/CncMoveSequence.cpp$(ObjectSuffix): CncMoveSequence.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncMoveSequence.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncMoveSequence.cpp$(DependSuffix) -MM CncMoveSequence.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncMoveSequence.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncMoveSequence.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncMoveSequence.cpp$(PreprocessSuffix): CncMoveSequence.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncMoveSequence.cpp$(PreprocessSuffix) CncMoveSequence.cpp

$(IntermediateDirectory)/wxCrafterToolMagazine_bitmaps.cpp$(ObjectSuffix): wxCrafterToolMagazine_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterToolMagazine_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterToolMagazine_bitmaps.cpp$(DependSuffix) -MM wxCrafterToolMagazine_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterToolMagazine_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterToolMagazine_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterToolMagazine_bitmaps.cpp$(PreprocessSuffix): wxCrafterToolMagazine_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterToolMagazine_bitmaps.cpp$(PreprocessSuffix) wxCrafterToolMagazine_bitmaps.cpp

$(IntermediateDirectory)/CncStatisticsPane.cpp$(ObjectSuffix): CncStatisticsPane.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncStatisticsPane.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncStatisticsPane.cpp$(DependSuffix) -MM CncStatisticsPane.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncStatisticsPane.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncStatisticsPane.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncStatisticsPane.cpp$(PreprocessSuffix): CncStatisticsPane.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncStatisticsPane.cpp$(PreprocessSuffix) CncStatisticsPane.cpp

$(IntermediateDirectory)/wxCrafterPathGenerator.cpp$(ObjectSuffix): wxCrafterPathGenerator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterPathGenerator.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterPathGenerator.cpp$(DependSuffix) -MM wxCrafterPathGenerator.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterPathGenerator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterPathGenerator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterPathGenerator.cpp$(PreprocessSuffix): wxCrafterPathGenerator.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterPathGenerator.cpp$(PreprocessSuffix) wxCrafterPathGenerator.cpp

$(IntermediateDirectory)/wxCrafterMotionMonitor.cpp$(ObjectSuffix): wxCrafterMotionMonitor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterMotionMonitor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterMotionMonitor.cpp$(DependSuffix) -MM wxCrafterMotionMonitor.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterMotionMonitor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterMotionMonitor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterMotionMonitor.cpp$(PreprocessSuffix): wxCrafterMotionMonitor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterMotionMonitor.cpp$(PreprocessSuffix) wxCrafterMotionMonitor.cpp

$(IntermediateDirectory)/CncSetterListCtrl.cpp$(ObjectSuffix): CncSetterListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSetterListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSetterListCtrl.cpp$(DependSuffix) -MM CncSetterListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSetterListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSetterListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSetterListCtrl.cpp$(PreprocessSuffix): CncSetterListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSetterListCtrl.cpp$(PreprocessSuffix) CncSetterListCtrl.cpp

$(IntermediateDirectory)/CncPathListInterface.cpp$(ObjectSuffix): CncPathListInterface.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPathListInterface.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPathListInterface.cpp$(DependSuffix) -MM CncPathListInterface.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPathListInterface.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPathListInterface.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPathListInterface.cpp$(PreprocessSuffix): CncPathListInterface.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPathListInterface.cpp$(PreprocessSuffix) CncPathListInterface.cpp

$(IntermediateDirectory)/CncFileNameService.cpp$(ObjectSuffix): CncFileNameService.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncFileNameService.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncFileNameService.cpp$(DependSuffix) -MM CncFileNameService.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncFileNameService.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncFileNameService.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncFileNameService.cpp$(PreprocessSuffix): CncFileNameService.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncFileNameService.cpp$(PreprocessSuffix) CncFileNameService.cpp

$(IntermediateDirectory)/CncSecureSlidepad.cpp$(ObjectSuffix): CncSecureSlidepad.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSecureSlidepad.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSecureSlidepad.cpp$(DependSuffix) -MM CncSecureSlidepad.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSecureSlidepad.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSecureSlidepad.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSecureSlidepad.cpp$(PreprocessSuffix): CncSecureSlidepad.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSecureSlidepad.cpp$(PreprocessSuffix) CncSecureSlidepad.cpp

$(IntermediateDirectory)/CncSecureManuallyMovePanel.cpp$(ObjectSuffix): CncSecureManuallyMovePanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSecureManuallyMovePanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSecureManuallyMovePanel.cpp$(DependSuffix) -MM CncSecureManuallyMovePanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSecureManuallyMovePanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSecureManuallyMovePanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSecureManuallyMovePanel.cpp$(PreprocessSuffix): CncSecureManuallyMovePanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSecureManuallyMovePanel.cpp$(PreprocessSuffix) CncSecureManuallyMovePanel.cpp

$(IntermediateDirectory)/SerialEndPoint.cpp$(ObjectSuffix): SerialEndPoint.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialEndPoint.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialEndPoint.cpp$(DependSuffix) -MM SerialEndPoint.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialEndPoint.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialEndPoint.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialEndPoint.cpp$(PreprocessSuffix): SerialEndPoint.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialEndPoint.cpp$(PreprocessSuffix) SerialEndPoint.cpp

$(IntermediateDirectory)/CncConfigProperties.cpp$(ObjectSuffix): CncConfigProperties.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncConfigProperties.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncConfigProperties.cpp$(DependSuffix) -MM CncConfigProperties.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncConfigProperties.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncConfigProperties.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncConfigProperties.cpp$(PreprocessSuffix): CncConfigProperties.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncConfigProperties.cpp$(PreprocessSuffix) CncConfigProperties.cpp

$(IntermediateDirectory)/CncSerialSpyListCtrl.cpp$(ObjectSuffix): CncSerialSpyListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSerialSpyListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSerialSpyListCtrl.cpp$(DependSuffix) -MM CncSerialSpyListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSerialSpyListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSerialSpyListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSerialSpyListCtrl.cpp$(PreprocessSuffix): CncSerialSpyListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSerialSpyListCtrl.cpp$(PreprocessSuffix) CncSerialSpyListCtrl.cpp

$(IntermediateDirectory)/CncTemplateContextSummaryDialog.cpp$(ObjectSuffix): CncTemplateContextSummaryDialog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncTemplateContextSummaryDialog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncTemplateContextSummaryDialog.cpp$(DependSuffix) -MM CncTemplateContextSummaryDialog.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncTemplateContextSummaryDialog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncTemplateContextSummaryDialog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncTemplateContextSummaryDialog.cpp$(PreprocessSuffix): CncTemplateContextSummaryDialog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncTemplateContextSummaryDialog.cpp$(PreprocessSuffix) CncTemplateContextSummaryDialog.cpp

$(IntermediateDirectory)/CncFileDialog.cpp$(ObjectSuffix): CncFileDialog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncFileDialog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncFileDialog.cpp$(DependSuffix) -MM CncFileDialog.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncFileDialog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncFileDialog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncFileDialog.cpp$(PreprocessSuffix): CncFileDialog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncFileDialog.cpp$(PreprocessSuffix) CncFileDialog.cpp

$(IntermediateDirectory)/3D_GLLabelCluser.cpp$(ObjectSuffix): 3D/GLLabelCluser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLLabelCluser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLLabelCluser.cpp$(DependSuffix) -MM 3D/GLLabelCluser.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLLabelCluser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLLabelCluser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLLabelCluser.cpp$(PreprocessSuffix): 3D/GLLabelCluser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLLabelCluser.cpp$(PreprocessSuffix) 3D/GLLabelCluser.cpp

$(IntermediateDirectory)/wxCrafterArduinoEnv_bitmaps.cpp$(ObjectSuffix): wxCrafterArduinoEnv_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterArduinoEnv_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterArduinoEnv_bitmaps.cpp$(DependSuffix) -MM wxCrafterArduinoEnv_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterArduinoEnv_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterArduinoEnv_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterArduinoEnv_bitmaps.cpp$(PreprocessSuffix): wxCrafterArduinoEnv_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterArduinoEnv_bitmaps.cpp$(PreprocessSuffix) wxCrafterArduinoEnv_bitmaps.cpp

$(IntermediateDirectory)/CncReferenceEvaluation.cpp$(ObjectSuffix): CncReferenceEvaluation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncReferenceEvaluation.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncReferenceEvaluation.cpp$(DependSuffix) -MM CncReferenceEvaluation.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncReferenceEvaluation.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncReferenceEvaluation.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncReferenceEvaluation.cpp$(PreprocessSuffix): CncReferenceEvaluation.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncReferenceEvaluation.cpp$(PreprocessSuffix) CncReferenceEvaluation.cpp

$(IntermediateDirectory)/SerialEmulatorNull.cpp$(ObjectSuffix): SerialEmulatorNull.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialEmulatorNull.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialEmulatorNull.cpp$(DependSuffix) -MM SerialEmulatorNull.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialEmulatorNull.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialEmulatorNull.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialEmulatorNull.cpp$(PreprocessSuffix): SerialEmulatorNull.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialEmulatorNull.cpp$(PreprocessSuffix) SerialEmulatorNull.cpp

$(IntermediateDirectory)/GamepadThread.cpp$(ObjectSuffix): GamepadThread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GamepadThread.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GamepadThread.cpp$(DependSuffix) -MM GamepadThread.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GamepadThread.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GamepadThread.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GamepadThread.cpp$(PreprocessSuffix): GamepadThread.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GamepadThread.cpp$(PreprocessSuffix) GamepadThread.cpp

$(IntermediateDirectory)/CncVideoCapturePanel.cpp$(ObjectSuffix): CncVideoCapturePanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncVideoCapturePanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncVideoCapturePanel.cpp$(DependSuffix) -MM CncVideoCapturePanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncVideoCapturePanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncVideoCapturePanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncVideoCapturePanel.cpp$(PreprocessSuffix): CncVideoCapturePanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncVideoCapturePanel.cpp$(PreprocessSuffix) CncVideoCapturePanel.cpp

$(IntermediateDirectory)/CncBoundarySpace.cpp$(ObjectSuffix): CncBoundarySpace.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncBoundarySpace.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncBoundarySpace.cpp$(DependSuffix) -MM CncBoundarySpace.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncBoundarySpace.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncBoundarySpace.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncBoundarySpace.cpp$(PreprocessSuffix): CncBoundarySpace.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncBoundarySpace.cpp$(PreprocessSuffix) CncBoundarySpace.cpp

$(IntermediateDirectory)/CncSourceEditor.cpp$(ObjectSuffix): CncSourceEditor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSourceEditor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSourceEditor.cpp$(DependSuffix) -MM CncSourceEditor.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSourceEditor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSourceEditor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSourceEditor.cpp$(PreprocessSuffix): CncSourceEditor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSourceEditor.cpp$(PreprocessSuffix) CncSourceEditor.cpp

$(IntermediateDirectory)/CncFlowPositionConverter.cpp$(ObjectSuffix): CncFlowPositionConverter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncFlowPositionConverter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncFlowPositionConverter.cpp$(DependSuffix) -MM CncFlowPositionConverter.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncFlowPositionConverter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncFlowPositionConverter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncFlowPositionConverter.cpp$(PreprocessSuffix): CncFlowPositionConverter.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncFlowPositionConverter.cpp$(PreprocessSuffix) CncFlowPositionConverter.cpp

$(IntermediateDirectory)/CncSecureScrollButton.cpp$(ObjectSuffix): CncSecureScrollButton.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSecureScrollButton.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSecureScrollButton.cpp$(DependSuffix) -MM CncSecureScrollButton.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSecureScrollButton.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSecureScrollButton.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSecureScrollButton.cpp$(PreprocessSuffix): CncSecureScrollButton.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSecureScrollButton.cpp$(PreprocessSuffix) CncSecureScrollButton.cpp

$(IntermediateDirectory)/3D_GLGuidePath.cpp$(ObjectSuffix): 3D/GLGuidePath.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLGuidePath.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLGuidePath.cpp$(DependSuffix) -MM 3D/GLGuidePath.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLGuidePath.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLGuidePath.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLGuidePath.cpp$(PreprocessSuffix): 3D/GLGuidePath.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLGuidePath.cpp$(PreprocessSuffix) 3D/GLGuidePath.cpp

$(IntermediateDirectory)/CncTouchBlockDetector.cpp$(ObjectSuffix): CncTouchBlockDetector.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncTouchBlockDetector.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncTouchBlockDetector.cpp$(DependSuffix) -MM CncTouchBlockDetector.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncTouchBlockDetector.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncTouchBlockDetector.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncTouchBlockDetector.cpp$(PreprocessSuffix): CncTouchBlockDetector.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncTouchBlockDetector.cpp$(PreprocessSuffix) CncTouchBlockDetector.cpp

$(IntermediateDirectory)/CncSvgControl.cpp$(ObjectSuffix): CncSvgControl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSvgControl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSvgControl.cpp$(DependSuffix) -MM CncSvgControl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSvgControl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSvgControl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSvgControl.cpp$(PreprocessSuffix): CncSvgControl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSvgControl.cpp$(PreprocessSuffix) CncSvgControl.cpp

$(IntermediateDirectory)/wxCrafterTemplateObserver.cpp$(ObjectSuffix): wxCrafterTemplateObserver.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterTemplateObserver.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterTemplateObserver.cpp$(DependSuffix) -MM wxCrafterTemplateObserver.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterTemplateObserver.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterTemplateObserver.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterTemplateObserver.cpp$(PreprocessSuffix): wxCrafterTemplateObserver.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterTemplateObserver.cpp$(PreprocessSuffix) wxCrafterTemplateObserver.cpp

$(IntermediateDirectory)/wxCrafterContextObserver_bitmaps.cpp$(ObjectSuffix): wxCrafterContextObserver_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterContextObserver_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterContextObserver_bitmaps.cpp$(DependSuffix) -MM wxCrafterContextObserver_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterContextObserver_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterContextObserver_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterContextObserver_bitmaps.cpp$(PreprocessSuffix): wxCrafterContextObserver_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterContextObserver_bitmaps.cpp$(PreprocessSuffix) wxCrafterContextObserver_bitmaps.cpp

$(IntermediateDirectory)/3D_GLContextCncPathBase.cpp$(ObjectSuffix): 3D/GLContextCncPathBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLContextCncPathBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLContextCncPathBase.cpp$(DependSuffix) -MM 3D/GLContextCncPathBase.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextCncPathBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLContextCncPathBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLContextCncPathBase.cpp$(PreprocessSuffix): 3D/GLContextCncPathBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLContextCncPathBase.cpp$(PreprocessSuffix) 3D/GLContextCncPathBase.cpp

$(IntermediateDirectory)/3D_CncMetricRulerSetup.cpp$(ObjectSuffix): 3D/CncMetricRulerSetup.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_CncMetricRulerSetup.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_CncMetricRulerSetup.cpp$(DependSuffix) -MM 3D/CncMetricRulerSetup.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/CncMetricRulerSetup.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_CncMetricRulerSetup.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_CncMetricRulerSetup.cpp$(PreprocessSuffix): 3D/CncMetricRulerSetup.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_CncMetricRulerSetup.cpp$(PreprocessSuffix) 3D/CncMetricRulerSetup.cpp

$(IntermediateDirectory)/Clipper_clipper.cpp$(ObjectSuffix): Clipper/clipper.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Clipper_clipper.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Clipper_clipper.cpp$(DependSuffix) -MM Clipper/clipper.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/Clipper/clipper.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Clipper_clipper.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Clipper_clipper.cpp$(PreprocessSuffix): Clipper/clipper.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Clipper_clipper.cpp$(PreprocessSuffix) Clipper/clipper.cpp

$(IntermediateDirectory)/GL3DDrawPane.cpp$(ObjectSuffix): GL3DDrawPane.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GL3DDrawPane.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GL3DDrawPane.cpp$(DependSuffix) -MM GL3DDrawPane.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GL3DDrawPane.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GL3DDrawPane.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GL3DDrawPane.cpp$(PreprocessSuffix): GL3DDrawPane.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GL3DDrawPane.cpp$(PreprocessSuffix) GL3DDrawPane.cpp

$(IntermediateDirectory)/CncArduinoEnvironment.cpp$(ObjectSuffix): CncArduinoEnvironment.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncArduinoEnvironment.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncArduinoEnvironment.cpp$(DependSuffix) -MM CncArduinoEnvironment.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncArduinoEnvironment.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncArduinoEnvironment.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncArduinoEnvironment.cpp$(PreprocessSuffix): CncArduinoEnvironment.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncArduinoEnvironment.cpp$(PreprocessSuffix) CncArduinoEnvironment.cpp

$(IntermediateDirectory)/CncPodiumManagement.cpp$(ObjectSuffix): CncPodiumManagement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPodiumManagement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPodiumManagement.cpp$(DependSuffix) -MM CncPodiumManagement.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPodiumManagement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPodiumManagement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPodiumManagement.cpp$(PreprocessSuffix): CncPodiumManagement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPodiumManagement.cpp$(PreprocessSuffix) CncPodiumManagement.cpp

$(IntermediateDirectory)/wxCrafterPodiumManagement.cpp$(ObjectSuffix): wxCrafterPodiumManagement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterPodiumManagement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterPodiumManagement.cpp$(DependSuffix) -MM wxCrafterPodiumManagement.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterPodiumManagement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterPodiumManagement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterPodiumManagement.cpp$(PreprocessSuffix): wxCrafterPodiumManagement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterPodiumManagement.cpp$(PreprocessSuffix) wxCrafterPodiumManagement.cpp

$(IntermediateDirectory)/CncCommandDecoder.cpp$(ObjectSuffix): CncCommandDecoder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncCommandDecoder.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncCommandDecoder.cpp$(DependSuffix) -MM CncCommandDecoder.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncCommandDecoder.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncCommandDecoder.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncCommandDecoder.cpp$(PreprocessSuffix): CncCommandDecoder.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncCommandDecoder.cpp$(PreprocessSuffix) CncCommandDecoder.cpp

$(IntermediateDirectory)/Tests_Test_Base.cpp$(ObjectSuffix): Tests/Test_Base.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Tests_Test_Base.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Tests_Test_Base.cpp$(DependSuffix) -MM Tests/Test_Base.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/Tests/Test_Base.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Tests_Test_Base.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Tests_Test_Base.cpp$(PreprocessSuffix): Tests/Test_Base.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Tests_Test_Base.cpp$(PreprocessSuffix) Tests/Test_Base.cpp

$(IntermediateDirectory)/3D_GLContextBase.cpp$(ObjectSuffix): 3D/GLContextBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLContextBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLContextBase.cpp$(DependSuffix) -MM 3D/GLContextBase.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLContextBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLContextBase.cpp$(PreprocessSuffix): 3D/GLContextBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLContextBase.cpp$(PreprocessSuffix) 3D/GLContextBase.cpp

$(IntermediateDirectory)/CncMotionMonitor.cpp$(ObjectSuffix): CncMotionMonitor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncMotionMonitor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncMotionMonitor.cpp$(DependSuffix) -MM CncMotionMonitor.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncMotionMonitor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncMotionMonitor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncMotionMonitor.cpp$(PreprocessSuffix): CncMotionMonitor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncMotionMonitor.cpp$(PreprocessSuffix) CncMotionMonitor.cpp

$(IntermediateDirectory)/SerialPort.cpp$(ObjectSuffix): SerialPort.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialPort.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialPort.cpp$(DependSuffix) -MM SerialPort.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialPort.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialPort.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialPort.cpp$(PreprocessSuffix): SerialPort.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialPort.cpp$(PreprocessSuffix) SerialPort.cpp

$(IntermediateDirectory)/CncSummaryListCtrl.cpp$(ObjectSuffix): CncSummaryListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSummaryListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSummaryListCtrl.cpp$(DependSuffix) -MM CncSummaryListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSummaryListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSummaryListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSummaryListCtrl.cpp$(PreprocessSuffix): CncSummaryListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSummaryListCtrl.cpp$(PreprocessSuffix) CncSummaryListCtrl.cpp

$(IntermediateDirectory)/CncSecureNumpad.cpp$(ObjectSuffix): CncSecureNumpad.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSecureNumpad.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSecureNumpad.cpp$(DependSuffix) -MM CncSecureNumpad.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSecureNumpad.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSecureNumpad.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSecureNumpad.cpp$(PreprocessSuffix): CncSecureNumpad.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSecureNumpad.cpp$(PreprocessSuffix) CncSecureNumpad.cpp

$(IntermediateDirectory)/CncPathListManager.cpp$(ObjectSuffix): CncPathListManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPathListManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPathListManager.cpp$(DependSuffix) -MM CncPathListManager.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPathListManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPathListManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPathListManager.cpp$(PreprocessSuffix): CncPathListManager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPathListManager.cpp$(PreprocessSuffix) CncPathListManager.cpp

$(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(ObjectSuffix): CncLargeScaleListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(DependSuffix) -MM CncLargeScaleListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncLargeScaleListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(PreprocessSuffix): CncLargeScaleListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncLargeScaleListCtrl.cpp$(PreprocessSuffix) CncLargeScaleListCtrl.cpp

$(IntermediateDirectory)/CncPathListEntry.cpp$(ObjectSuffix): CncPathListEntry.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPathListEntry.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPathListEntry.cpp$(DependSuffix) -MM CncPathListEntry.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPathListEntry.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPathListEntry.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPathListEntry.cpp$(PreprocessSuffix): CncPathListEntry.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPathListEntry.cpp$(PreprocessSuffix) CncPathListEntry.cpp

$(IntermediateDirectory)/CncAnchorPosition.cpp$(ObjectSuffix): CncAnchorPosition.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncAnchorPosition.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncAnchorPosition.cpp$(DependSuffix) -MM CncAnchorPosition.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncAnchorPosition.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncAnchorPosition.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncAnchorPosition.cpp$(PreprocessSuffix): CncAnchorPosition.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncAnchorPosition.cpp$(PreprocessSuffix) CncAnchorPosition.cpp

$(IntermediateDirectory)/CncGamePad.cpp$(ObjectSuffix): CncGamePad.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncGamePad.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncGamePad.cpp$(DependSuffix) -MM CncGamePad.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncGamePad.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncGamePad.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncGamePad.cpp$(PreprocessSuffix): CncGamePad.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncGamePad.cpp$(PreprocessSuffix) CncGamePad.cpp

$(IntermediateDirectory)/CncUnitCalculator.cpp$(ObjectSuffix): CncUnitCalculator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncUnitCalculator.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncUnitCalculator.cpp$(DependSuffix) -MM CncUnitCalculator.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncUnitCalculator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncUnitCalculator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncUnitCalculator.cpp$(PreprocessSuffix): CncUnitCalculator.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncUnitCalculator.cpp$(PreprocessSuffix) CncUnitCalculator.cpp

$(IntermediateDirectory)/CncLoggerProxy.cpp$(ObjectSuffix): CncLoggerProxy.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncLoggerProxy.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncLoggerProxy.cpp$(DependSuffix) -MM CncLoggerProxy.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncLoggerProxy.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncLoggerProxy.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncLoggerProxy.cpp$(PreprocessSuffix): CncLoggerProxy.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncLoggerProxy.cpp$(PreprocessSuffix) CncLoggerProxy.cpp

$(IntermediateDirectory)/CncUsbConnectionObserver.cpp$(ObjectSuffix): CncUsbConnectionObserver.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncUsbConnectionObserver.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncUsbConnectionObserver.cpp$(DependSuffix) -MM CncUsbConnectionObserver.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncUsbConnectionObserver.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncUsbConnectionObserver.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncUsbConnectionObserver.cpp$(PreprocessSuffix): CncUsbConnectionObserver.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncUsbConnectionObserver.cpp$(PreprocessSuffix) CncUsbConnectionObserver.cpp

$(IntermediateDirectory)/Codelite_cl_aui_dock_art.cpp$(ObjectSuffix): Codelite/cl_aui_dock_art.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Codelite_cl_aui_dock_art.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Codelite_cl_aui_dock_art.cpp$(DependSuffix) -MM Codelite/cl_aui_dock_art.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/Codelite/cl_aui_dock_art.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Codelite_cl_aui_dock_art.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Codelite_cl_aui_dock_art.cpp$(PreprocessSuffix): Codelite/cl_aui_dock_art.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Codelite_cl_aui_dock_art.cpp$(PreprocessSuffix) Codelite/cl_aui_dock_art.cpp

$(IntermediateDirectory)/GamepadEvent.cpp$(ObjectSuffix): GamepadEvent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GamepadEvent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GamepadEvent.cpp$(DependSuffix) -MM GamepadEvent.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GamepadEvent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GamepadEvent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GamepadEvent.cpp$(PreprocessSuffix): GamepadEvent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GamepadEvent.cpp$(PreprocessSuffix) GamepadEvent.cpp

$(IntermediateDirectory)/CncGamepadSpy.cpp$(ObjectSuffix): CncGamepadSpy.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncGamepadSpy.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncGamepadSpy.cpp$(DependSuffix) -MM CncGamepadSpy.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncGamepadSpy.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncGamepadSpy.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncGamepadSpy.cpp$(PreprocessSuffix): CncGamepadSpy.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncGamepadSpy.cpp$(PreprocessSuffix) CncGamepadSpy.cpp

$(IntermediateDirectory)/CncVector.cpp$(ObjectSuffix): CncVector.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncVector.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncVector.cpp$(DependSuffix) -MM CncVector.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncVector.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncVector.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncVector.cpp$(PreprocessSuffix): CncVector.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncVector.cpp$(PreprocessSuffix) CncVector.cpp

$(IntermediateDirectory)/DataControlModel.cpp$(ObjectSuffix): DataControlModel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/DataControlModel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/DataControlModel.cpp$(DependSuffix) -MM DataControlModel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/DataControlModel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/DataControlModel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/DataControlModel.cpp$(PreprocessSuffix): DataControlModel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/DataControlModel.cpp$(PreprocessSuffix) DataControlModel.cpp

$(IntermediateDirectory)/wxCrafterSecurePanel.cpp$(ObjectSuffix): wxCrafterSecurePanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterSecurePanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterSecurePanel.cpp$(DependSuffix) -MM wxCrafterSecurePanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterSecurePanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterSecurePanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterSecurePanel.cpp$(PreprocessSuffix): wxCrafterSecurePanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterSecurePanel.cpp$(PreprocessSuffix) wxCrafterSecurePanel.cpp

$(IntermediateDirectory)/wxCrafterSerialSpy.cpp$(ObjectSuffix): wxCrafterSerialSpy.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterSerialSpy.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterSerialSpy.cpp$(DependSuffix) -MM wxCrafterSerialSpy.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterSerialSpy.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterSerialSpy.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterSerialSpy.cpp$(PreprocessSuffix): wxCrafterSerialSpy.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterSerialSpy.cpp$(PreprocessSuffix) wxCrafterSerialSpy.cpp

$(IntermediateDirectory)/wxCrafterSerialSpy_bitmaps.cpp$(ObjectSuffix): wxCrafterSerialSpy_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterSerialSpy_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterSerialSpy_bitmaps.cpp$(DependSuffix) -MM wxCrafterSerialSpy_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterSerialSpy_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterSerialSpy_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterSerialSpy_bitmaps.cpp$(PreprocessSuffix): wxCrafterSerialSpy_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterSerialSpy_bitmaps.cpp$(PreprocessSuffix) wxCrafterSerialSpy_bitmaps.cpp

$(IntermediateDirectory)/CncMotionMonitorVertexTrace.cpp$(ObjectSuffix): CncMotionMonitorVertexTrace.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncMotionMonitorVertexTrace.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncMotionMonitorVertexTrace.cpp$(DependSuffix) -MM CncMotionMonitorVertexTrace.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncMotionMonitorVertexTrace.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncMotionMonitorVertexTrace.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncMotionMonitorVertexTrace.cpp$(PreprocessSuffix): CncMotionMonitorVertexTrace.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncMotionMonitorVertexTrace.cpp$(PreprocessSuffix) CncMotionMonitorVertexTrace.cpp

$(IntermediateDirectory)/wxCrafterLoggerPanel.cpp$(ObjectSuffix): wxCrafterLoggerPanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterLoggerPanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterLoggerPanel.cpp$(DependSuffix) -MM wxCrafterLoggerPanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterLoggerPanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterLoggerPanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterLoggerPanel.cpp$(PreprocessSuffix): wxCrafterLoggerPanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterLoggerPanel.cpp$(PreprocessSuffix) wxCrafterLoggerPanel.cpp

$(IntermediateDirectory)/3D_GLCommon.cpp$(ObjectSuffix): 3D/GLCommon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLCommon.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLCommon.cpp$(DependSuffix) -MM 3D/GLCommon.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLCommon.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLCommon.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLCommon.cpp$(PreprocessSuffix): 3D/GLCommon.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLCommon.cpp$(PreprocessSuffix) 3D/GLCommon.cpp

$(IntermediateDirectory)/CncTraceCtrl.cpp$(ObjectSuffix): CncTraceCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncTraceCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncTraceCtrl.cpp$(DependSuffix) -MM CncTraceCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncTraceCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncTraceCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncTraceCtrl.cpp$(PreprocessSuffix): CncTraceCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncTraceCtrl.cpp$(PreprocessSuffix) CncTraceCtrl.cpp

$(IntermediateDirectory)/wxCrafterLoggerPanel_bitmaps.cpp$(ObjectSuffix): wxCrafterLoggerPanel_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterLoggerPanel_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterLoggerPanel_bitmaps.cpp$(DependSuffix) -MM wxCrafterLoggerPanel_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterLoggerPanel_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterLoggerPanel_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterLoggerPanel_bitmaps.cpp$(PreprocessSuffix): wxCrafterLoggerPanel_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterLoggerPanel_bitmaps.cpp$(PreprocessSuffix) wxCrafterLoggerPanel_bitmaps.cpp

$(IntermediateDirectory)/wxCrafterMessageDialog.cpp$(ObjectSuffix): wxCrafterMessageDialog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterMessageDialog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterMessageDialog.cpp$(DependSuffix) -MM wxCrafterMessageDialog.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterMessageDialog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterMessageDialog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterMessageDialog.cpp$(PreprocessSuffix): wxCrafterMessageDialog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterMessageDialog.cpp$(PreprocessSuffix) wxCrafterMessageDialog.cpp

$(IntermediateDirectory)/CncMoveSequenceListCtrl.cpp$(ObjectSuffix): CncMoveSequenceListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncMoveSequenceListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncMoveSequenceListCtrl.cpp$(DependSuffix) -MM CncMoveSequenceListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncMoveSequenceListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncMoveSequenceListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncMoveSequenceListCtrl.cpp$(PreprocessSuffix): CncMoveSequenceListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncMoveSequenceListCtrl.cpp$(PreprocessSuffix) CncMoveSequenceListCtrl.cpp

$(IntermediateDirectory)/wxCrafterLCDPanel_bitmaps.cpp$(ObjectSuffix): wxCrafterLCDPanel_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterLCDPanel_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterLCDPanel_bitmaps.cpp$(DependSuffix) -MM wxCrafterLCDPanel_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterLCDPanel_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterLCDPanel_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterLCDPanel_bitmaps.cpp$(PreprocessSuffix): wxCrafterLCDPanel_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterLCDPanel_bitmaps.cpp$(PreprocessSuffix) wxCrafterLCDPanel_bitmaps.cpp

$(IntermediateDirectory)/3D_GLContextTestCube.cpp$(ObjectSuffix): 3D/GLContextTestCube.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLContextTestCube.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLContextTestCube.cpp$(DependSuffix) -MM 3D/GLContextTestCube.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextTestCube.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLContextTestCube.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLContextTestCube.cpp$(PreprocessSuffix): 3D/GLContextTestCube.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLContextTestCube.cpp$(PreprocessSuffix) 3D/GLContextTestCube.cpp

$(IntermediateDirectory)/wxCrafterLCDPanel.cpp$(ObjectSuffix): wxCrafterLCDPanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterLCDPanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterLCDPanel.cpp$(DependSuffix) -MM wxCrafterLCDPanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterLCDPanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterLCDPanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterLCDPanel.cpp$(PreprocessSuffix): wxCrafterLCDPanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterLCDPanel.cpp$(PreprocessSuffix) wxCrafterLCDPanel.cpp

$(IntermediateDirectory)/wxCrafterArduinoEnv.cpp$(ObjectSuffix): wxCrafterArduinoEnv.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterArduinoEnv.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterArduinoEnv.cpp$(DependSuffix) -MM wxCrafterArduinoEnv.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterArduinoEnv.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterArduinoEnv.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterArduinoEnv.cpp$(PreprocessSuffix): wxCrafterArduinoEnv.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterArduinoEnv.cpp$(PreprocessSuffix) wxCrafterArduinoEnv.cpp

$(IntermediateDirectory)/wxCrafterSecurePanel_bitmaps.cpp$(ObjectSuffix): wxCrafterSecurePanel_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterSecurePanel_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterSecurePanel_bitmaps.cpp$(DependSuffix) -MM wxCrafterSecurePanel_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterSecurePanel_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterSecurePanel_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterSecurePanel_bitmaps.cpp$(PreprocessSuffix): wxCrafterSecurePanel_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterSecurePanel_bitmaps.cpp$(PreprocessSuffix) wxCrafterSecurePanel_bitmaps.cpp

$(IntermediateDirectory)/wxCrafterMotionMonitor_bitmaps.cpp$(ObjectSuffix): wxCrafterMotionMonitor_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterMotionMonitor_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterMotionMonitor_bitmaps.cpp$(DependSuffix) -MM wxCrafterMotionMonitor_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterMotionMonitor_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterMotionMonitor_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterMotionMonitor_bitmaps.cpp$(PreprocessSuffix): wxCrafterMotionMonitor_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterMotionMonitor_bitmaps.cpp$(PreprocessSuffix) wxCrafterMotionMonitor_bitmaps.cpp

$(IntermediateDirectory)/CncConfig.cpp$(ObjectSuffix): CncConfig.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncConfig.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncConfig.cpp$(DependSuffix) -MM CncConfig.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncConfig.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncConfig.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncConfig.cpp$(PreprocessSuffix): CncConfig.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncConfig.cpp$(PreprocessSuffix) CncConfig.cpp

$(IntermediateDirectory)/wxCrafterFileView_bitmaps.cpp$(ObjectSuffix): wxCrafterFileView_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterFileView_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterFileView_bitmaps.cpp$(DependSuffix) -MM wxCrafterFileView_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterFileView_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterFileView_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterFileView_bitmaps.cpp$(PreprocessSuffix): wxCrafterFileView_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterFileView_bitmaps.cpp$(PreprocessSuffix) wxCrafterFileView_bitmaps.cpp

$(IntermediateDirectory)/wxCrafterFileView.cpp$(ObjectSuffix): wxCrafterFileView.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterFileView.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterFileView.cpp$(DependSuffix) -MM wxCrafterFileView.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterFileView.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterFileView.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterFileView.cpp$(PreprocessSuffix): wxCrafterFileView.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterFileView.cpp$(PreprocessSuffix) wxCrafterFileView.cpp

$(IntermediateDirectory)/3D_GLViewPort.cpp$(ObjectSuffix): 3D/GLViewPort.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLViewPort.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLViewPort.cpp$(DependSuffix) -MM 3D/GLViewPort.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLViewPort.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLViewPort.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLViewPort.cpp$(PreprocessSuffix): 3D/GLViewPort.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLViewPort.cpp$(PreprocessSuffix) 3D/GLViewPort.cpp

$(IntermediateDirectory)/wxCrafterToolMagazine.cpp$(ObjectSuffix): wxCrafterToolMagazine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterToolMagazine.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterToolMagazine.cpp$(DependSuffix) -MM wxCrafterToolMagazine.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterToolMagazine.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterToolMagazine.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterToolMagazine.cpp$(PreprocessSuffix): wxCrafterToolMagazine.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterToolMagazine.cpp$(PreprocessSuffix) wxCrafterToolMagazine.cpp

$(IntermediateDirectory)/ContextInterface.cpp$(ObjectSuffix): ContextInterface.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ContextInterface.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ContextInterface.cpp$(DependSuffix) -MM ContextInterface.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/ContextInterface.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ContextInterface.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ContextInterface.cpp$(PreprocessSuffix): ContextInterface.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ContextInterface.cpp$(PreprocessSuffix) ContextInterface.cpp

$(IntermediateDirectory)/wxCrafterBaseDialogs_bitmaps.cpp$(ObjectSuffix): wxCrafterBaseDialogs_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterBaseDialogs_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterBaseDialogs_bitmaps.cpp$(DependSuffix) -MM wxCrafterBaseDialogs_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterBaseDialogs_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterBaseDialogs_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterBaseDialogs_bitmaps.cpp$(PreprocessSuffix): wxCrafterBaseDialogs_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterBaseDialogs_bitmaps.cpp$(PreprocessSuffix) wxCrafterBaseDialogs_bitmaps.cpp

$(IntermediateDirectory)/wxCrafterBaseDialogs.cpp$(ObjectSuffix): wxCrafterBaseDialogs.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterBaseDialogs.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterBaseDialogs.cpp$(DependSuffix) -MM wxCrafterBaseDialogs.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterBaseDialogs.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterBaseDialogs.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterBaseDialogs.cpp$(PreprocessSuffix): wxCrafterBaseDialogs.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterBaseDialogs.cpp$(PreprocessSuffix) wxCrafterBaseDialogs.cpp

$(IntermediateDirectory)/wxCrafterReferencePosition_bitmaps.cpp$(ObjectSuffix): wxCrafterReferencePosition_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterReferencePosition_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterReferencePosition_bitmaps.cpp$(DependSuffix) -MM wxCrafterReferencePosition_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterReferencePosition_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterReferencePosition_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterReferencePosition_bitmaps.cpp$(PreprocessSuffix): wxCrafterReferencePosition_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterReferencePosition_bitmaps.cpp$(PreprocessSuffix) wxCrafterReferencePosition_bitmaps.cpp

$(IntermediateDirectory)/wxCrafterSpeedMonitor_bitmaps.cpp$(ObjectSuffix): wxCrafterSpeedMonitor_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterSpeedMonitor_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterSpeedMonitor_bitmaps.cpp$(DependSuffix) -MM wxCrafterSpeedMonitor_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterSpeedMonitor_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterSpeedMonitor_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterSpeedMonitor_bitmaps.cpp$(PreprocessSuffix): wxCrafterSpeedMonitor_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterSpeedMonitor_bitmaps.cpp$(PreprocessSuffix) wxCrafterSpeedMonitor_bitmaps.cpp

$(IntermediateDirectory)/CfgSVGOptionPage.cpp$(ObjectSuffix): CfgSVGOptionPage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CfgSVGOptionPage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CfgSVGOptionPage.cpp$(DependSuffix) -MM CfgSVGOptionPage.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CfgSVGOptionPage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CfgSVGOptionPage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CfgSVGOptionPage.cpp$(PreprocessSuffix): CfgSVGOptionPage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CfgSVGOptionPage.cpp$(PreprocessSuffix) CfgSVGOptionPage.cpp

$(IntermediateDirectory)/wxCrafterUnitTest.cpp$(ObjectSuffix): wxCrafterUnitTest.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterUnitTest.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterUnitTest.cpp$(DependSuffix) -MM wxCrafterUnitTest.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterUnitTest.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterUnitTest.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterUnitTest.cpp$(PreprocessSuffix): wxCrafterUnitTest.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterUnitTest.cpp$(PreprocessSuffix) wxCrafterUnitTest.cpp

$(IntermediateDirectory)/wxCrafterTemplateObserver_bitmaps.cpp$(ObjectSuffix): wxCrafterTemplateObserver_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterTemplateObserver_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterTemplateObserver_bitmaps.cpp$(DependSuffix) -MM wxCrafterTemplateObserver_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterTemplateObserver_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterTemplateObserver_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterTemplateObserver_bitmaps.cpp$(PreprocessSuffix): wxCrafterTemplateObserver_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterTemplateObserver_bitmaps.cpp$(PreprocessSuffix) wxCrafterTemplateObserver_bitmaps.cpp

$(IntermediateDirectory)/wxCrafterPathGenerator_bitmaps.cpp$(ObjectSuffix): wxCrafterPathGenerator_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterPathGenerator_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterPathGenerator_bitmaps.cpp$(DependSuffix) -MM wxCrafterPathGenerator_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterPathGenerator_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterPathGenerator_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterPathGenerator_bitmaps.cpp$(PreprocessSuffix): wxCrafterPathGenerator_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterPathGenerator_bitmaps.cpp$(PreprocessSuffix) wxCrafterPathGenerator_bitmaps.cpp

$(IntermediateDirectory)/CncGamepadCommandHistory.cpp$(ObjectSuffix): CncGamepadCommandHistory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncGamepadCommandHistory.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncGamepadCommandHistory.cpp$(DependSuffix) -MM CncGamepadCommandHistory.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncGamepadCommandHistory.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncGamepadCommandHistory.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncGamepadCommandHistory.cpp$(PreprocessSuffix): CncGamepadCommandHistory.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncGamepadCommandHistory.cpp$(PreprocessSuffix) CncGamepadCommandHistory.cpp

$(IntermediateDirectory)/CncLCDPositionPanel.cpp$(ObjectSuffix): CncLCDPositionPanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncLCDPositionPanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncLCDPositionPanel.cpp$(DependSuffix) -MM CncLCDPositionPanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncLCDPositionPanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncLCDPositionPanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncLCDPositionPanel.cpp$(PreprocessSuffix): CncLCDPositionPanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncLCDPositionPanel.cpp$(PreprocessSuffix) CncLCDPositionPanel.cpp

$(IntermediateDirectory)/wxCrafterMessageDialog_bitmaps.cpp$(ObjectSuffix): wxCrafterMessageDialog_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterMessageDialog_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterMessageDialog_bitmaps.cpp$(DependSuffix) -MM wxCrafterMessageDialog_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterMessageDialog_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterMessageDialog_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterMessageDialog_bitmaps.cpp$(PreprocessSuffix): wxCrafterMessageDialog_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterMessageDialog_bitmaps.cpp$(PreprocessSuffix) wxCrafterMessageDialog_bitmaps.cpp

$(IntermediateDirectory)/wxCrafterMainFrame.cpp$(ObjectSuffix): wxCrafterMainFrame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterMainFrame.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterMainFrame.cpp$(DependSuffix) -MM wxCrafterMainFrame.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterMainFrame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterMainFrame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterMainFrame.cpp$(PreprocessSuffix): wxCrafterMainFrame.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterMainFrame.cpp$(PreprocessSuffix) wxCrafterMainFrame.cpp

$(IntermediateDirectory)/wxCrafterImages_bitmaps.cpp$(ObjectSuffix): wxCrafterImages_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterImages_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterImages_bitmaps.cpp$(DependSuffix) -MM wxCrafterImages_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterImages_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterImages_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterImages_bitmaps.cpp$(PreprocessSuffix): wxCrafterImages_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterImages_bitmaps.cpp$(PreprocessSuffix) wxCrafterImages_bitmaps.cpp

$(IntermediateDirectory)/wxCrafterImages.cpp$(ObjectSuffix): wxCrafterImages.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterImages.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterImages.cpp$(DependSuffix) -MM wxCrafterImages.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterImages.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterImages.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterImages.cpp$(PreprocessSuffix): wxCrafterImages.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterImages.cpp$(PreprocessSuffix) wxCrafterImages.cpp

$(IntermediateDirectory)/wxCrafterGamePort.cpp$(ObjectSuffix): wxCrafterGamePort.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterGamePort.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterGamePort.cpp$(DependSuffix) -MM wxCrafterGamePort.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterGamePort.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterGamePort.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterGamePort.cpp$(PreprocessSuffix): wxCrafterGamePort.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterGamePort.cpp$(PreprocessSuffix) wxCrafterGamePort.cpp

$(IntermediateDirectory)/MainFrameProxy.cpp$(ObjectSuffix): MainFrameProxy.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/MainFrameProxy.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/MainFrameProxy.cpp$(DependSuffix) -MM MainFrameProxy.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/MainFrameProxy.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MainFrameProxy.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MainFrameProxy.cpp$(PreprocessSuffix): MainFrameProxy.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MainFrameProxy.cpp$(PreprocessSuffix) MainFrameProxy.cpp

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/wxCrafterMainFrame_bitmaps.cpp$(ObjectSuffix): wxCrafterMainFrame_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterMainFrame_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterMainFrame_bitmaps.cpp$(DependSuffix) -MM wxCrafterMainFrame_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterMainFrame_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterMainFrame_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterMainFrame_bitmaps.cpp$(PreprocessSuffix): wxCrafterMainFrame_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterMainFrame_bitmaps.cpp$(PreprocessSuffix) wxCrafterMainFrame_bitmaps.cpp

$(IntermediateDirectory)/MainFrame.cpp$(ObjectSuffix): MainFrame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/MainFrame.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/MainFrame.cpp$(DependSuffix) -MM MainFrame.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/MainFrame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MainFrame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MainFrame.cpp$(PreprocessSuffix): MainFrame.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MainFrame.cpp$(PreprocessSuffix) MainFrame.cpp

$(IntermediateDirectory)/CncGamepadControllerState.cpp$(ObjectSuffix): CncGamepadControllerState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncGamepadControllerState.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncGamepadControllerState.cpp$(DependSuffix) -MM CncGamepadControllerState.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncGamepadControllerState.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncGamepadControllerState.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncGamepadControllerState.cpp$(PreprocessSuffix): CncGamepadControllerState.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncGamepadControllerState.cpp$(PreprocessSuffix) CncGamepadControllerState.cpp

$(IntermediateDirectory)/StdStreamRedirector.cpp$(ObjectSuffix): StdStreamRedirector.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/StdStreamRedirector.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/StdStreamRedirector.cpp$(DependSuffix) -MM StdStreamRedirector.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/StdStreamRedirector.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/StdStreamRedirector.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/StdStreamRedirector.cpp$(PreprocessSuffix): StdStreamRedirector.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/StdStreamRedirector.cpp$(PreprocessSuffix) StdStreamRedirector.cpp

$(IntermediateDirectory)/3D_GLHelperModel.cpp$(ObjectSuffix): 3D/GLHelperModel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLHelperModel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLHelperModel.cpp$(DependSuffix) -MM 3D/GLHelperModel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLHelperModel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLHelperModel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLHelperModel.cpp$(PreprocessSuffix): 3D/GLHelperModel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLHelperModel.cpp$(PreprocessSuffix) 3D/GLHelperModel.cpp

$(IntermediateDirectory)/CncSecureNumpadDialog.cpp$(ObjectSuffix): CncSecureNumpadDialog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSecureNumpadDialog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSecureNumpadDialog.cpp$(DependSuffix) -MM CncSecureNumpadDialog.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSecureNumpadDialog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSecureNumpadDialog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSecureNumpadDialog.cpp$(PreprocessSuffix): CncSecureNumpadDialog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSecureNumpadDialog.cpp$(PreprocessSuffix) CncSecureNumpadDialog.cpp

$(IntermediateDirectory)/SerialEmulatorBinaryStreamer.cpp$(ObjectSuffix): SerialEmulatorBinaryStreamer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialEmulatorBinaryStreamer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialEmulatorBinaryStreamer.cpp$(DependSuffix) -MM SerialEmulatorBinaryStreamer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialEmulatorBinaryStreamer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialEmulatorBinaryStreamer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialEmulatorBinaryStreamer.cpp$(PreprocessSuffix): SerialEmulatorBinaryStreamer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialEmulatorBinaryStreamer.cpp$(PreprocessSuffix) SerialEmulatorBinaryStreamer.cpp

$(IntermediateDirectory)/sha1.cpp$(ObjectSuffix): sha1.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sha1.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/sha1.cpp$(DependSuffix) -MM sha1.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/sha1.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sha1.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sha1.cpp$(PreprocessSuffix): sha1.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sha1.cpp$(PreprocessSuffix) sha1.cpp

$(IntermediateDirectory)/OSD_CncCameraHelper.cpp$(ObjectSuffix): OSD/CncCameraHelper.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OSD_CncCameraHelper.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OSD_CncCameraHelper.cpp$(DependSuffix) -MM OSD/CncCameraHelper.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/OSD/CncCameraHelper.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OSD_CncCameraHelper.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OSD_CncCameraHelper.cpp$(PreprocessSuffix): OSD/CncCameraHelper.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OSD_CncCameraHelper.cpp$(PreprocessSuffix) OSD/CncCameraHelper.cpp

$(IntermediateDirectory)/CncGamepadGtk.cpp$(ObjectSuffix): CncGamepadGtk.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncGamepadGtk.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncGamepadGtk.cpp$(DependSuffix) -MM CncGamepadGtk.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncGamepadGtk.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncGamepadGtk.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncGamepadGtk.cpp$(PreprocessSuffix): CncGamepadGtk.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncGamepadGtk.cpp$(PreprocessSuffix) CncGamepadGtk.cpp

$(IntermediateDirectory)/UnitTestFrame.cpp$(ObjectSuffix): UnitTestFrame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/UnitTestFrame.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/UnitTestFrame.cpp$(DependSuffix) -MM UnitTestFrame.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/UnitTestFrame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/UnitTestFrame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/UnitTestFrame.cpp$(PreprocessSuffix): UnitTestFrame.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/UnitTestFrame.cpp$(PreprocessSuffix) UnitTestFrame.cpp

$(IntermediateDirectory)/CncGamepadFilter.cpp$(ObjectSuffix): CncGamepadFilter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncGamepadFilter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncGamepadFilter.cpp$(DependSuffix) -MM CncGamepadFilter.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncGamepadFilter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncGamepadFilter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncGamepadFilter.cpp$(PreprocessSuffix): CncGamepadFilter.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncGamepadFilter.cpp$(PreprocessSuffix) CncGamepadFilter.cpp

$(IntermediateDirectory)/GlobalFunctions.cpp$(ObjectSuffix): GlobalFunctions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GlobalFunctions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GlobalFunctions.cpp$(DependSuffix) -MM GlobalFunctions.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GlobalFunctions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GlobalFunctions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GlobalFunctions.cpp$(PreprocessSuffix): GlobalFunctions.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GlobalFunctions.cpp$(PreprocessSuffix) GlobalFunctions.cpp

$(IntermediateDirectory)/CncContext.cpp$(ObjectSuffix): CncContext.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncContext.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncContext.cpp$(DependSuffix) -MM CncContext.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncContext.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncContext.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncContext.cpp$(PreprocessSuffix): CncContext.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncContext.cpp$(PreprocessSuffix) CncContext.cpp

$(IntermediateDirectory)/HexDecoder.cpp$(ObjectSuffix): HexDecoder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/HexDecoder.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/HexDecoder.cpp$(DependSuffix) -MM HexDecoder.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/HexDecoder.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/HexDecoder.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/HexDecoder.cpp$(PreprocessSuffix): HexDecoder.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/HexDecoder.cpp$(PreprocessSuffix) HexDecoder.cpp

$(IntermediateDirectory)/CncPodiumMgmtMovement.cpp$(ObjectSuffix): CncPodiumMgmtMovement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPodiumMgmtMovement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPodiumMgmtMovement.cpp$(DependSuffix) -MM CncPodiumMgmtMovement.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPodiumMgmtMovement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPodiumMgmtMovement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPodiumMgmtMovement.cpp$(PreprocessSuffix): CncPodiumMgmtMovement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPodiumMgmtMovement.cpp$(PreprocessSuffix) CncPodiumMgmtMovement.cpp

$(IntermediateDirectory)/CncPathListRunner.cpp$(ObjectSuffix): CncPathListRunner.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPathListRunner.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPathListRunner.cpp$(DependSuffix) -MM CncPathListRunner.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPathListRunner.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPathListRunner.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPathListRunner.cpp$(PreprocessSuffix): CncPathListRunner.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPathListRunner.cpp$(PreprocessSuffix) CncPathListRunner.cpp

$(IntermediateDirectory)/WireCircularBuffer.cpp$(ObjectSuffix): WireCircularBuffer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/WireCircularBuffer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/WireCircularBuffer.cpp$(DependSuffix) -MM WireCircularBuffer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/WireCircularBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/WireCircularBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/WireCircularBuffer.cpp$(PreprocessSuffix): WireCircularBuffer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/WireCircularBuffer.cpp$(PreprocessSuffix) WireCircularBuffer.cpp

$(IntermediateDirectory)/wxCode_lcdwindow.cpp$(ObjectSuffix): wxCode/lcdwindow.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCode_lcdwindow.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCode_lcdwindow.cpp$(DependSuffix) -MM wxCode/lcdwindow.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCode/lcdwindow.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCode_lcdwindow.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCode_lcdwindow.cpp$(PreprocessSuffix): wxCode/lcdwindow.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCode_lcdwindow.cpp$(PreprocessSuffix) wxCode/lcdwindow.cpp

$(IntermediateDirectory)/CncPathListInterfaceCnc.cpp$(ObjectSuffix): CncPathListInterfaceCnc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPathListInterfaceCnc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPathListInterfaceCnc.cpp$(DependSuffix) -MM CncPathListInterfaceCnc.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPathListInterfaceCnc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPathListInterfaceCnc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPathListInterfaceCnc.cpp$(PreprocessSuffix): CncPathListInterfaceCnc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPathListInterfaceCnc.cpp$(PreprocessSuffix) CncPathListInterfaceCnc.cpp

$(IntermediateDirectory)/3D_GLOpenGLPathBufferStore.cpp$(ObjectSuffix): 3D/GLOpenGLPathBufferStore.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLOpenGLPathBufferStore.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLOpenGLPathBufferStore.cpp$(DependSuffix) -MM 3D/GLOpenGLPathBufferStore.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLOpenGLPathBufferStore.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLOpenGLPathBufferStore.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLOpenGLPathBufferStore.cpp$(PreprocessSuffix): 3D/GLOpenGLPathBufferStore.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLOpenGLPathBufferStore.cpp$(PreprocessSuffix) 3D/GLOpenGLPathBufferStore.cpp

$(IntermediateDirectory)/CncTemplateContext.cpp$(ObjectSuffix): CncTemplateContext.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncTemplateContext.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncTemplateContext.cpp$(DependSuffix) -MM CncTemplateContext.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncTemplateContext.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncTemplateContext.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncTemplateContext.cpp$(PreprocessSuffix): CncTemplateContext.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncTemplateContext.cpp$(PreprocessSuffix) CncTemplateContext.cpp

$(IntermediateDirectory)/CncStartPositionResolver.cpp$(ObjectSuffix): CncStartPositionResolver.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncStartPositionResolver.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncStartPositionResolver.cpp$(DependSuffix) -MM CncStartPositionResolver.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncStartPositionResolver.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncStartPositionResolver.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncStartPositionResolver.cpp$(PreprocessSuffix): CncStartPositionResolver.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncStartPositionResolver.cpp$(PreprocessSuffix) CncStartPositionResolver.cpp

$(IntermediateDirectory)/CncSecurePortListCtrl.cpp$(ObjectSuffix): CncSecurePortListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncSecurePortListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncSecurePortListCtrl.cpp$(DependSuffix) -MM CncSecurePortListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSecurePortListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncSecurePortListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncSecurePortListCtrl.cpp$(PreprocessSuffix): CncSecurePortListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncSecurePortListCtrl.cpp$(PreprocessSuffix) CncSecurePortListCtrl.cpp

$(IntermediateDirectory)/CncFileViewLists.cpp$(ObjectSuffix): CncFileViewLists.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncFileViewLists.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncFileViewLists.cpp$(DependSuffix) -MM CncFileViewLists.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncFileViewLists.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncFileViewLists.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncFileViewLists.cpp$(PreprocessSuffix): CncFileViewLists.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncFileViewLists.cpp$(PreprocessSuffix) CncFileViewLists.cpp

$(IntermediateDirectory)/ManuallyParser.cpp$(ObjectSuffix): ManuallyParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ManuallyParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ManuallyParser.cpp$(DependSuffix) -MM ManuallyParser.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/ManuallyParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ManuallyParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ManuallyParser.cpp$(PreprocessSuffix): ManuallyParser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ManuallyParser.cpp$(PreprocessSuffix) ManuallyParser.cpp

$(IntermediateDirectory)/CncContextListCtrl.cpp$(ObjectSuffix): CncContextListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncContextListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncContextListCtrl.cpp$(DependSuffix) -MM CncContextListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncContextListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncContextListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncContextListCtrl.cpp$(PreprocessSuffix): CncContextListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncContextListCtrl.cpp$(PreprocessSuffix) CncContextListCtrl.cpp

$(IntermediateDirectory)/CncGCodeSequenceListCtrl.cpp$(ObjectSuffix): CncGCodeSequenceListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncGCodeSequenceListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncGCodeSequenceListCtrl.cpp$(DependSuffix) -MM CncGCodeSequenceListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncGCodeSequenceListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncGCodeSequenceListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncGCodeSequenceListCtrl.cpp$(PreprocessSuffix): CncGCodeSequenceListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncGCodeSequenceListCtrl.cpp$(PreprocessSuffix) CncGCodeSequenceListCtrl.cpp

$(IntermediateDirectory)/CncPathListEntryListCtrl.cpp$(ObjectSuffix): CncPathListEntryListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPathListEntryListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPathListEntryListCtrl.cpp$(DependSuffix) -MM CncPathListEntryListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPathListEntryListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPathListEntryListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPathListEntryListCtrl.cpp$(PreprocessSuffix): CncPathListEntryListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPathListEntryListCtrl.cpp$(PreprocessSuffix) CncPathListEntryListCtrl.cpp

$(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(ObjectSuffix): CncStatisticSummaryListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(DependSuffix) -MM CncStatisticSummaryListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncStatisticSummaryListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(PreprocessSuffix): CncStatisticSummaryListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncStatisticSummaryListCtrl.cpp$(PreprocessSuffix) CncStatisticSummaryListCtrl.cpp

$(IntermediateDirectory)/wxCrafterReferencePosition.cpp$(ObjectSuffix): wxCrafterReferencePosition.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterReferencePosition.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterReferencePosition.cpp$(DependSuffix) -MM wxCrafterReferencePosition.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterReferencePosition.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterReferencePosition.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterReferencePosition.cpp$(PreprocessSuffix): wxCrafterReferencePosition.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterReferencePosition.cpp$(PreprocessSuffix) wxCrafterReferencePosition.cpp

$(IntermediateDirectory)/BinaryPathHandlerBase.cpp$(ObjectSuffix): BinaryPathHandlerBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/BinaryPathHandlerBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/BinaryPathHandlerBase.cpp$(DependSuffix) -MM BinaryPathHandlerBase.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/BinaryPathHandlerBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/BinaryPathHandlerBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/BinaryPathHandlerBase.cpp$(PreprocessSuffix): BinaryPathHandlerBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/BinaryPathHandlerBase.cpp$(PreprocessSuffix) BinaryPathHandlerBase.cpp

$(IntermediateDirectory)/CircularBuffer.cpp$(ObjectSuffix): CircularBuffer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CircularBuffer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CircularBuffer.cpp$(DependSuffix) -MM CircularBuffer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CircularBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CircularBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CircularBuffer.cpp$(PreprocessSuffix): CircularBuffer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CircularBuffer.cpp$(PreprocessSuffix) CircularBuffer.cpp

$(IntermediateDirectory)/BinaryFileParser.cpp$(ObjectSuffix): BinaryFileParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/BinaryFileParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/BinaryFileParser.cpp$(DependSuffix) -MM BinaryFileParser.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/BinaryFileParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/BinaryFileParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/BinaryFileParser.cpp$(PreprocessSuffix): BinaryFileParser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/BinaryFileParser.cpp$(PreprocessSuffix) BinaryFileParser.cpp

$(IntermediateDirectory)/CncGamePadMsw.cpp$(ObjectSuffix): CncGamePadMsw.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncGamePadMsw.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncGamePadMsw.cpp$(DependSuffix) -MM CncGamePadMsw.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncGamePadMsw.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncGamePadMsw.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncGamePadMsw.cpp$(PreprocessSuffix): CncGamePadMsw.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncGamePadMsw.cpp$(PreprocessSuffix) CncGamePadMsw.cpp

$(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(ObjectSuffix): ManuallyPathHandlerCnc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(DependSuffix) -MM ManuallyPathHandlerCnc.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/ManuallyPathHandlerCnc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(PreprocessSuffix): ManuallyPathHandlerCnc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ManuallyPathHandlerCnc.cpp$(PreprocessSuffix) ManuallyPathHandlerCnc.cpp

$(IntermediateDirectory)/GCodeFileParser.cpp$(ObjectSuffix): GCodeFileParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodeFileParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodeFileParser.cpp$(DependSuffix) -MM GCodeFileParser.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodeFileParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodeFileParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodeFileParser.cpp$(PreprocessSuffix): GCodeFileParser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodeFileParser.cpp$(PreprocessSuffix) GCodeFileParser.cpp

$(IntermediateDirectory)/GCodePathHandlerBase.cpp$(ObjectSuffix): GCodePathHandlerBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodePathHandlerBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodePathHandlerBase.cpp$(DependSuffix) -MM GCodePathHandlerBase.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodePathHandlerBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodePathHandlerBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodePathHandlerBase.cpp$(PreprocessSuffix): GCodePathHandlerBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodePathHandlerBase.cpp$(PreprocessSuffix) GCodePathHandlerBase.cpp

$(IntermediateDirectory)/GCodeBlock.cpp$(ObjectSuffix): GCodeBlock.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodeBlock.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodeBlock.cpp$(DependSuffix) -MM GCodeBlock.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodeBlock.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodeBlock.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodeBlock.cpp$(PreprocessSuffix): GCodeBlock.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodeBlock.cpp$(PreprocessSuffix) GCodeBlock.cpp

$(IntermediateDirectory)/GCodeField.cpp$(ObjectSuffix): GCodeField.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodeField.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodeField.cpp$(DependSuffix) -MM GCodeField.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodeField.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodeField.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodeField.cpp$(PreprocessSuffix): GCodeField.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodeField.cpp$(PreprocessSuffix) GCodeField.cpp

$(IntermediateDirectory)/CncOpenGLContextObserverListCtrl.cpp$(ObjectSuffix): CncOpenGLContextObserverListCtrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncOpenGLContextObserverListCtrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncOpenGLContextObserverListCtrl.cpp$(DependSuffix) -MM CncOpenGLContextObserverListCtrl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncOpenGLContextObserverListCtrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncOpenGLContextObserverListCtrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncOpenGLContextObserverListCtrl.cpp$(PreprocessSuffix): CncOpenGLContextObserverListCtrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncOpenGLContextObserverListCtrl.cpp$(PreprocessSuffix) CncOpenGLContextObserverListCtrl.cpp

$(IntermediateDirectory)/3D_GLLineCluster.cpp$(ObjectSuffix): 3D/GLLineCluster.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLLineCluster.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLLineCluster.cpp$(DependSuffix) -MM 3D/GLLineCluster.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLLineCluster.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLLineCluster.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLLineCluster.cpp$(PreprocessSuffix): 3D/GLLineCluster.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLLineCluster.cpp$(PreprocessSuffix) 3D/GLLineCluster.cpp

$(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(ObjectSuffix): GCodePathHandlerCnc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(DependSuffix) -MM GCodePathHandlerCnc.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodePathHandlerCnc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(PreprocessSuffix): GCodePathHandlerCnc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodePathHandlerCnc.cpp$(PreprocessSuffix) GCodePathHandlerCnc.cpp

$(IntermediateDirectory)/PathGeneratorFrame.cpp$(ObjectSuffix): PathGeneratorFrame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PathGeneratorFrame.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PathGeneratorFrame.cpp$(DependSuffix) -MM PathGeneratorFrame.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/PathGeneratorFrame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PathGeneratorFrame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PathGeneratorFrame.cpp$(PreprocessSuffix): PathGeneratorFrame.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PathGeneratorFrame.cpp$(PreprocessSuffix) PathGeneratorFrame.cpp

$(IntermediateDirectory)/GCodePathHandlerGL.cpp$(ObjectSuffix): GCodePathHandlerGL.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GCodePathHandlerGL.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GCodePathHandlerGL.cpp$(DependSuffix) -MM GCodePathHandlerGL.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/GCodePathHandlerGL.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GCodePathHandlerGL.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GCodePathHandlerGL.cpp$(PreprocessSuffix): GCodePathHandlerGL.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GCodePathHandlerGL.cpp$(PreprocessSuffix) GCodePathHandlerGL.cpp

$(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(ObjectSuffix): OSD/CncAsyncKeyboardState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(DependSuffix) -MM OSD/CncAsyncKeyboardState.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/OSD/CncAsyncKeyboardState.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(PreprocessSuffix): OSD/CncAsyncKeyboardState.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OSD_CncAsyncKeyboardState.cpp$(PreprocessSuffix) OSD/CncAsyncKeyboardState.cpp

$(IntermediateDirectory)/3D_GLCncPathData.cpp$(ObjectSuffix): 3D/GLCncPathData.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLCncPathData.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLCncPathData.cpp$(DependSuffix) -MM 3D/GLCncPathData.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLCncPathData.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLCncPathData.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLCncPathData.cpp$(PreprocessSuffix): 3D/GLCncPathData.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLCncPathData.cpp$(PreprocessSuffix) 3D/GLCncPathData.cpp

$(IntermediateDirectory)/CncPositionStorageView.cpp$(ObjectSuffix): CncPositionStorageView.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncPositionStorageView.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncPositionStorageView.cpp$(DependSuffix) -MM CncPositionStorageView.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPositionStorageView.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncPositionStorageView.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncPositionStorageView.cpp$(PreprocessSuffix): CncPositionStorageView.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncPositionStorageView.cpp$(PreprocessSuffix) CncPositionStorageView.cpp

$(IntermediateDirectory)/3D_GLContextCncPath.cpp$(ObjectSuffix): 3D/GLContextCncPath.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/3D_GLContextCncPath.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/3D_GLContextCncPath.cpp$(DependSuffix) -MM 3D/GLContextCncPath.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextCncPath.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/3D_GLContextCncPath.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/3D_GLContextCncPath.cpp$(PreprocessSuffix): 3D/GLContextCncPath.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/3D_GLContextCncPath.cpp$(PreprocessSuffix) 3D/GLContextCncPath.cpp

$(IntermediateDirectory)/BaseEndPoint.cpp$(ObjectSuffix): BaseEndPoint.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/BaseEndPoint.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/BaseEndPoint.cpp$(DependSuffix) -MM BaseEndPoint.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/BaseEndPoint.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/BaseEndPoint.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/BaseEndPoint.cpp$(PreprocessSuffix): BaseEndPoint.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/BaseEndPoint.cpp$(PreprocessSuffix) BaseEndPoint.cpp

$(IntermediateDirectory)/SerialAdminChannel.cpp$(ObjectSuffix): SerialAdminChannel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialAdminChannel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialAdminChannel.cpp$(DependSuffix) -MM SerialAdminChannel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialAdminChannel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialAdminChannel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialAdminChannel.cpp$(PreprocessSuffix): SerialAdminChannel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialAdminChannel.cpp$(PreprocessSuffix) SerialAdminChannel.cpp

$(IntermediateDirectory)/OSD_SerialMsw.cpp$(ObjectSuffix): OSD/SerialMsw.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OSD_SerialMsw.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OSD_SerialMsw.cpp$(DependSuffix) -MM OSD/SerialMsw.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/OSD/SerialMsw.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OSD_SerialMsw.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OSD_SerialMsw.cpp$(PreprocessSuffix): OSD/SerialMsw.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OSD_SerialMsw.cpp$(PreprocessSuffix) OSD/SerialMsw.cpp

$(IntermediateDirectory)/OSD_SerialGtk.cpp$(ObjectSuffix): OSD/SerialGtk.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OSD_SerialGtk.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OSD_SerialGtk.cpp$(DependSuffix) -MM OSD/SerialGtk.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/OSD/SerialGtk.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OSD_SerialGtk.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OSD_SerialGtk.cpp$(PreprocessSuffix): OSD/SerialGtk.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OSD_SerialGtk.cpp$(PreprocessSuffix) OSD/SerialGtk.cpp

$(IntermediateDirectory)/SerialThreadStub.cpp$(ObjectSuffix): SerialThreadStub.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialThreadStub.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialThreadStub.cpp$(DependSuffix) -MM SerialThreadStub.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialThreadStub.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialThreadStub.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialThreadStub.cpp$(PreprocessSuffix): SerialThreadStub.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialThreadStub.cpp$(PreprocessSuffix) SerialThreadStub.cpp

$(IntermediateDirectory)/CncMessageDialog.cpp$(ObjectSuffix): CncMessageDialog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncMessageDialog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncMessageDialog.cpp$(DependSuffix) -MM CncMessageDialog.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncMessageDialog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncMessageDialog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncMessageDialog.cpp$(PreprocessSuffix): CncMessageDialog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncMessageDialog.cpp$(PreprocessSuffix) CncMessageDialog.cpp

$(IntermediateDirectory)/SerialEmulatorGCodeStreamer.cpp$(ObjectSuffix): SerialEmulatorGCodeStreamer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialEmulatorGCodeStreamer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialEmulatorGCodeStreamer.cpp$(DependSuffix) -MM SerialEmulatorGCodeStreamer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialEmulatorGCodeStreamer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialEmulatorGCodeStreamer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialEmulatorGCodeStreamer.cpp$(PreprocessSuffix): SerialEmulatorGCodeStreamer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialEmulatorGCodeStreamer.cpp$(PreprocessSuffix) SerialEmulatorGCodeStreamer.cpp

$(IntermediateDirectory)/SerialEmulatorTextStreamer.cpp$(ObjectSuffix): SerialEmulatorTextStreamer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SerialEmulatorTextStreamer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SerialEmulatorTextStreamer.cpp$(DependSuffix) -MM SerialEmulatorTextStreamer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SerialEmulatorTextStreamer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SerialEmulatorTextStreamer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SerialEmulatorTextStreamer.cpp$(PreprocessSuffix): SerialEmulatorTextStreamer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SerialEmulatorTextStreamer.cpp$(PreprocessSuffix) SerialEmulatorTextStreamer.cpp

$(IntermediateDirectory)/CncMillingSoundController.cpp$(ObjectSuffix): CncMillingSoundController.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncMillingSoundController.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncMillingSoundController.cpp$(DependSuffix) -MM CncMillingSoundController.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncMillingSoundController.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncMillingSoundController.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncMillingSoundController.cpp$(PreprocessSuffix): CncMillingSoundController.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncMillingSoundController.cpp$(PreprocessSuffix) CncMillingSoundController.cpp

$(IntermediateDirectory)/CncExternalViewBox.cpp$(ObjectSuffix): CncExternalViewBox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CncExternalViewBox.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CncExternalViewBox.cpp$(DependSuffix) -MM CncExternalViewBox.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncExternalViewBox.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CncExternalViewBox.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CncExternalViewBox.cpp$(PreprocessSuffix): CncExternalViewBox.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CncExternalViewBox.cpp$(PreprocessSuffix) CncExternalViewBox.cpp

$(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(ObjectSuffix): OSD/CncTimeFunctions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(DependSuffix) -MM OSD/CncTimeFunctions.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/OSD/CncTimeFunctions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(PreprocessSuffix): OSD/CncTimeFunctions.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OSD_CncTimeFunctions.cpp$(PreprocessSuffix) OSD/CncTimeFunctions.cpp

$(IntermediateDirectory)/FileParser.cpp$(ObjectSuffix): FileParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/FileParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/FileParser.cpp$(DependSuffix) -MM FileParser.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/FileParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FileParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/FileParser.cpp$(PreprocessSuffix): FileParser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/FileParser.cpp$(PreprocessSuffix) FileParser.cpp

$(IntermediateDirectory)/PathHandlerBase.cpp$(ObjectSuffix): PathHandlerBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PathHandlerBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PathHandlerBase.cpp$(DependSuffix) -MM PathHandlerBase.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/PathHandlerBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PathHandlerBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PathHandlerBase.cpp$(PreprocessSuffix): PathHandlerBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PathHandlerBase.cpp$(PreprocessSuffix) PathHandlerBase.cpp

$(IntermediateDirectory)/wxCrafterContextObserver.cpp$(ObjectSuffix): wxCrafterContextObserver.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxCrafterContextObserver.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxCrafterContextObserver.cpp$(DependSuffix) -MM wxCrafterContextObserver.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterContextObserver.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxCrafterContextObserver.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxCrafterContextObserver.cpp$(PreprocessSuffix): wxCrafterContextObserver.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxCrafterContextObserver.cpp$(PreprocessSuffix) wxCrafterContextObserver.cpp

$(IntermediateDirectory)/SVGNodeParser.cpp$(ObjectSuffix): SVGNodeParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SVGNodeParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SVGNodeParser.cpp$(DependSuffix) -MM SVGNodeParser.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/SVGNodeParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SVGNodeParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SVGNodeParser.cpp$(PreprocessSuffix): SVGNodeParser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SVGNodeParser.cpp$(PreprocessSuffix) SVGNodeParser.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(ConfigurationName)/


