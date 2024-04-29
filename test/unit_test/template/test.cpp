/******************************************************************************
 *brief: gtest file template
 *author: cF-embedded.pl
 ******************************************************************************/

extern "C"
{
}

#include <gtest/gtest.h>

class template_test : public ::testing::Test
{
  protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(template_test, add_two_numers)
{
    uint8_t x = 2;

    uint8_t y = x + 2;
    ASSERT_EQ(x + 2, y);
}
