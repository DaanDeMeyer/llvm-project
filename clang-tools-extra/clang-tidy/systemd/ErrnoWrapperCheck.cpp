//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ErrnoWrapperCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang::tidy::systemd {

// List of ERRNO_IS_XXX macros that have ERRNO_IS_NEG_XXX counterparts
static constexpr llvm::StringLiteral ErrnoMacros[] = {
    "ERRNO_IS_TRANSIENT",     "ERRNO_IS_DISCONNECT",   "ERRNO_IS_ACCEPT_AGAIN",
    "ERRNO_IS_RESOURCE",      "ERRNO_IS_NOT_SUPPORTED", "ERRNO_IS_PRIVILEGE",
    "ERRNO_IS_DISK_SPACE",    "ERRNO_IS_DEVICE_ABSENT", "ERRNO_IS_XATTR_ABSENT",
};

void ErrnoWrapperCheck::registerMatchers(MatchFinder *Finder) {
  // Match patterns like: (r < 0 && ERRNO_IS_XXX(r))
  // or: (ERRNO_IS_XXX(r) && r < 0)
  //
  // We look for:
  // - A binary operator with && 
  // - One side is a comparison: expr < 0
  // - The other side is a call to one of the ERRNO_IS_XXX macros
  // - Both sides reference the same expression

  // Match: expr < 0
  const auto LessThanZero = binaryOperator(
      hasOperatorName("<"),
      hasRHS(ignoringParenImpCasts(integerLiteral(equals(0)))));

  // Build a matcher for any of the ERRNO_IS_XXX function calls
  const auto ErrnoCall = callExpr(callee(functionDecl(hasAnyName(
      "ERRNO_IS_TRANSIENT", "ERRNO_IS_DISCONNECT", "ERRNO_IS_ACCEPT_AGAIN",
      "ERRNO_IS_RESOURCE", "ERRNO_IS_NOT_SUPPORTED", "ERRNO_IS_PRIVILEGE",
      "ERRNO_IS_DISK_SPACE", "ERRNO_IS_DEVICE_ABSENT", "ERRNO_IS_XATTR_ABSENT"))));

  // Match the overall pattern: (r < 0 && ERRNO_IS_XXX(r))
  // The expr in "r < 0" should be the same as the argument to ERRNO_IS_XXX
  Finder->addMatcher(
      binaryOperator(
          hasOperatorName("&&"),
          anyOf(
              // r < 0 && ERRNO_IS_XXX(r)
              allOf(hasLHS(LessThanZero.bind("comparison")),
                    hasRHS(ignoringParenImpCasts(ErrnoCall.bind("errnoCall")))),
              // ERRNO_IS_XXX(r) && r < 0
              allOf(hasRHS(LessThanZero.bind("comparison")),
                    hasLHS(ignoringParenImpCasts(ErrnoCall.bind("errnoCall"))))))
          .bind("binaryOp"),
      this);
}

void ErrnoWrapperCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *BinaryOp = Result.Nodes.getNodeAs<BinaryOperator>("binaryOp");
  const auto *Comparison = Result.Nodes.getNodeAs<BinaryOperator>("comparison");
  const auto *ErrnoCall = Result.Nodes.getNodeAs<CallExpr>("errnoCall");

  if (!BinaryOp || !Comparison || !ErrnoCall)
    return;

  // Get the expression being compared (the LHS of "expr < 0")
  const Expr *ComparedExpr = Comparison->getLHS()->IgnoreParenImpCasts();

  // Get the argument to the ERRNO_IS_XXX call
  if (ErrnoCall->getNumArgs() != 1)
    return;
  const Expr *ErrnoArg = ErrnoCall->getArg(0)->IgnoreParenImpCasts();

  // Check if both expressions are equivalent (same variable/expression)
  // We use a simple heuristic: compare the source text
  const SourceManager &SM = *Result.SourceManager;
  const LangOptions &LangOpts = Result.Context->getLangOpts();

  StringRef ComparedText = Lexer::getSourceText(
      CharSourceRange::getTokenRange(ComparedExpr->getSourceRange()), SM,
      LangOpts);
  StringRef ErrnoArgText = Lexer::getSourceText(
      CharSourceRange::getTokenRange(ErrnoArg->getSourceRange()), SM, LangOpts);

  if (ComparedText != ErrnoArgText)
    return;

  // Get the function name
  const FunctionDecl *FD = ErrnoCall->getDirectCallee();
  if (!FD)
    return;
  
  StringRef FuncName = FD->getName();
  
  // Build the replacement: ERRNO_IS_XXX -> ERRNO_IS_NEG_XXX
  std::string Replacement;
  if (FuncName.starts_with("ERRNO_IS_")) {
    StringRef Suffix = FuncName.drop_front(9); // Drop "ERRNO_IS_"
    Replacement = ("ERRNO_IS_NEG_" + Suffix + "(" + ErrnoArgText + ")").str();
  } else {
    return;
  }

  // Get the full source range of the binary operation
  // We need to handle the case where the expression might be wrapped in parens
  SourceRange ReplacementRange = BinaryOp->getSourceRange();

  // Check if the parent is a ParenExpr and include it in the replacement
  const auto &Parents = Result.Context->getParents(*BinaryOp);
  if (!Parents.empty()) {
    if (const auto *Paren = Parents[0].get<ParenExpr>()) {
      ReplacementRange = Paren->getSourceRange();
    }
  }

  diag(BinaryOp->getBeginLoc(),
       "use %0 instead of combining %1 with a negative check")
      << ("ERRNO_IS_NEG_" + FuncName.drop_front(9)).str() << FuncName
      << FixItHint::CreateReplacement(
             CharSourceRange::getTokenRange(ReplacementRange), Replacement);
}

} // namespace clang::tidy::systemd
