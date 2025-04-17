source library.tcl

if { [file exists "${case}.odb"] == 0 } {
  puts "${case}.odb not found"
  read_verilog ${cur_dir}/${case}.v
  link_design ${case}
  read_sdc ${cur_dir}/${case}.sdc
  read_def -floorplan_initialize ${cur_dir}/../../def/${case}_fp.def
  # Save the design
  write_db ${case}.odb
} else {
  puts "${case}.odb found"
  read_db ${case}.odb
  read_sdc ${cur_dir}/${case}.sdc
}

source rc.tcl

