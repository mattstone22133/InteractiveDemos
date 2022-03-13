
macro(HTMLPreloadDirectory TARGET FOLDER_PATH)

	if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
		#message(STATUS "emscripten webassembly build detected in HTMLPreloadDirectory.")

		##below appears to cause issue in that, if you need to preload multiple files, it only does a single --preload file, then lists the directories after it.
		#but that is not the proper way to do it, at least it doesn't seem to be -- based on the number of times --preoad_file is included in this example. https://stackoverflow.com/questions/45260216/emscripten-cmake-pass-emscripten-options-in-cmakelist-file
		#target_link_options(${TARGET} PUBLIC --preload-file "${FOLDER_PATH}") # must use quotes for spaces in file paths to work
		
		#below does not appear to append, rather it overwrites. perhaps need to query like the debug get_target_property and append to what is here.
		#set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "--preload-file \"${FOLDER_PATH}\"") # must use quotes for spaces in file paths to work

		get_target_property(PreviousLinkFlags ${TARGET} LINK_FLAGS)
		if(${PreviousLinkFlags} MATCHES "PreviousLinkFlags-NOTFOUND")
			#https://cmake.org/cmake/help/latest/command/get_target_property.html can funtions not return bools? seems bad to populate a variable with error garbage and have to do string comparisons to clean it up...
			set(PreviousLinkFlags "") 
		endif()
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${PreviousLinkFlags} --preload-file \"${FOLDER_PATH}\"") # must use quotes for spaces in file paths to work

		##-------------debug set_target_properties-------------
		#get_target_property(LinkFlagsVar ${TARGET} LINK_FLAGS)
		#message(STATUS "DEBUG html target properties LINK_FLAGS: ${LinkFlagsVar}")
	endif()

endmacro()


macro(HTMLUseTemplateHtmlFile TARGET HTML_TEMPLATE_PATH)
	if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
		#perhaps target_link_options can work here instead, but it did have issues with removing all but one --preload-file argument
		get_target_property(PreviousLinkFlags ${TARGET} LINK_FLAGS)
		if(${PreviousLinkFlags} MATCHES "PreviousLinkFlags-NOTFOUND")			
			set(PreviousLinkFlags "") 
		endif()
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${PreviousLinkFlags} --shell-file \"${HTML_TEMPLATE_PATH}\"") # must use quotes for spaces in file paths to work
	endif()
endmacro()

macro(HTML_AppendLinkFlag TARGET HTML_LINK_FLAG)
	if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
		get_target_property(PreviousLinkFlags ${TARGET} LINK_FLAGS)
		if(${PreviousLinkFlags} MATCHES "PreviousLinkFlags-NOTFOUND")			
			set(PreviousLinkFlags "") 
		endif()
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${PreviousLinkFlags} ${HTML_LINK_FLAG}") # must use quotes for spaces in file paths to work
	endif()
endmacro()