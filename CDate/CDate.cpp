#include "CDate.h"

CDate::CDate(unsigned day, Month month, unsigned year)
{
    m_valid = CheckValidity({ day, month, year });

    if (!m_valid)
    {
        m_daysFrom1970 = UINT32_MAX;
    }
    else
    {
        m_daysFrom1970 = ToDays(day, month, year);
    }
}

CDate::CDate(unsigned timestamp)
{
    m_daysFrom1970 = timestamp;
    Date date = FromDaysToDate();
    m_valid = CheckValidity(date);
}

CDate::CDate() : m_daysFrom1970(0), m_valid(true) {}

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
    if (!m_valid)
    {
        return WeekDay::SUNDAY;
    }
    return static_cast<WeekDay>((static_cast<unsigned>(WeekDay::THURSDAY) + m_daysFrom1970) % DAYS_IN_WEEK);
}

bool CDate::IsValid() const
{
    return m_valid;
}

CDate& CDate::operator++()
{
    if (!m_valid)
    {
        return *this;
    }

    if (m_daysFrom1970 == DaysBeforeYear(MAX_YEAR + 1) - 1)
    {
        m_valid = false;
        return *this;
    }

    ++m_daysFrom1970;
    return *this;
}

CDate CDate::operator++(int)
{
    CDate date = *this;
    ++(*this);
    return date;
}

CDate& CDate::operator--()
{
    if (!m_valid)
    {
        return *this;
    }

    if (m_daysFrom1970 == 0)
    {
        m_valid = false;
        return *this;
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
    if (!m_valid)
    {
        return *this;
    }

    int64_t newDays = static_cast<int64_t>(m_daysFrom1970) + days;
    if (newDays < 0 || newDays > static_cast<int64_t>(DaysBeforeYear(MAX_YEAR + 1) - 1))
    {
        return CDate::Invalid();
    }

    return CDate(static_cast<uint32_t>(newDays));
}

CDate CDate::operator-(int days) const
{
    return *this + (-days);
}

int CDate::operator-(const CDate& other) const
{
    if (!m_valid || !other.IsValid())
    {
        return 0;
    }

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
    if (!m_valid || !other.m_valid)
    {
        return false;
    }
    return m_daysFrom1970 == other.m_daysFrom1970;
}

bool CDate::operator!=(const CDate& other) const
{
    return !(*this == other);
}

bool CDate::operator<(const CDate& other) const
{
    if (!m_valid || !other.m_valid)
    {
        return false;
    }
    return m_daysFrom1970 < other.m_daysFrom1970;
}

bool CDate::operator>(const CDate& other) const
{
    if (!m_valid || !other.m_valid)
    {
        return false;
    }
    return other < *this;
}

bool CDate::operator<=(const CDate& other) const
{
    if (!m_valid || !other.m_valid)
    {
        return false;
    }
    return !(*this > other);
}

bool CDate::operator>=(const CDate& other) const
{
    if (!m_valid || !other.m_valid)
    {
        return false;
    }
    return !(*this < other);
}

std::ostream& operator<<(std::ostream& os, const CDate& date)
{
    if (!date.IsValid())
    {
        os << "INVALID";
        return os;
    }

    Date dateFromDays = date.FromDaysToDate();
    os << std::setfill('0') << std::setw(2) << dateFromDays.day << '.'
        << std::setw(2) << static_cast<unsigned>(dateFromDays.month) << '.'
        << std::setw(4) << dateFromDays.year;

    return os;
}

std::istream& operator>>(std::istream& is, CDate& date)
{
    std::string line;
    if (!(is >> line))
    {
        date = CDate::Invalid();
        return is;
    }

    std::istringstream ss(line);
    unsigned day, month, year;
    char dot1, dot2;

    if (!(ss >> day >> dot1 >> month >> dot2 >> year) && dot1 != '.' && dot2 != '.' && month < 1 && month > 12)
    {
        date = CDate::Invalid();
        return is;
    }

    CDate newDate(day, static_cast<Month>(month), year);
    if (!newDate.IsValid())
    {
        date = CDate::Invalid();
        return is;
    }

    date = newDate;
    return is;
}

bool CDate::IsLeapYear(unsigned year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

unsigned CDate::DaysInMonth(Month month, unsigned year)
{
    switch (month)
    {
    case Month::JANUARY: return 31;
    case Month::FEBRUARY: return IsLeapYear(year) ? 29 : 28;
    case Month::MARCH: return 31;
    case Month::APRIL: return 30;
    case Month::MAY: return 31;
    case Month::JUNE: return 30;
    case Month::JULY: return 31;
    case Month::AUGUST: return 31;
    case Month::SEPTEMBER: return 30;
    case Month::OCTOBER: return 31;
    case Month::NOVEMBER: return 30;
    case Month::DECEMBER: return 31;
    default: return 0;
    }
}

bool CDate::CheckValidity(const Date& date) const
{
    if (date.year < MIN_YEAR || date.year > MAX_YEAR)
    {
        return false;
    }
    if (date.month < Month::JANUARY || date.month > Month::DECEMBER)
    {
        return false;
    }
    if (date.day == 0 || date.day > DaysInMonth(date.month, date.year))
    {
        return false;
    }
    return true;
}

unsigned CDate::ToDays(unsigned day, Month month, unsigned year)
{
    unsigned days = DaysBeforeYear(year);

    for (unsigned m = 1; m < static_cast<unsigned>(month); ++m)
    {
        days += DaysInMonth(static_cast<Month>(m), year);
    }

    days += day - 1;
    return days;
}

Date CDate::FromDaysToDate() const
{
    if (m_daysFrom1970 == UINT32_MAX)
    {
        return { 0, Month::JANUARY, 0 };
    }

    unsigned year = GetYearFromDays(m_daysFrom1970);
    unsigned daysIntoYear = m_daysFrom1970 - DaysBeforeYear(year);

    Month month;
    unsigned day;
    GetMonthAndDayFromDays(daysIntoYear, year, month, day);

    return { day, month, year };
}

unsigned CDate::GetYearFromDays(unsigned days) const
{
    unsigned min = MIN_YEAR;
    unsigned max = MAX_YEAR + 1;

    while (min < max)
    {
        unsigned mid = min + (max - min) / 2;
        unsigned daysBeforeMid = DaysBeforeYear(mid);
        if (daysBeforeMid <= days)
        {
            min = mid + 1;
        }
        else
        {
            max = mid;
        }
    }

    return min - 1;
}

void CDate::GetMonthAndDayFromDays(unsigned daysIntoYear, unsigned year, Month& outMonth, unsigned& outDay) const
{
    Month month = Month::JANUARY;

    while (true)
    {
        unsigned dayInMonth = DaysInMonth(month, year);
        if (daysIntoYear < dayInMonth)
        {
            break;
        }
        daysIntoYear -= dayInMonth;
        month = static_cast<Month>(static_cast<unsigned>(month) + 1);
    }

    outMonth = month;
    outDay = daysIntoYear + 1;
}

unsigned CDate::DaysBeforeYear(unsigned year)
{
    if (year <= MIN_YEAR)
    {
        return 0;
    }

    unsigned daysUntilYear = (year - 1) * DAYS_IN_USUALLY_YEAR + CountLeapYears(year - 1);
    unsigned daysUntil1970 = (MIN_YEAR - 1) * DAYS_IN_USUALLY_YEAR + CountLeapYears(MIN_YEAR - 1);

    return daysUntilYear - daysUntil1970;
}

unsigned CDate::CountLeapYears(unsigned year)
{
    return year / 4 - year / 100 + year / 400;
}

CDate CDate::Invalid()
{
    CDate d;
    d.m_daysFrom1970 = UINT32_MAX;
    d.m_valid = false;
    return d;
}