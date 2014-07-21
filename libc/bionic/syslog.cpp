/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <syslog.h>

#include <stdlib.h>

#include "private/libc_logging.h"

static const char* syslog_log_tag = NULL;
static int syslog_priority_mask = 0xff;

void closelog() {
}

void openlog(const char* log_tag, int /*options*/, int /*facility*/) {
  syslog_log_tag = log_tag;
}

int setlogmask(int new_mask) {
  int old_mask = syslog_priority_mask;
  // 0 is used to query the current mask.
  if (new_mask != 0) {
    syslog_priority_mask = new_mask;
  }
  return old_mask;
}

void syslog(int priority, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsyslog(priority, fmt, args);
  va_end(args);
}

void vsyslog(int priority, const char* fmt, va_list args) {
  // Check whether we're supposed to be logging messages of this priority.
  if ((syslog_priority_mask & LOG_MASK(LOG_PRI(priority))) == 0) {
    return;
  }

  // What's our log tag?
  const char* log_tag = syslog_log_tag;
  if (log_tag == NULL) {
    log_tag = getprogname();
  }

  // What's our Android log priority?
  priority &= LOG_PRIMASK;
  int android_log_priority;
  if (priority < LOG_ERR) {
    android_log_priority = ANDROID_LOG_ERROR;
  } else if (priority == LOG_WARNING) {
    android_log_priority = ANDROID_LOG_WARN;
  } else if (priority <= LOG_INFO) {
    android_log_priority = ANDROID_LOG_INFO;
  } else {
    android_log_priority = ANDROID_LOG_DEBUG;
  }

  __libc_format_log_va_list(android_log_priority, log_tag, fmt, args);
}
