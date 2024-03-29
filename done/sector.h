#pragma once

/**
 * @file  sector.h
 * @brief block-level accessor function.
 *
 * @author Edouard Bugnion
 * @date summer 2022
 */

#include <stdint.h>
#include <stdio.h>
#include "mount.h"
#include "unixv6fs.h" 

#ifdef __cplusplus
extern "C" {
#endif

/* *************************************************** *
 * TODO WEEK 04										   *
 * *************************************************** */
/**
 * @brief read one 512-byte sector from the virtual disk
 * @param f open file of the virtual disk
 * @param sector the location (in sector units, not bytes) within the virtual disk
 * @param data a pointer to 512-bytes of memory (OUT)
 * @return 0 on success; <0 on error
 */
int sector_read(FILE *f, uint32_t sector, void *data);


/* *************************************************** *
 * TODO WEEK 11										   *
 * *************************************************** */
/**
 * @brief write one 512-byte sector from the virtual disk
 * @param f open file of the virtual disk
 * @param sector the location (in sector units, not bytes) within the virtual disk
 * @param data a pointer to 512-bytes of memory (IN)
 * @return 0 on success; <0 on error
 */
int sector_write(FILE *f, uint32_t sector, const void *data);

#ifdef __cplusplus
}
#endif


