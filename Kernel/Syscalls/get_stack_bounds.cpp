/*
 * Copyright (c) 2018-2021, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Panic.h>
#include <Kernel/Process.h>
#include <Kernel/VM/Region.h>

namespace Kernel {

KResultOr<int> Process::sys$get_stack_bounds(Userspace<FlatPtr*> user_stack_base, Userspace<size_t*> user_stack_size)
{
    auto& regs = Thread::current()->get_register_dump_from_stack();
    FlatPtr stack_pointer;
#if ARCH(I386)
    stack_pointer = regs.userspace_esp;
#else
    stack_pointer = regs.userspace_rsp;
#endif
    auto* stack_region = space().find_region_containing(Range { VirtualAddress(stack_pointer), 1 });

    // The syscall handler should have killed us if we had an invalid stack pointer.
    VERIFY(stack_region);

    FlatPtr stack_base = stack_region->range().base().get();
    size_t stack_size = stack_region->size();
    if (!copy_to_user(user_stack_base, &stack_base))
        return EFAULT;
    if (!copy_to_user(user_stack_size, &stack_size))
        return EFAULT;
    return 0;
}

}
