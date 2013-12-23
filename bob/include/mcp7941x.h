/*
 * mcp7941x.h
 *
 *  Created on: Apr 8, 2013
 *      Author: pi
 */

#ifndef MCP7941X_H_
#define MCP7941X_H_

#define MCP7941X_OK							0
#define MCP7941X_ERROR						1

#define  MCP7941X_DEFAULT_SLAVE_ADDRESS		0x6F


#define MCP7941X_SRAM_RTCC_CONTROL_BYTE_READ	0b11011111
#define MCP7941X_SRAM_RTCC_CONTROL_BYTE_WRITE	0b11011110

/** Time and Configuration Registers (TCR) **/
#define MCP7941X_RTCC_TCR_SECONDS				0x00
#define MCP7941X_RTCC_TCR_MINUTES				0x01
#define MCP7941X_RTCC_TCR_HOURS					0x02
#define MCP7941X_RTCC_TCR_DAY					0x03
#define MCP7941X_RTCC_TCR_DATE					0x04
#define MCP7941X_RTCC_TCR_MONTH					0x05
#define MCP7941X_RTCC_TCR_YEAR					0x06

#define MCP7941X_RTCC_BIT_ST					0x80
#define MCP7941X_RTCC_BIT_VBATEN				0x08

struct rtc_time {
	int tm_sec; /* Seconds.	[0-60] (1 leap second) */
	int tm_min; /* Minutes.	[0-59] */
	int tm_hour; /* Hours.	[0-23] */
	int tm_mday; /* Day.		[1-31] */
	int tm_mon; /* Month.	[0-11] */
	int tm_year; /* Year	- 1900.  */
	int tm_wday; /* Day of week.	[0-6] */
	int tm_yday; /* Days in year.[0-365]	*/
	int tm_isdst; /* DST.		[-1/0/1]*/
};

extern int mcp7941x_start (char);
extern void mcp7941x_end (void);

extern void mcp7941x_get_date_time(struct rtc_time *tm);
extern void mcp7941x_set_date_time(struct rtc_time *tm);

#endif /* MCP7941X_H_ */
