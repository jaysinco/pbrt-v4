#!/bin/bash

set -e

# flags

do_clean=0
do_build_debug=0
do_with_gpu=0

while [[ $# -gt 0 ]]; do
    case $1 in
        -h)
            echo
            echo "Usage: `basename "$0"` [options]"
            echo
            echo "Build Options:"
            echo "  -c         clean before build"
            echo "  -d         build debug version"
            echo "  -g         build with gpu support"
            echo "  -h         print command line options"
            echo
            exit 0
            ;;
        -c) do_clean=1 && shift ;;
        -d) do_build_debug=1 && shift ;;
        -g) do_with_gpu=1 && shift ;;
         *) echo "unknown argument: $1" && exit 1 ;;
    esac
done

# build

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
git_root="$(git rev-parse --show-toplevel)"

build_type=Release
if [ $do_build_debug -eq 1 ]; then
    build_type=Debug
fi

arch=`arch`
case "$OSTYPE" in
    linux*)   os=linux ;;
    msys*)    os=windows ;;
esac

desc_gpu=$([ $do_with_gpu -eq 0 ] && echo "cpu" || echo "gpu")
tuple_name=${build_type,,}-$desc_gpu
build_folder=$git_root/out/$tuple_name
binary_folder=$git_root/bin/$tuple_name

function clean_build() {
    rm -rf $build_folder
    rm -rf $binary_folder
}

function cmake_build() {
    compiler_c=$([ "$os" == "linux" ] && echo "gcc" || echo "cl")
    compiler_cxx=$([ "$os" == "linux" ] && echo "g++" || echo "cl")
    compiler_cuda=$([ "$os" == "linux" ] && echo "/usr/local/cuda/bin/nvcc" || echo "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.4/bin/nvcc.exe")
    optix7_path=$([ "$os" == "linux" ] && echo "/opt/nvidia/NVIDIA-OptiX-SDK-7.3.0-linux64-aarch64" || echo "C:/ProgramData/NVIDIA Corporation/OptiX SDK 7.3.0")
    compiler_cuda=$([ $do_with_gpu -eq 0 ] && echo "OFF" || echo "$compiler_cuda")
    cuda_arch=$([ "$os" == "linux" ] && echo "87" || echo "86")

    mkdir -p \
        $build_folder \
    && \
    pushd $build_folder \
    && \
    cmake $git_root -G "Ninja" \
        -DCMAKE_C_COMPILER=$compiler_c \
        -DCMAKE_CXX_COMPILER=$compiler_cxx \
        -DCMAKE_BUILD_TYPE=$build_type \
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$binary_folder \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_CUDA_COMPILER="$compiler_cuda" \
        -DCMAKE_CUDA_ARCHITECTURES=$cuda_arch \
        -DPBRT_OPTIX7_PATH="$optix7_path" \
    && \
    cp $build_folder/compile_commands.json $build_folder/.. \
    && \
    cmake --build . --parallel=`nproc`
}

if [ $do_clean -eq 1 ]; then
    clean_build
fi \
&& \
cmake_build \
&& \
echo done!
