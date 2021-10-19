#ifndef _UEFIMAIN_PCISCAN_PROTOCOL_H_
#define _UEFIMAIN_PCISCAN_PROTOCOL_H_

#include <Uefi.h>
#define Config_Addr 0xcf8
#define Config_Data 0xcfc
#define PCI_ADDR(bus, dev, fun, off)  (0x80000000 + ((bus<<16) + (dev<<11) + (fun<<8) + (off & 0xfc)))
UINT32
EFIAPI
PCIRead32 (
  IN UINT32 bus,
  IN UINT32 dev, 
  IN UINT32 fun, 
  IN UINT8 off
  )
{
  UINT32 IoData;
  UINT32 TempData;
  IoData = IoRead32 (Config_Addr);
  IoWrite32 (Config_Addr, PCI_ADDR(bus, dev, fun, off));
  TempData = IoRead32 (Config_Data);
  IoWrite32 (Config_Addr,IoData);
  return TempData;
}

UINT16
EFIAPI
PCIRead16 (
  IN UINT32 bus,
  IN UINT32 dev, 
  IN UINT32 fun, 
  IN UINT8 off
  )
{
  UINT32 IoData;
  UINT16 TempData;
  IoData = IoRead32 (Config_Addr);
  IoWrite32 (Config_Addr, PCI_ADDR(bus, dev, fun, off));
  TempData = IoRead16 (Config_Data + (UINT16)(off & 2));
  IoWrite32 (Config_Addr,IoData);
  return TempData;
}

UINT8
EFIAPI
PCIRead8 (
  IN UINT32 bus,
  IN UINT32 dev, 
  IN UINT32 fun, 
  IN UINT8 off
  )
{
  UINT32 IoData;
  UINT8 TempData;
  IoData = IoRead32 (Config_Addr);
  IoWrite32 (Config_Addr, PCI_ADDR(bus, dev, fun, off));
  TempData = IoRead8 (Config_Data + (UINT16)(off & 3));
  IoWrite32 (Config_Addr,IoData);
  return TempData;
}

EFI_STATUS
EFIAPI
Config_Information (
  IN UINT32 bus,
  IN UINT32 dev, 
  IN UINT32 fun 
  )
{
  UINT32 data;
  Print(L"bus: %x, device: %x, function: %x\n", bus, dev, fun);
  Print(L"Configuration information is :\n");
  Print(L"  Vendor ID  is:%04x", PCIRead16(bus, dev, fun, 0x00));
  Print(L"  Device ID  is:%04x\n", PCIRead16(bus, dev, fun, 0x02));
  Print(L"  Command    is:%04x", PCIRead16(bus, dev, fun, 0x04));
  Print(L"   Status    is:%04x\n", PCIRead16(bus, dev, fun, 0x06));
  Print(L"Revision ID  is:%02x", PCIRead8(bus, dev, fun, 0x08));
  data = PCIRead32(bus, dev, fun, 0x08) & 0xffffff00;
  Print(L" Class Code  is:%06x\n", data>>8);
  Print(L" Cache Line  is:%02x", PCIRead8(bus, dev, fun, 0x0c));
  Print(L"LatencyTimer is:%02x\n", PCIRead8(bus, dev, fun, 0x0d));
  Print(L" HeaderType  is:%02x", PCIRead8(bus, dev, fun, 0x0e));
  Print(L"    BIST     is:%02x\n", PCIRead8(bus, dev, fun,0x0f));
  Print(L"    BAR0     is:%08x\n", PCIRead32(bus, dev, fun, 0x10));
  Print(L"    BAR1     is:%08x\n", PCIRead32(bus, dev, fun, 0x14));
  Print(L"    BAR2     is:%08x\n", PCIRead32(bus, dev, fun, 0x18));
  Print(L"    BAR3     is:%08x\n", PCIRead32(bus, dev, fun, 0x1c));
  Print(L"    BAR4     is:%08x\n", PCIRead32(bus, dev, fun, 0x20));
  Print(L"    BAR5     is:%08x\n", PCIRead32(bus, dev, fun, 0x24));
  Print(L" Cardbus CIS is:%08x\n", PCIRead32(bus, dev, fun, 0x28));
  Print(L" SubVendorID is:%04x", PCIRead16(bus, dev, fun, 0x2c));
  Print(L" SubDeviceID is:%04x\n", PCIRead16(bus, dev, fun, 0x2e));
  Print(L" Expanslon Rom Base Address is:%08x\n", PCIRead32(bus, dev, fun, 0x30));
  Print(L" Interrupt Line  is:%02x", PCIRead8(bus, dev, fun, 0x3c));
  Print(L" Interrupt Pin   is:%02x\n", PCIRead8(bus, dev, fun, 0x3d));
  Print(L"     Min_Gnt     is:%02x", PCIRead8(bus, dev, fun, 0x3e));
  Print(L"     Max_Lat     is:%02x\n", PCIRead8(bus, dev, fun, 0x3f));
  return EFI_SUCCESS;
}
#endif