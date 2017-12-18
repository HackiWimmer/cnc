##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=Arduino
ConfigurationName      :=Release
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/Arduino
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=18/12/2017
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
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="Arduino.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/@Development/Compilers/TDM-GCC-64/bin/windres.exe
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
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
CXXFLAGS :=  -O2 -Wall $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/@Development/Compilers/TDM-GCC-64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
WXWIN:=C:\@Development\wxWidgets-3.1.0
WXCFG:=gcc_dll\mswu
Objects0=$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_StepperControl_1.ino$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncStepper.ino$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncController.ino$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncTestSuite.ino$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CommonFunctions.ino$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_LastErrorCodes.ino$(ObjectSuffix) 



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
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@$(MakeDirCommand) "./Release"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Release"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_StepperControl_1.ino$(ObjectSuffix): ../../../@StepperControl/StepperControl_1/StepperControl_1.ino $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_StepperControl_1.ino$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/@StepperControl/StepperControl_1/StepperControl_1.ino" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_StepperControl_1.ino$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_StepperControl_1.ino$(DependSuffix): ../../../@StepperControl/StepperControl_1/StepperControl_1.ino
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_StepperControl_1.ino$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_StepperControl_1.ino$(DependSuffix) -MM ../../../@StepperControl/StepperControl_1/StepperControl_1.ino

$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_StepperControl_1.ino$(PreprocessSuffix): ../../../@StepperControl/StepperControl_1/StepperControl_1.ino
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_StepperControl_1.ino$(PreprocessSuffix) ../../../@StepperControl/StepperControl_1/StepperControl_1.ino

$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncStepper.ino$(ObjectSuffix): ../../../@StepperControl/StepperControl_1/CncStepper.ino $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncStepper.ino$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/@StepperControl/StepperControl_1/CncStepper.ino" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncStepper.ino$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncStepper.ino$(DependSuffix): ../../../@StepperControl/StepperControl_1/CncStepper.ino
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncStepper.ino$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncStepper.ino$(DependSuffix) -MM ../../../@StepperControl/StepperControl_1/CncStepper.ino

$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncStepper.ino$(PreprocessSuffix): ../../../@StepperControl/StepperControl_1/CncStepper.ino
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncStepper.ino$(PreprocessSuffix) ../../../@StepperControl/StepperControl_1/CncStepper.ino

$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncController.ino$(ObjectSuffix): ../../../@StepperControl/StepperControl_1/CncController.ino $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncController.ino$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/@StepperControl/StepperControl_1/CncController.ino" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncController.ino$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncController.ino$(DependSuffix): ../../../@StepperControl/StepperControl_1/CncController.ino
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncController.ino$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncController.ino$(DependSuffix) -MM ../../../@StepperControl/StepperControl_1/CncController.ino

$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncController.ino$(PreprocessSuffix): ../../../@StepperControl/StepperControl_1/CncController.ino
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncController.ino$(PreprocessSuffix) ../../../@StepperControl/StepperControl_1/CncController.ino

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/Arduino/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncTestSuite.ino$(ObjectSuffix): ../../../@StepperControl/StepperControl_1/CncTestSuite.ino $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncTestSuite.ino$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/@StepperControl/StepperControl_1/CncTestSuite.ino" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncTestSuite.ino$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncTestSuite.ino$(DependSuffix): ../../../@StepperControl/StepperControl_1/CncTestSuite.ino
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncTestSuite.ino$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncTestSuite.ino$(DependSuffix) -MM ../../../@StepperControl/StepperControl_1/CncTestSuite.ino

$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncTestSuite.ino$(PreprocessSuffix): ../../../@StepperControl/StepperControl_1/CncTestSuite.ino
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CncTestSuite.ino$(PreprocessSuffix) ../../../@StepperControl/StepperControl_1/CncTestSuite.ino

$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CommonFunctions.ino$(ObjectSuffix): ../../../@StepperControl/StepperControl_1/CommonFunctions.ino $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CommonFunctions.ino$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/@StepperControl/StepperControl_1/CommonFunctions.ino" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CommonFunctions.ino$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CommonFunctions.ino$(DependSuffix): ../../../@StepperControl/StepperControl_1/CommonFunctions.ino
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CommonFunctions.ino$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CommonFunctions.ino$(DependSuffix) -MM ../../../@StepperControl/StepperControl_1/CommonFunctions.ino

$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CommonFunctions.ino$(PreprocessSuffix): ../../../@StepperControl/StepperControl_1/CommonFunctions.ino
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_CommonFunctions.ino$(PreprocessSuffix) ../../../@StepperControl/StepperControl_1/CommonFunctions.ino

$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_LastErrorCodes.ino$(ObjectSuffix): ../../../@StepperControl/StepperControl_1/LastErrorCodes.ino $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_LastErrorCodes.ino$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/@StepperControl/StepperControl_1/LastErrorCodes.ino" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_LastErrorCodes.ino$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_LastErrorCodes.ino$(DependSuffix): ../../../@StepperControl/StepperControl_1/LastErrorCodes.ino
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_LastErrorCodes.ino$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_LastErrorCodes.ino$(DependSuffix) -MM ../../../@StepperControl/StepperControl_1/LastErrorCodes.ino

$(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_LastErrorCodes.ino$(PreprocessSuffix): ../../../@StepperControl/StepperControl_1/LastErrorCodes.ino
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_@StepperControl_StepperControl_1_LastErrorCodes.ino$(PreprocessSuffix) ../../../@StepperControl/StepperControl_1/LastErrorCodes.ino


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Release/


