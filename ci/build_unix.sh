#!/bin/bash
#
# Copyright (c) 2019-2020 Ruben Perez Hidalgo (rubenperez038 at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#


# SHA256 functionality is only supported in MySQL 8+. From our
# CI systems, only OSX has this version.

cp ci/*.pem /tmp # Copy SSL certs/keys to a known location
if [ $TRAVIS_OS_NAME == "osx" ]; then
    brew update
    brew install $DATABASE lcov
    cp ci/unix-ci.cnf ~/.my.cnf  # This location is checked by both MySQL and MariaDB
    sudo mkdir -p /var/run/mysqld/
    sudo chmod 777 /var/run/mysqld/
    mysql.server start # Note that running this with sudo fails
    if [ $DATABASE == "mariadb" ]; then
        sudo mysql -u root < ci/root_user_setup.sql
    else
        export BOOST_MYSQL_HAS_SHA256=1
    fi
else
    sudo cp ci/unix-ci.cnf /etc/mysql/conf.d/
    sudo service mysql restart
    wget https://github.com/Kitware/CMake/releases/download/v3.17.0/cmake-3.17.0-Linux-x86_64.sh -q -O cmake-latest.sh
    mkdir -p /tmp/cmake-latest
    bash cmake-latest.sh --prefix=/tmp/cmake-latest --skip-license
    export PATH=/tmp/cmake-latest/bin:$PATH
fi


mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE \
    $(if [ $USE_VALGRIND ]; then echo -DBOOST_MYSQL_VALGRIND_TESTS=ON; fi) \
    $(if [ $USE_COVERAGE ]; then echo -DBOOST_MYSQL_COVERAGE=ON; fi) \
    $CMAKE_OPTIONS \
    .. 
make -j6 CTEST_OUTPUT_ON_FAILURE=1 all test

# Coverage collection
if [ $USE_COVERAGE ]; then
    if [ "$TRAVIS_COMPILER" == "clang" ]; then
        GCOV_TOOL="$TRAVIS_BUILD_DIR/ci/clang-gcov.sh"
    else
        GCOV_TOOL=gcov
    fi;
    lcov --capture --directory . -o coverage.info --gcov-tool "$GCOV_TOOL"
    lcov -o coverage.info --extract coverage.info "**include/boost/mysql/**"
    curl -s https://codecov.io/bash -o codecov.sh
    bash +x codecov.sh -f coverage.info
fi
