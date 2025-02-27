/*
   Copyright 2010 Sun Microsystems, Inc.
   All rights reserved. Use is subject to license terms.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

package testsuite.clusterj;

import testsuite.clusterj.model.TimeAsUtilDateTypes;
import java.util.Date;

import testsuite.clusterj.model.IdBase;

public class QueryTimeAsUtilDateTypesTest extends AbstractQueryTest {

    @Override
    public Class<TimeAsUtilDateTypes> getInstanceType() {
        return TimeAsUtilDateTypes.class;
    }

    @Override
    void createInstances(int number) {
        createAllTimeAsUtilDateTypesInstances(number);
    }

    /** Test all single- and double-predicate queries using TimeTypes.
drop table if exists timetypes;
create table timetypes (
 id int not null primary key,

 time_null_hash time,
 time_null_btree time,
 time_null_both time,
 time_null_none time,

 time_not_null_hash time,
 time_not_null_btree time,
 time_not_null_both time,
 time_not_null_none time

) ENGINE=ndbcluster DEFAULT CHARSET=latin1;

create unique index idx_time_null_hash using hash on timetypes(time_null_hash);
create index idx_time_null_btree on timetypes(time_null_btree);
create unique index idx_time_null_both on timetypes(time_null_both);

create unique index idx_time_not_null_hash using hash on timetypes(time_not_null_hash);
create index idx_time_not_null_btree on timetypes(time_not_null_btree);
create unique index idx_time_not_null_both on timetypes(time_not_null_both);

     */

    public void test() {
        btreeIndexScanTime();
        hashIndexScanTime();
        bothIndexScanTime();
        noneIndexScanTime();
        failOnError();
    }

    public void btreeIndexScanTime() {
        equalQuery("time_not_null_btree", "idx_time_not_null_btree", getTimeFor(8), 8);
        greaterEqualQuery("time_not_null_btree", "idx_time_not_null_btree", getTimeFor(7), 7, 8, 9);
        greaterThanQuery("time_not_null_btree", "idx_time_not_null_btree", getTimeFor(6), 7, 8, 9);
        lessEqualQuery("time_not_null_btree", "idx_time_not_null_btree", getTimeFor(4), 4, 3, 2, 1, 0);
        lessThanQuery("time_not_null_btree", "idx_time_not_null_btree", getTimeFor(4), 3, 2, 1, 0);
        betweenQuery("time_not_null_btree", "idx_time_not_null_btree", getTimeFor(4), getTimeFor(6), 4, 5, 6);
        greaterEqualAndLessEqualQuery("time_not_null_btree", "idx_time_not_null_btree", getTimeFor(4), getTimeFor(6), 4, 5, 6);
        greaterThanAndLessEqualQuery("time_not_null_btree", "idx_time_not_null_btree", getTimeFor(4), getTimeFor(6), 5, 6);
        greaterEqualAndLessThanQuery("time_not_null_btree", "idx_time_not_null_btree", getTimeFor(4), getTimeFor(6), 4, 5);
        greaterThanAndLessThanQuery("time_not_null_btree", "idx_time_not_null_btree", getTimeFor(4), getTimeFor(6), 5);
    }

    public void hashIndexScanTime() {
        equalQuery("time_not_null_hash", "idx_time_not_null_hash", getTimeFor(8), 8);
        greaterEqualQuery("time_not_null_hash", "none", getTimeFor(7), 7, 8, 9);
        greaterThanQuery("time_not_null_hash", "none", getTimeFor(6), 7, 8, 9);
        lessEqualQuery("time_not_null_hash", "none", getTimeFor(4), 4, 3, 2, 1, 0);
        lessThanQuery("time_not_null_hash", "none", getTimeFor(4), 3, 2, 1, 0);
        betweenQuery("time_not_null_hash", "none", getTimeFor(4), getTimeFor(6), 4, 5, 6);
        greaterEqualAndLessEqualQuery("time_not_null_hash", "none", getTimeFor(4), getTimeFor(6), 4, 5, 6);
        greaterThanAndLessEqualQuery("time_not_null_hash", "none", getTimeFor(4), getTimeFor(6), 5, 6);
        greaterEqualAndLessThanQuery("time_not_null_hash", "none", getTimeFor(4), getTimeFor(6), 4, 5);
        greaterThanAndLessThanQuery("time_not_null_hash", "none", getTimeFor(4), getTimeFor(6), 5);
    }

    public void bothIndexScanTime() {
        equalQuery("time_not_null_both", "idx_time_not_null_both", getTimeFor(8), 8);
        greaterEqualQuery("time_not_null_both", "idx_time_not_null_both", getTimeFor(7), 7, 8, 9);
        greaterThanQuery("time_not_null_both", "idx_time_not_null_both", getTimeFor(6), 7, 8, 9);
        lessEqualQuery("time_not_null_both", "idx_time_not_null_both", getTimeFor(4), 4, 3, 2, 1, 0);
        lessThanQuery("time_not_null_both", "idx_time_not_null_both", getTimeFor(4), 3, 2, 1, 0);
        betweenQuery("time_not_null_both", "idx_time_not_null_both", getTimeFor(4), getTimeFor(6), 4, 5, 6);
        greaterEqualAndLessEqualQuery("time_not_null_both", "idx_time_not_null_both", getTimeFor(4), getTimeFor(6), 4, 5, 6);
        greaterThanAndLessEqualQuery("time_not_null_both", "idx_time_not_null_both", getTimeFor(4), getTimeFor(6), 5, 6);
        greaterEqualAndLessThanQuery("time_not_null_both", "idx_time_not_null_both", getTimeFor(4), getTimeFor(6), 4, 5);
        greaterThanAndLessThanQuery("time_not_null_both", "idx_time_not_null_both", getTimeFor(4), getTimeFor(6), 5);
    }

    public void noneIndexScanTime() {
        equalQuery("time_not_null_none", "none", getTimeFor(8), 8);
        greaterEqualQuery("time_not_null_none", "none", getTimeFor(7), 7, 8, 9);
        greaterThanQuery("time_not_null_none", "none", getTimeFor(6), 7, 8, 9);
        lessEqualQuery("time_not_null_none", "none", getTimeFor(4), 4, 3, 2, 1, 0);
        lessThanQuery("time_not_null_none", "none", getTimeFor(4), 3, 2, 1, 0);
        betweenQuery("time_not_null_none", "none", getTimeFor(4), getTimeFor(6), 4, 5, 6);
        greaterEqualAndLessEqualQuery("time_not_null_none", "none", getTimeFor(4), getTimeFor(6), 4, 5, 6);
        greaterThanAndLessEqualQuery("time_not_null_none", "none", getTimeFor(4), getTimeFor(6), 5, 6);
        greaterEqualAndLessThanQuery("time_not_null_none", "none", getTimeFor(4), getTimeFor(6), 4, 5);
        greaterThanAndLessThanQuery("time_not_null_none", "none", getTimeFor(4), getTimeFor(6), 5);
    }


    private void createAllTimeAsUtilDateTypesInstances(int number) {
        for (int i = 0; i < number; ++i) {
            TimeAsUtilDateTypes instance = session.newInstance(TimeAsUtilDateTypes.class);
            instance.setId(i);
            instance.setTime_not_null_hash(getTimeFor(i));
            instance.setTime_not_null_btree(getTimeFor(i));
            instance.setTime_not_null_both(getTimeFor(i));
            instance.setTime_not_null_none(getTimeFor(i));
            instances.add(instance);
        }
    }

    protected Date getTimeFor(int i) {
        return new Date(getMillisFor(0, 0, i, 0));
    }

    public static String toString(IdBase instance) {
        TimeAsUtilDateTypes timetype = (TimeAsUtilDateTypes)instance;
        StringBuffer buffer = new StringBuffer("TimeTypes id: ");
        buffer.append(timetype.getId());
        buffer.append("; time_not_null_both: ");
        buffer.append(timetype.getTime_not_null_both().toString());
        buffer.append("; time_not_null_btree: ");
        buffer.append(timetype.getTime_not_null_btree().toString());
        buffer.append("; time_not_null_hash: ");
        buffer.append(timetype.getTime_not_null_hash().toString());
        buffer.append("; time_not_null_none: ");
        buffer.append(timetype.getTime_not_null_none().toString());
        return buffer.toString();
    }
}
 