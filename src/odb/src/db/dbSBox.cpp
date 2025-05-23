// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2025, The OpenROAD Authors

#include "dbSBox.h"

#include <string>
#include <vector>

#include "dbBlock.h"
#include "dbBox.h"
#include "dbDatabase.h"
#include "dbSWire.h"
#include "dbTable.h"
#include "dbTable.hpp"
#include "dbTech.h"
#include "dbTechLayer.h"
#include "dbTechVia.h"
#include "dbVia.h"
#include "odb/db.h"
#include "odb/dbShape.h"

namespace odb {

template class dbTable<_dbSBox>;

bool _dbSBox::operator==(const _dbSBox& rhs) const
{
  if (_sflags._wire_type != rhs._sflags._wire_type) {
    return false;
  }

  if (_sflags._direction != rhs._sflags._direction) {
    return false;
  }

  if (_sflags._via_bottom_mask != rhs._sflags._via_bottom_mask) {
    return false;
  }

  if (_sflags._via_cut_mask != rhs._sflags._via_cut_mask) {
    return false;
  }

  if (_sflags._via_top_mask != rhs._sflags._via_top_mask) {
    return false;
  }

  if (_dbBox::operator!=(rhs)) {
    return false;
  }

  return true;
}

int _dbSBox::equal(const _dbSBox& rhs) const
{
  if (_sflags._wire_type != rhs._sflags._wire_type) {
    return false;
  }

  if (_sflags._direction != rhs._sflags._direction) {
    return false;
  }

  if (_sflags._via_bottom_mask != rhs._sflags._via_bottom_mask) {
    return false;
  }

  if (_sflags._via_cut_mask != rhs._sflags._via_cut_mask) {
    return false;
  }

  if (_sflags._via_top_mask != rhs._sflags._via_top_mask) {
    return false;
  }

  return _dbBox::equal(rhs);
}

bool _dbSBox::operator<(const _dbSBox& rhs) const
{
  if (_sflags._wire_type < rhs._sflags._wire_type) {
    return true;
  }

  if (_sflags._direction < rhs._sflags._direction) {
    return true;
  }

  if (_sflags._via_bottom_mask < rhs._sflags._via_bottom_mask) {
    return true;
  }

  if (_sflags._via_cut_mask < rhs._sflags._via_cut_mask) {
    return true;
  }

  if (_sflags._via_top_mask < rhs._sflags._via_top_mask) {
    return true;
  }

  if (_sflags._wire_type > rhs._sflags._wire_type) {
    return false;
  }

  if (_sflags._direction > rhs._sflags._direction) {
    return false;
  }

  if (_sflags._via_bottom_mask > rhs._sflags._via_bottom_mask) {
    return false;
  }

  if (_sflags._via_cut_mask > rhs._sflags._via_cut_mask) {
    return false;
  }

  if (_sflags._via_top_mask > rhs._sflags._via_top_mask) {
    return false;
  }

  return _dbBox::operator<(rhs);
}

////////////////////////////////////////////////////////////////////
//
// dbSBox - Methods
//
////////////////////////////////////////////////////////////////////

dbWireShapeType dbSBox::getWireShapeType()
{
  _dbSBox* box = (_dbSBox*) this;
  return dbWireShapeType(box->_sflags._wire_type);
}

dbSBox::Direction dbSBox::getDirection()
{
  _dbSBox* box = (_dbSBox*) this;
  return (dbSBox::Direction) box->_sflags._direction;
}

dbSWire* dbSBox::getSWire()
{
  return (dbSWire*) getBoxOwner();
}

Oct dbSBox::getOct()
{
  _dbSBox* box = (_dbSBox*) this;
  return box->_shape._oct;
}

uint dbSBox::getViaBottomLayerMask()
{
  _dbSBox* box = (_dbSBox*) this;
  return box->_sflags._via_bottom_mask;
}

uint dbSBox::getViaCutLayerMask()
{
  _dbSBox* box = (_dbSBox*) this;
  return box->_sflags._via_cut_mask;
}

uint dbSBox::getViaTopLayerMask()
{
  _dbSBox* box = (_dbSBox*) this;
  return box->_sflags._via_top_mask;
}

bool dbSBox::hasViaLayerMasks()
{
  _dbSBox* box = (_dbSBox*) this;
  return box->_sflags._via_bottom_mask != 0 || box->_sflags._via_cut_mask != 0
         || box->_sflags._via_top_mask != 0;
}

void dbSBox::setViaLayerMask(uint bottom, uint cut, uint top)
{
  _dbSBox* box = (_dbSBox*) this;
  box->checkMask(bottom);
  box->checkMask(cut);
  box->checkMask(top);

  box->_sflags._via_bottom_mask = bottom;
  box->_sflags._via_cut_mask = cut;
  box->_sflags._via_top_mask = top;
}

dbSBox* dbSBox::create(dbSWire* wire_,
                       dbTechLayer* layer_,
                       int x1,
                       int y1,
                       int x2,
                       int y2,
                       dbWireShapeType type,
                       Direction dir,
                       int width)
{
  _dbSWire* wire = (_dbSWire*) wire_;
  _dbBlock* block = (_dbBlock*) wire->getOwner();
  _dbSBox* box = block->_sbox_tbl->create();

  uint dx;
  if (x2 > x1) {
    dx = x2 - x1;
  } else {
    dx = x1 - x2;
  }

  uint dy;
  if (y2 > y1) {
    dy = y2 - y1;
  } else {
    dy = y1 - y2;
  }

  switch (dir) {
    case UNDEFINED:
      if ((dx & 1) && (dy & 1)) {  // both odd
        return nullptr;
      }

      break;

    case HORIZONTAL:
      if (dy & 1) {  // dy odd
        return nullptr;
      }
      break;

    case VERTICAL:
      if (dx & 1) {  // dy odd
        return nullptr;
      }
      break;
    case OCTILINEAR:
      if (dx != dy) {
        return nullptr;
      }
      break;
  }

  box->_flags._layer_id = layer_->getImpl()->getOID();
  box->_flags._owner_type = dbBoxOwner::SWIRE;
  if (dir == OCTILINEAR) {
    Point p1(x1, y1);
    Point p2(x2, y2);
    new (&box->_shape._oct) Oct();
    box->_shape._oct.init(p1, p2, width);
    box->_flags._octilinear = true;
    block->add_oct(box->_shape._oct);
  } else {
    box->_shape._rect.init(x1, y1, x2, y2);
    box->_flags._octilinear = false;
    block->add_rect(box->_shape._rect);
  }

  box->_sflags._wire_type = type.getValue();
  box->_sflags._direction = dir;

  wire->addSBox(box);

  return (dbSBox*) box;
}

dbSBox* dbSBox::create(dbSWire* wire_,
                       dbVia* via_,
                       int x,
                       int y,
                       dbWireShapeType type)
{
  _dbSWire* wire = (_dbSWire*) wire_;
  _dbVia* via = (_dbVia*) via_;
  _dbBlock* block = (_dbBlock*) wire->getOwner();

  if (via->_bbox == 0) {
    return nullptr;
  }

  _dbBox* vbbox = block->_box_tbl->getPtr(via->_bbox);
  int xmin = vbbox->_shape._rect.xMin() + x;
  int ymin = vbbox->_shape._rect.yMin() + y;
  int xmax = vbbox->_shape._rect.xMax() + x;
  int ymax = vbbox->_shape._rect.yMax() + y;
  _dbSBox* box = block->_sbox_tbl->create();
  box->_flags._owner_type = dbBoxOwner::SWIRE;
  box->_shape._rect.init(xmin, ymin, xmax, ymax);
  box->_flags._is_block_via = 1;
  box->_flags._via_id = via->getOID();
  box->_flags._octilinear = false;
  box->_sflags._wire_type = type.getValue();

  wire->addSBox(box);

  block->add_rect(box->_shape._rect);
  return (dbSBox*) box;
}

dbSBox* dbSBox::create(dbSWire* wire_,
                       dbTechVia* via_,
                       int x,
                       int y,
                       dbWireShapeType type)
{
  _dbSWire* wire = (_dbSWire*) wire_;
  _dbTechVia* via = (_dbTechVia*) via_;
  _dbBlock* block = (_dbBlock*) wire->getOwner();

  if (via->_bbox == 0) {
    return nullptr;
  }

  _dbTech* tech = (_dbTech*) via->getOwner();
  _dbBox* vbbox = tech->_box_tbl->getPtr(via->_bbox);
  int xmin = vbbox->_shape._rect.xMin() + x;
  int ymin = vbbox->_shape._rect.yMin() + y;
  int xmax = vbbox->_shape._rect.xMax() + x;
  int ymax = vbbox->_shape._rect.yMax() + y;
  _dbSBox* box = block->_sbox_tbl->create();
  box->_flags._owner_type = dbBoxOwner::SWIRE;
  box->_shape._rect.init(xmin, ymin, xmax, ymax);
  box->_flags._is_tech_via = 1;
  box->_flags._via_id = via->getOID();
  box->_flags._octilinear = false;
  box->_sflags._wire_type = type.getValue();

  wire->addSBox(box);

  block->add_rect(box->_shape._rect);
  return (dbSBox*) box;
}

dbSBox* dbSBox::getSBox(dbBlock* block_, uint dbid_)
{
  _dbBlock* block = (_dbBlock*) block_;
  return (dbSBox*) block->_sbox_tbl->getPtr(dbid_);
}

void dbSBox::destroy(dbSBox* box_)
{
  _dbSWire* wire = (_dbSWire*) box_->getSWire();
  _dbBlock* block = (_dbBlock*) wire->getOwner();
  _dbSBox* box = (_dbSBox*) box_;

  wire->removeSBox(box);

  block->remove_rect(box->_shape._rect);
  dbProperty::destroyProperties(box);
  block->_sbox_tbl->destroy(box);
}

std::vector<dbSBox*> dbSBox::smashVia()
{
  if (!isVia()) {
    return {};
  }

  auto* block_via = getBlockVia();

  if (block_via == nullptr) {
    return {};
  }

  if (block_via->getTechVia() != nullptr) {
    return {};
  }

  auto params = block_via->getViaParams();

  if (params.getNumCutCols() == 1 && params.getNumCutRows() == 1) {
    // nothing to do
    return {};
  }

  const std::string name = block_via->getName() + "_smashed";

  _dbSWire* wire = (_dbSWire*) getSWire();
  dbBlock* block = (dbBlock*) wire->getOwner();
  dbSWire* swire = (dbSWire*) wire;

  odb::dbVia* new_block_via = block->findVia(name.c_str());

  if (new_block_via == nullptr) {
    new_block_via = odb::dbVia::create(block, name.c_str());

    params.setNumCutCols(1);
    params.setNumCutRows(1);

    new_block_via->setViaParams(params);
  }

  std::vector<dbSBox*> new_boxes;

  std::vector<odb::dbShape> via_boxes;
  getViaBoxes(via_boxes);
  for (const auto& via_box : via_boxes) {
    auto* layer = via_box.getTechLayer();
    if (layer->getType() != odb::dbTechLayerType::CUT) {
      continue;
    }

    const auto& box = via_box.getBox();
    auto* sbox_via = odb::dbSBox::create(
        swire, new_block_via, box.xCenter(), box.yCenter(), getWireShapeType());
    new_boxes.push_back(sbox_via);

    if (hasViaLayerMasks()) {
      sbox_via->setViaLayerMask(
          getViaBottomLayerMask(), getViaCutLayerMask(), getViaTopLayerMask());
    }
  }

  return new_boxes;
}

}  // namespace odb
