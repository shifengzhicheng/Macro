#include "clusterEngine.h"
#include "optimiser.h"

#include <functional>

namespace mpl3 {

// Constructor
LogicalHierarchy::LogicalHierarchy(utl::Logger* logger)
    : logger_(logger), large_net_threshold_(0), id_(0)
{
}

// Sets the large net threshold
void LogicalHierarchy::setLargeNetThreshold(int large_net_threshold)
{
  large_net_threshold_ = large_net_threshold;
}

void LogicalHierarchy::buildLogicalHierarchyDFS(odb::dbModule* top_module,
                                                Cluster* root)
{
  for (odb::dbModInst* child_mod_inst : top_module->getChildren()) {
    odb::dbModule* child_module = child_mod_inst->getMaster();
    std::unique_ptr<Cluster> cluster = std::make_unique<Cluster>(
        ++id_, child_module->getHierarchicalName(), logger_);
    cluster->setParent(root);
    buildLogicalHierarchyDFS(child_module, cluster.get());
    root->addChild(std::move(cluster));
  }
  for (auto inst : top_module->getInsts()) {
    root->addLeafInst(inst);
  }
  updateInstsCorresponse(root);
}

Cluster* LogicalHierarchy::getModule(odb::dbInst* inst)
{
  auto it = inst_to_logical_module.find(inst);
  return (it != inst_to_logical_module.end()) ? it->second : nullptr;
}

Cluster* LogicalHierarchy::getRoot()
{
  return root.get();
}

size_t LogicalHierarchy::getRecursiveClusterStdCellsSize(const Cluster* cluster)
{
  size_t size = cluster->getNumLeafStdCells();
  for (auto& child : cluster->getChildren()) {
    size += getRecursiveClusterStdCellsSize(child.get());
  }
  return size;
}

size_t LogicalHierarchy::getRecursiveClusterMacrosSize(Cluster* cluster)
{
  size_t size = cluster->getNumLeafMacros();
  for (auto& child : cluster->getChildren()) {
    size += getRecursiveClusterMacrosSize(child.get());
  }
  return size;
}

void LogicalHierarchy::setRoot(std::unique_ptr<Cluster> root)
{
  this->root = std::move(root);
}

void LogicalHierarchy::updateInstsCorresponse(Cluster* cluster)
{
  for (auto inst : cluster->getLeafStdCells())
    inst_to_logical_module[inst] = cluster;
  for (auto inst : cluster->getLeafMacros())
    inst_to_logical_module[inst] = cluster;
}

mapModule2InstVec LogicalHierarchy::findModulesForCluster(
    std::vector<odb::dbInst*>& cluster)
{
  mapModule2InstVec map_module2inst_vec;
  for (auto inst : cluster) {
    Cluster* module = getModule(inst);
    map_module2inst_vec[module].push_back(inst);
  }
  return map_module2inst_vec;
}

Cluster* LogicalHierarchy::findLCA(Cluster* module1, Cluster* module2)
{
  std::set<Cluster*> ancestors;
  while (module1) {
    ancestors.insert(module1);
    module1 = module1->getParent();
  }
  while (module2) {
    if (ancestors.find(module2) != ancestors.end())
      return module2;
    module2 = module2->getParent();
  }
  return nullptr;
}

Cluster* LogicalHierarchy::findLCAForModules(std::set<Cluster*>& modules)
{
  if (modules.empty())
    return nullptr;
  auto it = modules.begin();
  Cluster* lca = *it;
  ++it;
  for (; it != modules.end(); ++it) {
    lca = findLCA(lca, *it);
  }
  return lca;
}

Cluster* LogicalHierarchy::createChildModuleForModule(
    Cluster* parent_module,
    std::vector<odb::dbInst*>& sub_cluster)
{
  std::string module_name
      = fmt::format("{}/{}_", parent_module->getName(), ++id_);
  std::unique_ptr<Cluster> new_module
      = std::make_unique<Cluster>(id_, module_name, logger_);
  parent_module->removeLeafInsts(sub_cluster);
  for (auto inst : sub_cluster)
    new_module->addLeafInst(inst);
  Cluster* new_module_ptr = new_module.get();
  updateInstsCorresponse(new_module_ptr);
  new_module_ptr->setParent(parent_module);
  parent_module->addChild(std::move(new_module));
  return new_module_ptr;
}

Cluster* LogicalHierarchy::mergeModulesAsChild(Cluster* parent_module,
                                               std::set<Cluster*>& modules)
{
  std::string module_name
      = fmt::format("{}/{}_", parent_module->getName(), ++id_);
  std::unique_ptr<Cluster> child_module
      = std::make_unique<Cluster>(id_, module_name, logger_);
  Cluster* child_module_ptr = child_module.get();
  child_module->setParent(parent_module);
  parent_module->addChild(std::move(child_module));
  for (auto candidate : modules) {
    for (auto inst : candidate->getLeafStdCells())
      child_module_ptr->addLeafInst(inst);
    for (auto inst : candidate->getLeafMacros())
      child_module_ptr->addLeafInst(inst);
    candidate->clearLeafStdCells();
    candidate->clearLeafMacros();
    candidate->getParent()->releaseChild(candidate);
  }
  updateInstsCorresponse(child_module_ptr);
  return child_module_ptr;
}

void LogicalHierarchy::printLogicalHierarchyTree(Cluster* parent, int level)
{
  std::string line;
  for (int i = 0; i < level; i++) {
    line += "+---";
  }
  line += fmt::format(
      "{}  ({})  num_macro :  {}   num_std_cell :  {} cluster type: {} {}",
      parent->getName(),
      parent->getId(),
      getRecursiveClusterMacrosSize(parent),
      getRecursiveClusterStdCellsSize(parent),
      parent->getIsLeafString(),
      parent->getClusterTypeString());
  logger_->report("{}", line);
  for (auto& child : parent->getChildren()) {
    printLogicalHierarchyTree(child.get(), level + 1);
  }
}

LogicalHierarchy::CandidateConnections LogicalHierarchy::getClusterConnectivity(
    const std::set<Cluster*>& candidates)
{
  CandidateConnections candidate_connections;
  std::unordered_set<odb::dbNet*> processedNets;
  for (Cluster* candidate : candidates) {
    for (odb::dbInst* inst : candidate->getLeafStdCells()) {
      for (odb::dbITerm* iterm : inst->getITerms()) {
        odb::dbNet* net = iterm->getNet();
        if (!net)
          continue;
        if (processedNets.find(net) != processedNets.end())
          continue;
        processedNets.insert(net);
        if (net->getITerms().size() > large_net_threshold_)
          continue;
        for (odb::dbITerm* net_iterm : net->getITerms()) {
          if (net_iterm == iterm)
            continue;
          else {
            odb::dbInst* net_inst = net_iterm->getInst();
            if (net_inst->isBlock())
              candidate_connections.module_to_macro[candidate][net_inst] += 1.0;
            else {
              Cluster* cluster = getModule(net_inst);
              if (cluster && cluster != candidate) {
                candidate_connections.module_to_module[candidate][cluster]
                    += 1.0;
                candidate_connections.module_to_module[cluster][candidate]
                    += 1.0;
              }
            }
          }
        }
      }
    }
    if (logger_->debugCheck(MPL, "build_logical_hierarchy", 1)) {
      std::string connInfo
          = "Candidate: " + candidate->getName() + " Size: "
            + std::to_string(getRecursiveClusterStdCellsSize(candidate))
            + " connections: \nModule connections: \n";
      for (const auto& entry :
           candidate_connections.module_to_module[candidate]) {
        connInfo += "(" + entry.first->getName() + ", "
                    + std::to_string(entry.second) + ") ";
      }
      connInfo += "\nMacro connections: \n";
      for (const auto& entry :
           candidate_connections.module_to_macro[candidate]) {
        connInfo += "(" + entry.first->getName() + ", "
                    + std::to_string(entry.second) + ") ";
      }
      debugPrint(logger_, MPL, "build_logical_hierarchy", 1, "{}\n", connInfo);
    }
  }
  return candidate_connections;
}

void LogicalHierarchy::releaseChildModule(Cluster* child_module)
{
  Cluster* parent_module = nullptr;
  if (child_module != nullptr)
    parent_module = child_module->getParent();
  if (parent_module != nullptr) {
    for (auto inst : child_module->getLeafStdCells())
      parent_module->addLeafInst(inst);
    parent_module->releaseChild(child_module);
    updateInstsCorresponse(parent_module);
  }
}

void LogicalHierarchy::releaseChildModule(std::set<Cluster*> child_modules)
{
  for (auto child_module : child_modules) {
    releaseChildModule(child_module);
  }
}

float LogicalHierarchy::calMoveGain(Cluster* from,
                                    Cluster* to,
                                    CandidateConnections& connection_map)
{
  assert(from != nullptr);
  assert(to != nullptr);
  assert(from->getParent() != to->getParent());

  const float alpha = 1.0f;
  const float beta = 1.0f;
  const float gamma = 10.0f;

  float remote_module_connection = 0.0f;
  auto it_from = connection_map.module_to_module.find(from);
  if (it_from != connection_map.module_to_module.end()) {
    auto it_to = it_from->second.find(to);
    if (it_to != it_from->second.end())
      remote_module_connection = it_to->second;
  }

  float parent_module_connection = 0.0f;
  Cluster* parent = from->getParent();
  if (parent && (it_from != connection_map.module_to_module.end())) {
    auto it_parent = it_from->second.find(parent);
    if (it_parent != it_from->second.end())
      parent_module_connection = it_parent->second;
  }

  float parent_macro_connection = 0.0f;
  auto it_macro = connection_map.module_to_macro.find(from);
  if (it_macro != connection_map.module_to_macro.end()) {
    for (auto it : it_macro->second)
      parent_macro_connection += it.second;
  }
  if (logger_->debugCheck(MPL, "build_logical_hierarchy", 1)) {
    debugPrint(logger_,
               MPL,
               "build_logical_hierarchy",
               1,
               "Move gain for {} to {}: remote_module_connection: {}, "
               "parent_module_connection: {}, parent_macro_connection: {}",
               from->getName(),
               to->getName(),
               remote_module_connection,
               parent_module_connection,
               parent_macro_connection);
  }
  float move_gain = alpha * remote_module_connection
                    - beta * parent_module_connection
                    - gamma * parent_macro_connection;
  return move_gain;
}

Cluster* LogicalHierarchy::findUltimateDestination(
    Cluster* cluster,
    const std::map<Cluster*, Cluster*>& merge_pairs)
{
  auto it = merge_pairs.find(cluster);
  if (it->second == nullptr)
    return cluster;
  else
    return findUltimateDestination(it->second, merge_pairs);
}

std::unordered_map<Cluster*, std::set<Cluster*>>
LogicalHierarchy::buildMergeSetMap(
    const std::map<Cluster*, Cluster*>& merge_pairs)
{
  std::unordered_map<Cluster*, std::set<Cluster*>> merge_map;
  for (const auto& p : merge_pairs) {
    Cluster* src = p.first;
    Cluster* ultimate_dest = findUltimateDestination(src, merge_pairs);
    if (ultimate_dest != src)
      merge_map[ultimate_dest].insert(src);
  }
  return merge_map;
}

void LogicalHierarchy::clearEmptyModules(Cluster* top_module)
{
  if (top_module != nullptr) {
    if (getRecursiveClusterStdCellsSize(top_module) == 0
        && getRecursiveClusterMacrosSize(top_module) == 0) {
      debugPrint(logger_,
                 MPL,
                 "build_logical_hierarchy",
                 1,
                 "Delete EmptyModule: {}, std cell size:{}, macro cell size:{}",
                 top_module->getName(),
                 getRecursiveClusterStdCellsSize(top_module),
                 getRecursiveClusterMacrosSize(top_module));
      top_module->getParent()->releaseChild(top_module);
    }
  }
}

void MacroRefinement::refineMacros()
{
  // macro cluster detection, for grouped macros and single macro (get from Macro cluster that contains single macro)
  std::function<void(Cluster*, std::vector<Cluster*>&, std::vector<Cluster*>&)> fetchMacroClusters;
  fetchMacroClusters = [&](Cluster* module,
                           std::vector<Cluster*>& singleMacroClusters,
                           std::vector<Cluster*>& groupMacroClusters) {
    if (module->isLeaf()) {
      if (module->getNumLeafMacros() == 1) {
        singleMacroClusters.push_back(module);
      } else if (module->getNumLeafMacros() > 1) {
        groupMacroClusters.push_back(module);
      }
    } else {
      for (auto& child : module->getChildren()) {
        fetchMacroClusters(child.get(), singleMacroClusters, groupMacroClusters);
      }
    }
  };
  // need to be pulled)
  std::vector<Cluster*> singleMacroClusters;
  std::vector<Cluster*> groupMacroClusters;
  // Recursively traverse the tree to find clusters with a single macro.
  Cluster* top_module = tree_->root.get();
  fetchMacroClusters(top_module, singleMacroClusters, groupMacroClusters);
  // macro group detection (Can get from the Physical hierarchy tree)
  
  // force directed macro refinement

  // alignment (How to do this?)
}

HardMacro* MacroRefinement::getHardMacro(odb::dbInst* inst) {
    HardMacro* hard_macro = tree_->maps.inst_to_hard[inst].get();
    return hard_macro;
}

void MacroRefinement::generateGroups(Cluster* root,
                                     odb::dbGroup* parentGroup)
{
  if (root == nullptr)
    return;
  odb::dbGroup* currentGroup = nullptr;
  if (parentGroup)
    currentGroup = odb::dbGroup::create(parentGroup,
                                        std::to_string(root->getId()).c_str());
  else
    currentGroup
        = odb::dbGroup::create(block_, std::to_string(root->getId()).c_str());
  for (auto macro : root->getLeafMacros()) {
    // Add each macro in the cluster to the group.
    currentGroup->addInst(macro);
  }
  for (auto std_cell : root->getLeafStdCells()) {
    // Add each macro in the cluster to the group.
    currentGroup->addInst(std_cell);
  }
  for (const auto& child : root->getChildren()) {
    generateGroups(child.get(), currentGroup);
  }
  // Log the creation of the group.
  logger_->report("Created dbGroup for Cluster ID: {}  Name: {}",
                  root->getId(),
                  currentGroup->getName());
}

}  // namespace mpl3
