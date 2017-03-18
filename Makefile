.PHONY: clean All

All:
	@echo "----------Building project:[ CncControlerGui - Release ]----------"
	@cd "CncControlerGui" && "$(MAKE)" -f  "CncControlerGui.mk"
clean:
	@echo "----------Cleaning project:[ CncControlerGui - Release ]----------"
	@cd "CncControlerGui" && "$(MAKE)" -f  "CncControlerGui.mk" clean
