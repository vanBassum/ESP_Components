/*
 * DateTime.h
 *
 *  Created on: 8 Sep 2020
 *      Author: Bas
 */

#ifndef MAIN_SMARTHOME_LIB_DATETIMECPP_H_
#define MAIN_SMARTHOME_LIB_DATETIMECPP_H_

#include "time.h"
#include <esp_log.h>


/// <summary>
/// Represents a date and time. Mimics the behavour of the C# DateTime class.
/// </summary>
class DateTimeCPP
{
public:
	struct tm _time;

	/// <summary>
	/// Clears all mebers of the tm_time structure to all zeroes.
	/// </summary>
	void Clear()
	{
		_time.tm_sec   = 0;
		_time.tm_min   = 0;
		_time.tm_hour  = 0;
		_time.tm_mday  = 0;
		_time.tm_mon   = 0;
		_time.tm_year  = 0;
		_time.tm_wday  = 0;
		_time.tm_yday  = 0;
		_time.tm_isdst = 0;
	}

	/// <summary>
	/// Converts the datetime to time_t
	/// </summary>
	time_t MKTime() const
	{
		struct tm tm = _time;
		return mktime(&tm);
	}

	/// <summary>
	/// Returns an instance of DateTimeCPP of the current date and time.
	/// </summary>
	static DateTimeCPP Now()
	{
		time_t t = time(NULL);
		struct tm *now = localtime(&t);
		return FromTM(*now);
	}

	/// <summary>
	/// Populates the DateTime from a tm_time structure
	/// </summary>
	void PopulateFromTM(struct tm tm)
	{
		_time.tm_sec   = tm.tm_sec  ;
		_time.tm_min   = tm.tm_min  ;
		_time.tm_hour  = tm.tm_hour ;
		_time.tm_mday  = tm.tm_mday ;
		_time.tm_mon   = tm.tm_mon  ;
		_time.tm_year  = tm.tm_year ;
		_time.tm_wday  = tm.tm_wday ;
		_time.tm_yday  = tm.tm_yday ;
		_time.tm_isdst = tm.tm_isdst;
	}

	/// <summary>
	/// Creates a DateTime from a tm_time structure
	/// </summary>
	static DateTimeCPP FromTM(struct tm tm)
	{
		DateTimeCPP dt;
		dt.PopulateFromTM(tm);
		return dt;
	}

	/// <summary>
	/// Creates a DateTime from a time_t
	/// </summary>
	static DateTimeCPP FromTimeT(time_t t)
	{
		struct tm *tm = localtime(&t);
		return FromTM(*tm);
	}

	/// <summary>
	/// Adds a sertain ammount of seconds to the datetime
	/// </summary>
	DateTimeCPP AddSeconds(time_t seconds)	//seconds is the right hand side
	{
		time_t t = MKTime() + seconds;
		return FromTimeT(t);
	}
};



inline bool operator==(DateTimeCPP const &lhs, DateTimeCPP const &rhs) { return difftime(lhs.MKTime(), rhs.MKTime()) == 0; }
inline bool operator!=(DateTimeCPP const &lhs, DateTimeCPP const &rhs) { return difftime(lhs.MKTime(), rhs.MKTime()) != 0; }
inline bool operator< (DateTimeCPP const &lhs, DateTimeCPP const &rhs) { return difftime(lhs.MKTime(), rhs.MKTime()) <  0; }
inline bool operator> (DateTimeCPP const &lhs, DateTimeCPP const &rhs) { return difftime(lhs.MKTime(), rhs.MKTime()) >  0; }
inline bool operator<=(DateTimeCPP const &lhs, DateTimeCPP const &rhs) { return difftime(lhs.MKTime(), rhs.MKTime()) <= 0; }
inline bool operator>=(DateTimeCPP const &lhs, DateTimeCPP const &rhs) { return difftime(lhs.MKTime(), rhs.MKTime()) >= 0; }







#endif /* MAIN_SMARTHOME_LIB_DATETIMECPP_H_ */
