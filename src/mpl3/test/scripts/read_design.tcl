source library.tcl

puts "read ariane_fp.def"

read_verilog ./ariane.v
link_design ariane
read_sdc ./ariane.sdc
read_def -floorplan_initialize ./ariane_fp_HierRTLMP.def
# read_def -floorplan_initialize ./ariane_fp.def
# read_def -floorplan_initialize ../dreamplace/ariane.def
# Save the design
# write_def ../dreamplace/ariane.def
source rc.tcl

