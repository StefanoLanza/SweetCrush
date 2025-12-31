#pragma once

// The current major version
#define SWEETCRUSH_MAJOR_VERSION 0

// The current minor version
#define SWEETCRUSH_MINOR_VERSION 1

// The current micro (or patchlevel) version
#define SWEETCRUSH_MICRO_VERSION 0

#define SWEETCRUSH_VERSIONNUM(major, minor, patch) \
    ((major) * 1000000 + (minor) * 1000 + (patch))

#define SWEETCRUSH_VERSION \
    SWEETCRUSH_VERSIONNUM(SWEETCRUSH_MAJOR_VERSION, SWEETCRUSH_MINOR_VERSION, SWEETCRUSH_MICRO_VERSION)
