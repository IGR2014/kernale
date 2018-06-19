////////////////////////////////////////////////////////////////
//
//	Memory paging for x86
//
//	File:	paging.cpp
//	Date:	19 Jun. 2018
//
//	Copyright (c) 2018, Igor Baklykov
//	All rights reserved.
//


#include <include/paging.hpp>


// Arch-dependent code zone
namespace arch {


	// Workaround becouse of unimplemented yet physical memory allocator
	extern "C" {


		// Page directory
		extern t_u32	pageDirectory[1024];
		// Page table
		extern t_u32	pageTable[1024];


	}	// extern "C"


	// Bitwise OR operator for flags
	PAGING_FLAGS operator|(PAGING_FLAGS l, PAGING_FLAGS r) {

		// Underlying type acquisition
		using U = std::underlying_type<PAGING_FLAGS>::type;
		// OR on underlying types
		return	static_cast<PAGING_FLAGS>(
			static_cast<U>(l) |
			static_cast<U>(r)
			);

	}

	// Bitwise AND operator for flags
	PAGING_FLAGS operator&(PAGING_FLAGS l, PAGING_FLAGS r) {

		// Underlying type acquisition
		using U = std::underlying_type<PAGING_FLAGS>::type;
		// AND on underlying types
		return	static_cast<PAGING_FLAGS>(
			static_cast<U>(l) &
			static_cast<U>(r)
			);

	}

	// Bitwise XOR operator for flags
	PAGING_FLAGS operator^(PAGING_FLAGS l, PAGING_FLAGS r) {

		// Underlying type acquisition
		using U = std::underlying_type<PAGING_FLAGS>::type;
		// XOR on underlying types
		return	static_cast<PAGING_FLAGS>(
			static_cast<U>(l) ^
			static_cast<U>(r)
			);

	}

	// Bitwise NOT operator for flags
	PAGING_FLAGS operator~(PAGING_FLAGS l) {

		// Underlying type acquisition
		using U = std::underlying_type<PAGING_FLAGS>::type;
		// NOT on underlying types
		return	static_cast<PAGING_FLAGS>(
			~static_cast<U>(l)
			);

	}

	// Bitwise OR assignment operator for flags
	PAGING_FLAGS operator|=(PAGING_FLAGS &l, PAGING_FLAGS r) {

		// Underlying type acquisition
		using U = std::underlying_type<PAGING_FLAGS>::type;
		// OR on underlying types
		l =	static_cast<PAGING_FLAGS>(
			static_cast<U>(l) |
			static_cast<U>(r)
			);

	}

	// Bitwise AND assignment operator for flags
	PAGING_FLAGS operator&=(PAGING_FLAGS &l, PAGING_FLAGS r) {

		// Underlying type acquisition
		using U = std::underlying_type<PAGING_FLAGS>::type;
		// AND on underlying types
		l =	static_cast<PAGING_FLAGS>(
			static_cast<U>(l) &
			static_cast<U>(r)
			);

	}

	// Bitwise XOR assignment operator for flags
	PAGING_FLAGS operator^=(PAGING_FLAGS &l, PAGING_FLAGS r) {

		// Underlying type acquisition
		using U = std::underlying_type<PAGING_FLAGS>::type;
		// XOR on underlying types
		l =	static_cast<PAGING_FLAGS>(
			static_cast<U>(l) ^
			static_cast<U>(r)
			);

	}


	// Set page directory flags
	void pagingSetPDFlags(t_ptr pageDirEntry, const PAGING_FLAGS flags) {

		// Page directory dirty flag does not exists
		// It's reserved as 0
		*static_cast<t_u32*>(pageDirEntry) |= (flags & ~(PAGE_DIRTY));
	
	}

	// Set page table flags
	void pagingSetPTFlags(t_ptr pageTableEntry, const PAGING_FLAGS flags) {

		// Page directory size flag does not exists
		// It's reserved as 0
		*static_cast<t_u32*>(pageTableEntry) |= (flags & ~(PAGE_SIZE));

	}


	// Setup paging
	void pagingSetup() {

		// Other page directories are unused
		for (t_u32 i = 0; i < 1024; ++i) {

			pageDirectory[i] = 0x00000000;
			pagingSetPDFlags(&pageDirectory[i], PAGE_CLEAR);

		}

		// Map first 4 MB of physical RAM to first 4 MB of virtual RAM
		pageDirectory[0]	= reinterpret_cast<t_u32>(pageTable);
		pagingSetPDFlags(&pageDirectory[0],	PAGE_WRITABLE | PAGE_PRESENT);

		// Also map first 4MB of physical RAM to first 4MB after 3GB in virtual memory
		// (This shoul be useful for higher-half kernel)
		pageDirectory[768]	= reinterpret_cast<t_u32>(pageTable);
		pagingSetPDFlags(&pageDirectory[768],	PAGE_WRITABLE | PAGE_PRESENT);

		// Map page table itself to the last page of virtual memory
		pageDirectory[1023]	= reinterpret_cast<t_u32>(pageDirectory);
		pagingSetPDFlags(&pageDirectory[1023],	PAGE_WRITABLE | PAGE_PRESENT);

		// Map all pages of first 4MB to first page table
		for (t_i32 j = 0; j < 1024; ++j) {

			pageTable[j]	= (j << 12);
			pagingSetPTFlags(&pageTable[j],	PAGE_WRITABLE | PAGE_PRESENT);

		}

		// Setup page directory
		arch::pagingSetupPD(pageDirectory);
		// Enable paging
		arch::pagingEnable();

	}


	// Convert virtual address to physical address
	t_ptr pagingVirtToPhys(t_ptr virtAddr) {

		// Page directory entry index from virtual address
		t_u32 pdEntryIndex	= reinterpret_cast<t_u32>(virtAddr) >> 22;
		// Page table entry index from virtual address
		t_u32 ptEntryIndex	= (reinterpret_cast<t_u32>(virtAddr) >> 12) & 0x3FF;

		// Physical pointer to page table
		t_u32 pageTablePtr	= pageDirectory[pdEntryIndex];

		// Check if page table is present or not
		if ((pageTablePtr & PAGE_PRESENT) != PAGE_CLEAR) {

			// Physical pointer to page
			t_u32 pagePtr = reinterpret_cast<t_u32p>(pageTablePtr & ~0xFFF)[ptEntryIndex];

			// Check if page is present or not
			if ((pagePtr & PAGE_PRESENT) != PAGE_CLEAR) {

				// Get physical address of page from page table (20 MSB)
				t_u32 physPageAddr	= pagePtr & ~0xFFF;
				// Get physical offset from virtual address`s (12 LSB)
				t_u32 physPageOffset	= reinterpret_cast<t_u32>(virtAddr) & 0xFFF;

				// Return physical address
				return reinterpret_cast<t_ptr>(physPageAddr | physPageOffset);

			}

		}

		// Page or table is not present
		return nullptr;

	}


}	// namespace arch
