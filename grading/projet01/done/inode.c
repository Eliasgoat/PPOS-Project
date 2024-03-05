#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "unixv6fs.h"
#include "sector.h"
#include "mount.h"
#include "error.h"
#include "inode.h"


/**
 * @brief read all inodes from disk and print out their content to
 *        stdout according to the assignment
 * @param u the filesystem
 * @return 0 on success; < 0 on error.
 */
int inode_scan_print(const struct unix_filesystem *u){
    M_REQUIRE_NON_NULL(u);
    uint16_t size_sector = (u->s).s_isize;
    struct inode inode = {};
    for(size_t ind = ROOT_INUMBER; ind < size_sector * INODES_PER_SECTOR; ++ind){
        int res = inode_read(u, (uint16_t)ind, &inode);
        if(res == ERR_NONE){
            // correcteur : copy-paste
            if (inode.i_mode & IFDIR){
                pps_printf("inode %zu (%s) len %d\n", ind, SHORT_DIR_NAME, inode_getsize(&inode));
            }
            else{
                pps_printf("inode %zu (%s) len %d\n", ind, SHORT_FIL_NAME, inode_getsize(&inode));
            }
        }else if(res != ERR_UNALLOCATED_INODE){
            return res;
        }
    }
    return ERR_NONE;

}

/**
 * @brief read the content of an inode from disk
 * @param u the filesystem (IN)
 * @param inr the inode number of the inode to read (IN)
 * @param inode the inode structure, read from disk (OUT)
 * @return 0 on success; <0 on error
 */
int inode_read(const struct unix_filesystem *u, uint16_t inr, struct inode *inode){
    M_REQUIRE_NON_NULL(u);
    M_REQUIRE_NON_NULL(inode);
    FILE* f = u->f;
    uint16_t size_sector = (u->s).s_isize;
    uint16_t inode_start = (u->s).s_inode_start;
    uint32_t start = inode_start + inr / INODES_PER_SECTOR; 

    if(inr < ROOT_INUMBER || inr >= size_sector * INODES_PER_SECTOR){
        return ERR_INODE_OUT_OF_RANGE;
    }else{
        struct inode_sector array_inodes = {};    //
        int res = sector_read(f, start, array_inodes.inodes);
        if(res == ERR_NONE){
            if(array_inodes.inodes[inr % INODES_PER_SECTOR].i_mode & IALLOC){
                *inode = array_inodes.inodes[inr % INODES_PER_SECTOR];
            }else{
                return ERR_UNALLOCATED_INODE;
            }
        }
        return res;
    }
}

/**
 * @brief identify the sector that corresponds to a given portion of a file
 * @param u the filesystem (IN)
 * @param inode the inode (IN) (prealablement rempli par inode_read())
 * @param file_sec_off the offset within the file (in sector-size units)
 * @return >0: the sector on disk;  <0 error
 */
int inode_findsector(const struct unix_filesystem *u, const struct inode *i, int32_t file_sec_off){
    M_REQUIRE_NON_NULL(u);
    M_REQUIRE_NON_NULL(i);

    if(!(i->i_mode & IALLOC)){
        return ERR_UNALLOCATED_INODE;
    }

    int32_t inode_size = inode_getsize(i);

    if(file_sec_off < 0 || file_sec_off  > (inode_size - 1) / SECTOR_SIZE){
        return ERR_OFFSET_OUT_OF_RANGE;
    }

    if(inode_size <= ADDR_SMALL_LENGTH * SECTOR_SIZE){
        //cas ou la taille fait moins de 8 secteurs
        return i->i_addr[file_sec_off];
    }else if(inode_size > ADDR_SMALL_LENGTH * SECTOR_SIZE && inode_size <= (ADDR_SMALL_LENGTH - 1) * ADDRESSES_PER_SECTOR * SECTOR_SIZE){
        //cas ou la taille est strictement superieur a 8 secteurs et inferieur ou egale a 7*256 secteurs
        int address_index = ((inode_size - 1) / SECTOR_SIZE) / ADDRESSES_PER_SECTOR;
        uint16_t addresses[ADDRESSES_PER_SECTOR];
        int err = sector_read(u->f, i->i_addr[address_index], addresses);
        if(err != ERR_NONE){
            return err;
        }else{
            return addresses[file_sec_off % ADDRESSES_PER_SECTOR];
        }
    }else{
        //cas ou le fichier est trop grand
        return ERR_FILE_TOO_LARGE;
    }
}