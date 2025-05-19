check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/cognitive-radio-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME cognitive-radio
    SOURCE_FILES model/cognitive-radio.cc
                 model/cognitive-radio-phy.cc
                 model/cognitive-spectrum-interference.cc
                 model/aloha-noack-cognitive-net-device.cc
                 model/cognitive-radio-energy-model.cc
                 helper/adhoc-aloha-noack-cognitive-phy-helper.cc
                 helper/cognitive-radio-helper.cc
                 helper/cognitive-radio-energy-model-helper.cc

    HEADER_FILES model/cognitive-radio.h
                 model/cognitive-radio-phy.h
                 model/cognitive-spectrum-interference.h
                 model/cognitive-radio-energy-model.h
                 model/aloha-noack-cognitive-net-device.h
                 helper/cognitive-radio-helper.h
                 helper/adhoc-aloha-noack-cognitive-phy-helper.h
                 helper/cognitive-radio-energy-model-helper.h

    LIBRARIES_TO_LINK ${libspectrum}
                      ${libnetwork}
                      ${libmobility}
                      ${libantenna}
                      ${libenergy}  

    TEST_SOURCES test/cognitive-radio-test-suite.cc
                 ${examples_as_tests_sources}
)
