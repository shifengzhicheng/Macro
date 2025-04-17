INSTALL_PATH="/OpenROAD-flow-scripts/tools/install"
CMAKE_INSTALL_RPATH="/dependencies/lib:/dependencies/lib64"
OPENROAD_APP_ARGS=""
OPENROAD_APP_ARGS+=" -D CMAKE_INSTALL_PREFIX=${INSTALL_PATH}/OpenROAD"
OPENROAD_APP_ARGS+=" -D CMAKE_INSTALL_RPATH=${CMAKE_INSTALL_RPATH}"
OPENROAD_APP_ARGS+=" -D CMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE"

echo "[INFO FLW-0018] Compiling OpenROAD"
cd build && cmake .. $OPENROAD_APP_ARGS && make -j$(nproc) && make install
echo "[INFO FLW-0018] OpenROAD compiled and installed successfully"

