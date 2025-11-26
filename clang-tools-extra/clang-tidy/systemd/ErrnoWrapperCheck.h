//===--- ErrnoWrapperCheck.h - clang-tidy -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_SYSTEMD_ERRNOWRAPPERCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_SYSTEMD_ERRNOWRAPPERCHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::systemd {

/// Detects patterns like `(r < 0 && ERRNO_IS_XXX(r))` and suggests
/// replacing them with `ERRNO_IS_NEG_XXX(r)`.
///
/// This is equivalent to the errno-wrapper.cocci Coccinelle semantic patch
/// used in the systemd project.
///
/// For the user-facing documentation see:
/// https://clang.llvm.org/extra/clang-tidy/checks/systemd/errno-wrapper.html
class ErrnoWrapperCheck : public ClangTidyCheck {
public:
  ErrnoWrapperCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::systemd

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_SYSTEMD_ERRNOWRAPPERCHECK_H
