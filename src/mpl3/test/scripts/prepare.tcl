source library.tcl

read_verilog ./${case}.v
link_design ${case}
read_sdc ./${case}.sdc
read_def -floorplan_initialize ../../def/${case}_fp.def
write_db ./${case}.odb
write_def ../dreamplace/${case}.def

source rc.tcl

