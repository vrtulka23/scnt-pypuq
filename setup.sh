#!/usr/bin/env bash

set -e

. settings.env
DIR_ROOT=$(pwd)

function build_docs {
    sphinx-build -M html docs/source/ docs/build/
}

function clean_code {
    if [[ -d $DIR_BUILD ]]; then
	    rm -r $DIR_BUILD
    fi
}

function build_code {    
    if [[ ! -d $DIR_BUILD ]]; then
	    mkdir $DIR_BUILD
    fi
    cmake -B $DIR_BUILD -DCODE_VERSION="${CODE_VERSION}"
    cd $DIR_BUILD
    make -j 10
    cd $DIR_ROOT
}

function install_code {
    cd $DIR_BUILD
    make install
    cd $DIR_ROOT
}

function grep_code {
    if [[ "${1}" != "" ]]; then
	    grep -Inr $1 ./src ./pytest
    fi
}

function build_pypi {
    if [[ -d dist ]]; then
	rm -r dist
    fi
    python setup.py sdist
}

function show_help {
    echo "Physical Units and Quantities"
    echo ""
    echo "Options:"
    echo " -d|--docs           build documentation"
    echo " -c|--clean          clean the build directory"
    echo " -b|--build          build code"
    echo " -i|--install        install puq"
    echo " -g|--grep <expr>    find expression in a code"
    echo " -h|--help           show this help"
    echo " -B|--build-pypi     build PyPi distribution"
    echo " -U|--upload-pypi    upload distribution to Pypi"
    echo " -T|--pytest         run pytest"
    echo ""
    echo "Examples:"
    echo "./setup.sh -c -b               clean and build the code"
    echo "./setup.sh -g class            find 'class' in the code or test"
}

if [[ "${1}" == "" ]]; then
    show_help
fi
while [[ $# -gt 0 ]]; do
    case $1 in
	-d|--docs)
	    build_docs; shift;;
	-c|--clean)
	    clean_code; shift;;
	-b|--build)
	    build_code; shift;;
	-i|--install)
	    install_code; shift;;
	-g|--grep)
	    grep_code $2; shift; shift;;
	-h|--help)
	    show_help; shift;;
	-B|--build-pypi)
	    build_pypi; shift;;
	-I|--test-pypi)
	    pip install "dist/pypuq-${CODE_VERSION}.tar.gz"
	    shift;;
	-U|--upload-pypi)
	    twine upload dist/*
	    shift;;
	-T|--pytest)
	    pytest
	    shift;;
	-*|--*)
	    show_help; exit 1;;
	*)
	    show_help; shift;;
    esac
done
