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
#include "Query.h"
#include "DatabasePimplData_i.h"
#include "QueryPimplData_i.h"
#include "Column.h"
#include "Database.h"

namespace SimpleDB {
  Query::Query(Database& database) :
    pimplData(new QueryPimplData) {
    /*
     * Increment the counter that keeps track of the number of queries that
     * are being used with the database connection
     */
    DatabasePimplData& databasePimplData = database.getPimplData();
    databasePimplData.numQueries++;

    long return_value=SQLAllocHandle(SQL_HANDLE_STMT,
				     databasePimplData.connectionHandle,
				     &(pimplData->statementHandle));
    if ((return_value != SQL_SUCCESS) &&
	(return_value != SQL_SUCCESS_WITH_INFO)){
      databasePimplData.numQueries--;
      throw Exception(std::string("Error allocating statement handle"));
    }
    pimplData->databasePimplData = &databasePimplData;
  }

  Query::Query(const Query& that) : pimplData(new QueryPimplData) {
    /*
     * Increment the counter that keeps track of the number of queries that
     * are being used with the database connection
     */
    DatabasePimplData& databasePimplData = *that.pimplData->databasePimplData;
    databasePimplData.numQueries++;

    long return_value=SQLAllocHandle(SQL_HANDLE_STMT,
				     databasePimplData.connectionHandle,
				     &(pimplData->statementHandle));
    if ((return_value != SQL_SUCCESS) &&
	(return_value != SQL_SUCCESS_WITH_INFO)){
      databasePimplData.numQueries--;
      throw Exception(std::string("Error allocating statement handle"));
    }
    pimplData->databasePimplData = &databasePimplData;
  }

  Query::~Query() {
    if(NULL != pimplData->statementHandle)
    {
      SQLFreeHandle(SQL_HANDLE_STMT,pimplData->statementHandle);
      pimplData->statementHandle = NULL;
    }

    /*
     * Decrement the counter of the number of queries on the database
     * object
     */
    pimplData->databasePimplData->numQueries --;
  }

  void Query::bind(Column* columns[], int numberColumns) {
    for(int i = 0; i < numberColumns; i++) {
      columns[i]->doBind(*pimplData, i+1);
    }
  }

  static void throwError(SQLRETURN return_value, const std::string& errorPrefix, SQLHSTMT statementHandle)
  {
    std::string errorMessage(errorPrefix);

    // Extra info is available for SQL_EROOR and SQL_SUCCESS_WITH_INFO return
    // values
    std::string errorInfo;
    if((SQL_ERROR == return_value) || (SQL_SUCCESS_WITH_INFO == return_value))
    {
      char sqlState[10];
      SQLSMALLINT V_OD_mlen;
      SQLINTEGER V_OD_err;
      char messageBuffer[400];
      if(SQLGetDiagRecA(SQL_HANDLE_STMT, statementHandle,1,
          reinterpret_cast<SQLCHAR *>(sqlState),&V_OD_err,
          reinterpret_cast<SQLCHAR *>(messageBuffer),
          400,&V_OD_mlen) == SQL_SUCCESS) {
        errorInfo = "Info: ";
        errorInfo += sqlState;
        errorInfo += ": ";
        errorInfo += messageBuffer;
      }
    }

    switch(return_value) {
    case SQL_SUCCESS:                   // Not really an error
      errorMessage += "SQL_SUCCESS";
      break;
    case SQL_SUCCESS_WITH_INFO:
      errorMessage += "SQL_SUCCESS_WITH_INFO";
      break;
    case SQL_NEED_DATA:
      errorMessage += "SQL need data";
      break;
    case SQL_STILL_EXECUTING:
      errorMessage += "SQL still executing";
      break;
    case SQL_ERROR:
      errorMessage += "SQL error";
      break;
    case SQL_INVALID_HANDLE:
      errorMessage += "SQL invalid handle";
      break;
    case SQL_NO_DATA:
      throw SimpleDB::Database::NoDataException();
      break;
    default:
      errorMessage += "Unknown error";
    }
    throw Exception(errorMessage + errorInfo);
  }

  void Query::execute(const std::string& sqlQuery) {
    if(pimplData->databasePimplData->rollingBack)
      throw Exception("Cannot execute query while rolling back a transaction");

    /*
     * Close any open cursors before executing this query
     */
    SQLCloseCursor(pimplData->statementHandle);

    long return_value;
    return_value=SQLExecDirectA(pimplData->statementHandle,
    		reinterpret_cast<SQLCHAR *>(const_cast<char *>(sqlQuery.c_str())),
			       SQL_NTS);
    if ((return_value != SQL_SUCCESS) &&
	(return_value != SQL_SUCCESS_WITH_INFO)){
      throwError(static_cast<SQLRETURN>(return_value), "Error ExecDirect: ", pimplData->statementHandle);
    }
  }

  bool Query::fetchRow() {
    const SQLRETURN retVal(SQLFetch(pimplData->statementHandle));
    if(SQL_NO_DATA == retVal)
      return false;
    else if((SQL_SUCCESS == retVal) || (SQL_SUCCESS_WITH_INFO == retVal))
    {
      return true;
    }
    else
    {
      throwError(retVal, "Error Fetch: ", pimplData->statementHandle);
      return false;
    }
  }
}