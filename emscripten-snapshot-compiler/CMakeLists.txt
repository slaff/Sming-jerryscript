# Copyright JS Foundation and other contributors, http://js.foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


cmake_minimum_required (VERSION 2.8.12)
# The name of the files will be jsc.js and jsc.wasm.
# jsc stands for Jerryscript Snapshot Compiler
set(EMSCRIPTEN_SNAPSHOT_COMPILER jsc)
set(CMAKE_TOOLCHAIN_FILE $ENV{EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${JERRY_PROFILE}")
project (${EMSCRIPTEN_SNAPSHOT_COMPILER} C)

file(GLOB PRE_JS pre.js)
file(GLOB POST_JS post.js)

# Work-around for https://public.kitware.com/Bug/view.php?id=15826
set(EMCC_LINK_FLAGS
        "--pre-js ${PRE_JS}"
        "--post-js ${POST_JS}"
        "-s ERROR_ON_UNDEFINED_SYMBOLS=1"
        "-s SAFE_HEAP=1"
        "-s EXPORTED_FUNCTIONS=\"['_emscripten_snapshot_compiler_get_version', '_emscripten_snapshot_compiler_compile']\""
        "-s EXPORTED_RUNTIME_METHODS=\"['cwrap']\""
        "-gsource-map"
        "--no-entry"
        "--memory-init-file 0"
        "-s STANDALONE_WASM=1")

add_executable(${EMSCRIPTEN_SNAPSHOT_COMPILER} "emscripten-snapshot-compiler.c")
target_include_directories(${EMSCRIPTEN_SNAPSHOT_COMPILER} PRIVATE ${CMAKE_SOURCE_DIR}/../jerryscript/jerry-core/include)
target_link_directories(${EMSCRIPTEN_SNAPSHOT_COMPILER} PRIVATE ${CMAKE_BINARY_DIR}/jerryscript/lib)
target_link_libraries(${EMSCRIPTEN_SNAPSHOT_COMPILER} jerry-core jerry-port-default ${EMCC_LINK_FLAGS})
set_target_properties(${EMSCRIPTEN_SNAPSHOT_COMPILER} PROPERTIES LINK_DEPENDS "${POST_JS};${PRE_JS}")
set_target_properties(${EMSCRIPTEN_SNAPSHOT_COMPILER} PROPERTIES C_STANDARD 11)

# In EM_ASM macros, C arguments are named $0, $1, etc.
target_compile_options(${EMSCRIPTEN_SNAPSHOT_COMPILER} PRIVATE "-Wno-dollar-in-identifier-extension")
target_compile_options(${EMSCRIPTEN_SNAPSHOT_COMPILER} PRIVATE "-Wno-gnu-zero-variadic-macro-arguments")
