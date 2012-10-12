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
#include "Database.h"
#include "DatabasePimplData_i.h"
#include "QueryPimplData_i.h"
#include "Column.h"

namespace SimpleDB {
  Database::Database(const std::string& dsn, const ConnectionMethod connectionMethod) : pimplData(new DatabasePimplData) {
    pimplData->numQueries = 0;
    pimplData->transactionCount = 0;
    pimplData->rollingBack = false;

    SQLRETURN return_value;
    return_value = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,
				  &(pimplData->odbc_environment_handle));
    std::string s;
    if ((return_value != SQL_SUCCESS) &&
	(return_value != SQL_SUCCESS_WITH_INFO))
      throw Exception(std::string("Error AllocHandle"));

    return_value = SQLSetEnvAttr(pimplData->odbc_environment_handle,
				 SQL_ATTR_ODBC_VERSION, reinterpret_cast<void*>(SQL_OV_ODBC3), 0);
    if ((return_value != SQL_SUCCESS) &&
	(return_value != SQL_SUCCESS_WITH_INFO))
      throw Exception(std::string("Error SetEnv"));

    return_value = SQLAllocHandle(SQL_HANDLE_DBC,
				  pimplData->odbc_environment_handle,
				  &(pimplData->connectionHandle));
    if ((return_value != SQL_SUCCESS) &&
	(return_value != SQL_SUCCESS_WITH_INFO)) {
      SQLFreeHandle(SQL_HANDLE_ENV, pimplData->odbc_environment_handle);
      throw Exception(std::string("Error AllocHDB"));
    }

    SQLSetConnectAttr(pimplData->connectionHandle, SQL_LOGIN_TIMEOUT,
    		reinterpret_cast<SQLPOINTER *>(5), 0);

    switch(connectionMethod)
    {
    case CONNECTION_METHOD_SQLConnect:
      return_value = SQLConnectA(pimplData->connectionHandle,
                  reinterpret_cast<SQLCHAR*>(const_cast<char*>(dsn.c_str())), SQL_NTS,
                                NULL, SQL_NTS,
                                NULL, SQL_NTS);
      break;
    case CONNECTION_METHOD_SQLDriverConnect:
      {
        SQLCHAR outConnectionString[1024];
        SQLSMALLINT outstrlen = 0;
        return_value = SQLDriverConnectA(
            pimplData->connectionHandle,// connection handle
            NULL,                       // window handle
            reinterpret_cast<SQLCHAR*>(const_cast<char*>(dsn.c_str())),
            static_cast<SQLSMALLINT>(dsn.length()),
            outConnectionString,
            sizeof(outConnectionString),
            &outstrlen,
            SQL_DRIVER_NOPROMPT);
      }
      break;
    default:
      throw Exception("Invalid connection method");
    }

    if ((return_value != SQL_SUCCESS) &&
	(return_value != SQL_SUCCESS_WITH_INFO)) {
      std::string errorString = "Error SQLConnect: ";
      SQLCHAR sqlState[6];
      SQLINTEGER errorCode;
      SQLSMALLINT messageLength;
      SQLCHAR errorMessage[400];
      long errValue = SQLGetDiagRecA(SQL_HANDLE_DBC, pimplData->connectionHandle,1,
    		  reinterpret_cast<SQLCHAR *>(sqlState),&errorCode,
    		  reinterpret_cast<SQLCHAR *>(errorMessage),
				    400,&messageLength);
      if(errValue == SQL_SUCCESS) {
	errorString += reinterpret_cast<char *>(errorMessage);
      } else if (errValue == SQL_SUCCESS_WITH_INFO) {
	std::cerr << "Error message too long at " << messageLength <<
	  " characters" <<  std::endl;
      } else if (errValue == SQL_ERROR) {
	errorString += "RecNumber was negative or 0 or BufferLength was less tha 0";
      } else if (errValue == SQL_NO_DATA) {
	errorString += "SQL no data";
      }

      SQLFreeHandle(SQL_HANDLE_DBC, pimplData->connectionHandle);
      SQLFreeHandle(SQL_HANDLE_ENV, pimplData->odbc_environment_handle);
      throw Exception(errorString);
    }
  }

  Query Database::newQuery () {
    Query tmp(*this);
    return tmp;
  }

  void Database::voidQuery (const std::string& query) {
    try {
      SimpleDB::Query queryWrapper(*this);
      queryWrapper.execute(query);
    } catch(SimpleDB::Query::Exception &s)  {
      throw (SimpleDB::Database::Exception (std::string(s.what()))) ;
    }
    catch(SimpleDB::Column::UnboundException)  {
      throw (SimpleDB::Database::Exception
	     (std::string("A Column was not bound"))) ;
    }
  }

  long Database::intQuery (const std::string& query) {
    try {
      SimpleDB::Query queryWrapper(*this);
      SimpleDB::IntColumn intCol;
      SimpleDB::Column * columns[1] = {&intCol};
      queryWrapper.bind(columns, 1);
      queryWrapper.execute(query);
      if(!queryWrapper.fetchRow())
	throw SimpleDB::Database::NoDataException();
      return intCol.value() ;
    } catch(SimpleDB::Query::Exception &s)  {
      throw SimpleDB::Database::Exception (std::string(s.what()));
    } catch(SimpleDB::Column::UnboundException)  {
      throw SimpleDB::Database::Exception (std::string("A Column was not bound"));
    }
  }

  std::string Database::strQuery (const std::string& query, long bufSize) {
    try {
      SimpleDB::Query queryWrapper(*this);
      SimpleDB::StringColumn col(bufSize);
      SimpleDB::Column * columns[1] = {&col};
      queryWrapper.bind(columns, sizeof(columns)/sizeof(columns[0]));
      queryWrapper.execute(query);
      queryWrapper.fetchRow();
      return col.value() ;
    }
    catch(SimpleDB::Query::Exception &s)  {
      throw (SimpleDB::Database::Exception (std::string(s.what()))) ;
    }
    catch(SimpleDB::Column::UnboundException)  {
      throw (SimpleDB::Database::Exception (std::string("A Column was not bound"))) ;
    }
    return "" ;
  }

  std::string Database::escapeString(const std::string& input) {
    std::string output = "";
    char c;
    for (unsigned int i = 0; i < input.length(); i++) {
      c = input.at(i) ;
      switch (c) {
      case '\'' :
	output += "''";
	break;
      case '\\' :
	output += "\\\\";
	break ;
      default:
	output += c;
	break ;
      }
    }
    return output;
  }

  std::string  Database::boolToStr (const bool in) {
    std::string rtStr = "FALSE" ;
    if (in)
      rtStr = "TRUE" ;
    return rtStr ;
  }

  DatabasePimplData& Database::getPimplData() {
    return *pimplData;
  }

  int Database::getTransactionCount() const {
    return pimplData->transactionCount;
  }

  Database::~Database() {
    /*
     * Should assert here if there are any queries or transactions still in scope
     */

    SQLDisconnect(pimplData->connectionHandle);
    SQLFreeHandle(SQL_HANDLE_DBC, pimplData->connectionHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, pimplData->odbc_environment_handle);
  }
}