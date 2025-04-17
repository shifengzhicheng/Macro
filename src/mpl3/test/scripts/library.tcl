# Setting lef files
# check if pwd have case1.odb
set tech_dir "/scripts/MacroPlacement/Enablements/NanGate45"
set cur_dir "/scripts/MacroPlacement/Flows/NanGate45/mempool_group/scripts/OpenROAD"
set syn_dir "/OpenROAD-flow-scripts/flow/results/nangate45/mempool_group/base"

# Setting lef files
# check if pwd have case1.odb
set tech_dir "/scripts/MacroPlacement/Enablements/NanGate45"
set tech_lef "${tech_dir}/lef/NangateOpenCellLibrary.tech.lef"
set std_lef "${tech_dir}/lef/NangateOpenCellLibrary.macro.mod.lef"
set std_lib "${tech_dir}/lib/NangateOpenCellLibrary_typical.lib"
set case "mempool_group"

set lefs "
  ${tech_dir}/lef/fakeram45_256x32.lef \
  ${tech_dir}/lef/fakeram45_64x64.lef \
  ${tech_dir}/lef/fakeram45_128x32.lef \
  ${tech_dir}/lef/fakeram45_128x256.lef \
"
# Setting lib files
set libs "
  ${tech_dir}/lib/fakeram45_256x32.lib \
  ${tech_dir}/lib/fakeram45_128x32.lib \
  ${tech_dir}/lib/fakeram45_64x64.lib \
  ${tech_dir}/lib/fakeram45_128x256.lib \
"

read_lef $tech_lef
read_lef $std_lef
read_lib $std_lib

foreach lef_file ${lefs} {
  read_lef $lef_file
}

foreach lib_file ${libs} {
  read_liberty $lib_file
}