//
// Created by master on 17. 6. 13.
//

# include "gtest/gtest.h"
# include "velocity_profile.h"

TEST(VelocityProfileTest, negative) {
    sephi::TrVelocityProfile vp;
    EXPECT_EQ(0, vp.Duration());
}