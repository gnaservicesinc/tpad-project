cmake_minimum_required(VERSION 3.21.1)

project(tpad VERSION 7.3.0.6 LANGUAGES C)

include(GNUInstallDirs)
include(CTest)

if(APPLE OR WIN32)
	set(_tpad_qt_default ON)
	set(_tpad_qt_deploy_default ON)
else()
	set(_tpad_qt_default OFF)
	set(_tpad_qt_deploy_default OFF)
endif()

option(TPAD_USE_QT "Build the Qt 6 frontend instead of the GTK 4 frontend"
	${_tpad_qt_default})
option(TPAD_USE_GTK3 "Build the legacy GTK 3 frontend instead of GTK 4" OFF)
set(TPAD_QT_PATH "" CACHE PATH
	"Qt installation prefix (for example /opt/Qt/6.11.1/macos)")
option(TPAD_QT_STATIC
	"Require and link a Qt installation that was built statically" OFF)
option(TPAD_QT_DEPLOY
	"Deploy Qt runtime dependencies when installing a shared-Qt build"
	${_tpad_qt_deploy_default})

if(TPAD_USE_QT AND TPAD_USE_GTK3)
	message(FATAL_ERROR
		"TPAD_USE_QT and TPAD_USE_GTK3 are mutually exclusive. Disable "
		"TPAD_USE_QT to build the GTK 3 frontend.")
endif()

if(TPAD_USE_QT)
	enable_language(CXX)

	if(TPAD_QT_PATH)
		get_filename_component(TPAD_QT_PATH "${TPAD_QT_PATH}" REALPATH)
		if(NOT IS_DIRECTORY "${TPAD_QT_PATH}")
			message(FATAL_ERROR
				"TPAD_QT_PATH does not name an existing Qt installation: "
				"${TPAD_QT_PATH}")
		endif()
		list(PREPEND CMAKE_PREFIX_PATH "${TPAD_QT_PATH}")
		set(Qt6_ROOT "${TPAD_QT_PATH}")
	endif()

	# Tpad intentionally uses APIs available before Qt 6.11, while the build
	# and deployment setup is kept compatible with the current Qt 6.11.1 kit.
	if(TPAD_QT_PATH)
		find_package(Qt6 6.5 REQUIRED
			COMPONENTS Core Gui Widgets PrintSupport
			PATHS "${TPAD_QT_PATH}"
			NO_DEFAULT_PATH)
	else()
		find_package(Qt6 6.5 REQUIRED
			COMPONENTS Core Gui Widgets PrintSupport)
	endif()
	qt_standard_project_setup(REQUIRES 6.5 SUPPORTS_UP_TO 6.11)

	# A user-supplied kit must win outright.  Otherwise CMake can silently
	# fall back to a system Qt, defeating both version and static-kit choices.
	if(TPAD_QT_PATH)
		get_filename_component(_tpad_qt_config_dir "${Qt6_DIR}" REALPATH)
		file(RELATIVE_PATH _tpad_qt_config_relative
			"${TPAD_QT_PATH}" "${_tpad_qt_config_dir}")
		if(_tpad_qt_config_relative MATCHES "^\\.\\.($|/)"
				OR IS_ABSOLUTE "${_tpad_qt_config_relative}")
			message(FATAL_ERROR
				"CMake found Qt outside TPAD_QT_PATH. Requested "
				"${TPAD_QT_PATH}, but Qt6_DIR is ${Qt6_DIR}.")
		endif()
	endif()

	set(_tpad_qt_sources
		src/qt/main.cpp
		src/qt/tpad_application.cpp
		src/qt/tpad_application.h
		src/qt/tpad_main_window.cpp
		src/qt/tpad_main_window.h
		src/qt/recent_files.cpp
		src/qt/recent_files.h
		src/qt/code_editor.cpp
		src/qt/code_editor.h
		src/qt/syntax_highlighter.cpp
		src/qt/syntax_highlighter.h
		src/qt/spell_checker.cpp
		src/qt/spell_checker.h
		src/qt/resources.qrc
	)

	if(APPLE)
		enable_language(OBJCXX)
		set(_tpad_macos_icon "${CMAKE_CURRENT_SOURCE_DIR}/cmake/macos/tpad.icns")
		set_source_files_properties("${_tpad_macos_icon}" PROPERTIES
			MACOSX_PACKAGE_LOCATION Resources)
		list(APPEND _tpad_qt_sources
			src/qt/spell_checker_mac.mm
			src/qt/writing_tools_mac.mm
			src/qt/writing_tools_mac.h
			"${_tpad_macos_icon}"
		)
	endif()

	if(WIN32)
		configure_file(cmake/windows/tpad.rc.in
			"${CMAKE_CURRENT_BINARY_DIR}/tpad.rc" @ONLY)
		list(APPEND _tpad_qt_sources "${CMAKE_CURRENT_BINARY_DIR}/tpad.rc")
	endif()

	qt_add_executable(tpad WIN32 MACOSX_BUNDLE ${_tpad_qt_sources})
	set_target_properties(tpad PROPERTIES AUTORCC ON)
	target_compile_features(tpad PRIVATE cxx_std_17)
	target_compile_definitions(tpad PRIVATE
		TPAD_USE_QT=1
		TPAD_VERSION="${PROJECT_VERSION}"
		QT_NO_CAST_FROM_ASCII
		QT_NO_CAST_TO_ASCII
	)
	target_link_libraries(tpad PRIVATE
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::PrintSupport
	)

	if(MSVC)
		target_compile_options(tpad PRIVATE
			"$<$<COMPILE_LANGUAGE:CXX,OBJCXX>:/W4;/permissive-;/utf-8>")
	else()
		target_compile_options(tpad PRIVATE
			"$<$<COMPILE_LANGUAGE:CXX,OBJCXX>:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wsign-conversion>")
	endif()

	if(APPLE)
		set(_tpad_apple_version
			"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
		foreach(_tpad_version_component IN ITEMS
				PROJECT_VERSION_MAJOR PROJECT_VERSION_MINOR
				PROJECT_VERSION_PATCH PROJECT_VERSION_TWEAK)
			if(${_tpad_version_component} GREATER 99)
				message(FATAL_ERROR
					"Apple bundle versions support this four-part mapping only "
					"when every project version component is at most 99.")
			endif()
		endforeach()
		# Apple accepts only three numeric components.  Fold major and minor
		# into a monotonically increasing release train, leaving patch and
		# tweak distinct so maintenance releases never reuse a bundle build.
		math(EXPR _tpad_apple_release_train
			"${PROJECT_VERSION_MAJOR} * 100 + ${PROJECT_VERSION_MINOR}")
		if(_tpad_apple_release_train LESS 1)
			message(FATAL_ERROR
				"Apple bundle versions require a positive release train.")
		endif()
		set(_tpad_apple_build_version
			"${_tpad_apple_release_train}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")
		set_target_properties(tpad PROPERTIES
			MACOSX_BUNDLE TRUE
			MACOSX_BUNDLE_BUNDLE_NAME "Tpad"
			MACOSX_BUNDLE_GUI_IDENTIFIER "com.gnaservicesinc.tpad"
			MACOSX_BUNDLE_BUNDLE_VERSION "${_tpad_apple_build_version}"
			MACOSX_BUNDLE_SHORT_VERSION_STRING "${_tpad_apple_version}"
			MACOSX_BUNDLE_ICON_FILE "tpad.icns"
			MACOSX_BUNDLE_INFO_PLIST
				"${CMAKE_CURRENT_SOURCE_DIR}/cmake/macos/Info.plist.in"
		)
		target_link_libraries(tpad PRIVATE "-framework AppKit")
	elseif(WIN32)
		set_target_properties(tpad PROPERTIES
			QT_WINDOWS_APP_PROJECT_IDENTIFIER "com.gnaservicesinc.tpad")
		target_link_libraries(tpad PRIVATE ole32 uuid)
	endif()

	if(BUILD_TESTING)
		set(_tpad_qt_settings_test_sources
			src/qt/tests/settings_restore_test.cpp
			src/qt/tpad_application.cpp
			src/qt/tpad_application.h
			src/qt/tpad_main_window.cpp
			src/qt/tpad_main_window.h
			src/qt/recent_files.cpp
			src/qt/recent_files.h
			src/qt/code_editor.cpp
			src/qt/code_editor.h
			src/qt/syntax_highlighter.cpp
			src/qt/syntax_highlighter.h
			src/qt/spell_checker.cpp
			src/qt/spell_checker.h
		)
		if(APPLE)
			list(APPEND _tpad_qt_settings_test_sources
				src/qt/spell_checker_mac.mm
				src/qt/writing_tools_mac.mm
				src/qt/writing_tools_mac.h
			)
		endif()

		qt_add_executable(tpad_qt_settings_restore_test
			${_tpad_qt_settings_test_sources})
		set_target_properties(tpad_qt_settings_restore_test PROPERTIES
			EXCLUDE_FROM_ALL TRUE)
		target_compile_features(tpad_qt_settings_restore_test PRIVATE cxx_std_17)
		target_compile_definitions(tpad_qt_settings_restore_test PRIVATE
			TPAD_USE_QT=1
			TPAD_VERSION="${PROJECT_VERSION}"
			QT_NO_CAST_FROM_ASCII
			QT_NO_CAST_TO_ASCII
		)
		target_link_libraries(tpad_qt_settings_restore_test PRIVATE
			Qt6::Core
			Qt6::Gui
			Qt6::Widgets
			Qt6::PrintSupport
		)
		if(MSVC)
			target_compile_options(tpad_qt_settings_restore_test PRIVATE
				"$<$<COMPILE_LANGUAGE:CXX,OBJCXX>:/W4;/permissive-;/utf-8>")
		else()
			target_compile_options(tpad_qt_settings_restore_test PRIVATE
				"$<$<COMPILE_LANGUAGE:CXX,OBJCXX>:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wsign-conversion>")
		endif()
		if(APPLE)
			target_link_libraries(tpad_qt_settings_restore_test PRIVATE
				"-framework AppKit")
		elseif(WIN32)
			target_link_libraries(tpad_qt_settings_restore_test PRIVATE
				ole32 uuid)
		endif()

		add_test(NAME qt-settings-restore
			COMMAND tpad_qt_settings_restore_test -platform offscreen)
		set_tests_properties(qt-settings-restore PROPERTIES
			ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
		add_custom_target(check-qt-settings
			COMMAND "${CMAKE_CTEST_COMMAND}" --output-on-failure
				-R "^qt-settings-restore"
			DEPENDS tpad_qt_settings_restore_test
			USES_TERMINAL
		)
	endif()

	get_target_property(_tpad_qt_core_type Qt6::Core TYPE)
	if(TPAD_QT_STATIC AND NOT _tpad_qt_core_type STREQUAL "STATIC_LIBRARY")
		message(FATAL_ERROR
			"TPAD_QT_STATIC=ON requires a statically built Qt installation. "
			"TPAD_QT_PATH currently selects ${_tpad_qt_core_type}. Point "
			"TPAD_QT_PATH at a Qt configured with -static and configure again.")
	elseif(NOT TPAD_QT_STATIC AND _tpad_qt_core_type STREQUAL "STATIC_LIBRARY")
		message(FATAL_ERROR
			"TPAD_QT_STATIC=OFF requires a shared Qt installation, but "
			"TPAD_QT_PATH selects a static build. Pass -DTPAD_QT_STATIC=ON "
			"or point TPAD_QT_PATH at a shared Qt installation.")
	endif()

	install(TARGETS tpad
		BUNDLE DESTINATION .
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	)

	if(TPAD_QT_DEPLOY AND NOT TPAD_QT_STATIC)
		# qt_generate_deploy_app_script() runs only during installation.  Make
		# the build-tree macOS bundle runnable too, because it is the bundle
		# developers naturally launch after `cmake --build`.  macdeployqt
		# copies every linked Qt framework (including PrintSupport), deploys
		# the Cocoa platform plugin, and fixes their install names/rpaths.
		if(APPLE)
			if(TARGET Qt6::macdeployqt)
				set(_tpad_macdeployqt "$<TARGET_FILE:Qt6::macdeployqt>")
			else()
				# Qt 6.11 exports Qt6::macdeployqt.  Keep a program-search
				# fallback for custom kits that omit imported tool targets.
				set(_tpad_macdeployqt_hints "${Qt6_DIR}/../../../bin")
				if(TPAD_QT_PATH)
					list(PREPEND _tpad_macdeployqt_hints
						"${TPAD_QT_PATH}/bin")
				endif()
				if(QT_HOST_PATH)
					list(PREPEND _tpad_macdeployqt_hints
						"${QT_HOST_PATH}/bin")
				endif()
				find_program(_tpad_macdeployqt NAMES macdeployqt
					HINTS ${_tpad_macdeployqt_hints})
				if(NOT _tpad_macdeployqt)
					message(FATAL_ERROR
						"The selected Qt kit does not provide macdeployqt, which is "
						"required when TPAD_QT_DEPLOY=ON on macOS.")
				endif()
			endif()
			set(_tpad_macos_deploy_options
				-always-overwrite
				"$<$<CONFIG:Debug>:-use-debug-libs>"
			)
			add_custom_command(TARGET tpad POST_BUILD
				COMMAND "${_tpad_macdeployqt}"
					"$<TARGET_BUNDLE_DIR:tpad>"
					${_tpad_macos_deploy_options}
				COMMAND "${CMAKE_COMMAND}"
					"-DTPAD_APP_BUNDLE=$<TARGET_BUNDLE_DIR:tpad>"
					-P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/macos/verify_bundle.cmake"
				COMMENT
					"Deploying and verifying Qt frameworks in the macOS application bundle"
				VERBATIM
			)
			set(_tpad_install_deploy_arguments
				DEPLOY_TOOL_OPTIONS ${_tpad_macos_deploy_options})
		endif()

		qt_generate_deploy_app_script(
			TARGET tpad
			OUTPUT_SCRIPT _tpad_deploy_script
			${_tpad_install_deploy_arguments}
		)
		install(SCRIPT "${_tpad_deploy_script}")
	endif()

	message(STATUS "Tpad frontend: Qt ${Qt6_VERSION}")
	if(TPAD_QT_PATH)
		message(STATUS "Tpad Qt prefix: ${TPAD_QT_PATH}")
	else()
		message(STATUS "Tpad Qt config: ${Qt6_DIR}")
	endif()
	message(STATUS "Tpad static Qt: ${TPAD_QT_STATIC}")
else()
	if(APPLE OR WIN32)
		message(FATAL_ERROR
			"The GTK frontend is supported by this CMake build on Linux and "
			"other Unix desktops only. Use -DTPAD_USE_QT=ON on macOS/Windows.")
	endif()

	find_package(PkgConfig REQUIRED)
	if(TPAD_USE_GTK3)
		pkg_check_modules(TPAD_GTK REQUIRED IMPORTED_TARGET
			gtk+-3.0 gtksourceview-3.0 gtkspell3-3.0)
		set(_tpad_gtk_frontend_sources
			src/gtk3/error_warn.c
			src/gtk3/tpad_about.c
			src/gtk3/tpad_buff.c
			src/gtk3/tpad_doc.c
			src/gtk3/tpad_file.c
			src/gtk3/tpad_find.c
			src/gtk3/tpad_find_and_replace.c
			src/gtk3/tpad_font.c
			src/gtk3/tpad_frequency_analysis.c
			src/gtk3/tpad_hash.c
			src/gtk3/tpad_hconnect.c
			src/gtk3/tpad_history.c
			src/gtk3/tpad_lang.c
			src/gtk3/tpad_main.c
			src/gtk3/tpad_new_file.c
			src/gtk3/tpad_open_file.c
			src/gtk3/tpad_pref.c
			src/gtk3/tpad_print.c
			src/gtk3/tpad_quit.c
			src/gtk3/tpad_save.c
			src/gtk3/tpad_show_file.c
			src/gtk3/tpad_spelling.c
			src/gtk3/tpad_textstats.c
			src/gtk3/tpad_toggle.c
			src/gtk3/tpad_ui.c
		)
		set(_tpad_gtk_version 3)
	else()
		pkg_check_modules(TPAD_GTK REQUIRED IMPORTED_TARGET
			gtk4>=4.10 gtksourceview-5 libspelling-1)
		set(_tpad_gtk_frontend_sources
			src/error_warn.c
			src/tpad_gtk.c
			src/tpad_about.c
			src/tpad_buff.c
			src/tpad_doc.c
			src/tpad_file.c
			src/tpad_find.c
			src/tpad_find_and_replace.c
			src/tpad_font.c
			src/tpad_frequency_analysis.c
			src/tpad_hash.c
			src/tpad_hconnect.c
			src/tpad_history.c
			src/tpad_lang.c
			src/tpad_main.c
			src/tpad_new_file.c
			src/tpad_open_file.c
			src/tpad_pref.c
			src/tpad_print.c
			src/tpad_quit.c
			src/tpad_save.c
			src/tpad_show_file.c
			src/tpad_spelling.c
			src/tpad_textstats.c
			src/tpad_toggle.c
			src/tpad_ui.c
		)
		set(_tpad_gtk_version 4)
	endif()
	pkg_check_modules(TPAD_SYSTEMD QUIET IMPORTED_TARGET libsystemd>=233)
	if(TPAD_SYSTEMD_FOUND)
		set(TPAD_HAVE_APP_SPECIFIC_ID 1)
	else()
		pkg_check_modules(TPAD_SYSTEMD QUIET IMPORTED_TARGET libsystemd)
		if(NOT TPAD_SYSTEMD_FOUND)
			pkg_check_modules(TPAD_SYSTEMD REQUIRED IMPORTED_TARGET
				libsystemd-id128)
		endif()
		set(TPAD_HAVE_APP_SPECIFIC_ID 0)
	endif()

	set(TPAD_PACKAGE_LOCALE_DIR "${CMAKE_INSTALL_FULL_LOCALEDIR}")
	configure_file(cmake/config.h.in
		"${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

	add_executable(tpad
		${_tpad_gtk_frontend_sources}
		src/main.c
		src/tpad_id.c
		src/malloc.c
		src/tpad_config.c
		src/tpad_bom8.c
		src/tpad_codeset.c
		src/tpad_copy.c
		src/tpad_control.c
		src/tpad_reload.c
		src/tpad_unity.c
		src/tpad_title.c
		src/tpad_fp.c
		src/tpad_open_guard.c
		src/tpad_recent.c
		src/tpad_tree.c
		src/tpad_spawn.c
		src/tpad_string.c
		src/base64.c
		src/md5.c
		src/sha256.c
		src/sha512.c
		src/platform_util.c
	)
	target_compile_features(tpad PRIVATE c_std_11)
	target_compile_definitions(tpad PRIVATE
		TPAD_USE_GTK=1
		HAVE_CONFIG_H=1
		_XOPEN_SOURCE=700
		PACKAGE_LOCALE_DIR="${TPAD_PACKAGE_LOCALE_DIR}"
		PACKAGE_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src"
		PACKAGE_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/tpad"
	)
	if(TPAD_USE_GTK3)
		target_compile_definitions(tpad PRIVATE TPAD_USE_GTK3=1)
		target_include_directories(tpad PRIVATE
			"${CMAKE_CURRENT_SOURCE_DIR}/src/gtk3")
	endif()
	target_include_directories(tpad PRIVATE
		"${CMAKE_CURRENT_BINARY_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}/src"
	)
	target_compile_options(tpad PRIVATE
		-Wall -Wextra -Werror -Wformat=2 -Wstrict-prototypes
		-Wmissing-prototypes -Wshadow -Wpointer-arith -Wwrite-strings)
	target_link_libraries(tpad PRIVATE
		PkgConfig::TPAD_GTK
		PkgConfig::TPAD_SYSTEMD
	)

	install(TARGETS tpad RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
	install(FILES icon/tpad_128.png
		DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps"
		RENAME tpad.png)
	message(STATUS "Tpad frontend: GTK ${_tpad_gtk_version}")
endif()
