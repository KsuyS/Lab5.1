#include "CDate.h"
#include <stdexcept>
#include <limits>

CDate::CDate(unsigned day, Month month, unsigned year)
{
    Date date = { day, month, year };
    CheckAndThrow(date);
    m_daysFrom1970 = ToDays(day, month, year);
}

CDate::CDate(unsigned timestamp) : m_daysFrom1970(timestamp)
{
    Date date = FromDaysToDate();
    CheckAndThrow(date);
}

CDate::CDate() : m_daysFrom1970(0) {}

unsigned CDate::GetDay() const
{
    return FromDaysToDate().day;
}

Month CDate::GetMonth() const
{
    return FromDaysToDate().month;
}

unsigned CDate::GetYear() const
{
    return FromDaysToDate().year;
}

WeekDay CDate::GetWeekDay() const
{
    return static_cast<WeekDay>((static_cast<unsigned>(WeekDay::THURSDAY) + m_daysFrom1970) % DAYS_IN_WEEK);
}

CDate& CDate::operator++()
{
    if (m_daysFrom1970 == DaysBeforeYear(MAX_YEAR + 1) - 1)
    {
        throw std::out_of_range("Превышен максимальный диапазон дат");
    }
    ++m_daysFrom1970;
    return *this;
}

CDate CDate::operator++(int)
{
    CDate tmp = *this;
    ++(*this);
    return tmp;
}

CDate& CDate::operator--()
{
    if (m_daysFrom1970 == 0)
    {
        throw std::out_of_range("Выход за минимальную границу дат");
    }
    --m_daysFrom1970;
    return *this;
}

CDate CDate::operator--(int)
{
    CDate temp = *this;
    --(*this);
    return temp;
}

CDate CDate::operator+(int days) const
{
    int64_t newDaysFrom1970 = static_cast<int64_t>(m_daysFrom1970) + days;
    if (newDaysFrom1970 < 0 || newDaysFrom1970 > static_cast<int64_t>(DaysBeforeYear(MAX_YEAR + 1) - 1))
    {
        throw std::out_of_range("Результат выходит за диапазон дат");
    }
    return CDate(static_cast<uint32_t>(newDaysFrom1970));
}

CDate CDate::operator-(int days) const
{
    return *this + (-days);
}

int CDate::operator-(const CDate& other) const
{
    return static_cast<int>(m_daysFrom1970) - static_cast<int>(other.m_daysFrom1970);
}

CDate& CDate::operator+=(int days)
{
    *this = *this + days;
    return *this;
}

CDate& CDate::operator-=(int days)
{
    *this = *this - days;
    return *this;
}

bool CDate::operator==(const CDate& other) const
{
    return m_daysFrom1970 == other.m_daysFrom1970;
}

bool CDate::operator!=(const CDate& other) const
{
    return !(*this == other);
}

bool CDate::operator<(const CDate& other) const
{
    return m_daysFrom1970 < other.m_daysFrom1970;
}

bool CDate::operator>(const CDate& other) const
{
    return other < *this;
}

bool CDate::operator<=(const CDate& other) const
{
    return !(*this > other);
}

bool CDate::operator>=(const CDate& other) const
{
    return !(*this < other);
}

std::ostream& operator<<(std::ostream& os, const CDate& date)
{
    Date dat = date.FromDaysToDate();
    os << std::setfill('0') << std::setw(2) << dat.day << '.'
        << std::setw(2) << static_cast<unsigned>(dat.month) << '.'
        << std::setw(4) << dat.year;
    return os;
}

std::istream& operator>>(std::istream& is, CDate& date)
{
    std::string input;
    if (!(is >> input))
    {
        throw std::invalid_argument("Ошибка ввода даты");
    }

    std::istringstream ss(input);
    unsigned day, month, year;
    char dot1, dot2;
    if (!(ss >> day >> dot1 >> month >> dot2 >> year) || dot1 != '.' || dot2 != '.' || month < 1 || month > 12)
    {
        throw std::invalid_argument("Неверный формат даты");
    }
    date = CDate(day, static_cast<Month>(month), year);
    return is;
}

void CDate::CheckAndThrow(const Date& date) const
{
    if (date.year < MIN_YEAR || date.year > MAX_YEAR)
        throw std::invalid_argument("Год вне допустимого диапазона");
    if (date.month < Month::JANUARY || date.month > Month::DECEMBER)
        throw std::invalid_argument("Месяц вне допустимого диапазона");
    if (date.day == 0 || date.day > DaysInMonth(date.month, date.year))
        throw std::invalid_argument("Неверное число для данного месяца");
}

unsigned CDate::ToDays(unsigned day, Month month, unsigned year)
{
    unsigned days = DaysBeforeYear(year);
    for (unsigned mon = 1; mon < static_cast<unsigned>(month); ++mon)
    {
        days += DaysInMonth(static_cast<Month>(mon), year);
    }
    days += day - 1;
    return days;
}

Date CDate::FromDaysToDate() const
{
    unsigned year = GetYearFromDays(m_daysFrom1970);
    unsigned daysIntoYear = m_daysFrom1970 - DaysBeforeYear(year);
    Month month;
    unsigned day;
    GetMonthAndDayFromDays(daysIntoYear, year, month, day);
    return { day, month, year };
}

unsigned CDate::GetYearFromDays(unsigned days) const
{
    unsigned minYear = MIN_YEAR;
    unsigned maxYear = MAX_YEAR + 1;
    while (minYear < maxYear)
    {
        unsigned middleYear = minYear + (maxYear - minYear) / 2;
        unsigned daysBeforeMiddleYear = DaysBeforeYear(middleYear);
        if (daysBeforeMiddleYear <= days)
            minYear = middleYear + 1;
        else
            maxYear = middleYear;
    }
    return minYear - 1;
}

void CDate::GetMonthAndDayFromDays(unsigned daysIntoYear, unsigned year, Month& outMonth, unsigned& outDay) const
{
    Month month = Month::JANUARY;
    while (true)
    {
        unsigned daysInMon = DaysInMonth(month, year);
        if (daysIntoYear < daysInMon)break;
        daysIntoYear -= daysInMon;
        month = static_cast<Month>(static_cast<unsigned>(month) + 1);
    }
    outMonth = month;
    outDay = daysIntoYear + 1;
}

bool CDate::IsLeapYear(unsigned year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

unsigned CDate::DaysInMonth(Month month, unsigned year)
{
    switch (month)
    {
    case Month::JANUARY:   return 31;
    case Month::FEBRUARY:  return IsLeapYear(year) ? 29 : 28;
    case Month::MARCH:     return 31;
    case Month::APRIL:     return 30;
    case Month::MAY:       return 31;
    case Month::JUNE:      return 30;
    case Month::JULY:      return 31;
    case Month::AUGUST:    return 31;
    case Month::SEPTEMBER: return 30;
    case Month::OCTOBER:   return 31;
    case Month::NOVEMBER:  return 30;
    case Month::DECEMBER:  return 31;
    default:               return 0;
    }
}

unsigned CDate::CountLeapYears(unsigned year)
{
    return year / 4 - year / 100 + year / 400;
}

unsigned CDate::DaysBeforeYear(unsigned year)
{
    if (year <= MIN_YEAR) return 0;
    unsigned daysUntilYear = (year - 1) * DAYS_IN_USUALLY_YEAR + CountLeapYears(year - 1);
    unsigned daysUntil1970 = (MIN_YEAR - 1) * DAYS_IN_USUALLY_YEAR + CountLeapYears(MIN_YEAR - 1);
    return daysUntilYear - daysUntil1970;
}