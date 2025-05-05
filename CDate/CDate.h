#pragma once

#include <iomanip>
#include <ostream>
#include <sstream>

enum class Month
{
    JANUARY = 1, FEBRUARY, MARCH, APRIL,
    MAY, JUNE, JULY, AUGUST, SEPTEMBER,
    OCTOBER, NOVEMBER, DECEMBER
};

enum class WeekDay
{
    SUNDAY = 0, MONDAY, TUESDAY, WEDNESDAY,
    THURSDAY, FRIDAY, SATURDAY
};

struct Date
{
    unsigned day;
    Month month;
    unsigned year;
};

class CDate
{
public:
    CDate(unsigned day, Month month, unsigned year);
    explicit CDate(unsigned timestamp);
    CDate();

    unsigned GetDay() const;
    Month GetMonth() const;
    unsigned GetYear() const;
    WeekDay GetWeekDay() const;
    bool IsValid() const;
    Date FromDaysToDate() const;

    CDate& operator++();
    CDate operator++(int);
    CDate& operator--();
    CDate operator--(int);
    CDate operator+(int days) const;
    CDate operator-(int days) const;
    int operator-(const CDate& other) const;
    CDate& operator+=(int days);
    CDate& operator-=(int days);
    bool operator==(const CDate& other) const;
    bool operator!=(const CDate& other) const;
    bool operator<(const CDate& other) const;
    bool operator>(const CDate& other) const;
    bool operator<=(const CDate& other) const;
    bool operator>=(const CDate& other) const;

    static CDate Invalid();

private:
    uint32_t m_daysFrom1970;
    bool m_valid;

    static const unsigned MIN_YEAR = 1970;
    static const unsigned MAX_YEAR = 9999;
    static const unsigned DAYS_IN_WEEK = 7;
    static const unsigned DAYS_IN_USUALLY_YEAR = 365;

    unsigned ToDays(unsigned day, Month month, unsigned year);
    bool CheckValidity(const Date& date) const;
    unsigned GetYearFromDays(unsigned days) const;
    void GetMonthAndDayFromDays(unsigned daysIntoYear, unsigned year, Month& outMonth, unsigned& outDay) const;

    static bool IsLeapYear(unsigned year);
    static unsigned DaysInMonth(Month month, unsigned year);
    static unsigned CountLeapYears(unsigned year);
    static unsigned DaysBeforeYear(unsigned year);
};

std::ostream& operator<<(std::ostream& os, const CDate& date);
std::istream& operator>>(std::istream& is, CDate& date);
