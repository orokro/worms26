/*
	FileData.h
	----------

	Main header for FileData.c
*/

#ifndef FILEDATA_H
#define FILEDATA_H


// function prototypes

/**
 * @brief This will set all the defaults for the variables if no file was found to load
 */
extern void FileData_setGameDefaults();


/**
 * @brief Loads data from "wormsdat" or creates file if doesn't exit
 */
extern void FileData_loadData();


/**
 * @brief Saves our game match settings to a binary data file
 */
extern void FileData_saveData();

#endif
