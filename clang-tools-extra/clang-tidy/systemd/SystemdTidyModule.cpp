//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "../ClangTidy.h"
#include "../ClangTidyModule.h"
#include "../ClangTidyModuleRegistry.h"
#include "ErrnoWrapperCheck.h"

namespace clang::tidy {
namespace systemd {

class SystemdModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<ErrnoWrapperCheck>("systemd-errno-wrapper");
  }
};

// Register the SystemdModule using this statically initialized variable.
static ClangTidyModuleRegistry::Add<SystemdModule>
    X("systemd-module", "Add systemd project checks.");

} // namespace systemd

// This anchor is used to force the linker to link in the generated object file
// and thus register the SystemdModule.
volatile int SystemdModuleAnchorSource = 0; // NOLINT(misc-use-internal-linkage)

} // namespace clang::tidy
