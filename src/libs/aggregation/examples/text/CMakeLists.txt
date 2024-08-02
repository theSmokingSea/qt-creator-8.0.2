add_qtc_executable(AggregationExample
        DEPENDS Qt5::Core  Qt5::Widgets  Qt5::Gui

        SOURCES
        main.cpp
        main.h
        main.ui
        myinterfaces.h

        aggregate.h
        aggregate.cpp
)
