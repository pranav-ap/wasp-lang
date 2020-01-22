#include "pch.h"
//#include "../wasp-lang/Lexer.h"

int add(int a, int b)
{
	return a + b;
}

TEST(Calculator_Test_Case, Addition)
{
	EXPECT_TRUE(true);
	EXPECT_EQ(add(3, 3), 6);
}

class Calc_TEST : public ::testing::Test
{
	int a;
public:
	Calc_TEST()
	{
		a = 50;
	}
protected:
	void SetUp()
	{
		a = 5;
	}

	/*virtual void TearDown()
	{
	}*/

	int get_a()
	{
		return a;
	}
};

TEST_F(Calc_TEST, GET_5)
{
	EXPECT_EQ(this->get_a(), 5);
}