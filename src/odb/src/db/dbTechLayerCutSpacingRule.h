// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020-2025, The OpenROAD Authors

// Generator Code Begin Header
#pragma once

#include "dbCore.h"
#include "odb/odb.h"

namespace odb {
class dbIStream;
class dbOStream;
class _dbDatabase;
class _dbTechLayer;
class _dbTechLayerCutClassRule;

struct dbTechLayerCutSpacingRuleFlags
{
  bool center_to_center_ : 1;
  bool same_net_ : 1;
  bool same_metal_ : 1;
  bool same_via_ : 1;
  uint cut_spacing_type_ : 3;
  bool stack_ : 1;
  bool orthogonal_spacing_valid_ : 1;
  bool above_width_enclosure_valid_ : 1;
  bool short_edge_only_ : 1;
  bool concave_corner_width_ : 1;
  bool concave_corner_parallel_ : 1;
  bool concave_corner_edge_length_ : 1;
  bool concave_corner_ : 1;
  bool extension_valid_ : 1;
  bool non_eol_convex_corner_ : 1;
  bool eol_width_valid_ : 1;
  bool min_length_valid_ : 1;
  bool above_width_valid_ : 1;
  bool mask_overlap_ : 1;
  bool wrong_direction_ : 1;
  uint adjacent_cuts_ : 2;
  bool exact_aligned_ : 1;
  bool cut_class_to_all_ : 1;
  bool no_prl_ : 1;
  bool same_mask_ : 1;
  bool except_same_pgnet_ : 1;
  bool side_parallel_overlap_ : 1;
  bool except_same_net_ : 1;
  bool except_same_metal_ : 1;
  bool except_same_metal_overlap_ : 1;
  bool except_same_via_ : 1;
  bool above_ : 1;
  bool except_two_edges_ : 1;
  bool two_cuts_valid_ : 1;
  bool same_cut_ : 1;
  bool long_edge_only_ : 1;
  bool prl_valid_ : 1;
  bool below_ : 1;
  bool par_within_enclosure_valid_ : 1;
  uint spare_bits_ : 22;
};

class _dbTechLayerCutSpacingRule : public _dbObject
{
 public:
  _dbTechLayerCutSpacingRule(_dbDatabase*);

  bool operator==(const _dbTechLayerCutSpacingRule& rhs) const;
  bool operator!=(const _dbTechLayerCutSpacingRule& rhs) const
  {
    return !operator==(rhs);
  }
  bool operator<(const _dbTechLayerCutSpacingRule& rhs) const;
  void collectMemInfo(MemInfo& info);

  dbTechLayerCutSpacingRuleFlags flags_;
  int cut_spacing_;
  dbId<_dbTechLayer> second_layer_;
  int orthogonal_spacing_;
  int width_;
  int enclosure_;
  int edge_length_;
  int par_within_;
  int par_enclosure_;
  int edge_enclosure_;
  int adj_enclosure_;
  int above_enclosure_;
  int above_width_;
  int min_length_;
  int extension_;
  int eol_width_;
  // EXACTALIGNED exactAlignedCut | EXCEPTSAMEVIA numCuts
  uint num_cuts_;
  // WITHIN cutWithin | PARALLELWITHIN within | SAMEMETALSHAREDEDGE parwithin
  int within_;
  // WITHIN cutWithin cutWithin2
  int second_within_;
  dbId<_dbTechLayerCutClassRule> cut_class_;
  uint two_cuts_;
  uint prl_;
  uint par_length_;
  int cut_area_;
};
dbIStream& operator>>(dbIStream& stream, _dbTechLayerCutSpacingRule& obj);
dbOStream& operator<<(dbOStream& stream, const _dbTechLayerCutSpacingRule& obj);
}  // namespace odb
   // Generator Code End Header
