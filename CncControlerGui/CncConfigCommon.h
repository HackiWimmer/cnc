#ifndef CNC_CONFIG_COMMON_H
#define CNC_CONFIG_COMMON_H

	#define CncToolMagazine_SECTION_NAME					"CncWork.ToolMagazine"
	#define CncToolMagazineParam_SECTION_NAME				"CncWork.ToolMagazineParameter"
	#define CncToolMagazineParam_USE_DEF_TOOL				"USE_DEFAULT_TOOL"
	#define CncToolMagazineParam_MAP_DEF_TOOL_TO			"MAP_DEFAULT_TOOL_TO"
		
	#define Attribute_READONLY								"READONLY"

	#define CncConfig_MAX_SPEED_XYZ_MM_MIN					"Cnc.Config/MAX_SPEED_XYZ_MM_MIN"
	#define CncConfig_MAX_SPEED_XY_MM_MIN					"Cnc.Config/MAX_SPEED_XY_MM_MIN"
	#define CncConfig_MAX_SPEED_X_MM_MIN					"Cnc.Config/MAX_SPEED_X_MM_MIN"
	#define CncConfig_MAX_SPEED_Y_MM_MIN					"Cnc.Config/MAX_SPEED_Y_MM_MIN"
	#define CncConfig_MAX_SPEED_Z_MM_MIN					"Cnc.Config/MAX_SPEED_Z_MM_MIN"
	
	#define CncConfig_ACCEL_START_SPEED_X_MM_MIN			"Cnc.Config/ACCEL_START_SPEED_X_MM_MIN"
	#define CncConfig_ACCEL_STOP_SPEED_X_MM_MIN				"Cnc.Config/ACCEL_STOP_SPEED_X_MM_MIN"
	#define CncConfig_ACCEL_START_SPEED_Y_MM_MIN			"Cnc.Config/ACCEL_START_SPEED_Y_MM_MIN"
	#define CncConfig_ACCEL_STOP_SPEED_Y_MM_MIN				"Cnc.Config/ACCEL_STOP_SPEED_Y_MM_MIN"
	#define CncConfig_ACCEL_START_SPEED_Z_MM_MIN			"Cnc.Config/ACCEL_START_SPEED_Z_MM_MIN"
	#define CncConfig_ACCEL_STOP_SPEED_Z_MM_MIN				"Cnc.Config/ACCEL_STOP_SPEED_Z_MM_MIN"
	
	#define CncConfig_MAX_WORK_SPEED_X_MM_MIN				"Cnc.Config/MAX_WORK_SPEED_X_MM_MIN"
	#define CncConfig_MAX_WORK_SPEED_Y_MM_MIN				"Cnc.Config/MAX_WORK_SPEED_Y_MM_MIN"
	#define CncConfig_MAX_WORK_SPEED_Z_MM_MIN				"Cnc.Config/MAX_WORK_SPEED_Z_MM_MIN"
	
	#define CncConfig_DEF_SPEED_MODE_XYZ					"Cnc.Config/DEF_SPEED_MODE_XYZ"
	#define CncConfig_DEF_RAPID_SPEED_PERCENT 				"Cnc.Config/DEF_RAPID_SPEED_PEERCENT"
	#define CncConfig_DEF_WORK_SPEED_PERCENT				"Cnc.Config/DEF_WORK_SPEED_PERCENT"
	#define CncConfig_DEF_RAPID_SPEED_MM_MIN 				"Cnc.Config/DEF_RAPID_SPEED_MM_SEC"
	#define CncConfig_DEF_WORK_SPEED_MM_MIN 				"Cnc.Config/DEF_WORK_SPEED_MM_SEC"
	
	#define CncConfig_MAX_DIMENSION_X 						"Cnc.Config/MAX_DIMENSION_X"
	#define CncConfig_MAX_DIMENSION_Y 						"Cnc.Config/MAX_DIMENSION_Y"
	#define CncConfig_MAX_DIMENSION_Z 						"Cnc.Config/MAX_DIMENSION_Z"
	#define CncConfig_STEPS_X 								"Cnc.Config/STEPS_X"
	#define CncConfig_STEPS_Y 								"Cnc.Config/STEPS_Y"
	#define CncConfig_STEPS_Z								"Cnc.Config/STEPS_Z"
	#define CncConfig_PITCH_X 								"Cnc.Config/PITCH_X"
	#define CncConfig_PITCH_Y 								"Cnc.Config/PITCH_Y"
	#define CncConfig_PITCH_Z 								"Cnc.Config/PITCH_Z"
	#define CncConfig_MULTIPLIER_X 							"Cnc.Config/MULTIPLIER_X"
	#define CncConfig_MULTIPLIER_Y 							"Cnc.Config/MULTIPLIER_Y"
	#define CncConfig_MULTIPLIER_Z 							"Cnc.Config/MULTIPLIER_Z"
	#define CncConfig_PULSE_WIDTH_HIGH_X					"Cnc.Config/PULSE_WIDTH_HIGH_X"
	#define CncConfig_PULSE_WIDTH_HIGH_Y					"Cnc.Config/PULSE_WIDTH_HIGH_Y"
	#define CncConfig_PULSE_WIDTH_HIGH_Z					"Cnc.Config/PULSE_WIDTH_HIGH_Z"

	#define CncRuntime_DISPLAY_FACT_X 						"#/CncRuntime/DISPLAY_FACT_X"
	#define CncRuntime_DISPLAY_FACT_Y 						"#/CncRuntime/DISPLAY_FACT_Y"
	#define CncRuntime_DISPLAY_FACT_Z 						"#/CncRuntime/DISPLAY_FACT_Z"
	#define CncRuntime_DISPLAY_FACT_3D_X 					"#/CncRuntime/DISPLAY_FACT_3D_X"
	#define CncRuntime_DISPLAY_FACT_3D_Y					"#/CncRuntime/DISPLAY_FACT_3D_Y"
	#define CncRuntime_DISPLAY_FACT_3D_Z 					"#/CncRuntime/DISPLAY_FACT_3D_Z"
	#define CncRuntime_CALCULATION_FACT_X 					"#/CncRuntime/CALCULATION_FACT_X"
	#define CncRuntime_CALCULATION_FACT_Y 					"#/CncRuntime/CALCULATION_FACT_Y"
	#define CncRuntime_CALCULATION_FACT_Z 					"#/CncRuntime/CALCULATION_FACT_Z"
	
	#define CncRuntime_Z_MAX_DURATIONS						"#/CncRuntime/Z_MAX_DURATIONS"
	#define CncRuntime_Z_WORKPIECE_OFFSET					"#/CncRuntime/Z_WORKPIECE_OFFSET"
	#define CncRuntime_Z_MAX_DURATION_THICKNESS				"#/CncRuntime/Z_MAX_DURATION_THICKNESS"
	#define CncRuntime_Z_CALCULATED_DURATIONS				"#/CncRuntime/Z_CALCULATED_DURATIONS"
	#define CncRuntime_Z_CURRENT_Z_DISTANCE					"#/CncRuntime/Z_CURRENT_Z_DISTANCE"
	#define CncRuntime_Z_WORKPIECE_INCLUDED					"#/CncRuntime/Z_WORKPIECE_INCLUDED"
	#define CncRuntime_Z_DURATION_THICKNESS					"#/CncRuntime/Z_DURATION_THICKNESS"
	
	#define CncApplication_SHOW_SPLASH_SCREEN 				"CncApplication/SHOW_SPLASH_SCREEN"
	#define CncApplication_DEF_DISPLAY_UNIT 				"CncApplication/DEF_DISPLAY_UNIT"
	#define CncApplication_AUTO_CONNECT 					"CncApplication/AUTO_CONNECT"
	#define CncApplication_AUTO_OPEN_LAST 					"CncApplication/AUTO_OPEN_LAST"
	#define CncApplication_AUTO_PROCESS 					"CncApplication/AUTO_PROCESS"
	#define CncApplication_AUTO_OPEN_OGL_OBSERVER			"CncApplication/AUTO_OPEN_OGL_OBSERVER"
	#define CncApplication_SHOW_TEST_MENU 					"CncApplication/SHOW_TEST_MENU"
	#define CncApplication_CONFIRMATION_MODE				"CncApplication/CONFIRMATION_MODE"
	#define CncApplication_USE_MAIN_PREVIEW					"CncApplication/USE_MAIN_PREVIEW"
	#define CncApplication_USE_MONITOR_PREVIEW				"CncApplication/USE_MONITOR_PREVIEW"
	#define CncApplication_Com_DEFALT_PORT 					"CncApplication.Port/DEFAULT_PORT"
	#define CncApplication_Tpl_DEFALT_DIRECTORY				"CncApplication.Template/OSD_DEFAULT_DIRECTORY"
	#define CncApplication_Tpl_DEFALT_FILE 					"CncApplication.Template/OSD_DEFAULT_FILE"
	#define CncApplication_Tool_FILE_BROWSER				"CncApplication.Tools/OSD_FILE_BROWSER"
	#define CncApplication_Tool_SVG_FILE_VIEWER 			"CncApplication.Tools/OSD_SVG_FILE_VIEWER"
	#define CncApplication_Tool_BIN_FILE_VIEWER 			"CncApplication.Tools/OSD_BIN_FILE_VIEWER"
	#define CncApplication_Tool_GCODE_FILE_VIEWER 			"CncApplication.Tools/OSD_GCODE_FILE_VIEWER"
	#define CncApplication_Tool_XML_FILE_VIEWER 			"CncApplication.Tools/OSD_XML_FILE_VIEWER"
	#define CncApplication_Tool_BROWSER					 	"CncApplication.Tools/OSD_BROWSER"
	#define CncApplication_Tool_EXTERNAL_EDITOR 			"CncApplication.Tools/OSD_EXTERNAL_EDITOR"
	#define CncApplication_Tool_PY_CAM 						"CncApplication.Tools/OSD_PY_CAM"
	
	#define CncSvg_Parser_REVERSE_Y_AXIS					"CncSvg.Parser/CONVERT_TO_RIGHT_HAND"
	
	#define CncWork_Wpt_MAX_THICKNESS_CROSS					"CncWork.Workpiece/MAX_THICKNESS_PER_CROSSING"

	#define CncWork_Ctl_REPLY_THRESHOLD_METRIC				"CncWork.Controller/REPLY_THRESHOLD_METRIC"
	#define CncWork_Ctl_REPLY_THRESHOLD_SETPS				"CncWork.Controller/REPLY_THRESHOLD_STEPS"
	#define CncWork_Ctl_AVOID_DUP_SETTER_VALUES				"CncWork.Controller/AVOID_DUP_SETTER_VALUES"
	#define CncWork_Ctl_REQUEST_IDLE_REQUESTS				"CncWork.Controller/REQUEST_IDLE_REQUESTS"
	#define CncWork_Ctl_INTERRUPT_BY_POS_OUTOFRANGE			"CncWork.Controller/INTERRUPT_BY_POS_OUTOFRANGE"
	#define CncWork_Ctl_INVERSE_CTL_DIRECTION_X				"CncWork.Controller/INVERSE_CTL_DIRECTION_X"
	#define CncWork_Ctl_INVERSE_CTL_DIRECTION_Y				"CncWork.Controller/INVERSE_CTL_DIRECTION_Y"
	#define CncWork_Ctl_INVERSE_CTL_DIRECTION_Z				"CncWork.Controller/INVERSE_CTL_DIRECTION_Z"
	#define CncWork_Ctl_PRE_PROSSOR_ANALYSE					"CncWork.Controller/PRE_PROSSOR_ANALYSE"
	#define CncWork_Ctl_PRE_PROSSOR_SKIP_EMPTY				"CncWork.Controller/PRE_PROSSOR_SKIP_EMPTY"
	#define CncWork_Ctl_PRE_PROSSOR_COMBINE_MOVES			"CncWork.Controller/PRE_PROSSOR_COMBINE_MOVES"
	#define CncWork_Ctl_PRE_PROSSOR_CNT_PATH_LIST_ENTRIES	"CncWork.Controller/PRE_PROSSOR_CNT_PATH_LIST_ENTRIES"
	#define CncWork_Ctl_PRE_PROSSOR_CNT_SEQUENCE_MOVES		"CncWork.Controller/PRE_PROSSOR_CNT_SEQUENCE_MOVES"

	#define CncTEST_TEST									"CncTest.Test/DUMMY"

#endif