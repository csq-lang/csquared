/**
 * @file diag.h
 * @brief Master header for diagnostic and error reporting system.
 * @details Aggregates all diagnostic system components: location tracking,
 * metadata-driven diagnostics, reporter functionality, and backwards compatibility.
 *
 * Includes:
 * - diag/location.h: Source location tracking with ID-based lookup
 * - diag/metadata.h: Diagnostic metadata and severity levels
 * - diag/reporter.h: Main diagnostic reporter interface
 * - diag/compat.h: Backwards compatibility layer
 */

#ifndef CORE_DIAG_H
#define CORE_DIAG_H

///< Location management
#include <core/diag/location.h>

///< Metadata and severity definitions
#include <core/diag/metadata.h>

///< Reporter implementation
#include <core/diag/reporter.h>

#endif
