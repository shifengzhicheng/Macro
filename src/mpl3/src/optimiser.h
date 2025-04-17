///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2021, The Regents of the University of California
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <map>
#include <queue>
#include <set>
#include <unordered_set>
#include <vector>

#include "object.h"

namespace utl {
class Logger;
}

namespace mpl3 {

using mapModule2InstVec
    = std::unordered_map<Cluster*, std::vector<odb::dbInst*>>;

class LogicalHierarchy
{
 private:
  std::unique_ptr<Cluster> root;
  std::unordered_map<odb::dbInst*, Cluster*> inst_to_logical_module;
  utl::Logger* logger_{nullptr};
  int large_net_threshold_{0};
  int id_{0};

 public:
  LogicalHierarchy(utl::Logger* logger);
  void setLargeNetThreshold(int large_net_threshold);
  void buildLogicalHierarchyDFS(odb::dbModule* top_module, Cluster* root);
  Cluster* getModule(odb::dbInst* inst);
  Cluster* getRoot();
  size_t getRecursiveClusterStdCellsSize(const Cluster* cluster);
  size_t getRecursiveClusterMacrosSize(Cluster* cluster);
  void setRoot(std::unique_ptr<Cluster> root);
  void updateInstsCorresponse(Cluster* cluster);
  mapModule2InstVec findModulesForCluster(std::vector<odb::dbInst*>& cluster);
  Cluster* findLCA(Cluster* module1, Cluster* module2);
  Cluster* findLCAForModules(std::set<Cluster*>& modules);
  Cluster* createChildModuleForModule(Cluster* parent_module,
                                      std::vector<odb::dbInst*>& sub_cluster);
  Cluster* mergeModulesAsChild(Cluster* parent_module,
                               std::set<Cluster*>& modules);
  void printLogicalHierarchyTree(Cluster* parent, int level);

  struct CandidateConnections
  {
    std::map<Cluster*, std::map<Cluster*, float>> module_to_module;
    std::map<Cluster*, std::map<odb::dbInst*, float>> module_to_macro;
  };
  CandidateConnections getClusterConnectivity(
      const std::set<Cluster*>& candidates);

  void releaseChildModule(Cluster* child_module);
  void releaseChildModule(std::set<Cluster*> child_modules);
  float calMoveGain(Cluster* from,
                    Cluster* to,
                    CandidateConnections& connection_map);
  Cluster* findUltimateDestination(
      Cluster* cluster,
      const std::map<Cluster*, Cluster*>& merge_pairs);
  std::unordered_map<Cluster*, std::set<Cluster*>> buildMergeSetMap(
      const std::map<Cluster*, Cluster*>& merge_pairs);
  void clearEmptyModules(Cluster* top_module);
};

class MacroRefinement
{
 public:
  MacroRefinement(utl::Logger* logger,
                  odb::dbBlock* block,
                  PhysicalHierarchy* tree)
  {
    logger_ = logger;
    block_ = block;
    tree_ = tree;
  }
  void refineMacros();
  void generateGroups(Cluster* root, odb::dbGroup* parentGroup = nullptr);
 private:
  utl::Logger* logger_;
  odb::dbBlock* block_;
  PhysicalHierarchy* tree_;
  HardMacro* getHardMacro(odb::dbInst* inst);
};

}  // namespace mpl3