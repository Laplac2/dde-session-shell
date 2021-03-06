#include "timewidget.h"
#include "userinfo.h"

#include <gtest/gtest.h>

class UT_TimeWidget : public testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;

    TimeWidget *timeWidget;
};

void UT_TimeWidget::SetUp()
{
    timeWidget = new TimeWidget();
}

void UT_TimeWidget::TearDown()
{
    delete timeWidget;
}

TEST_F(UT_TimeWidget, init)
{
    std::shared_ptr<NativeUser> nativeUser(new NativeUser("/com/deepin/daemon/Accounts/User"+QString::number((getuid()))));
    bool format = nativeUser->is24HourFormat();
    nativeUser->getUserInter()->Use24HourFormatChanged(!format);
    timeWidget->set24HourFormat(!format);
    EXPECT_EQ(timeWidget->get24HourFormat(), !format);

    timeWidget->setWeekdayFormatType(1);
    timeWidget->setShortDateFormat(1);
    timeWidget->setShortTimeFormat(1);
}
