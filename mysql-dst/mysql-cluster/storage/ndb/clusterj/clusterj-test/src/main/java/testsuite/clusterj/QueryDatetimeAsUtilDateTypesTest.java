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

import testsuite.clusterj.model.DatetimeAsUtilDateTypes;
import java.util.Date;

import testsuite.clusterj.model.IdBase;

public class QueryDatetimeAsUtilDateTypesTest extends AbstractQueryTest {

    @Override
    public Class<DatetimeAsUtilDateTypes> getInstanceType() {
        return DatetimeAsUtilDateTypes.class;
    }

    @Override
    void createInstances(int number) {
        createAllDateTimeTypesInstances(number);
    }

    /** Test all single- and double-predicate queries using DateTimeTypes.
drop table if exists datetimetypes;
create table datetimetypes (
 id int not null primary key,

 datetime_null_hash datetime,
 datetime_null_btree datetime,
 datetime_null_both datetime,
 datetime_null_none datetime,

 datetime_not_null_hash datetime,
 datetime_not_null_btree datetime,
 datetime_not_null_both datetime,
 datetime_not_null_none datetime

) ENGINE=ndbcluster DEFAULT CHARSET=latin1;

create unique index idx_datetime_null_hash using hash on datetimetypes(datetime_null_hash);
create index idx_datetime_null_btree on datetimetypes(datetime_null_btree);
create unique index idx_datetime_null_both on datetimetypes(datetime_null_both);

create unique index idx_datetime_not_null_hash using hash on datetimetypes(datetime_not_null_hash);
create index idx_datetime_not_null_btree on datetimetypes(datetime_not_null_btree);
create unique index idx_datetime_not_null_both on datetimetypes(datetime_not_null_both);

     */

    public void test() {
        btreeIndexScanDateTime();
        hashIndexScanDateTime();
        bothIndexScanDateTime();
        noneIndexScanDateTime();
        failOnError();
    }

    public void btreeIndexScanDateTime() {
        equalQuery("datetime_not_null_btree", "idx_datetime_not_null_btree", getDateTimeFor(8), 8);
        greaterEqualQuery("datetime_not_null_btree", "idx_datetime_not_null_btree", getDateTimeFor(7), 7, 8, 9);
        greaterThanQuery("datetime_not_null_btree", "idx_datetime_not_null_btree", getDateTimeFor(6), 7, 8, 9);
        lessEqualQuery("datetime_not_null_btree", "idx_datetime_not_null_btree", getDateTimeFor(4), 4, 3, 2, 1, 0);
        lessThanQuery("datetime_not_null_btree", "idx_datetime_not_null_btree", getDateTimeFor(4), 3, 2, 1, 0);
        betweenQuery("datetime_not_null_btree", "idx_datetime_not_null_btree", getDateTimeFor(4), getDateTimeFor(6), 4, 5, 6);
        greaterEqualAndLessEqualQuery("datetime_not_null_btree", "idx_datetime_not_null_btree", getDateTimeFor(4), getDateTimeFor(6), 4, 5, 6);
        greaterThanAndLessEqualQuery("datetime_not_null_btree", "idx_datetime_not_null_btree", getDateTimeFor(4), getDateTimeFor(6), 5, 6);
        greaterEqualAndLessThanQuery("datetime_not_null_btree", "idx_datetime_not_null_btree", getDateTimeFor(4), getDateTimeFor(6), 4, 5);
        greaterThanAndLessThanQuery("datetime_not_null_btree", "idx_datetime_not_null_btree", getDateTimeFor(4), getDateTimeFor(6), 5);
    }

    public void hashIndexScanDateTime() {
        equalQuery("datetime_not_null_hash", "idx_datetime_not_null_hash", getDateTimeFor(8), 8);
        greaterEqualQuery("datetime_not_null_hash", "none", getDateTimeFor(7), 7, 8, 9);
        greaterThanQuery("datetime_not_null_hash", "none", getDateTimeFor(6), 7, 8, 9);
        lessEqualQuery("datetime_not_null_hash", "none", getDateTimeFor(4), 4, 3, 2, 1, 0);
        lessThanQuery("datetime_not_null_hash", "none", getDateTimeFor(4), 3, 2, 1, 0);
        betweenQuery("datetime_not_null_hash", "none", getDateTimeFor(4), getDateTimeFor(6), 4, 5, 6);
        greaterEqualAndLessEqualQuery("datetime_not_null_hash", "none", getDateTimeFor(4), getDateTimeFor(6), 4, 5, 6);
        greaterThanAndLessEqualQuery("datetime_not_null_hash", "none", getDateTimeFor(4), getDateTimeFor(6), 5, 6);
        greaterEqualAndLessThanQuery("datetime_not_null_hash", "none", getDateTimeFor(4), getDateTimeFor(6), 4, 5);
        greaterThanAndLessThanQuery("datetime_not_null_hash", "none", getDateTimeFor(4), getDateTimeFor(6), 5);
    }

    public void bothIndexScanDateTime() {
        equalQuery("datetime_not_null_both", "idx_datetime_not_null_both", getDateTimeFor(8), 8);
        greaterEqualQuery("datetime_not_null_both", "idx_datetime_not_null_both", getDateTimeFor(7), 7, 8, 9);
        greaterThanQuery("datetime_not_null_both", "idx_datetime_not_null_both", getDateTimeFor(6), 7, 8, 9);
        lessEqualQuery("datetime_not_null_both", "idx_datetime_not_null_both", getDateTimeFor(4), 4, 3, 2, 1, 0);
        lessThanQuery("datetime_not_null_both", "idx_datetime_not_null_both", getDateTimeFor(4), 3, 2, 1, 0);
        betweenQuery("datetime_not_null_both", "idx_datetime_not_null_both", getDateTimeFor(4), getDateTimeFor(6), 4, 5, 6);
        greaterEqualAndLessEqualQuery("datetime_not_null_both", "idx_datetime_not_null_both", getDateTimeFor(4), getDateTimeFor(6), 4, 5, 6);
        greaterThanAndLessEqualQuery("datetime_not_null_both", "idx_datetime_not_null_both", getDateTimeFor(4), getDateTimeFor(6), 5, 6);
        greaterEqualAndLessThanQuery("datetime_not_null_both", "idx_datetime_not_null_both", getDateTimeFor(4), getDateTimeFor(6), 4, 5);
        greaterThanAndLessThanQuery("datetime_not_null_both", "idx_datetime_not_null_both", getDateTimeFor(4), getDateTimeFor(6), 5);
    }

    public void noneIndexScanDateTime() {
        equalQuery("datetime_not_null_none", "none", getDateTimeFor(8), 8);
        greaterEqualQuery("datetime_not_null_none", "none", getDateTimeFor(7), 7, 8, 9);
        greaterThanQuery("datetime_not_null_none", "none", getDateTimeFor(6), 7, 8, 9);
        lessEqualQuery("datetime_not_null_none", "none", getDateTimeFor(4), 4, 3, 2, 1, 0);
        lessThanQuery("datetime_not_null_none", "none", getDateTimeFor(4), 3, 2, 1, 0);
        betweenQuery("datetime_not_null_none", "none", getDateTimeFor(4), getDateTimeFor(6), 4, 5, 6);
        greaterEqualAndLessEqualQuery("datetime_not_null_none", "none", getDateTimeFor(4), getDateTimeFor(6), 4, 5, 6);
        greaterThanAndLessEqualQuery("datetime_not_null_none", "none", getDateTimeFor(4), getDateTimeFor(6), 5, 6);
        greaterEqualAndLessThanQuery("datetime_not_null_none", "none", getDateTimeFor(4), getDateTimeFor(6), 4, 5);
        greaterThanAndLessThanQuery("datetime_not_null_none", "none", getDateTimeFor(4), getDateTimeFor(6), 5);
    }


    private void createAllDateTimeTypesInstances(int number) {
        for (int i = 0; i < number; ++i) {
            DatetimeAsUtilDateTypes instance = session.newInstance(DatetimeAsUtilDateTypes.class);
            instance.setId(i);
            instance.setDatetime_not_null_hash(getDateTimeFor(i));
            instance.setDatetime_not_null_btree(getDateTimeFor(i));
            instance.setDatetime_not_null_both(getDateTimeFor(i));
            instance.setDatetime_not_null_none(getDateTimeFor(i));
            instances.add(instance);
        }
    }

    protected Date getDateTimeFor(int i) {
        return new Date(getMillisFor(1980, 0, 1, 0, 0, i));
    }

    public static String toString(IdBase instance) {
        DatetimeAsUtilDateTypes timetype = (DatetimeAsUtilDateTypes)instance;
        StringBuffer buffer = new StringBuffer("DateTimeTypes id: ");
        buffer.append(timetype.getId());
        buffer.append("; datetime_not_null_both: ");
        buffer.append(timetype.getDatetime_not_null_both().toString());
        buffer.append("; datetime_not_null_btree: ");
        buffer.append(timetype.getDatetime_not_null_btree().toString());
        buffer.append("; datetime_not_null_hash: ");
        buffer.append(timetype.getDatetime_not_null_hash().toString());
        buffer.append("; datetime_not_null_none: ");
        buffer.append(timetype.getDatetime_not_null_none().toString());
        return buffer.toString();
    }
}
 