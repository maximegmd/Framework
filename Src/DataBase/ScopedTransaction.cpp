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

#include "ScopedTransaction.h"
#include "sqlwrap.h"
#include "Database.h"
#include "DatabasePimplData_i.h"

namespace SimpleDB {
  ScopedTransaction::ScopedTransaction(Database& database)
    : _commit(false), databasePimplData(database.getPimplData()) {
    /*
     * Begin the transaction upon entering the first ScopedTransaction
     */
    if(databasePimplData.transactionCount == 0) {
      SQLRETURN ret = SQLSetConnectAttr
	(databasePimplData.connectionHandle, SQL_ATTR_AUTOCOMMIT,
	 static_cast<SQLPOINTER>(SQL_AUTOCOMMIT_OFF), SQL_IS_UINTEGER);
      if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	throw Database::Exception("Couldn't turn off autocommit mode");
    }
    databasePimplData.transactionCount ++;
  }

  void ScopedTransaction::commit() {
    if(!_commit) {
      _commit = true;
      if ((--databasePimplData.transactionCount) == 0) {
	SQLEndTran(SQL_HANDLE_DBC, databasePimplData.connectionHandle,
		   SQL_COMMIT);
      }
    } else {
      throw Database::Exception("Attempted to commit a transaction more than "
				"once");
    }
  }

  void ScopedTransaction::rollback() {
    if(_commit)
      throw Database::Exception("Cannot rollback a transaction after there "
				"has been a commit");
    databasePimplData.rollingBack = true;
  }

  ScopedTransaction::~ScopedTransaction() {
    if(!_commit) {
      databasePimplData.rollingBack = true;
      databasePimplData.transactionCount --;
    }

    if((databasePimplData.transactionCount == 0) &&
       (databasePimplData.rollingBack)) {
      SQLRETURN ret = SQLEndTran(SQL_HANDLE_DBC,
				 databasePimplData.connectionHandle,
				 SQL_ROLLBACK);
      if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	throw Database::Exception("Couldn't roll-back transaction");
      databasePimplData.rollingBack = false;
      ret = SQLSetConnectAttr
	(databasePimplData.connectionHandle, SQL_ATTR_AUTOCOMMIT,
	 reinterpret_cast<SQLPOINTER>(SQL_AUTOCOMMIT_ON), SQL_IS_UINTEGER);
      if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	throw Database::Exception("Couldn't turn on autocommit mode");
    }
  }
}