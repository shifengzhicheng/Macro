set tech_dir "/scripts/MacroPlacement/Enablements/NanGate45"
set cur_dir "/scripts/MacroPlacement/Flows/NanGate45/ariane133/scripts/OpenROAD"
set syn_dir "/OpenROAD-flow-scripts/flow/results/nangate45/ariane133/base"

set tech_lef "${tech_dir}/lef/NangateOpenCellLibrary.tech.lef"
set std_lef "${tech_dir}/lef/NangateOpenCellLibrary.macro.mod.lef"
set case "ariane"
set lefs "
    ${cur_dir}/fakeram45_256x16.lef
"
# Setting lib files
set libs "
    ${tech_dir}/lib/NangateOpenCellLibrary_typical.lib \
    ${cur_dir}/fakeram45_256x16.lib \
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
  puts "${case}.odb not found"
  # read_verilog ${syn_dir}/1_synth.v
  read_verilog ${cur_dir}/${case}.v
  link_design ${case}
  # read_sdc ${syn_dir}/1_synth.sdc
  read_sdc ${cur_dir}/${case}.sdc
  read_def -floorplan_initialize ${cur_dir}/${case}_fp.def
  # Save the design
  write_db odb/${case}_io.odb
} else {
  puts "odb/${case}_io.odb found"
  read_db odb/${case}_io.odb
  # read_sdc ${syn_dir}/1_synth.sdc
  read_sdc ${cur_dir}/${case}.sdc
}

set_debug_level MPL "build_logical_hierarchy" 1
set_debug_level MPL "multilevel_autoclustering" 1
set_debug_level MPL "refineMacroPlacement" 1
set_debug_level MPL "go_with_the_winner" 1
set_debug_level MPL "syncup" 1
rtl_macro_placer2 -halo_height 5 -halo_width 5 -leiden_iteration 10 -max_num_macro 16 -min_num_macro 4 -max_num_inst 30000 -min_num_inst 5000 -signature_net_threshold 30
puts "Finished Macro Placement"
write_db ./odb/ariane133.odb
gui::show