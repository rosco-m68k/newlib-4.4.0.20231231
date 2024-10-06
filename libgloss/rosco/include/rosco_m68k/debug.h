/*
 *------------------------------------------------------------
 *                                  ___ ___ _   
 *  ___ ___ ___ ___ ___       _____|  _| . | |_ 
 * |  _| . |_ -|  _| . |     |     | . | . | '_|
 * |_| |___|___|___|___|_____|_|_|_|___|___|_,_| 
 *                     |_____|           stdlibs               
 * ------------------------------------------------------------
 * Copyright (c)2024 Ross Bamford
 * See top-level LICENSE.md for licence information.
 *
 * GDB support
 * ------------------------------------------------------------
 */

#ifndef __ROSCOM68K_LIBC_GDB_H__
#define __ROSCOM68K_LIBC_GDB_H__

/**
 * Execute a software breakpoint, and trap to the debugger.
 * 
 * If the debugger is not yet connected, this will pause until it
 * is, which allows you to wait for connection. 
 */
void breakpoint(void);

#endif//__ROSCOM68K_LIBC_GDB_H__


