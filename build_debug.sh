INSTALL_PATH="/OpenROAD-flow-scripts/tools/install"
CMAKE_INSTALL_RPATH="/dependencies/lib:/dependencies/lib64"
OPENROAD_APP_ARGS=""
OPENROAD_APP_ARGS+=" -D CMAKE_INSTALL_PREFIX=${INSTALL_PATH}/OpenROAD"
OPENROAD_APP_ARGS+=" -D CMAKE_INSTALL_RPATH=${CMAKE_INSTALL_RPATH}"
OPENROAD_APP_ARGS+=" -D CMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE"
OPENROAD_APP_ARGS+=" -D CMAKE_BUILD_TYPE=DEBUG"

echo "[INFO FLW-0018] Compiling OpenROAD Debug."
cd build_debug && cmake .. $OPENROAD_APP_ARGS && make -j$(nproc)
echo "[INFO FLW-0018] OpenROAD Debug compiled successfully."

