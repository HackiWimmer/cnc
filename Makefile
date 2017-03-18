.PHONY: clean All

All:
	@echo "----------Building project:[ CncControllerGui - Release ]----------"
	@cd "CncControlerGui" && "$(MAKE)" -f  "CncControllerGui.mk"
clean:
	@echo "----------Cleaning project:[ CncControllerGui - Release ]----------"
	@cd "CncControlerGui" && "$(MAKE)" -f  "CncControllerGui.mk" clean
