#include <gtest/gtest.h>
#include <QCoreApplication>

// Simple sanity test
TEST(Sanity, TrueIsTrue) {
    EXPECT_TRUE(true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    QCoreApplication app(argc, argv);
    return RUN_ALL_TESTS();
}
