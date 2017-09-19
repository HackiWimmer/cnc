.PHONY: clean All

All:
	@echo "----------Building project:[ FreeGlutWorkspace - Release ]----------"
	@cd "FreeGlutWorkspace" && "$(MAKE)" -f  "FreeGlutWorkspace.mk"
clean:
	@echo "----------Cleaning project:[ FreeGlutWorkspace - Release ]----------"
	@cd "FreeGlutWorkspace" && "$(MAKE)" -f  "FreeGlutWorkspace.mk" clean
