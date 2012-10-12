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

#ifndef _SIMPLEDB_COLUMN_H_
#define _SIMPLEDB_COLUMN_H_

#include "macros.h"
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include "Exception.h" // for Database::Exception

namespace SimpleDB {
  class Database;
  struct QueryPimplData;

  /** The abstract base class for the SimpleDB types.
   * This type contains the actual data as well as meta-data.
   */
  class Column {
    /** A structure containing the pimpl data
     */
    struct PimplData;

    /** Query is a friend so it can call the doBind function, but so that
     * the doBind function can be hidden from outside (for encapsulation)
     */
    friend class Query;

  public:

    /** Exception class.
     * Thrown when a column's value is requested but the column hasn't been
     * bound
     */
    class UnboundException : public Exception {
    public:

      /** Constructor used when creating a new UnboundException object.
       */
      UnboundException() : Exception("Column is not bound.") {};
    };

    /** Tests if the current value is NULL
     * \exception UnboundException if the column hasn't been bound
     * @return True if the value is NULL.
     */
    virtual bool isNull() const;

    /** Function called to write a column value to the output stream.
     * This function is implemented by each column type to write the value
     * in a suitable form to the output stream.
     *
     * @param oStream Output stream object.
     * @return Output stream object to which data was written.
     */
    virtual std::ostream& oStream(std::ostream& oStream) = 0;

    /** A virtual destructor that can be overridden if required.
     * This can be used, for example, to deallocate memory.
     */
    virtual ~Column();

    /** Constructor used to set initial value of columnBound variable.
     */
    Column();

    /** This method is to be called by all derived classes to make sure
     * the column was bound. It will cause an exception to be thrown if the
     * column wasn't bound.
     */
	void val();

  protected:

    /** Used for children to get the pimplData
     * @return The pimpl data
     */
    PimplData& getPimplData();

    /** This abstract function is called by Query when the Query.bind function
     * is invoked.
     * This function has an implementation that handles making sure columns are
     * bound. This function should be called by any derived objects.
     *
     * @param queryPimplData The implementation data from the query object
     * @param columnNumber The column number to bind this column to..
     */
    virtual void doBind(QueryPimplData& queryPimplData, int columnNumber) = 0;

  private:
    /** A structure containing the pimpl data
     */
    std::auto_ptr<PimplData> pimplData;
  };

  /** Class representing string values.
   */
  class StringColumn : public Column {
  public:

    /** Constructor to allocate memory for this column.
     * This function will throw an Exception if the memory cannot be allocated.
     *
     * @param bufferSize The maximum number of bytes the value will hold.
     */
    StringColumn(const int bufferSize);

    /** Destructor frees allocated memory.
     */
    ~StringColumn();

    std::ostream& oStream(std::ostream& oStream);

    /** Returns the string value of this object.
     *
     * @return The string value.
     */
    std::string value();

  protected:

    /** \see Column::doBind()
     */
    virtual void doBind(QueryPimplData& queryPimplData, int columnNumber);

  private:
    std::vector<char> buffer;           // Use the vector as a dynamic array
  };

  /** Class representing SQL big integers
   */
  class BigintColumn : public Column {
  public:
    std::ostream& oStream(std::ostream& oStream);

    /** Used to retrieve the value of this column.
     * @return The value of this column.
     */
    int64_t value();
  protected:

    virtual void doBind(QueryPimplData& queryPimplData, int columnNumber);

  private:

    /** The column value
     */
    int64_t colValue;
  };

  /** Class representing SQL integers
   */
  class IntColumn : public Column {
  public:

    /** Constructor
     */
    IntColumn();

    std::ostream& oStream(std::ostream& oStream);

    /** Used to retrieve the value of this column.
     * @return The value of this column.
     */
    int32_t value();
  protected:

    virtual void doBind(QueryPimplData& queryPimplData, int columnNumber);

  private:

    /** The column value
     */
    int32_t colValue;
  };

  /** Class representing long values.
   * \deprecated Use IntColumn instead
   */
  class LongColumn : public Column {
  public:
    std::ostream& oStream(std::ostream& oStream);

    /** Used to retrieve the value of this column.
     *
     * @return The long value of this column.
     */
    long value();
  protected:

    virtual void doBind(QueryPimplData& queryPimplData, int columnNumber);

  private:
    /** The long column value
     */
    long colValue;
  };

  /** Class representing double precision vales
   */
  class DoubleColumn : public Column {
  public:
    std::ostream& oStream(std::ostream& oStream);

    /** Used to retrieve the value of this column
     * @return The double values of this column
     */
    double value();
  protected:

    virtual void doBind(QueryPimplData& queryPimplData, int columnNumber);

  private:

    /** The column value
     */
    double colValue;
  };

  /** Class representing bool values.
   */
  class BoolColumn : public Column {
  public:
    std::ostream& oStream(std::ostream& oStream);

    /** Used to retrieve the value of this column.
     *
     * @return The long value of this column.
     */
    bool value();

  protected:

    void doBind(QueryPimplData& queryPimplData,
		int columnNumber);

  private:

    /** The boolean column value
     */
    unsigned char colValue;
  };
}

inline
std::ostream& operator<<(std::ostream& strm, SimpleDB::Column& col) {
  return col.oStream(strm);
}

#endif