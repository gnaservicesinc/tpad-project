if(NOT DEFINED TPAD_APP_BUNDLE OR TPAD_APP_BUNDLE STREQUAL "")
	message(FATAL_ERROR "TPAD_APP_BUNDLE must name the deployed tpad.app bundle.")
endif()

set(_tpad_required_bundle_items
	"Contents/Frameworks/QtCore.framework/Versions/A/QtCore"
	"Contents/Frameworks/QtGui.framework/Versions/A/QtGui"
	"Contents/Frameworks/QtWidgets.framework/Versions/A/QtWidgets"
	"Contents/Frameworks/QtPrintSupport.framework/Versions/A/QtPrintSupport"
	"Contents/PlugIns/platforms/libqcocoa.dylib"
)

foreach(_tpad_bundle_item IN LISTS _tpad_required_bundle_items)
	if(NOT EXISTS "${TPAD_APP_BUNDLE}/${_tpad_bundle_item}")
		message(FATAL_ERROR
			"macdeployqt did not deploy ${_tpad_bundle_item} into "
			"${TPAD_APP_BUNDLE}. The application bundle would not be portable.")
	endif()
endforeach()

message(STATUS
	"Verified Qt frameworks, PrintSupport, and the Cocoa platform plugin in "
	"${TPAD_APP_BUNDLE}")
