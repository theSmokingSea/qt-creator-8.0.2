add_qtc_library(ExtensionSystem
  DEPENDS Aggregation Utils
  PUBLIC_DEPENDS Qt5::Core Qt5::Widgets
  SOURCES
    extensionsystem_global.h

    optionsparser.cpp optionsparser.h

    iplugin.cpp iplugin.h iplugin_p.h
    pluginmanager.cpp pluginmanager.h pluginmanager_p.h
    pluginspec.cpp pluginspec.h pluginspec_p.h

    # 插件错误或者展示插件信息的界面
    plugindetailsview.cpp plugindetailsview.h plugindetailsview.ui
    pluginerroroverview.cpp pluginerroroverview.h pluginerroroverview.ui
    pluginerrorview.cpp pluginerrorview.h pluginerrorview.ui
    pluginview.cpp pluginview.h
    # 这个类几乎没有用到
    invoker.cpp invoker.h


  EXPLICIT_MOC
    pluginmanager.h
    pluginmanager_p.h
  SKIP_AUTOMOC pluginmanager.cpp
)

find_package(Qt5 COMPONENTS Test QUIET)

extend_qtc_library(ExtensionSystem
  CONDITION TARGET Qt5::Test
  DEPENDS Qt5::Test
  DEFINES WITH_TESTS
)
