#!/bin/bash

OSCC_TEST_FRAMEWORK_DIR=$(pwd)/oscc_test_framework

mkdir $OSCC_TEST_FRAMEWORK_DIR


CGREEN_SOURCE_DIR=$OSCC_TEST_FRAMEWORK_DIR/cgreen_source
CGREEN_BUILD_DIR=$CGREEN_SOURCE_DIR/build
CGREEN_INSTALL_DIR=$OSCC_TEST_FRAMEWORK_DIR/cgreen

mkdir $CGREEN_INSTALL_DIR

git clone https://github.com/cgreen-devs/cgreen.git $CGREEN_SOURCE_DIR
git -C $CGREEN_SOURCE_DIR checkout 7030597
cmake -H$CGREEN_SOURCE_DIR -B$CGREEN_BUILD_DIR -DCGREEN_WITH_UNIT_TESTS=FALSE -DCMAKE_INSTALL_PREFIX=$CGREEN_INSTALL_DIR
make -C $CGREEN_BUILD_DIR install


CUCUMBER_SOURCE_DIR=$OSCC_TEST_FRAMEWORK_DIR/cucumber-cpp_source
CUCUMBER_BUILD_DIR=$CUCUMBER_SOURCE_DIR/build
CUCUMBER_INSTALL_DIR=$OSCC_TEST_FRAMEWORK_DIR/cucumber-cpp

mkdir -p $CUCUMBER_INSTALL_DIR/{include,lib}

sudo apt install libboost-thread-dev libboost-system-dev libboost-regex-dev libboost-date-time-dev libboost-program-options-dev libboost-filesystem-dev
git clone https://github.com/snewt/cucumber-cpp.git $CUCUMBER_SOURCE_DIR
git -C $CUCUMBER_SOURCE_DIR checkout 883cfaa
cmake -H$CUCUMBER_SOURCE_DIR -B$CUCUMBER_BUILD_DIR -DCUKE_DISABLE_BOOST_TEST=true -DCUKE_DISABLE_GTEST=true -DCUKE_DISABLE_UNIT_TESTS=true -DCUKE_DISABLE_E2E_TESTS=true -DCUKE_ENABLE_EXAMPLES=false -DCMAKE_PREFIX_PATH=$CGREEN_INSTALL_DIR
make -C $CUCUMBER_BUILD_DIR

ar cqT $OSCC_TEST_FRAMEWORK_DIR/cucumber-cpp/lib/libcucumber-cpp.a \
    $CUCUMBER_BUILD_DIR/src/libcucumber-cpp.a \
    /usr/lib/x86_64-linux-gnu/libboost_atomic.a \
    /usr/lib/x86_64-linux-gnu/libboost_chrono.a \
    /usr/lib/x86_64-linux-gnu/libboost_date_time.a \
    /usr/lib/x86_64-linux-gnu/libboost_filesystem.a \
    /usr/lib/x86_64-linux-gnu/libboost_program_options.a \
    /usr/lib/x86_64-linux-gnu/libboost_regex.a \
    /usr/lib/x86_64-linux-gnu/libboost_serialization.a \
    /usr/lib/x86_64-linux-gnu/libboost_system.a \
    /usr/lib/x86_64-linux-gnu/libboost_thread.a \
    /usr/lib/x86_64-linux-gnu/libboost_wserialization.a \
    && echo -e "create $CUCUMBER_INSTALL_DIR/lib/libcucumber-cpp.a\naddlib $CUCUMBER_INSTALL_DIR/lib/libcucumber-cpp.a\nsave\nend" \
    | ar -M

cp -r $CUCUMBER_SOURCE_DIR/include/cucumber-cpp/ $CUCUMBER_INSTALL_DIR/include/
