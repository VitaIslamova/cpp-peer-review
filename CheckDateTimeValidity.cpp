#include <stdexcept>
#include <array>
#include <cassert>

using namespace std;
using namespace std::literals;

namespace datetime {
    const int MinYear = 1;
    const int MaxYear = 9999;
    const int MinMonth = 1;
    const int MaxMonth = 12;
    const int MinDay = 1;
    const int MinHour = 0;
    const int MaxHour = 23;
    const int MinMinuteSecond = 0;
    const int MaxMinuteSecond = 59;
    const int LeapYearDivisor = 4;
    const int LeapYearExceptionDivisor1 = 100;
    const int LeapYearExceptionDivisor2 = 400;
    constexpr const array<int, 12> MonthLengthsNonLeapYear{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    constexpr const array<int, 12> MonthLengthsLeapYear{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void CheckYearValidity(int year) {
    if (year < MinYear) {
        throw domain_error("year_ is too small"s);
    }
    if (year > MaxYear) {
        throw domain_error("year_ is too big"s);
    }
}

void CheckMonthValidity(int month) {
    if (month < MinMonth) {
        throw domain_error("month_ is too small"s);
    }
    if (month > MaxMonth) {
        throw domain_error("month_ is too big"s);
    }
}

void CheckDayValidity(int year, int month, int day) {
    const bool is_leap_year = (year % LeapYearDivisor == 0) && !(year % LeapYearExceptionDivisor1 == 0 &&
                                                                 year % LeapYearExceptionDivisor2 != 0);

    const array<int, 12> &month_lengths = is_leap_year ? MonthLengthsLeapYear : MonthLengthsNonLeapYear;

    if (day < MinDay) {
        throw domain_error("day is too small"s);
    }
    if (day > month_lengths[month - 1]) {
        throw domain_error("day is too big"s);
    }
}

void CheckHourValidity(int hour) {
    if (hour < MinHour) {
        throw domain_error("hour is too small"s);
    }
    if (hour > MaxHour) {
        throw domain_error("hour is too big"s);
    }
}

void CheckMinuteValidity(int minute) {
    if (minute < MinMinuteSecond) {
        throw domain_error("minute_ is too small"s);
    }
    if (minute > MaxMinuteSecond) {
        throw domain_error("minute_ is too big"s);
    }
}

void CheckSecondValidity(int second) {
    if (second < MinMinuteSecond) {
        throw domain_error("second is too small");
    }
    if (second > MaxMinuteSecond) {
        throw domain_error("second is too big"s);
    }
}

class DateTime {
public:
    DateTime(int year, int month, int day, int hour, int minute, int second)
            : year_(year), month_(month), day_(day), hour_(hour), minute_(minute), second_(second) {
        Validate();
    }

private:
    int year_;
    int month_;
    int day_;
    int hour_;
    int minute_;
    int second_;

    void Validate() const {
        CheckYearValidity(year_);
        CheckMonthValidity(month_);
        CheckDayValidity(year_, month_, day_);
        CheckHourValidity(hour_);
        CheckMinuteValidity(minute_);
        CheckSecondValidity(second_);
    }
};

} // namespace datetime

void TestValidDateTime() {
    try {
        datetime::DateTime dt{2022, 12, 31, 23, 59, 59};
    } catch (...) {
        assert(false && "Valid datetime failed validation");
    }
}

void TestInvalidYear() {
    try {
        datetime::DateTime dt{10000, 1, 1, 0, 0, 0};
        assert(false && "Invalid year passed validation");
    } catch (std::domain_error &) {
    }
}

void TestInvalidMonth() {
    try {
        datetime::DateTime dt{2022, 13, 1, 0, 0, 0};
        assert(false && "Invalid month passed validation");
    } catch (std::domain_error &) {
    }
}

void TestInvalidDay() {
    try {
        // 2021 - not a leap year
        datetime::DateTime dt{2021, 2, 29, 0, 0, 0};
        assert(false && "Invalid day passed validation");
    } catch (std::domain_error &) {
    }
}

void TestInvalidHour() {
    try {
        datetime::DateTime dt{2022, 2, 28, 24, 0, 0};
        assert(false && "Invalid hour passed validation");
    } catch (std::domain_error &) {
    }
}

void TestInvalidMinute() {
    try {
        datetime::DateTime dt{2022, 2, 28, 23, 60, 0};
        assert(false && "Invalid minute passed validation");
    } catch (std::domain_error &) {
    }
}

void TestInvalidSecond() {
    try {
        datetime::DateTime dt{2022, 2, 28, 23, 59, 60};
        assert(false && "Invalid second passed validation");
    } catch (std::domain_error &) {
    }
}

int Tests() {
    TestValidDateTime();
    TestInvalidYear();
    TestInvalidMonth();
    TestInvalidDay();
    TestInvalidHour();
    TestInvalidMinute();
    TestInvalidSecond();
    return 0;
}

int main() {
    Tests();
    return 0;
}
