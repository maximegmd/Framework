/*
 * __BEGIN_COPYRIGHT
 * SimpleDB: C++ ODBC database API
 * Copyright (C) 2006 Eminence Technology Pty Ltd
 * Copyright (C) 2008-2010 Russell Kliese
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Russell Kliese can be contacted by email: russell [at] kliese [dot] id [dot] au
 *
 * __END_COPYRIGHT
 */

#include "sqlwrap.h"
#include "Column.h"
#include "QueryPimplData_i.h"
#include "ColumnPimplData_i.h"

namespace SimpleDB {
  Column::Column() : pimplData(new PimplData) {
    pimplData->bound = false;
  }

  void Column::doBind(QueryPimplData& queryPimplData, int columnNumber) {
    pimplData->bound = true;
  }

  bool Column::isNull() const {
    if (!pimplData->bound)
      throw UnboundException();
    if (pimplData->lengthOrIndex == SQL_NULL_DATA)
      return true;
    else
      return false;
  }

  void Column::val()
  {
    if (!pimplData->bound)
      throw UnboundException();
  }

  Column::~Column() {
  }

  Column::PimplData& Column::getPimplData() {
    return *pimplData;
  }
}