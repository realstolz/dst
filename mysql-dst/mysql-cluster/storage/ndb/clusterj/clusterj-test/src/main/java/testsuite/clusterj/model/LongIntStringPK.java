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

package testsuite.clusterj.model;

import com.mysql.clusterj.annotation.Column;
import com.mysql.clusterj.annotation.Index;
import com.mysql.clusterj.annotation.PersistenceCapable;
import com.mysql.clusterj.annotation.PrimaryKey;

/** Schema
 *
drop table if exists longintstringpk;
create table longintstringpk (
 stringpk varchar(10) not null,
 intpk int not null,
 longpk bigint not null,
 stringvalue varchar(10),
        CONSTRAINT PK_longlongstringpk PRIMARY KEY (longpk, intpk, stringpk)

) ENGINE=ndbcluster DEFAULT CHARSET=latin1 PARTITION BY KEY(intpk, longpk);

 */
@PersistenceCapable(table="longintstringpk")
public interface LongIntStringPK {

    @Column(name="longpk")
    long getLongpk();
    void setLongpk(long value);

    @Column(name="intpk")
    int getIntpk();
    void setIntpk(int value);

    @Column(name="stringpk")
    String getStringpk();
    void setStringpk(String value);

    @Column(name="stringvalue")
    String getStringvalue();
    void setStringvalue(String value);

}
 