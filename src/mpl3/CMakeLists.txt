#############################################################################
##
## Copyright (c) 2019, OpenROAD
## All rights reserved.
##
## BSD 3-Clause License
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are met:
##
## * Redistributions of source code must retain the above copyright notice, this
##   list of conditions and the following disclaimer.
##
## * Redistributions in binary form must reproduce the above copyright notice,
##   this list of conditions and the following disclaimer in the documentation
##   and/or other materials provided with the distribution.
##
## * Neither the name of the copyright holder nor the names of its
##   contributors may be used to endorse or promote products derived from
##   this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
## LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
## CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
## SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.

include("openroad")
find_package(ortools REQUIRED)
find_package(libleidenalg REQUIRED)
find_package(igraph REQUIRED)
find_package(OpenMP REQUIRED)

add_library(mpl3_lib
  src/rtl_mp.cpp
  src/hier_rtlmp.cpp
  src/object.cpp
  src/SimulatedAnnealingCore.cpp
  src/SACoreHardMacro.cpp
  src/SACoreSoftMacro.cpp
  src/bus_synthesis.cpp
  src/clusterEngine.cpp
  src/leidenClustering.cpp
  src/optimiser.cpp
)

target_include_directories(mpl3_lib
  PUBLIC
    include
)

target_link_libraries(mpl3_lib
    utl_lib
    igraph::igraph
    libleidenalg::libleidenalg
    OpenMP::OpenMP_CXX
    dbSta_lib
    OpenSTA
    odb
    ortools::ortools
    dl
    par_lib
)

swig_lib(NAME      mpl3
         NAMESPACE mpl3
         I_FILE    src/mpl.i
         SCRIPTS   src/mpl.tcl
)

target_sources(mpl3
  PRIVATE
    src/MakeMacroPlacer.cpp
    src/graphics.cpp
)

messages(
  TARGET mpl3
)

target_include_directories(mpl3
  PUBLIC
    include
  PRIVATE
    src
)

target_link_libraries(mpl3
    mpl3_lib
    gui
)
