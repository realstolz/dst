/*
   Copyright (c) 2003, 2018, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef _WIN32

#include "SysLogHandler.hpp"

#include <syslog.h>

//
// PUBLIC
//

SysLogHandler::SysLogHandler() :
  m_severity(LOG_INFO),
  m_pIdentity("NDB"),
  m_facility(LOG_USER),
  m_open(false)
{
}

SysLogHandler::SysLogHandler(const char* pIdentity, int facility) : 
  m_severity(LOG_INFO), 
  m_pIdentity(pIdentity),
  m_facility(facility),
  m_open(false)
{

}

SysLogHandler::~SysLogHandler()
{
}

bool
SysLogHandler::open()
{
  ::setlogmask(LOG_UPTO(LOG_DEBUG)); // Log from EMERGENCY down to DEBUG
  ::openlog(m_pIdentity, LOG_PID|LOG_CONS|LOG_ODELAY, m_facility); // PID, CONSOLE delay openlog
  m_open= true;
  return true;
}

bool
SysLogHandler::close()
{
  ::closelog();
  m_open= false;
  return true;
}

bool
SysLogHandler::is_open()
{
  return m_open;
}

void 
SysLogHandler::writeHeader(const char* pCategory, Logger::LoggerLevel level,
                           time_t now)
{
  // Save category to be used by writeMessage...
  m_pCategory = pCategory;
  // Map LogLevel to syslog severity
  switch (level)
  {
  case Logger::LL_ALERT:
    m_severity = LOG_ALERT;
    break;
  case Logger::LL_CRITICAL:
    m_severity = LOG_CRIT;
    break;
  case Logger::LL_ERROR:
    m_severity = LOG_ERR;
    break;
  case Logger::LL_WARNING:
    m_severity = LOG_WARNING;
    break;
  case Logger::LL_INFO:
    m_severity = LOG_INFO;
    break;
  case Logger::LL_DEBUG:
    m_severity = LOG_DEBUG;
    break;
  default:
    m_severity = LOG_INFO;
    break;
  }

}

void 
SysLogHandler::writeMessage(const char* pMsg)
{
  ::syslog(m_facility | m_severity, "[%s] %s", m_pCategory, pMsg); 
}

void 
SysLogHandler::writeFooter()
{
  // Need to close it everytime? Do we run out of file descriptors?
  //::closelog();
}

bool
SysLogHandler::setParam(const BaseString &param, const BaseString &value) {
  if(param == "facility") {
    return setFacility(value);
  }
  return false;
}

static const struct syslog_facility {
  const char *name;
  int value;
} facilitynames[] = {
  { "auth", LOG_AUTH },
#ifdef LOG_AUTHPRIV
  { "authpriv", LOG_AUTHPRIV },
#endif
  { "cron", LOG_CRON },
  { "daemon", LOG_DAEMON },
#ifdef LOG_FTP
  { "ftp", LOG_FTP },
#endif
  { "kern", LOG_KERN },
  { "lpr", LOG_LPR },
  { "mail", LOG_MAIL },
  { "news", LOG_NEWS },
  { "syslog", LOG_SYSLOG },
  { "user", LOG_USER },
  { "uucp", LOG_UUCP },
  { "local0", LOG_LOCAL0 },
  { "local1", LOG_LOCAL1 },
  { "local2", LOG_LOCAL2 },
  { "local3", LOG_LOCAL3 },
  { "local4", LOG_LOCAL4 },
  { "local5", LOG_LOCAL5 },
  { "local6", LOG_LOCAL6 },
  { "local7", LOG_LOCAL7 },
  { NULL, -1 }
};

bool
SysLogHandler::setFacility(const BaseString &facility) {
  const struct syslog_facility *c;
  for(c = facilitynames; c->name != NULL; c++) {
    if(facility == c->name) {
      m_facility = c->value;
      close();
      open();
      return true;
    }
  }
  setErrorStr("Invalid syslog facility name");
  return false;
}

#endif
 