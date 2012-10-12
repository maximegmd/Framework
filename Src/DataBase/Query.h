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

#ifndef _SIMPLEDB_QUERY_H_
#define _SIMPLEDB_QUERY_H_

#include <string>
#include <memory>                       // For auto_ptr
#include "Exception.h"
#include "macros.h"

namespace SimpleDB {
  class Column;
  class Database;
  struct DatabasePimplData;

  /** The structure type containing the pimpl data for the query object
   */
  struct QueryPimplData;

  /** Class to represent a database query.
   * This class is created by the Database object.
   * It encapsulates details pertaining to a database query.
   */
  class Query {
    friend class Database;

  public:

    /** Constructor called by the Database object.
     * @param database Create a new query on the given database.
     */
    Query(Database& database);

    /** The overloaded copy constructor that creates a fresh query object
     * It provides backwards compatability with some old code but
     * should not be used in future
     * \deprecated avoid copying query objects
     */
    DEPRECATED(Query(const Query& that));

    /** Query exception class.
     * Thrown whenever there is an error condition.
     */
    class Exception : public SimpleDB::Exception {
    public:
      Exception(const std::string& message) : SimpleDB::Exception(message) {};
    };

    /** Destructor deallocates any memory or resources.
     */
    ~Query();

    /** Bind the given columns to this query context.
     *
     * @param columns An array of columns to bind to this query.
     * @param numberColumns The number of columns in the array.
     */
    void bind(Column* columns[], int numberColumns);

    /** Execute the given query.
     *
     * @param sqlQuery The SQL query.
     * \exception Database::NoDataException if the return from the SQL
     * query is SQL_NO_DATA
     * \exception Exception When there is a problem with the database
     * or with the query itself.
     */
    void execute(const std::string& sqlQuery);

    /** Fetches a row of data returned by the SQL query.
     *
     * @return Returns false if there is no data to be returned.
     */
    bool fetchRow();

  private:

    std::auto_ptr<QueryPimplData> pimplData;

    /** You should need to assign queries
     */
    const Query& operator=(const Query& b);
  };
}

#endif