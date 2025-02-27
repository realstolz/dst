# Copyright (c) 2008, 2016, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/storage/ndb/src/mgmapi)
INCLUDE(${CMAKE_SOURCE_DIR}/storage/ndb/config/type_kernel.cmake)

ADD_CONVENIENCE_LIBRARY(ndbkernel
    VMSignal.cpp
    Emulator.cpp
    Configuration.cpp
    WatchDog.cpp
    SectionReader.cpp
    SegmentList.cpp
    Mutex.cpp
    SafeCounter.cpp
    Rope.cpp
    ndbd_malloc_impl.cpp
    ndbd_malloc.cpp
    Pool.cpp
    DynArr256.cpp
    LockQueue.cpp
    GlobalData.cpp
    SafeMutex.cpp
    Ndbinfo.cpp
    NdbinfoTables.cpp
    ArenaPool.cpp
    mt_thr_config.cpp
)
#    WOPool.cpp
#    RWPool.cpp

ADD_CONVENIENCE_LIBRARY(ndbsched
    TimeQueue.cpp
    ThreadConfig.cpp
    FastScheduler.cpp
    TransporterCallback_nonmt.cpp
    SimulatedBlock_nonmt.cpp
    LongSignal_nonmt.cpp
    SimplePropertiesSection_nonmt.cpp
    dummy_nonmt.cpp
)

IF(WITH_NDBMTD)
  ADD_CONVENIENCE_LIBRARY(ndbsched_mt
    SimulatedBlock_mt.cpp
    TransporterCallback_mt.cpp
    LongSignal_mt.cpp
    SimplePropertiesSection_mt.cpp
    mt.cpp
    dummy_mt.cpp)
ENDIF()

ADD_CONVENIENCE_LIBRARY(ndbtest
    test_context.cpp
    ../blocks/dbtup/tuppage.cpp
)

NDB_ADD_TEST(mt_thr_config-t mt_thr_config.cpp LIBS ndbgeneral)

FOREACH(testprog CountingPool DynArr256 IntrusiveList LHLevel SegmentList)
  NDB_ADD_TEST("${testprog}-t" "${testprog}.cpp" LIBS ndbtest ndbkernel ndbsched ndberror ndbtransport ndbmgmcommon ndbmgmapi ndbportlib ndbgeneral)
ENDFOREACH(testprog)

# Todo : Fix
#FOREACH(testprog SegmentList)
#  ADD_EXECUTABLE(${testprog}-t ${testprog}.cpp)
#  SET_TARGET_PROPERTIES(${testprog}-t PROPERTIES COMPILE_FLAGS "-DTAP_TEST")
#  TARGET_LINK_LIBRARIES(${testprog}-t ndbtest ndbkernel ndbsched ndberror
#                ndbtransport ndbmgmcommon ndbmgmapi ndbportlib ndbgeneral)
#ENDFOREACH(testprog)

IF(WITH_NDBMTD)
  NDB_ADD_TEST(mt-send-t mt-send-t.cpp LIBS ndbtest ndbkernel ndbsched ndberror ndbtransport ndbmgmcommon ndbmgmapi ndbportlib ndbgeneral)
ENDIF()
 