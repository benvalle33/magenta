// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2016 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#pragma once

#include <magenta/compiler.h>
#include <arch/x86/mmu.h>
#include <kernel/spinlock.h>

__BEGIN_CDECLS

#define ARCH_ASPACE_MAGIC 0x41524153 // ARAS

struct arch_aspace {
    /* magic value for use-after-free detection */
    uint32_t magic;

    /* pointer to the translation table */
    paddr_t pt_phys;
    pt_entry_t *pt_virt;

    uint flags;

    /* range of address space */
    vaddr_t base;
    size_t size;

    /* cpus that are currently executing in this aspace
     * actually an mp_cpu_mask_t, but header dependencies. */
    volatile int active_cpus;

    /* Pointer to a bitmap::RleBitmap representing the range of ports
     * enabled in this aspace. */
    void *io_bitmap;
    spin_lock_t io_bitmap_lock;
};

__END_CDECLS

#if 0
// XXX move
#include <kernel/vm/arch_vm_aspace.h>

class X86ArchVmAspace final : public ArchVmAspace {
public:
    X86ArchVmAspace();
    virtual ~X86ArchVmAspace() final;

    DISALLOW_COPY_ASSIGN_AND_MOVE(X86ArchVmAspace);

    virtual status_t Init(vaddr_t base, size_t size, uint mmu_flags);
    virtual status_t Destroy();

    // main methods
    virtual status_t Map(vaddr_t vaddr, paddr_t paddr, size_t count, uint mmu_flags, size_t* mapped);
    virtual status_t Unmap(vaddr_t vaddr, size_t count, size_t* unmapped);
    virtual status_t Protect(vaddr_t vaddr, size_t count, uint mmu_flags);
    virtual status_t Query(vaddr_t vaddr, paddr_t* paddr, uint* mmu_flags);

private:
    arch_aspace data_ = {};
};

using _ArchVmAspace = X86ArchVmAspace;
#endif
