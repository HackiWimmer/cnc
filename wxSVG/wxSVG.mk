##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=wxSVG
ConfigurationName      :=Release
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/wxSVG
IntermediateDirectory  :=Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=07/12/2018
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
OutputFile             :=$(IntermediateDirectory)/$(ProjectName).dll
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="wxSVG.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/@Development/Compilers/TDM-GCC-64/bin/windres.exe
LinkOptions            :=  /DEF:$(ProjectName).def
IncludePath            :=  $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/@Development/Compilers/TDM-GCC-64/bin/ar.exe rcu
CXX      := C:/@Development/Compilers/TDM-GCC-64/bin/g++.exe
CC       := C:/@Development/Compilers/TDM-GCC-64/bin/gcc.exe
CXXFLAGS :=   $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := C:/@Development/Compilers/TDM-GCC-64/bin/as.exe


##
## User defined environment variables
##
Objects0=$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransform.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextContentElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGSVGElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRect.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformList.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRadialGradientElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPreserveAspectRatio.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolygonElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPointList.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRectElement.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPoint.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathSegList.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGNumberList.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMatrix.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMarkerElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLocatable.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_NodeList.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAngle.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFitToViewBox.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolylineElement.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPaint.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_mediadec_ffmpeg.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStringList.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttribute.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_imagsvg.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvas.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_ExifHandler.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTests.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_SetAttribute.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGUseElement.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStylable.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttributes.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformable.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_GetSVGDocument.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSValue.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateTransformElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSStyleDeclaration.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_properties.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_HasAttribute.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_CopyConstructors.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_svgctrl.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_colors.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateMotionElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGVideoElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLengthList.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvasItem.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimationElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCircleElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGColor.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_values.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFilterElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument_CreateElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGElementInstanceList.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFEGaussianBlurElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGEllipseElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGImageElement.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimatedType.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLength.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLineElement.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)
	@$(MakeDirCommand) "C:\@Development\@Projekte\c++\CNCGuiController/.build-release"
	@echo rebuilt > "C:\@Development\@Projekte\c++\CNCGuiController/.build-release/wxSVG"

MakeIntermediateDirs:
	@$(MakeDirCommand) "Release"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "Release"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransform.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGTransform.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransform.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGTransform.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransform.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransform.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGTransform.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransform.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransform.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGTransform.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransform.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGTransform.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransform.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGTransform.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextContentElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGTextContentElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextContentElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGTextContentElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextContentElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextContentElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGTextContentElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextContentElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextContentElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGTextContentElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextContentElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGTextContentElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextContentElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGTextContentElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGSVGElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGSVGElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGSVGElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGSVGElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGSVGElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGSVGElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGSVGElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGSVGElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGSVGElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGSVGElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGSVGElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGSVGElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGSVGElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGSVGElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRect.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGRect.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRect.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGRect.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRect.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRect.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGRect.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRect.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRect.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGRect.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRect.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGRect.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRect.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGRect.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformList.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGTransformList.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGTransformList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformList.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGTransformList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformList.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGTransformList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformList.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGTransformList.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformList.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGTransformList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRadialGradientElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGRadialGradientElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRadialGradientElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGRadialGradientElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRadialGradientElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRadialGradientElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGRadialGradientElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRadialGradientElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRadialGradientElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGRadialGradientElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRadialGradientElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGRadialGradientElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRadialGradientElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGRadialGradientElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPreserveAspectRatio.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGPreserveAspectRatio.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPreserveAspectRatio.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGPreserveAspectRatio.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPreserveAspectRatio.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPreserveAspectRatio.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGPreserveAspectRatio.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPreserveAspectRatio.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPreserveAspectRatio.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGPreserveAspectRatio.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPreserveAspectRatio.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGPreserveAspectRatio.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPreserveAspectRatio.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGPreserveAspectRatio.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolygonElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGPolygonElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolygonElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGPolygonElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolygonElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolygonElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGPolygonElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolygonElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolygonElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGPolygonElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolygonElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGPolygonElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolygonElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGPolygonElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPointList.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGPointList.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPointList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGPointList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPointList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPointList.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGPointList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPointList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPointList.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGPointList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPointList.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGPointList.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPointList.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGPointList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRectElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGRectElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRectElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGRectElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRectElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRectElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGRectElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRectElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRectElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGRectElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRectElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGRectElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGRectElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGRectElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPoint.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGPoint.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPoint.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGPoint.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPoint.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPoint.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGPoint.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPoint.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPoint.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGPoint.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPoint.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGPoint.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPoint.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGPoint.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathSegList.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGPathSegList.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathSegList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGPathSegList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathSegList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathSegList.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGPathSegList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathSegList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathSegList.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGPathSegList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathSegList.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGPathSegList.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathSegList.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGPathSegList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGNumberList.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGNumberList.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGNumberList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGNumberList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGNumberList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGNumberList.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGNumberList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGNumberList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGNumberList.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGNumberList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGNumberList.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGNumberList.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGNumberList.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGNumberList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMatrix.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGMatrix.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMatrix.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGMatrix.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMatrix.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMatrix.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGMatrix.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMatrix.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMatrix.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGMatrix.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMatrix.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGMatrix.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMatrix.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGMatrix.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMarkerElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGMarkerElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMarkerElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGMarkerElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMarkerElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMarkerElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGMarkerElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMarkerElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMarkerElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGMarkerElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMarkerElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGMarkerElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGMarkerElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGMarkerElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLocatable.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGLocatable.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLocatable.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGLocatable.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLocatable.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLocatable.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGLocatable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLocatable.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLocatable.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGLocatable.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLocatable.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGLocatable.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLocatable.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGLocatable.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_NodeList.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/NodeList.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_NodeList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/NodeList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_NodeList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_NodeList.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/NodeList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_NodeList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_NodeList.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/NodeList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_NodeList.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/NodeList.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_NodeList.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/NodeList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAngle.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGAngle.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAngle.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGAngle.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAngle.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAngle.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGAngle.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAngle.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAngle.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGAngle.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAngle.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGAngle.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAngle.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGAngle.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFitToViewBox.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGFitToViewBox.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFitToViewBox.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGFitToViewBox.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFitToViewBox.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFitToViewBox.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGFitToViewBox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFitToViewBox.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFitToViewBox.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGFitToViewBox.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFitToViewBox.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGFitToViewBox.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFitToViewBox.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGFitToViewBox.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolylineElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGPolylineElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolylineElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGPolylineElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolylineElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolylineElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGPolylineElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolylineElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolylineElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGPolylineElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolylineElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGPolylineElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPolylineElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGPolylineElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPaint.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGPaint.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPaint.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGPaint.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPaint.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPaint.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGPaint.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPaint.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPaint.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGPaint.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPaint.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGPaint.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPaint.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGPaint.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_mediadec_ffmpeg.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/mediadec_ffmpeg.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_mediadec_ffmpeg.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/mediadec_ffmpeg.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_mediadec_ffmpeg.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_mediadec_ffmpeg.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/mediadec_ffmpeg.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_mediadec_ffmpeg.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_mediadec_ffmpeg.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/mediadec_ffmpeg.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_mediadec_ffmpeg.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/mediadec_ffmpeg.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_mediadec_ffmpeg.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/mediadec_ffmpeg.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStringList.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGStringList.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStringList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGStringList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStringList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStringList.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGStringList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStringList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStringList.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGStringList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStringList.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGStringList.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStringList.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGStringList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttribute.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/Elements_GetAttribute.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttribute.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/Elements_GetAttribute.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttribute.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttribute.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/Elements_GetAttribute.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttribute.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttribute.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/Elements_GetAttribute.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttribute.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/Elements_GetAttribute.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttribute.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/Elements_GetAttribute.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_imagsvg.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/imagsvg.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_imagsvg.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/imagsvg.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_imagsvg.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_imagsvg.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/imagsvg.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_imagsvg.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_imagsvg.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/imagsvg.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_imagsvg.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/imagsvg.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_imagsvg.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/imagsvg.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvas.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGCanvas.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvas.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGCanvas.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvas.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvas.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGCanvas.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvas.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvas.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGCanvas.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvas.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGCanvas.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvas.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGCanvas.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_ExifHandler.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/ExifHandler.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_ExifHandler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/ExifHandler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_ExifHandler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_ExifHandler.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/ExifHandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_ExifHandler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_ExifHandler.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/ExifHandler.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_ExifHandler.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/ExifHandler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_ExifHandler.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/ExifHandler.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTests.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGTests.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTests.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGTests.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTests.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTests.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGTests.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTests.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTests.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGTests.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTests.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGTests.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTests.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGTests.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_SetAttribute.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/Elements_SetAttribute.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_SetAttribute.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/Elements_SetAttribute.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_SetAttribute.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_SetAttribute.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/Elements_SetAttribute.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_SetAttribute.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_SetAttribute.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/Elements_SetAttribute.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_SetAttribute.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/Elements_SetAttribute.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_SetAttribute.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/Elements_SetAttribute.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGUseElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGUseElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGUseElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGUseElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGUseElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGUseElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGUseElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGUseElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGUseElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGUseElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGUseElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGUseElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGUseElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGUseElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStylable.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGStylable.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStylable.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGStylable.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStylable.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStylable.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGStylable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStylable.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStylable.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGStylable.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStylable.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGStylable.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGStylable.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGStylable.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttributes.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/Elements_GetAttributes.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttributes.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/Elements_GetAttributes.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttributes.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttributes.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/Elements_GetAttributes.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttributes.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttributes.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/Elements_GetAttributes.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttributes.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/Elements_GetAttributes.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_GetAttributes.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/Elements_GetAttributes.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformable.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGTransformable.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformable.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGTransformable.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformable.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformable.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGTransformable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformable.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformable.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGTransformable.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformable.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGTransformable.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTransformable.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGTransformable.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_GetSVGDocument.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/GetSVGDocument.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_GetSVGDocument.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/GetSVGDocument.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_GetSVGDocument.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_GetSVGDocument.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/GetSVGDocument.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_GetSVGDocument.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_GetSVGDocument.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/GetSVGDocument.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_GetSVGDocument.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/GetSVGDocument.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_GetSVGDocument.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/GetSVGDocument.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSValue.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/CSSValue.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSValue.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/CSSValue.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSValue.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSValue.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/CSSValue.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSValue.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSValue.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/CSSValue.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSValue.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/CSSValue.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSValue.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/CSSValue.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateTransformElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGAnimateTransformElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateTransformElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGAnimateTransformElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateTransformElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateTransformElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGAnimateTransformElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateTransformElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateTransformElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGAnimateTransformElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateTransformElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGAnimateTransformElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateTransformElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGAnimateTransformElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSStyleDeclaration.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/CSSStyleDeclaration.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSStyleDeclaration.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/CSSStyleDeclaration.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSStyleDeclaration.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSStyleDeclaration.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/CSSStyleDeclaration.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSStyleDeclaration.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSStyleDeclaration.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/CSSStyleDeclaration.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSStyleDeclaration.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/CSSStyleDeclaration.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_CSSStyleDeclaration.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/CSSStyleDeclaration.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_properties.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/css_properties.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_properties.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/css_properties.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_properties.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_properties.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/css_properties.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_properties.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_properties.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/css_properties.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_properties.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/css_properties.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_properties.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/css_properties.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_HasAttribute.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/Elements_HasAttribute.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_HasAttribute.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/Elements_HasAttribute.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_HasAttribute.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_HasAttribute.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/Elements_HasAttribute.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_HasAttribute.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_HasAttribute.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/Elements_HasAttribute.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_HasAttribute.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/Elements_HasAttribute.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_HasAttribute.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/Elements_HasAttribute.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_CopyConstructors.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/Elements_CopyConstructors.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_CopyConstructors.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/Elements_CopyConstructors.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_CopyConstructors.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_CopyConstructors.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/Elements_CopyConstructors.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_CopyConstructors.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_CopyConstructors.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/Elements_CopyConstructors.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_CopyConstructors.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/Elements_CopyConstructors.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_Elements_CopyConstructors.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/Elements_CopyConstructors.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_svgctrl.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/svgctrl.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_svgctrl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/svgctrl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_svgctrl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_svgctrl.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/svgctrl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_svgctrl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_svgctrl.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/svgctrl.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_svgctrl.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/svgctrl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_svgctrl.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/svgctrl.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGTextElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGTextElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGTextElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGTextElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGTextElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGTextElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGTextElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_colors.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/css_colors.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_colors.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/css_colors.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_colors.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_colors.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/css_colors.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_colors.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_colors.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/css_colors.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_colors.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/css_colors.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_colors.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/css_colors.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateMotionElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGAnimateMotionElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateMotionElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGAnimateMotionElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateMotionElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateMotionElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGAnimateMotionElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateMotionElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateMotionElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGAnimateMotionElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateMotionElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGAnimateMotionElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimateMotionElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGAnimateMotionElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGVideoElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGVideoElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGVideoElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGVideoElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGVideoElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGVideoElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGVideoElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGVideoElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGVideoElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGVideoElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGVideoElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGVideoElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGVideoElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGVideoElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLengthList.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGLengthList.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLengthList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGLengthList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLengthList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLengthList.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGLengthList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLengthList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLengthList.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGLengthList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLengthList.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGLengthList.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLengthList.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGLengthList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvasItem.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGCanvasItem.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvasItem.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGCanvasItem.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvasItem.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvasItem.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGCanvasItem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvasItem.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvasItem.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGCanvasItem.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvasItem.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGCanvasItem.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCanvasItem.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGCanvasItem.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimationElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGAnimationElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimationElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGAnimationElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimationElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimationElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGAnimationElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimationElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimationElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGAnimationElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimationElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGAnimationElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimationElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGAnimationElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCircleElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGCircleElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCircleElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGCircleElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCircleElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCircleElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGCircleElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCircleElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCircleElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGCircleElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCircleElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGCircleElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGCircleElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGCircleElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGColor.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGColor.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGColor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGColor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGColor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGColor.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGColor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGColor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGColor.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGColor.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGColor.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGColor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGColor.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGColor.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGDocument.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGDocument.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGDocument.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGDocument.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGDocument.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGDocument.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_values.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/css_values.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_values.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/css_values.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_values.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_values.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/css_values.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_values.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_values.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/css_values.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_values.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/css_values.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_css_values.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/css_values.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFilterElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGFilterElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFilterElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGFilterElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFilterElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFilterElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGFilterElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFilterElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFilterElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGFilterElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFilterElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGFilterElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFilterElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGFilterElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument_CreateElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGDocument_CreateElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument_CreateElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGDocument_CreateElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument_CreateElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument_CreateElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGDocument_CreateElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument_CreateElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument_CreateElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGDocument_CreateElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument_CreateElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGDocument_CreateElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGDocument_CreateElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGDocument_CreateElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGElementInstanceList.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGElementInstanceList.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGElementInstanceList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGElementInstanceList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGElementInstanceList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGElementInstanceList.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGElementInstanceList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGElementInstanceList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGElementInstanceList.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGElementInstanceList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGElementInstanceList.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGElementInstanceList.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGElementInstanceList.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGElementInstanceList.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGPathElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGPathElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGPathElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGPathElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGPathElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGPathElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGPathElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFEGaussianBlurElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGFEGaussianBlurElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFEGaussianBlurElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGFEGaussianBlurElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFEGaussianBlurElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFEGaussianBlurElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGFEGaussianBlurElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFEGaussianBlurElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFEGaussianBlurElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGFEGaussianBlurElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFEGaussianBlurElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGFEGaussianBlurElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGFEGaussianBlurElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGFEGaussianBlurElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGEllipseElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGEllipseElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGEllipseElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGEllipseElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGEllipseElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGEllipseElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGEllipseElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGEllipseElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGEllipseElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGEllipseElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGEllipseElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGEllipseElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGEllipseElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGEllipseElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGImageElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGImageElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGImageElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGImageElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGImageElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGImageElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGImageElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGImageElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGImageElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGImageElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGImageElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGImageElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGImageElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGImageElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimatedType.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGAnimatedType.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimatedType.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGAnimatedType.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimatedType.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimatedType.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGAnimatedType.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimatedType.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimatedType.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGAnimatedType.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimatedType.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGAnimatedType.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGAnimatedType.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGAnimatedType.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLength.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGLength.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLength.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGLength.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLength.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLength.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGLength.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLength.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLength.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGLength.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLength.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGLength.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLength.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGLength.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLineElement.cpp$(ObjectSuffix): ../../../../wxsvg-1.5.15/src/SVGLineElement.cpp $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLineElement.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/wxsvg-1.5.15/src/SVGLineElement.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLineElement.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLineElement.cpp$(DependSuffix): ../../../../wxsvg-1.5.15/src/SVGLineElement.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLineElement.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLineElement.cpp$(DependSuffix) -MM ../../../../wxsvg-1.5.15/src/SVGLineElement.cpp

$(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLineElement.cpp$(PreprocessSuffix): ../../../../wxsvg-1.5.15/src/SVGLineElement.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_up_wxsvg-1.5.15_src_SVGLineElement.cpp$(PreprocessSuffix) ../../../../wxsvg-1.5.15/src/SVGLineElement.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r Release/


