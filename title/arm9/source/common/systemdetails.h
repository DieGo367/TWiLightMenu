#pragma once
#ifndef __SYSTEM_DETAILS__
#define __SYSTEM_DETAILS__
#include "common/singleton.h"
#include <slim.h>
#include <stdio.h>
#include "nitrofs.h"
#include <nds.h>

class SystemDetails
{
  public:
    SystemDetails();
    ~SystemDetails() {}

  public:
    bool arm7SCFGLocked() { return _arm7SCFGLocked; }
    bool isRegularDS() { return _isRegularDS; }
    bool fatInitOk() { return _fatInitOk; }
    bool useNitroFS() { return _nitroFsInitOk; }
    bool isDSPhat() { return (_isRegularDS && !_isDSLite); }
    void initFilesystem(const char *nitrofsPath, const char *runningPath = NULL);

  private:
    bool _arm7SCFGLocked;
    bool _isRegularDS;
    bool _isDSLite;
    bool _fatInitOk;
    bool _nitroFsInitOk;
};

typedef singleton<SystemDetails> systemDetails_s;

inline SystemDetails &sys() { return systemDetails_s::instance(); }
#endif