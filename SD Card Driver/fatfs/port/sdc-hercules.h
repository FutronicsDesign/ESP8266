/*
 * sdc-hercules.h
 *
 *  Created on: 29-Jul-2021
 *      Author: reliance
 */

#ifndef DRIVER_FATFS_PORT_SDC_HERCULES_H_
#define DRIVER_FATFS_PORT_SDC_HERCULES_H_


void mmcSelectSpi(gioPORT_t *port, spiBASE_t *reg);
void disk_timerproc (void);
#endif /* DRIVER_FATFS_PORT_SDC_HERCULES_H_ */
