set tech_dir "/scripts/MacroPlacement/Enablements/NanGate45"
set cur_dir "/scripts/MacroPlacement/Flows/NanGate45/bp_quad/scripts/OpenROAD"
set syn_dir "/OpenROAD-flow-scripts/flow/results/nangate45/bp_quad/base"

set tech_lef "${tech_dir}/lef/NangateOpenCellLibrary.tech.lef"
set std_lef "${tech_dir}/lef/NangateOpenCellLibrary.macro.mod.lef"
set case "bsg_chip"

set lefs "
${tech_dir}/lef/fakeram45_256x48.lef \
  ${tech_dir}/lef/fakeram45_32x32.lef \
  ${tech_dir}/lef/fakeram45_64x124.lef \
  ${tech_dir}/lef/fakeram45_512x64.lef \
  ${tech_dir}/lef/fakeram45_64x62.lef \
  ${tech_dir}/lef/fakeram45_128x116.lef
"
# Setting lib files
set libs "
${tech_dir}/lib/fakeram45_256x48.lib \
  ${tech_dir}/lib/fakeram45_32x32.lib \
  ${tech_dir}/lib/fakeram45_64x124.lib \
  ${tech_dir}/lib/fakeram45_512x64.lib \
  ${tech_dir}/lib/fakeram45_64x62.lib \
  ${tech_dir}/lib/fakeram45_128x116.lib
${tech_dir}/lib/NangateOpenCellLibrary_typical.lib \
"

read_lef $tech_lef
read_lef $std_lef

foreach lef_file ${lefs} {
  read_lef $lef_file
}

foreach lib_file ${libs} {
  read_liberty $lib_file
}

if { [file exists "odb/${case}_io.odb"] == 0 } {
  puts "${case}_io.odb not found"
  read_verilog ${cur_dir}/${case}.v
  link_design ${case}
  read_sdc ${cur_dir}/${case}.sdc
  read_def -floorplan_initialize ${cur_dir}/../../def/${case}_fp.def
  # Save the design
  write_db odb/${case}_io.odb
} else {
  puts "${case}_io.odb found"
  read_db odb/${case}_io.odb
  read_sdc ${cur_dir}/${case}.sdc
}

# Liberty units are fF,kOhm
set_layer_rc -layer metal1 -resistance 5.4286e-03 -capacitance 7.41819E-02
set_layer_rc -layer metal2 -resistance 3.5714e-03 -capacitance 6.74606E-02
set_layer_rc -layer metal3 -resistance 3.5714e-03 -capacitance 8.88758E-02
set_layer_rc -layer metal4 -resistance 1.5000e-03 -capacitance 1.07121E-01
set_layer_rc -layer metal5 -resistance 1.5000e-03 -capacitance 1.08964E-01
set_layer_rc -layer metal6 -resistance 1.5000e-03 -capacitance 1.02044E-01
set_layer_rc -layer metal7 -resistance 1.8750e-04 -capacitance 1.10436E-01
set_layer_rc -layer metal8 -resistance 1.8750e-04 -capacitance 9.69714E-02
# No calibration data available for metal9 and metal10
#set_layer_rc -layer metal9 -resistance 3.7500e-05 -capacitance 3.6864e-02
#set_layer_rc -layer metal10 -resistance 3.7500e-05 -capacitance 2.8042e-02

set_wire_rc -signal -layer metal3
set_wire_rc -clock  -layer metal5


set_debug_level MPL "build_logical_hierarchy" 1
set_debug_level MPL "multilevel_autoclustering" 1
set_debug_level MPL "refineMacroPlacement" 1
set_debug_level MPL "go_with_the_winner" 1
set_debug_level MPL "syncup" 1
rtl_macro_placer2 -halo_height 1 -halo_width 1 -leiden_iteration 10 -max_num_macro 16 -min_num_macro 4 -max_num_inst 30000 -min_num_inst 5000 -target_util 0.7
puts "Finished Macro Placement"
write_db ./odb/${case}.odb
gui::show