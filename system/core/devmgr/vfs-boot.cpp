// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <ddk/device.h>
#include <fs/vfs.h>
#include <mxio/debug.h>
#include <mxio/io.h>
#include <mxio/vfs.h>
#include <mxtl/ref_ptr.h>
#include <mxtl/unique_ptr.h>

#include "dnode.h"
#include "memfs-private.h"

#define MXDEBUG 0

namespace memfs {

static mx_status_t add_file(mxtl::RefPtr<VnodeDir> vnb, const char* path, mx_handle_t vmo,
                            mx_off_t off, size_t len) {
    mx_status_t r;
    if ((path[0] == '/') || (path[0] == 0))
        return ERR_INVALID_ARGS;
    for (;;) {
        const char* nextpath = strchr(path, '/');
        if (nextpath == nullptr) {
            if (path[0] == 0) {
                return ERR_INVALID_ARGS;
            }
            return vnb->CreateFromVmo(path, strlen(path), vmo, off, len);
        } else {
            if (nextpath == path) {
                return ERR_INVALID_ARGS;
            }

            mxtl::RefPtr<fs::Vnode> out;
            r = vnb->Lookup(&out, path, nextpath - path);
            if (r == ERR_NOT_FOUND) {
                r = vnb->Create(&out, path, nextpath - path, S_IFDIR);
            }

            if (r < 0) {
                return r;
            }
            vnb = mxtl::RefPtr<VnodeDir>::Downcast(mxtl::move(out));
            path = nextpath + 1;
        }
    }
}

} // namespace memfs

// The following functions exist outside the memfs namespace so they can
// be exposed to C:

mx_status_t bootfs_add_file(const char* path, mx_handle_t vmo, mx_off_t off, size_t len) {
    return add_file(BootfsRoot(), path, vmo, off, len);
}

mx_status_t systemfs_add_file(const char* path, mx_handle_t vmo, mx_off_t off, size_t len) {
    return add_file(SystemfsRoot(), path, vmo, off, len);
}
