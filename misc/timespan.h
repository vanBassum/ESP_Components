/*
 * TimeSpan.h
 *
 *  Created on: 21 Sep 2020
 *      Author: Bas
 */

#ifndef COMPONENTS_CPP_LIB_MISC_TIMESPAN_H_
#define COMPONENTS_CPP_LIB_MISC_TIMESPAN_H_

#include "datetime.h"
#include <stdint.h>

class TimeSpan
{

public:

	int64_t ticks;	//Seconds

	TimeSpan(int64_t t)
	{
		ticks = t;
	}

	double GetTotalSeconds() const
	{
		return ticks;
	}

	double GetTotalMinutes() const
	{
		return ticks / 60;
	}




	static TimeSpan FromSeconds(int64_t seconds)
	{
		return TimeSpan(seconds);
	}


	static TimeSpan FromMinutes(int64_t seconds)
	{
		return TimeSpan(seconds * 60);
	}



};


inline bool operator==(TimeSpan const &lhs, TimeSpan const &rhs) { return lhs.GetTotalSeconds() == rhs.GetTotalSeconds();}
inline bool operator!=(TimeSpan const &lhs, TimeSpan const &rhs) { return lhs.GetTotalSeconds() != rhs.GetTotalSeconds();}
inline bool operator< (TimeSpan const &lhs, TimeSpan const &rhs) { return lhs.GetTotalSeconds() < rhs.GetTotalSeconds();}
inline bool operator> (TimeSpan const &lhs, TimeSpan const &rhs) { return lhs.GetTotalSeconds() > rhs.GetTotalSeconds();}
inline bool operator<=(TimeSpan const &lhs, TimeSpan const &rhs) { return lhs.GetTotalSeconds() <= rhs.GetTotalSeconds();}
inline bool operator>=(TimeSpan const &lhs, TimeSpan const &rhs) { return lhs.GetTotalSeconds() >= rhs.GetTotalSeconds();}




#endif /* COMPONENTS_CPP_LIB_MISC_TIMESPAN_H_ */
