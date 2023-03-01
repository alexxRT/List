RELEASE:
	@echo ----------------------------------------------------------
	g++ List.cpp ListTest.cpp Memory.cpp ListDebug.cpp -o list
	@echo ----------------------------------------------------------


DEBUG:
	@echo ---------------------------------------------------------------------------------------
	g++  List.cpp ListDebugTest.cpp ListDebug.cpp Memory.cpp -D DEBUG_VERSION -o list_debug
	@echo ---------------------------------------------------------------------------------------

CLEAR:
	rm list_debug