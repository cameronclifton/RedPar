/*------------------------------------------------------------------------
  Junction: Concurrent data structures in C++
  Copyright (c) 2016 Jeff Preshing

  Distributed under the Simplified BSD License.
  Original location: https://github.com/preshing/junction

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the LICENSE file for more information.
------------------------------------------------------------------------*/

#include <junction/Core.h>
#include <junction/details/Leapfrog.h>
#include <turf/Heap.h>

namespace junction {
namespace details {

TURF_TRACE_DEFINE_BEGIN(Leapfrog, 33) // autogenerated by TidySource.py
TURF_TRACE_DEFINE("[find] called")
TURF_TRACE_DEFINE("[find] found existing cell optimistically")
TURF_TRACE_DEFINE("[find] found existing cell")
TURF_TRACE_DEFINE("[insertOrFind] called")
TURF_TRACE_DEFINE("[insertOrFind] reserved first cell")
TURF_TRACE_DEFINE("[insertOrFind] race to reserve first cell")
TURF_TRACE_DEFINE("[insertOrFind] found in first cell")
TURF_TRACE_DEFINE("[insertOrFind] race to read hash")
TURF_TRACE_DEFINE("[insertOrFind] found in probe chain")
TURF_TRACE_DEFINE("[insertOrFind] reserved cell")
TURF_TRACE_DEFINE("[insertOrFind] race to reserve cell")
TURF_TRACE_DEFINE("[insertOrFind] found outside probe chain")
TURF_TRACE_DEFINE("[insertOrFind] found late-arriving cell in same bucket")
TURF_TRACE_DEFINE("[insertOrFind] set link on behalf of late-arriving cell")
TURF_TRACE_DEFINE("[insertOrFind] overflow")
TURF_TRACE_DEFINE("[beginTableMigrationToSize] called")
TURF_TRACE_DEFINE("[beginTableMigrationToSize] new migration already exists")
TURF_TRACE_DEFINE("[beginTableMigrationToSize] new migration already exists (double-checked)")
TURF_TRACE_DEFINE("[beginTableMigration] redirected while determining table size")
TURF_TRACE_DEFINE("[migrateRange] empty cell already redirected")
TURF_TRACE_DEFINE("[migrateRange] race to insert key")
TURF_TRACE_DEFINE("[migrateRange] race to insert value")
TURF_TRACE_DEFINE("[migrateRange] race inserted Redirect")
TURF_TRACE_DEFINE("[migrateRange] in-use cell already redirected")
TURF_TRACE_DEFINE("[migrateRange] racing update was erase")
TURF_TRACE_DEFINE("[migrateRange] race to update migrated value")
TURF_TRACE_DEFINE("[TableMigration::run] already ended")
TURF_TRACE_DEFINE("[TableMigration::run] detected end flag set")
TURF_TRACE_DEFINE("[TableMigration::run] destination overflow")
TURF_TRACE_DEFINE("[TableMigration::run] race to set m_overflowed")
TURF_TRACE_DEFINE("[TableMigration::run] out of migration units")
TURF_TRACE_DEFINE("[TableMigration::run] not the last worker")
TURF_TRACE_DEFINE("[TableMigration::run] a new TableMigration was already started")
TURF_TRACE_DEFINE_END(Leapfrog, 33)

} // namespace details
} // namespace junction
