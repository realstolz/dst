/* Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#include <my_global.h>
#include <gtest/gtest.h>
#include "mock_logger.h"
#include "file_io.h"
#include "test_utils.h"

#if defined(HAVE_PSI_INTERFACE)
#if !defined(MERGE_UNITTESTS)
namespace keyring
{
  PSI_memory_key key_memory_KEYRING = PSI_NOT_INSTRUMENTED;
  PSI_memory_key key_LOCK_keyring = PSI_NOT_INSTRUMENTED;
}
#endif
namespace keyring
{
  extern PSI_file_key keyring_file_data_key;
  extern PSI_file_key keyring_backup_file_data_key;
}
#endif

namespace keyring__file_io_unittest
{
  using ::testing::StrEq;
  using ::testing::StartsWith;
  using keyring::Mock_logger;
  using my_testing::Server_initializer;

  class File_io_test : public ::testing::Test
  {
  protected:
    virtual void SetUp()
    {
      keyring::keyring_file_data_key = PSI_NOT_INSTRUMENTED;
      keyring::keyring_backup_file_data_key = PSI_NOT_INSTRUMENTED;
      logger= new Mock_logger();
      initializer.SetUp();

      //Set user as super
      Security_context* sec_ctx= current_thd->security_context();
      sec_ctx->set_master_access(sec_ctx->master_access() | SUPER_ACL);
    }

    virtual void TearDown()
    {
      delete logger;
      initializer.TearDown();
    }

  protected:
    Server_initializer initializer; //needed to initialize current_thd
    Mock_logger *logger;
  };

  //Those tests are to check if methods inside File_io do not call my_error but
  //instead generate warnings for filesystem operations errors. If one of those
  //functions would call my_error then error would be set and we would have to call
  //initializer.set_expected_error. As we do not call this in any functions this
  //proves that my_error is not called. Also these tests make sure that correct
  //messages are generated

  TEST_F(File_io_test, OpenNotExistingFile)
  {
    keyring::File_io file_io(logger);
    remove("./some_funny_name"); //just to be sure some_funny_name does not exist

    EXPECT_CALL(*logger, log(MY_ERROR_LEVEL,
                             StrEq("File './some_funny_name' not found "
                                   "(Errcode: 2 - No such file or directory)")));
    File file= file_io.open(keyring::keyring_file_data_key, "./some_funny_name",
                             O_RDONLY, MYF(MY_WME));
    ASSERT_TRUE(file < 0); //could not open the file
  }

  TEST_F(File_io_test, CloseTwiceTheSame)
  {
    keyring::File_io file_io(logger);

    File file= file_io.open(keyring::keyring_file_data_key, "./some_funny_name",
                            O_RDONLY | O_CREAT, MYF(MY_WME));
    ASSERT_TRUE(file >= 0); //successfully created
    file_io.close(file, MYF(MY_WME));

    EXPECT_CALL(*logger, log(MY_ERROR_LEVEL,
                             StartsWith("Error on close of")));
    ASSERT_TRUE(file_io.close(file, MYF(MY_WME)) != 0);

    remove("./some_funny_name");
  }

  TEST_F(File_io_test, ReadFromInvalidFileDescriptor)
  {
    keyring::File_io file_io(logger);
    File file=2050;
    uchar buff[2];

    EXPECT_CALL(*logger, log(MY_ERROR_LEVEL,
                             StartsWith("Error reading file")));
    ASSERT_TRUE(file_io.read(file, buff, 10, MYF(MY_WME)) != 10);
  }

  TEST_F(File_io_test, WriteToInvalidFileDescriptor)
  {
    keyring::File_io file_io(logger);
    File file=2050;

    EXPECT_CALL(*logger, log(MY_ERROR_LEVEL,
                             StartsWith("Error writing file")));
    ASSERT_TRUE(file_io.write(file, reinterpret_cast<const uchar*>("123"), 10,
                              MYF(MY_WME)) != 10);
  }

  TEST_F(File_io_test, SeekOnInvalidFileDescriptor)
  {
    keyring::File_io file_io(logger);
    File file=2050;

    EXPECT_CALL(*logger, log(MY_ERROR_LEVEL,
                             StartsWith("Can't seek in file")));
    ASSERT_TRUE(file_io.seek(file, 0, MY_SEEK_END, MYF(MY_WME)) == MY_FILEPOS_ERROR);
  }

  TEST_F(File_io_test, TellOnInvalidFileDescriptor)
  {
    keyring::File_io file_io(logger);
    File file=2050;

    EXPECT_CALL(*logger, log(MY_ERROR_LEVEL,
                             StartsWith("Can't seek in file")));
    ASSERT_TRUE(file_io.tell(file, MYF(MY_WME)) == ((my_off_t) -1));
  }

  TEST_F(File_io_test, SyncOnInvalidFileDescriptor)
  {
    keyring::File_io file_io(logger);
    File file=2050;

    EXPECT_CALL(*logger, log(MY_ERROR_LEVEL,
                             StartsWith("Can't sync file")));
    ASSERT_TRUE(file_io.sync(file, MYF(MY_WME)) != 0);
  }

  TEST_F(File_io_test, FStatOnInvalidFileDescriptor)
  {
    keyring::File_io file_io(logger);
    File file=2050;

    EXPECT_CALL(*logger, log(MY_ERROR_LEVEL,
                             StartsWith("Error while reading stat for")));
    MY_STAT keyring_file_stat;
    ASSERT_TRUE(file_io.fstat(file, &keyring_file_stat, MYF(MY_WME)) != 0);
  }

  TEST_F(File_io_test, RemoveNotExistingFile)
  {
    keyring::File_io file_io(logger);
    remove("./some_funny_name"); //just to be sure some_funny_name does not exist

    EXPECT_CALL(*logger, log(MY_ERROR_LEVEL,
                             StrEq("Could not remove file ./some_funny_name OS "
                                   "retuned this error: No such file or directory")));
    ASSERT_TRUE(file_io.remove("./some_funny_name", MYF(MY_WME)) != 0);
  }

  TEST_F(File_io_test, TruncateOnNotExistingFile)
  {
    keyring::File_io file_io(logger);
    File file=2050;

    EXPECT_CALL(*logger, log(MY_ERROR_LEVEL,
                             StartsWith("Could not truncate file")));
    ASSERT_TRUE(file_io.truncate(file, MYF(MY_WME)) != 0);
  }

} //namespace keyring__file_io_unittest
 