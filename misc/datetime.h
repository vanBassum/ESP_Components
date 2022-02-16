/*
 * DateTime.h
 *
 *  Created on: 9 Sep 2020
 *      Author: Bas
 */

#ifndef COMPONENTS_CPP_LIB_MISC_DATETIME_H_
#define COMPONENTS_CPP_LIB_MISC_DATETIME_H_

#include <time.h>
#include "TimeSpan.h"
#include <string>
#define DATETIEMFORMAT	"%c"	//TODO this is C# format not c++
//#define DATETIEMFORMAT	"dd-MMM-yyyy HH:mm:ss.ffff K"	//TODO this is C# format not c++


class DateTime
{

public:

	time_t utc = 0;

	DateTime()
	{

	}

	DateTime(time_t utc)
	{
		this->utc = utc;
	}

	DateTime(std::string datetime)
	{
		struct tm result;
		if (strptime(datetime.c_str(), "%FT%TZ", &result))
		{
			utc = mktime(&result);
		}
	}

	DateTime(std::string datetime, std::string format)
	{
		struct tm result;
		if (strptime(datetime.c_str(), format.c_str(), &result))
		{
			utc = mktime(&result);
		}
	}

	static DateTime Now()
	{
		DateTime dt;
		dt.utc = time(NULL);
		return dt;
	}


	std::string ToString()
	{
		char buf[sizeof "2011-10-08T07:07:09Z"];
		strftime(buf, sizeof buf, "%FT%TZ", localtime(&utc));
		return buf;
	}
	
	
/*
	time_t MKTime() const
	{
		struct tm tm = _time;
		return mktime(&tm);
	}



	static DateTime Empty()
	{
		time_t t = 0;
		struct tm *now = localtime(&t);
		return FromTM(*now);
	}

	static DateTime FromTM(struct tm tm)
	{
		DateTime dt;
		dt._time.tm_sec   = tm.tm_sec  ;
		dt._time.tm_min   = tm.tm_min  ;
		dt._time.tm_hour  = tm.tm_hour ;
		dt._time.tm_mday  = tm.tm_mday ;
		dt._time.tm_mon   = tm.tm_mon  ;
		dt._time.tm_year  = tm.tm_year ;
		dt._time.tm_wday  = tm.tm_wday ;
		dt._time.tm_yday  = tm.tm_yday ;
		dt._time.tm_isdst = tm.tm_isdst;
		return dt;
	}

	static DateTime FromTimeT(time_t t)
	{
		struct tm *tm = localtime(&t);
		return FromTM(*tm);
	}

	DateTime AddSeconds(time_t seconds)	//seconds is the right hand side
	{
		time_t t = MKTime() + seconds;
		return FromTimeT(t);
	}

	static bool TryParse(std::string raw, DateTime *val)
	{
		return strptime(raw.c_str(), DATETIEMFORMAT, &val->_time) != NULL;	//TODO not completely safe. see https://man7.org/linux/man-pages/man3/strptime.3.html#RETURN_VALUE
	}

	std::string ToString()
	{
		char buff[64];
		strftime(buff, 64, DATETIEMFORMAT, &_time);
		return buff;
	}

*/
};



inline bool operator==(DateTime const &lhs, DateTime const &rhs) { return difftime(lhs.utc, rhs.utc) == 0; }
inline bool operator!=(DateTime const &lhs, DateTime const &rhs) { return difftime(lhs.utc, rhs.utc) != 0; }
inline bool operator< (DateTime const &lhs, DateTime const &rhs) { return difftime(lhs.utc, rhs.utc) <  0; }
inline bool operator> (DateTime const &lhs, DateTime const &rhs) { return difftime(lhs.utc, rhs.utc) >  0; }
inline bool operator<=(DateTime const &lhs, DateTime const &rhs) { return difftime(lhs.utc, rhs.utc) <= 0; }
inline bool operator>=(DateTime const &lhs, DateTime const &rhs) { return difftime(lhs.utc, rhs.utc) >= 0; }


inline TimeSpan operator-(DateTime const &lhs, DateTime const &rhs) { return TimeSpan::FromSeconds(lhs.utc - rhs.utc); }
inline DateTime operator+(DateTime const &lhs, TimeSpan const &rhs) { return DateTime(lhs.utc + rhs.ticks); }



#endif /* COMPONENTS_CPP_LIB_MISC_DATETIME_H_ */
