#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <pciscan.h>


EFI_STATUS
EFIAPI
PCIe_aspm (
  IN UINT32 bus,
  IN UINT32 dev, 
  IN UINT32 fun
  )
{
  UINT8 capid, addr;
  if (PCIRead32 (bus, dev, fun, 0x00) != 0xffffffff) {
  	addr = PCIRead8 (bus, dev, fun, 0x34);
  	while (addr != 0) {
  		capid = PCIRead8 (bus, dev, fun, addr);
  		addr = PCIRead8 (bus, dev, fun, addr + 1);
  		if (capid == 0x10) {
  			Print(L"This PCIe device support ASPM\n");
  			break;
  		}
  	}
  } else {
  	Print (L" the device is not exist !\n");
  }
  return EFI_SUCCESS;
}

EFI_STATUS UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINT32 bus, dev, fun;
  UINT8 off;
  for (bus = 0;bus < 256;bus++) {
  	for (dev = 0;dev < 32;dev++) {
  		if (PCIRead32 (bus, dev, 0x00, 0x00)== 0xffffffff)//function最低为0
  			continue;
  		if ((PCIRead8 (bus, dev, 0x00,0x0E) & 0x80) == 0) { //单功能设备
  			Print(L"DEVICE--bus : %x, dev : %x, fun : 0x00\n",bus, dev);
  			continue;
  		}
  		for (fun = 0;fun < 8;fun++) {
  			if (PCIRead32 (bus, dev, fun, 0x00)!=0xffffffff) {
  				Print(L"DEVICE--bus : %x, dev : %x, fun : %x\n",bus, dev, fun);
  			}
  		}
  	}
  }

  Print (L"bus: 0,device: 1,function: 0  Configuration information is :\n");
  for (off = 0; off <= 0x3c; off += 4) {
  	Print (L"%08x  ", PCIRead32(0x00, 0x01,0x00,off));
  	if (off % 0x08 == 4)
  		Print(L"\n");
  }
  Config_Information (0, 1, 0);
  PCIe_aspm (0, 1, 0);
  return EFI_SUCCESS;
}