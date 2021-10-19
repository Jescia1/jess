#include <Uefi.h>
#include <Library/UefiLib.h>
#include <IndustryStandard/SmBios.h>
#include <Library/UefiShellDebug1CommandsLib.h>
#include <Library/LibSmbiosView.h>

STATIC SMBIOS_STRUCTURE_POINTER     m_SmbiosStruct;
STATIC SMBIOS_STRUCTURE_POINTER     *mSmbiosStruct = &m_SmbiosStruct;
STATIC SMBIOS_TABLE_ENTRY_POINT     *Smbios_eps  = NULL;

CHAR8*
GetSmbiosString (
  IN  SMBIOS_STRUCTURE_POINTER    *Smbios,
  IN  UINT16                      StringNumber
  )
{
  UINT16  Index;
  CHAR8   *String;
  String = (CHAR8 *) (Smbios->Raw + Smbios->Hdr->Length);

  for (Index = 1; Index <= StringNumber; Index++) {
    if (StringNumber == Index) {
      return String;
    }

    for (; *String != 0; String++);
    String++;

    if (*String == 0) {
      Smbios->Raw = (UINT8 *)++String;
      return NULL;
    }
  }
  return NULL;
}

EFI_STATUS
GetSmbiosStructure (
  IN  OUT UINT16  *Handle,
  OUT UINT8       **Buffer,
  OUT UINT16      *Length
  )
{
  SMBIOS_STRUCTURE_POINTER  Smbios;
  SMBIOS_STRUCTURE_POINTER  SmbiosEnd;
  UINT8                     *Raw;

  if (*Handle == INVALID_HANDLE) {
    *Handle = mSmbiosStruct->Hdr->Handle;
    return DMI_INVALID_HANDLE;
  }

  if ((Buffer == NULL) || (Length == NULL)) {
    return DMI_INVALID_HANDLE;
  }

  *Length       = 0;
  Smbios.Hdr    = mSmbiosStruct->Hdr;
  SmbiosEnd.Raw = Smbios.Raw + Smbios_eps->TableLength;
  while (Smbios.Raw < SmbiosEnd.Raw) {
    if (Smbios.Hdr->Handle == *Handle) {

      Raw = Smbios.Raw;
      GetSmbiosString (&Smbios, (UINT16) (-1));
      *Length = (UINT16) (Smbios.Raw - Raw);
      *Buffer = Raw;

      if (Smbios.Raw < SmbiosEnd.Raw) {
        *Handle = Smbios.Hdr->Handle;
      } else {
        *Handle = INVALID_HANDLE;
      }
      return DMI_SUCCESS;
    }
    GetSmbiosString (&Smbios, (UINT16) (-1));
  }
  *Handle = INVALID_HANDLE;
  return DMI_INVALID_HANDLE;
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE              ImageHandle,
  IN EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                Status;
  SMBIOS_STRUCTURE_POINTER  SmBiosStruct;
  UINT16                    Index;
  UINT16                    Handle;
  UINT8                     *Buffer;
  UINT16                    Length;
  UINT8                     Type;

  Status = EfiGetSystemConfigurationTable (&gEfiSmbiosTableGuid, (VOID**)&Smbios_eps);
  if (EFI_ERROR(Status) || Smbios_eps == NULL) {
    Print (L" Get Smbios EPS failed!\n");
    return Status;
  }
  Print (L"SmBios Table:\n");
  Print (L"  AnchorString:%c%c%c%c\n",
          Smbios_eps->AnchorString[0],
          Smbios_eps->AnchorString[1],
          Smbios_eps->AnchorString[2],
          Smbios_eps->AnchorString[3]
          );
  Print (L"  EntryPointStructureChecksum:0x%02x\n  EntryPointLength:0x%02x\n  MajorVersion:0x%02x\n  MinorVersion:0x%02x\n",
          Smbios_eps->IntermediateChecksum,
          Smbios_eps->EntryPointLength,
          Smbios_eps->MajorVersion,
          Smbios_eps->MinorVersion
          );
  Print (L"  IntermediateAnchorString:%c%c%c%c%c\n",
          Smbios_eps->IntermediateAnchorString[0],
          Smbios_eps->IntermediateAnchorString[1],
          Smbios_eps->IntermediateAnchorString[2],
          Smbios_eps->IntermediateAnchorString[3],
          Smbios_eps->IntermediateAnchorString[4]
          );
  Print (L"  TableLength:0x%04x\n  TableAddress:0x%08x\n  NumberOfSmbiosStructures:0x%04x\n",
          Smbios_eps->TableLength,
          Smbios_eps->TableAddress,
          Smbios_eps->NumberOfSmbiosStructures
          );

  Print (L"Table Total:\n");

  mSmbiosStruct->Raw  = (UINT8 *) (UINTN) (Smbios_eps->TableAddress);
  Handle  = INVALID_HANDLE;
  GetSmbiosStructure (&Handle, NULL, NULL);
  for (Index = 0; Index < Smbios_eps->NumberOfSmbiosStructures; Index++) {
    if (Handle == INVALID_HANDLE) {
      break;
    }

    if (GetSmbiosStructure (&Handle, &Buffer, &Length) != DMI_SUCCESS) {
      break;
    }
    SmBiosStruct.Raw = Buffer;

    Type = SmBiosStruct.Hdr->Type;
    if (Type > 127 || (Type >44 && Type <126)) {
      continue;
    }
    Print (L"Handle:%04x Type:%04d Address:%08x Length:%04x\n",
      SmBiosStruct.Hdr->Handle,
      SmBiosStruct.Hdr->Type,
      Buffer,
      Length
      );
    DumpHex (2, 0, Length, Buffer);
  }
  return Status;
}


