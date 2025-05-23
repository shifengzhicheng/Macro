// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2025, The OpenROAD Authors

#include "PathRenderer.h"

#include <memory>

#include "db_sta/dbNetwork.hh"
#include "sta/PathExpanded.hh"
#include "sta/TimingRole.hh"

namespace sta {

const gui::Painter::Color PathRenderer::signal_color = gui::Painter::red;
const gui::Painter::Color PathRenderer::clock_color = gui::Painter::yellow;

PathRenderer::PathRenderer(dbSta* sta) : sta_(sta)
{
  gui::Gui::get()->registerRenderer(this);
}

PathRenderer::~PathRenderer() = default;

void PathRenderer::highlight(PathRef* path)
{
  path_ = std::make_unique<PathExpanded>(path, sta_);
}

void PathRenderer::drawObjects(gui::Painter& painter)
{
  if (path_ == nullptr) {
    return;
  }
  dbNetwork* network = sta_->getDbNetwork();
  for (unsigned int i = 0; i < path_->size(); i++) {
    const PathRef* path = path_->path(i);
    TimingArc* prev_arc = path_->prevArc(i);
    // Draw lines for wires on the path.
    if (prev_arc && prev_arc->role()->isWire()) {
      const PathRef* prev_path = path_->path(i - 1);
      const Pin* pin = path->pin(sta_);
      const Pin* prev_pin = prev_path->pin(sta_);
      const odb::Point pt1 = network->location(pin);
      const odb::Point pt2 = network->location(prev_pin);
      const gui::Painter::Color wire_color
          = sta_->isClock(pin) ? clock_color : signal_color;
      painter.setPen(wire_color, true);
      painter.drawLine(pt1, pt2);
      highlightInst(prev_pin, painter);
      if (i == path_->size() - 1) {
        highlightInst(pin, painter);
      }
    }
  }
}

// Color in the instances to make them more visible.
void PathRenderer::highlightInst(const Pin* pin, gui::Painter& painter)
{
  dbNetwork* network = sta_->getDbNetwork();
  const Instance* inst = network->instance(pin);
  if (!network->isTopInstance(inst)) {
    dbInst* db_inst = nullptr;
    dbModInst* mod_inst;
    network->staToDb(inst, db_inst, mod_inst);
    if (db_inst != nullptr) {
      odb::dbBox* bbox = db_inst->getBBox();
      const odb::Rect rect = bbox->getBox();
      const gui::Painter::Color inst_color
          = sta_->isClock(pin) ? clock_color : signal_color;
      painter.setBrush(inst_color);
      painter.drawRect(rect);
    }
  }
}

}  // namespace sta
