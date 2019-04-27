cmake_minimum_required(VERSION 3.7)
project(cnc)

set(CMAKE_CXX_STANDARD 11)


#[[
    wxWidgets_FOUND            - Set to TRUE if wxWidgets was found.
    wxWidgets_INCLUDE_DIRS     - Include directories for WIN32 i.e., where to find "wx/wx.h" and "wx/setup.h"; possibly empty for unices.
    wxWidgets_LIBRARIES        - Path to the wxWidgets libraries.
    wxWidgets_LIBRARY_DIRS     - compile time link dirs, useful for rpath on UNIX. Typically an empty string in WIN32 environment.
    wxWidgets_DEFINITIONS      - Contains defines required to compile/link against WX, e.g. WXUSINGDLL
    wxWidgets_DEFINITIONS_DEBUG- Contains defines required to compile/link against WX debug builds, e.g. __WXDEBUG__
    wxWidgets_CXX_FLAGS        - Include dirs and compiler flags for unices, empty on WIN32. Essentially "`wx-config --cxxflags`".
    wxWidgets_USE_FILE         - Convenience include file.

    -I/home/fw591/wxWidgets/wxWidgets-3.1.0/gtk-build/lib/wx/include/gtk2-unicode-3.1
    -I/home/fw591/wxWidgets/wxWidgets-3.1.0/include
    -D_FILE_OFFSET_BITS=64
    -DWXUSINGDLL
    -D__WXGTK__
    -pthread

    -I /home/fw591/wxWidgets/wxWidgets-3.1.0/gtk-build/lib/wx/include/gtk2-unicode-3.1 -I /home/fw591/wxWidgets/wxWidgets-3.1.0/include -D _FILE_OFFSET_BITS=64 -D WXUSINGDLL -D __WXGTK__ -pthread
]]

#[[
    set(wxWidgets_FOUND, TRUE)
    set(wxWidgets_CONFIG_EXECUTABLE "/home/fw591/wxWidgets/wxWidgets-3.1.0/gtk-build/wx-config ")
    set(wxWidgets_INCLUDE_DIRS "/home/fw591/wxWidgets/wxWidgets-3.1.0/include")
    set(wxWidgets_LIBRARIES "/home/fw591/wxWidgets/wxWidgets-3.1.0/gtk-build/lib")
    set(wxWidgets_DEFINITIONS "__WXGTK__")
    set(wxWidgets_CXX_FLAGS `/home/fw591/wxWidgets/wxWidgets-3.1.0/gtk-build/wx-config --cxxflags`)
    set(wxWidgets_USE_FILE "")
]]



set(CMAKE_LIBRARY_PATH "${LD_LIBRARY_PATH}")
set(CMAKE_CXX_COMPILER "/home/fw591/local/gcc-7.2.0/bin/g++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wno-return-local-addr")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fpic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -I /home/fw591/local/lib/wx/include/gtk2-unicode-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -I /home/fw591/local/include/wx-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -I /home/fw591/local/include/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -D _FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -D WXUSINGDLL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -D __WXGTK__")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D BOOST_STACKTRACE_USE_BACKTRACE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -D BOOST_STACKTRACE_USE_ADDR2LINE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -ansi ")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L/home/fw591/local/lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath,/home/fw591/local/lib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_gtk2u_gl-3.1 -lGL -lGLU -lwx_gtk2u_core-3.1 -lwx_baseu-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_baseu_xml-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_gtk2u_propgrid-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_gtk2u_stc-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_gtk2u_aui-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_gtk2u_xrc-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lglut")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwxtiff-3.1 -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0 -lX11 -lXxf86vm -lSM")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0 -lpng -lz -ljpeg")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwxregexu-3.1 -lwxexpat-3.1 -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwxsvg")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--version-script,/home/fw591/wxWidgets-3.1.0/gtk-build/version-script -lz -ldl -lm  -lz -ldl -lm")

#[[
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_baseu-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_baseu_net-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_baseu_xml-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwxexpat-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_gtk2u_adv-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_gtk2u_aui-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_gtk2u_core-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_gtk2u_gl-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_gtk2u_html-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwx_gtk2u_propgrid-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Lwx_gtk2u_qa-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Lwx_gtk2u_ribbon-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Lwx_gtk2u_richtext-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Lwx_gtk2u_stc-3.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Lwx_gtk2u_xrc-3.1")
]]

include_directories("/home/fw591/boost_1_66_0")
include_directories("//home/fw591/local/include/wx-3.1")
include_directories("/home/fw591/freeglut-3.0.0/include")
include_directories("/home/fw591/dev/cnc/CncControlerGui")

link_directories("/home/fw591/local/lib")

# to log all properties
if ( false )
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()

endif()

file(MAKE_DIRECTORY "/home/fw591/dev/cnc/CncControlerGui/Release")
set(EXECUTABLE_OUTPUT_PATH "/home/fw591/dev/cnc/CncControlerGui/Release")

set(SOURCE_FILES
	CncControlerGui/Tests/Test_Base.h
	CncControlerGui/Tests/Test_Base.cpp
	CncControlerGui/Tests/Test_CncPathListManager.h
	CncControlerGui/Tests/Test_FrameworkCallback.h
	CncControlerGui/Tests/Test_SVGNodeParser.h
	CncControlerGui/OSD/CncTimeFunctions.cpp
	CncControlerGui/3D/CncGCodePreview.cpp
	CncControlerGui/3D/CncGCodePreview.h
	CncControlerGui/3D/GLCncPathData.cpp
	CncControlerGui/3D/GLCncPathData.h
	CncControlerGui/3D/GLContextBase.cpp
	CncControlerGui/3D/GLContextBase.h
	CncControlerGui/3D/GLContextCncPath.cpp
	CncControlerGui/3D/GLContextCncPath.h
	CncControlerGui/3D/GLContextGCodePreview.cpp
	CncControlerGui/3D/GLContextGCodePreview.h
	CncControlerGui/3D/GLContextPathBase.cpp
	CncControlerGui/3D/GLContextPathBase.h
	CncControlerGui/3D/GLContextTestCube.cpp
	CncControlerGui/3D/GLContextTestCube.h
	CncControlerGui/3D/GLHelperCamera.h
	CncControlerGui/3D/GLHelperModel.h
	CncControlerGui/3D/GLViewPort.cpp
	CncControlerGui/3D/GLViewPort.h
	CncControlerGui/3D/VerticeData.h
	CncControlerGui/Clipper/clipper.cpp
	CncControlerGui/Clipper/clipper.hpp
	CncControlerGui/CfgApplicationPage.cpp
	CncControlerGui/CfgEnvironmentPage.cpp
	CncControlerGui/CfgGCodeOptionPage.cpp
	CncControlerGui/CfgGeneralPage.cpp
	CncControlerGui/CfgSVGOptionPage.cpp
	CncControlerGui/CfgWorkingPage.cpp
	CncControlerGui/CncArduino.cpp
	CncControlerGui/CncArduino.h
	CncControlerGui/CncCommon.cpp
	CncControlerGui/CncCommon.h
	CncControlerGui/CncConfig.cpp
	CncControlerGui/CncConfig.h
	CncControlerGui/CncConfigCommon.h
	CncControlerGui/CncConfigExt.h
	CncControlerGui/CncConfigProperty.cpp
	CncControlerGui/CncConfigProperty.h
	CncControlerGui/CncControl.cpp
	CncControlerGui/CncControl.h
	CncControlerGui/CncFileNameService.cpp
	CncControlerGui/CncFileNameService.h
	CncControlerGui/CncFilePreview.cpp
	CncControlerGui/CncFilePreview.h
	CncControlerGui/CncFilePreviewWnd.cpp
	CncControlerGui/CncFilePreviewWnd.h
	CncControlerGui/CncFileView.cpp
	CncControlerGui/CncFileView.h
	CncControlerGui/CncLargeScaleListCtrl.cpp
	CncControlerGui/CncLargeScaleListCtrl.h
	CncControlerGui/CncLimitStates.h
	CncControlerGui/CncMotionMonitor.cpp
	CncControlerGui/CncMotionMonitor.h
	CncControlerGui/CncNumberFormatter.cpp
	CncControlerGui/CncNumberFormatter.h
	CncControlerGui/CncPathListEntry.cpp
	CncControlerGui/CncPathListEntry.h
	CncControlerGui/CncPathListManager.cpp
	CncControlerGui/CncPathListManager.h
	CncControlerGui/CncPatternDefinitions.h
	CncControlerGui/CncPosition.h
	CncControlerGui/CncPosSpyListCtrl.cpp
	CncControlerGui/CncPosSpyListCtrl.h
	CncControlerGui/CncSetterListCtrl.cpp
	CncControlerGui/CncSetterListCtrl.h
	CncControlerGui/CncSpeedSimulator.cpp
	CncControlerGui/CncSpeedSimulator.h
	CncControlerGui/CncSerialSpyListCtrl.cpp
	CncControlerGui/CncSerialSpyListCtrl.h
	CncControlerGui/CncStatisticSummaryListCtrl.cpp
	CncControlerGui/CncStatisticSummaryListCtrl.h
	CncControlerGui/CncStreamBuffers.cpp
	CncControlerGui/CncStreamBuffers.h
	CncControlerGui/CncSummaryListCtrl.cpp
	CncControlerGui/CncSummaryListCtrl.h
	CncControlerGui/CncToolMagazine.cpp
	CncControlerGui/CncToolMagazine.h
	CncControlerGui/CncToolStateControl.cpp
	CncControlerGui/CncToolStateControl.h
	CncControlerGui/OSD/CncUsbPortScanner.cpp
	CncControlerGui/OSD/CncUsbPortScanner.h
	CncControlerGui/CncVectiesListCtrl.cpp
	CncControlerGui/CncVectiesListCtrl.h
	CncControlerGui/CncZView.cpp
	CncControlerGui/CncZView.h
	CncControlerGui/CommandTemplates.h
	CncControlerGui/ConcurrentQueue.h
	CncControlerGui/DataControlModel.cpp
	CncControlerGui/DataControlModel.h
	CncControlerGui/FileParser.cpp
	CncControlerGui/FileParser.h
	CncControlerGui/FileParserRunInfo.h
	CncControlerGui/GCodeBlock.cpp
	CncControlerGui/GCodeBlock.h
	CncControlerGui/GCodeCommands.cpp
	CncControlerGui/GCodeCommands.h
	CncControlerGui/GCodeField.cpp
	CncControlerGui/GCodeField.h
	CncControlerGui/GCodeFileParser.cpp
	CncControlerGui/GCodeFileParser.h
	CncControlerGui/GCodePathHandlerBase.cpp
	CncControlerGui/GCodePathHandlerBase.h
	CncControlerGui/GCodePathHandlerCnc.cpp
	CncControlerGui/GCodePathHandlerCnc.h
	CncControlerGui/GCodePathHandlerGL.cpp
	CncControlerGui/GCodePathHandlerGL.h
	CncControlerGui/GlobalFunctions.cpp
	CncControlerGui/GlobalFunctions.h
	CncControlerGui/HexDecoder.cpp
	CncControlerGui/HexDecoder.h
	CncControlerGui/LinearFunc.h
	CncControlerGui/LruFileList.h
	CncControlerGui/main.cpp
	CncControlerGui/MainFrame.cpp
	CncControlerGui/MainFrame.h
	CncControlerGui/ManuallyParser.cpp
	CncControlerGui/ManuallyParser.h
	CncControlerGui/ManuallyPathHandlerCnc.cpp
	CncControlerGui/ManuallyPathHandlerCnc.h
	CncControlerGui/NotebookInfo.h
	CncControlerGui/PathHandlerBase.cpp
	CncControlerGui/PathHandlerBase.h
	CncControlerGui/PenHandler.h
	CncControlerGui/PointAbs.h
	CncControlerGui/CncPerspective.cpp
	CncControlerGui/CncPerspective.h
	CncControlerGui/OSD/CncAsyncKeyboardState.cpp
	CncControlerGui/OSD/CncAsyncKeyboardState.h
	CncControlerGui/OSD/SerialMsw.cpp
	CncControlerGui/OSD/SerialGtk.cpp
	CncControlerGui/Codelite/wxPNGAnimation.cpp
	CncControlerGui/Codelite/wxPNGAnimation.h
	CncControlerGui/SerialEmulatorNull.cpp
	CncControlerGui/SerialEmulatorNull.h
	CncControlerGui/SerialPort.cpp
	CncControlerGui/SerialPort.h
	CncControlerGui/SerialSpyPort.cpp
	CncControlerGui/SerialSpyPort.h
	CncControlerGui/StdStreamRedirector.h
	CncControlerGui/strsafe.h
	CncControlerGui/SvgCncParameters.h
	CncControlerGui/SvgEditPopup.cpp
	CncControlerGui/SvgEditPopup.h
	CncControlerGui/SVGElementConverter.cpp
	CncControlerGui/SVGElementConverter.h
	CncControlerGui/SVGFileParser.cpp
	CncControlerGui/SVGFileParser.h
	CncControlerGui/SVGNodeParser.cpp
	CncControlerGui/SVGNodeParser.h
	CncControlerGui/SVGPathHandlerCnc.cpp
	CncControlerGui/SVGPathHandlerCnc.h
	CncControlerGui/SvgTransformMatrix.h
	CncControlerGui/SvgUnitCalculator.cpp
	CncControlerGui/SvgUnitCalculator.h
	CncControlerGui/SVGUserAgent.h
	CncControlerGui/SVGUserAgentInfo.h
	CncControlerGui/SvgViewBox.h
	CncControlerGui/UnitTestFrame.cpp
	CncControlerGui/UnitTestFrame.h
	CncControlerGui/UpdateManagerThread.cpp
	CncControlerGui/UpdateManagerThread.h
	CncControlerGui/wxcrafter.cpp
	CncControlerGui/wxcrafter.h
	CncControlerGui/3D/CncGLCanvas.cpp
	CncControlerGui/CncCurveLib.cpp
	CncControlerGui/CncBaseEditor.cpp
	CncControlerGui/CncSourceEditor.cpp
	CncControlerGui/CncOutboundEditor.cpp
	CncControlerGui/CncUnitCalculator.cpp
	CncControlerGui/SvgViewBox.cpp
	CncControlerGui/BinaryFileParser.cpp
	CncControlerGui/CncMonitorSplitterWindow.cpp
	CncControlerGui/3D/GLLabelCluser.cpp
	CncControlerGui/3D/GLLineCluster.cpp
	CncControlerGui/CncReferencePosition.cpp
	CncControlerGui/CncConnectProgress.cpp
	CncControlerGui/CfgAccelerationGraph.cpp
	CncControlerGui/CncGamepadControllerState.cpp
	CncControlerGui/GamepadThread.cpp
	CncControlerGui/CncBinaryTemplateStreamer.cpp
	CncControlerGui/SVGPathHandlerBase.cpp
	CncControlerGui/SerialEmulatorBinaryStreamer.cpp
	CncControlerGui/SerialEmulatorGCodeStreamer.cpp
	CncControlerGui/SerialEmulatorSvgStreamer.cpp
	CncControlerGui/SerialEmulatorTextStreamer.cpp
	CncControlerGui/sha1.cpp
	CncControlerGui/GL3DOptionPane.cpp
	CncControlerGui/GL3DDrawPane.cpp
	CncControlerGui/CncSecureRun.cpp
	CncControlerGui/BinaryPathHandlerBase.cpp
	CncControlerGui/BinaryPathHandlerCnc.cpp
	CncControlerGui/CncCommandDecoder.cpp
	CncControlerGui/CncOSDConfigList.cpp
	CncControlerGui/CncTemplateObserver.cpp
	CncControlerGui/CncMonitorReplayPane.cpp
	CncControlerGui/CncStatisticsPane.cpp
	CncControlerGui/CncMouseRemoteControl.cpp
	CncControlerGui/CncNavigatorPanel.cpp
	CncControlerGui/CncOSEnvironmentDialog.cpp
	CncControlerGui/CncSvgControl.cpp
	CncControlerGui/CncSpeedMonitor.cpp
	CncControlerGui/CncContext.cpp
	CncControlerGui/CncGameportController.cpp
	CncControlerGui/CncMessageDialog.cpp
	CncControlerGui/CncMoveSequence.cpp
	CncControlerGui/CncPathListRunner.cpp
	CncControlerGui/wxcrafter_bitmaps.cpp
)

add_executable(cnc ${SOURCE_FILES})
