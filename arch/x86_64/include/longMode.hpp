////////////////////////////////////////////////////////////////
//
//	Long mode low-level preparation operations
//
//	File:	longMode.hpp
//	Date:	20 Nov. 2017
//
//	Copyright (c) 2017, Igor Baklykov
//	All rights reserved.
//
//


#ifndef IGROS_ARCH_LONG_MODE_HPP
#define IGROS_ARCH_LONG_MODE_HPP


#include <include/types.hpp>


// Arch-dependent code zone
namespace arch {


	// Set GDT
	extern "C" {

		// Check if loaded with multiboot loader
		byte_t	checkMultiboot();
		// Check CPUID is extended
		byte_t	checkCPUID();
		// Check CPUID for long mode existance
		byte_t	checkLongMode();

		// Setup page tables
		void	setupPageTables();
		// Enable paging
		void	enablePaging();

		// Perform jum to long mode
		void	jumpToLongMode();

	}


}	// namespace arch


#endif	// IGROS_ARCH_LONG_MODE_HPP

