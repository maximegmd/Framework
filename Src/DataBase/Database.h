/* -*- c++ -*-
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

#ifndef _SIMPLEDB_DATABASE_H_
#define _SIMPLEDB_DATABASE_H_

#include <string>
#include <iostream>
#include "Exception.h"
#include <memory>
#include "Query.h"


namespace SimpleDB {
  /** The structure type that will contain the pimpl data
   */
  struct DatabasePimplData;

  /** This class represents a database connection.
   *
   * \todo: Overload the default assignment operator so that
   *        database objects can be assigned without causing
   *        problems with connection handles
   */
  class Database {
    friend class Query;
    friend class ScopedTransaction;

  public:

    /** Used to specify the connection method in the constructor
     */
    enum ConnectionMethod {
      /** Use SQLConnect
       */
      CONNECTION_METHOD_SQLConnect = 0,

      /** Use SQLDriverConnect
       */
      CONNECTION_METHOD_SQLDriverConnect
    };

    /** This class is thrown in case of any database exception.
     * A description (if available) is included.
     */
    class Exception : public SimpleDB::Exception {
    public:
      Exception(const std::string& message) : SimpleDB::Exception(message) {};
    };

    /** Exception class thrown when no data is available.
     * Thrown by intQuery function.
     */
    class NoDataException : public Exception {
    public:

      /** No parameters are passed to the constructor.
       * A default error message is included.
       */
      NoDataException() : Exception("No data returned.") {
      }
    };

    /** Constructor creates the database connection.
     *
     * @param dsn The ODBC data source to connect to.
     * @param connectionMethod Controls the SQL connection function to call
     */
    Database(const std::string& dsn, const ConnectionMethod connectionMethod = CONNECTION_METHOD_SQLConnect);

    /** Creates a query object for this database.
     * \deprecated use the Query constructor directly instead
     * @return Query The new query object.
     */
    DEPRECATED(Query newQuery());

    /** void Query, query the database and don't worry about return types or
     * if its a successful query.
     * @param query The query to execute.
     */
    void voidQuery (const std::string& query);

    /** long Query, query the database for a single integer result.
     *
     * \exception SimpleDB::Database::Exception Thrown if error with syntax
     *            etc.
     * \exception SimpleDB::Database::NoDataException Thrown if now rows
     *            are returned by the query.
     *
     * @param query the query to execute
     * @return the integer result from the query
     */
    long intQuery (const std::string& query);

    /** A Query interface that returns a string result
     * @param query The query to execute.
     * @param bufSize The max size of the string buffer expected to be
     *        returned.
     * @return resultant string of the query.
     */
    std::string strQuery (const std::string& query, long bufSize = 1000);

    /** Escapes special characters in a string to be sent as
     * part of a database query.
     *
     * @param input The input string to be escaped.
     * @return The escaped version of the input string.
     */
    static std::string escapeString(const std::string& input);

    /** Converts a boolean input to the equivalent database value.
     * @param in The boolean to convert
     * @return the converted string.
     */
    static std::string  boolToStr (const bool in);

    /** Get the number of nested transactions
     */
    int getTransactionCount() const;

    /** Destructor
     *
     */
    virtual ~Database() ;

  protected:

    /** Get the reference to the private implementation data
     */
    DatabasePimplData& getPimplData();

  private:

    std::auto_ptr<DatabasePimplData> pimplData;

    /** Don't allow copying or assignment of database objects
     */
    Database(const Database& that);
    const Database& operator=(const Database& that);
  };
}

inline
std::ostream& operator<<(std::ostream& strm, const std::exception &e) {
  strm << "Database::Exception: " << e.what();
  return strm;
}

#endif