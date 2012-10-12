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

#ifndef _SIMPLEDB_SCOPEDTRANSACTION_H_
#define _SIMPLEDB_SCOPEDTRANSACTION_H_


namespace SimpleDB {
  class Database;

  struct DatabasePimplData;

  /** ScopedTransaction class
   * Create a scoped transaction to make a block of code into a transaction
   * If this object destructs before the commit function is called, the
   * transaction will be rolled back. Otherwise, the transaction will be
   * committed. This makes it easy to create exception safe code.
   * You can nest ScopedTransaction as a count is kept on each database
   * connection so that a transations are only committed or rolled back
   * by the outermost transaction object.
   *
   * If a nested transaction is rolled back, no further queries can take
   * place until all of the nesting transactions have gone out of scope.
   *
   * Please note that "applications should not use Transact-SQL transaction
   * statements such as BEGIN TRANSACTION, COMMIT TRANSACTION, or ROLLBACK
   * TRANSACTION because this can cause indeterminate behavior in the driver"
   * http://msdn2.microsoft.com/en-us/library/ms131281(SQL.90).aspx
   *
   * I.e., use this object ;)
   */
  class ScopedTransaction {
  public:

    /** Begin a transaction on the given database connection
     * @param database The database connection
     */
    ScopedTransaction(Database& database);

    /** Commit the transaction
     */
    virtual void commit();

    /** Rollback the transaction
     */
    virtual void rollback();

    /** Virtual destructor
     * This will rollback the transaction if the commit function
     * was not called on the transaction
     */
    virtual ~ScopedTransaction();

  private:

    bool _commit;
    DatabasePimplData& databasePimplData;
  };
}
#endif