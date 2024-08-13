#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *sys_table) {
    // efi functions
    InitializeLib(image_handle, sys_table);
    Print(L"The bootloader says hello\r\n");

    return EFI_ABORTED;
}

