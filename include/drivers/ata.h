#ifndef ATA_H
#define ATA_H

#include <stdint.h>

/**
 * identify_disk() - Runs identify disk.
 * @identify_buffer: The data from the identify command.
 *
 * Prints the total disk size in MB and stores the 256-word identify payload
 * into the provided buffer.
 */
void identify_disk(uint16_t *identify_buffer);

/**
 * read_sectors() - Reads the selected amount of sectors from disk.
 * @LBA: Sector to start from.
 * @sector_count: Amount of sectors to read.
 * @buffer: Where to store the data.
 *
 * Synchronizes with the disk controller using a polling loop, then reads
 * the specified amount of sectors sequentially into the buffer.
 */
void read_sectors(uint32_t LBA, uint8_t sector_count, uint16_t *buffer);

/**
 * write_sectors() - Writes the selected amount of sectors on disk.
 * @LBA: Sector to start from.
 * @sector_count: Amount of sectors to write.
 * @buffer: The data to write on disk.
 *
 * Synchronizes with the disk controller using a polling loop, then writes
 * the specified amount of sectors sequentially from the buffer.
 */
void write_sectors(uint32_t LBA, uint8_t sector_count, uint16_t *buffer);

/**
 * flush_cache() - Flushes the hardware write cache.
 *
 * Necessary on some drives to guarantee data in the cache is physically
 * committed to the storage medium.
 */
void flush_cache(void);

#endif