---
type: architecture
title: OTA Updates Architecture Decision
created: 2026-01-17
tags:
  - architecture
  - ota
  - updates
  - phase-06
related:
  - "[[Phase-06-WiFi-and-Network-Features]]"
---

# OTA Updates Architecture Decision

## Summary

**Decision**: Defer full OTA (Over-The-Air) update implementation to Phase 07. Phase 06 focuses on time synchronization only.

**Date**: 2026-01-17

**Status**: Approved for Phase 06

## Context

Phase 06 originally included consideration of OTA updates as part of the network features. After analysis, this has been split into two components:

1. **Time Synchronization** (Phase 06) - IMPLEMENTED
2. **OTA System Updates** (Phase 07) - DEFERRED

## Decision

### Phase 06: Time Synchronization Only

Implemented features:
- Chrony NTP client for network time synchronization
- Automatic time sync on WiFi connection
- Manual time sync trigger via init script
- Time sync status monitoring

Implementation details:
- Added `BR2_PACKAGE_CHRONY=y` to Buildroot configuration
- Created `src/ereader/network/time_sync.c` and `time_sync.h` modules
- Created `/etc/init.d/S41chronyd` init script
- Integrated with existing WiFi manager via network startup

### Phase 07: OTA Updates (Deferred)

OTA updates are deferred to Phase 07 for the following reasons:

1. **Complexity**: OTA updates require:
   - Secure boot verification
   - Dual partition system (A/B partitioning)
   - Rollback mechanism
   - Update signature verification
   - Network error handling and retry logic
   - Atomic update application

2. **Security Requirements**:
   - Update signing infrastructure
   - Certificate management
   - Secure update server
   - Man-in-the-middle attack prevention
   - Update integrity verification

3. **Storage Constraints**:
   - Current rootfs size: 256MB
   - Dual partition requirement would need 512MB+ for rootfs alone
   - Update staging area requirements
   - Need to analyze SD card partitioning strategy

4. **Development Priority**:
   - Phase 06 focuses on basic network connectivity
   - Time sync is essential for certificate validation
   - OTA can be added once core features are stable
   - Users can manually update via SD card removal

## Alternative Update Methods (Current Phase 06)

Users can update the system using these methods:

1. **Manual SD Card Update**:
   - Remove SD card from device
   - Flash new image using Balena Etcher or `dd`
   - Reinsert SD card
   - **Pros**: Simple, reliable, no network dependency
   - **Cons**: Requires physical access, downtime

2. **Buildroot Rebuild**:
   - Rebuild full system image
   - Flash to SD card
   - **Pros**: Clean state, full control
   - **Cons**: Developer-focused, not user-friendly

3. **Package Manager** (Future consideration):
   - Enable `opkg` or `apt` in Buildroot
   - Individual package updates
   - **Pros**: Granular updates
   - **Cons**: Dependency management complexity

## Future OTA Architecture (Phase 07 Planning)

When OTA is implemented in Phase 07, consider:

### A/B Partition Scheme
```
├── Boot Partition (FAT32, 128MB)
│   ├── bootcode.bin
│   ├── start.elf
│   ├── config.txt
│   └── kernel-*.img (switchable)
├── Root A (ext4, 512MB) - Active
├── Root B (ext4, 512MB) - Inactive/Update Target
└── Data Partition (ext4, remaining) - Persistent user data
```

### Update Process Flow
1. Download update package to `/tmp` or data partition
2. Verify update signature (GPG/RSA)
3. Extract and write to inactive rootfs partition
4. Verify written partition integrity
5. Update boot configuration to switch partitions
6. Reboot and validate new system
7. On failure: automatic rollback to previous partition

### Security Considerations
- Use HTTPS for update downloads (already have wget with SSL)
- Implement update package signing with GPG
- Store update server certificate in rootfs
- Verify update metadata before download
- Atomic update application (all-or-nothing)
- Rollback on boot failure (watchdog timer)

### Update Server Requirements
- Static file server (nginx/Apache)
- Update manifest JSON with version info
- Signed update packages (.tar.gz.sig)
- Incremental update support (future)

### Storage Requirements
- Boot: 128MB (unchanged)
- Root A: 512MB (double current 256MB)
- Root B: 512MB (for updates)
- Data: Remaining space (~6.5GB on 8GB SD card)

## Implementation Checklist (Phase 07)

When implementing OTA in Phase 07:

- [ ] Design dual partition layout
- [ ] Update genimage configuration for A/B partitions
- [ ] Implement boot partition switching logic
- [ ] Create update package signing infrastructure
- [ ] Build update download and verification module
- [ ] Implement atomic update application
- [ ] Add rollback mechanism with watchdog
- [ ] Create update server infrastructure
- [ ] Test update scenarios (success, failure, rollback)
- [ ] Document update process for end users
- [ ] Add update notification UI
- [ ] Implement background update checks

## Risks and Mitigation

### Risk: Bricked Device
- **Mitigation**: A/B partitioning ensures fallback partition
- **Mitigation**: Watchdog timer for automatic rollback
- **Mitigation**: Manual recovery via SD card re-flash

### Risk: Network Failure During Update
- **Mitigation**: Download-verify-apply workflow
- **Mitigation**: Resume capability for large updates
- **Mitigation**: Retry logic with exponential backoff

### Risk: Malicious Updates
- **Mitigation**: Cryptographic signature verification
- **Mitigation**: HTTPS-only update channel
- **Mitigation**: Update server certificate pinning

## References

- [Mender OTA Update System](https://mender.io/) - Industry standard for embedded Linux OTA
- [SWUpdate](https://sbabic.github.io/swupdate/) - Alternative OTA solution for embedded
- [The Update Framework (TUF)](https://theupdateframework.io/) - Secure update framework
- [Buildroot Manual - OTA Updates](https://buildroot.org/downloads/manual/manual.html#_advanced_usage)

## Conclusion

Phase 06 successfully implements time synchronization using chrony NTP client. Full OTA update capability is deferred to Phase 07 to allow proper security design, storage planning, and testing. Current manual update methods are sufficient for development and early deployment.
