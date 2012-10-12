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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNUz
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

#ifndef _SIMPLEDB_DATABASE_PIMPLDATA_I_H_
#define _SIMPLEDB_DATABASE_PIMPLDATA_I_H_


namespace SimpleDB {
  struct DatabasePimplData {
    /** ODBC connection handle
     */
    SQLHDBC connectionHandle;

    /** ODBC environment handle.
     * This handle is created by the constructor and
     * freed in the destructor.
     */
    SQLHENV odbc_environment_handle;

    /** Keeps a track of the number of query objects
     * This variable is incremented/decremented by the actual query
     * objects.
     */
    int numQueries;

    /** Keeps track of the number of ScopedTransaction objects
     * to enable the nesting of transactions so that only the outermost
     * transaction takes effect
     */
    int transactionCount;

    /** Set to true when a nest of transactions should be rolled back.
     * Initialized to false when the datbase connection is instanciated.
     */
    bool rollingBack;
  };
}

#endif