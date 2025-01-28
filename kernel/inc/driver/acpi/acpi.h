#ifndef __DRIVER_ACPI
#define __DRIVER_ACPI

#include <types.h>

typedef struct {
    char     signature[8];
	uint8_t  checksum;
	char     oemid[6];
	uint8_t  revision;
	uint32_t rsdt_address;
} __attribute__((packed)) rsdp_descriptor_t;

typedef struct {
    rsdp_descriptor_t base;
    uint32_t length;
	uint64_t xsdt_address;
	uint8_t  ext_checksum;
	uint8_t  reserved[3];
} __attribute__((packed)) rsdp2_descriptor_t;

typedef struct {
	char     signature[4];
	uint32_t length;
	uint8_t  revision;
	uint8_t  checksum;
	char     oemid[6];
	char     oem_tableid[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} __attribute__((packed)) acpi_sdt_header_t;

typedef struct {
    acpi_sdt_header_t sdt_header;
    uint32_t sdt_addresses[];
} __attribute__((packed)) rsdt_t;

typedef struct {
    acpi_sdt_header_t sdt_header;
    uint64_t sdt_addresses[];
} __attribute__((packed)) xsdt_t;


static inline bool acpi_checksum(uint8_t *bytes, uint64_t size) {
    uint32_t sum = 0;
    uint64_t i;

    for (i = 0; i < size; ++i) 
        sum += bytes[i];

    return (sum & 0xFF) == 0;
}

#endif /* ifndef __DRIVER_ACPI */
