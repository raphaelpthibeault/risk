#ifndef ELF_H
#define ELF_H

#include <stdint.h>

/* see https://uclibc.org/docs/elf-64-gen.pdf */

/* Table 1. Elf-64 Data Types 
 * */
typedef uint64_t Elf64_Addr;	// Unsigned program address
typedef uint64_t Elf64_Off;	// Unsigned file offset
typedef uint16_t Elf64_Half;	// Unsigned medium integer
typedef uint32_t Elf64_Word;    // Unsigned integer
typedef int32_t  Elf64_Sword;	// Signed integer
typedef uint64_t Elf64_Xword;	// Unsigned long integer
typedef int64_t  Elf64_Sxword;	// Signed long integer 


/* Table 2. Elf Identification
 * Values for e_ident[EI_MAGn]
 * */
#define ELFMAG0   0x7f
#define ELFMAG1   'E'
#define ELFMAG2   'L'
#define ELFMAG3   'F'

/* Figure 2. ELf-64 Header */
typedef struct {
    unsigned char e_ident[16];	/* ELF identification */
    Elf64_Half e_type;		/* Object file type */
    Elf64_Half e_machine;	/* Machine type */
    Elf64_Word e_version;	/* Object file version */
    Elf64_Addr e_entry;		/* Entry point address */
    Elf64_Off  e_phoff;		/* Program header offset */
    Elf64_Off  e_shoff;		/* Section header offset */
    Elf64_Word e_flags;		/* Processor-specific flags */
    Elf64_Half e_ehsize;	/* ELF header size */
    Elf64_Half e_phentsize;	/* Size of program header entry */
    Elf64_Half e_phnum;		/* Number of program header entries */
    Elf64_Half e_shentsize;	/* Size of section header entry */
    Elf64_Half e_shnum;		/* Number of section header entries */
    Elf64_Half e_shstrndx;	/* Section name string table index */
} Elf64_Ehdr;

/*  --- RELOCATIONS  --- */

#define ELF64_R_SYM(i)    ((i) >> 32)
#define ELF64_R_TYPE(i)   ((i) & 0xFFFFFFFFL)
#define ELF64_R_INFO(s,t) (((s) << 32) + ((t) & 0xFFFFFFFFL))

/* Figure 5. Elf Relocations */
typedef struct {
    Elf64_Addr  r_offset; /* Address of reference */
    Elf64_Xword r_info;	  /* Symbol index and type of relocation */
} Elf64_Rel;

typedef struct {
    Elf64_Addr   r_offset;  /* Address of reference */
    Elf64_Xword  r_info;    /* Symbol index and type of relocation */
    Elf64_Sxword r_addend;  /* Constant part of expression */
} Elf64_Rela;

/* x86-64 SysV Relocation types
 * */
#define R_X86_64_NONE             0  
#define R_X86_64_64               1  
#define R_X86_64_PC32             2  
#define R_X86_64_GOT32            3  
#define R_X86_64_PLT32            4  
#define R_X86_64_COPY             5  
#define R_X86_64_GLOB_DAT         6  
#define R_X86_64_JUMP_SLOT        7  
#define R_X86_64_RELATIVE         8  
#define R_X86_64_GOTPCREL         9  
#define R_X86_64_32               10 
#define R_X86_64_32S              11 
/* vvv These should not appear in a valid file */
#define R_X86_64_16               12 
#define R_X86_64_PC16             13 
#define R_X86_64_8                14 
#define R_X86_64_PC8              15 
/* ^^^ These should not appear in a valid file */
#define R_X86_64_DTPMOD64         16 
#define R_X86_64_DTPOFF64         17 
#define R_X86_64_TPOFF64          18 
#define R_X86_64_TLSGD            19 
#define R_X86_64_TLSLD            20 
#define R_X86_64_DTPOFF32         21 
#define R_X86_64_GOTTPOFF         22 
#define R_X86_64_TPOFF32          23 
#define R_X86_64_PC64             24 
#define R_X86_64_GOTOFF64         25 
#define R_X86_64_GOTPC32          26 
/* Large model */
#define R_X86_64_GOT64            27 
#define R_X86_64_GOTPCREL64       28 
#define R_X86_64_GOTPC64          29 
#define R_X86_64_GOTPLT64         30 
#define R_X86_64_PLTOFF64         31 
/* ... */
#define R_X86_64_SIZE32           32 
#define R_X86_64_SIZE64           33 
#define R_X86_64_GOTPC32_TLSDESC  34 
#define R_X86_64_TLSDESC_CALL     35 
#define R_X86_64_TLSDESC          36 
#define R_X86_64_IRELATIVE        37 


#endif /* ifndef ELF_H */
