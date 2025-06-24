check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/cognitive-mac-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME cognitive-mac
    SOURCE_FILES model/cognitive-mac.cc
                 model/mac-frames.cc
                 model/cognitive-phy-device.cc
                 model/cognitive-general-net-device.cc
                 model/cognitive-control-application.cc
                 model/spectrum-control-module.cc
                 model/cognitive-radio-energy-model.cc
                 model/cognitive-spectrum-interference.cc
                 model/cognitive-control-message.cc
                 model/cognitive-routing-unite.cc
                 model/cognitive-routing-messages.cc
                 helper/cognitive-device-energy-model-helper.cc
                 helper/cognitive-net-device-helper.cc
                 helper/cognitive-mac-helper.cc
    HEADER_FILES model/cognitive-mac.h
                 model/mac-frames.h
                 model/cognitive-general-net-device.h
                 model/cognitive-phy-device.h
                 model/cognitive-mac-constants.h
                 model/cognitive-control-application.h
                 model/spectrum-control-module.h
                 model/cognitive-radio-energy-model.h
                 model/cognitive-spectrum-interference.h
                 model/cognitive-control-message.h
                 model/cognitive-routing-unite.h
                 model/cognitive-routing-messages.h
                 helper/cognitive-net-device-helper.h
                 helper/cognitive-device-energy-model-helper.h
                 helper/cognitive-mac-helper.h
    LIBRARIES_TO_LINK ${libcore}
    TEST_SOURCES test/cognitive-mac-test-suite.cc
                 ${examples_as_tests_sources}
)
