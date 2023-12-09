import os

def create_cmake_list():
    with open("CMakeLists.txt", mode = "a") as cmake:
        cmake.seek(os.SEEK_SET)

        print("# ==> Ensure proper CMake support\ncmake_minimum_required(VERSION 3.10)\n# ==> Force new C++ standard (we like new features here)\nset(CMAKE_CXX_STANDARD 20)\nset(CMAKE_CXX_STANDARD_REQUIRED ON)\n\
project(List VERSION 1.0)\n# ==> Build in release by default (makes difference for graphical projects)\nset(DEFAULT_BUILD_TYPE \"Release\")\n\
# ==> Add option to force ANSI-colors (useful for building with Ninja)\noption(FORCE_COLORED_OUTPUT \"Always produce ANSI-colored output (GNU/Clang only).\" FALSE)\nif (${FORCE_COLORED_OUTPUT})\nif     (\"${CMAKE_CXX_COMPILER_ID}\" STREQUAL \"GNU\")\n\
add_compile_options (-fdiagnostics-color=always)\nelseif (\"${CMAKE_CXX_COMPILER_ID}\" STREQUAL \"Clang\")\nadd_compile_options (-fcolor-diagnostics)\nendif ()\nendif ()", file = cmake)


        print ("# ==> Enable optimization\nset(CMAKE_CXX_FLAGS_RELEASE \"${CMAKE_CXX_FLAGS_RELEASE} -D NDEBUG -O2\")\n\
# ==> Flags for debugging, enable sanitizers and make debugging experience as good as possible\n\
# ==> -D DEBUG_VERSION flag enables self written list asserts for more agressive data control\n\
set(CMAKE_CXX_FLAGS_DEBUG \"${CMAKE_CXX_FLAGS_DEBUG} -D DEBUG_VERSION -ggdb3 -std=c++20 -Wall -Wextra -Weffc++ -Wcast-align -Wcast-qual -Wchar-subscripts -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -fPIE -march=native -mtune=native -O0\")\n\
if (\"${CMAKE_CXX_COMPILER_ID}\" STREQUAL \"GNU\")\n\
set(CMAKE_CXX_FLAGS_DEBUG \"${CMAKE_CXX_FLAGS_DEBUG} -Waggressive-loop-optimizations -Wconditionally-supported -Wformat-signedness -Wlogical-op -Wopenmp-simd -Wstrict-null-sentinel -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsync-nand -Wuseless-cast -fconcepts-diagnostics-depth=3 -Wno-literal-suffix\")\n\
endif ()", file = cmake)

        print ("# ==> Add libraries\nadd_subdirectory(list)", file = cmake)
        print ("# ==> Build tests\n", file = cmake)

if __name__ == "__main__":
    create_cmake_list()