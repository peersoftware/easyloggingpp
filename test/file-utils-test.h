#ifndef FILE_UTILS_TEST_H
#define FILE_UTILS_TEST_H

#include "test.h"

static const char* filename = "/tmp/files_utils_test";
static std::shared_ptr<el::base::type::fstream_t> fs;

TEST(FileUtilsTest, NewFileStream) {
    fs = File::newFileStream(filename);
    EXPECT_NE(nullptr, fs);
    EXPECT_TRUE(fs->is_open());
    cleanFile(filename, fs.get());
}

TEST(FileUtilsTest, GetSizeOfFile) {
    EXPECT_EQ(File::getSizeOfFile(fs.get()), 0);
    const char* data = "123";
    (*fs) << data;
    fs->flush();
    EXPECT_EQ(File::getSizeOfFile(fs.get()), strlen(data));
}

TEST(FileUtilsTest, CreatePath) {
    std::filesystem::path path = "/tmp/my/one/long/path";
    EXPECT_FALSE(std::filesystem::exists(path));

	EXPECT_TRUE(File::createPath(path));
    EXPECT_TRUE(std::filesystem::exists(path));
    std::filesystem::remove(path);

    EXPECT_FALSE(std::filesystem::exists(path));
}


TEST(FileUtilsTest, BuildStrippedFilename) {

    char buf[50] = "";

    File::buildStrippedFilename("this_is_myfile.cc", buf, 50);
    EXPECT_STREQ("this_is_myfile.cc", buf);

    Str::clearBuff(buf, 20);
    EXPECT_STREQ("", buf);

    File::buildStrippedFilename("this_is_myfilename_with_more_than_50_characters.cc", buf, 50);
    EXPECT_STREQ("..s_is_myfilename_with_more_than_50_characters.cc", buf);
}

#endif // FILE_UTILS_TEST_H
