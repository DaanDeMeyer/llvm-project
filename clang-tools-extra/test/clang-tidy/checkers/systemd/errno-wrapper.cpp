// RUN: %check_clang_tidy %s systemd-errno-wrapper %t

// Mock the ERRNO_IS_XXX macros as functions for testing purposes
// In real systemd code, these are macros that expand to inline functions
int ERRNO_IS_TRANSIENT(int r);
int ERRNO_IS_DISCONNECT(int r);
int ERRNO_IS_ACCEPT_AGAIN(int r);
int ERRNO_IS_RESOURCE(int r);
int ERRNO_IS_NOT_SUPPORTED(int r);
int ERRNO_IS_PRIVILEGE(int r);
int ERRNO_IS_DISK_SPACE(int r);
int ERRNO_IS_DEVICE_ABSENT(int r);
int ERRNO_IS_XATTR_ABSENT(int r);

void test_basic_patterns() {
  int r = 0;

  // CHECK-MESSAGES: :[[@LINE+2]]:7: warning: use ERRNO_IS_NEG_TRANSIENT instead of combining ERRNO_IS_TRANSIENT with a negative check
  // CHECK-FIXES: if (ERRNO_IS_NEG_TRANSIENT(r))
  if (r < 0 && ERRNO_IS_TRANSIENT(r))
    return;

  // CHECK-MESSAGES: :[[@LINE+2]]:7: warning: use ERRNO_IS_NEG_DISCONNECT instead of combining ERRNO_IS_DISCONNECT with a negative check
  // CHECK-FIXES: if (ERRNO_IS_NEG_DISCONNECT(r))
  if (r < 0 && ERRNO_IS_DISCONNECT(r))
    return;

  // CHECK-MESSAGES: :[[@LINE+2]]:7: warning: use ERRNO_IS_NEG_ACCEPT_AGAIN instead of combining ERRNO_IS_ACCEPT_AGAIN with a negative check
  // CHECK-FIXES: if (ERRNO_IS_NEG_ACCEPT_AGAIN(r))
  if (r < 0 && ERRNO_IS_ACCEPT_AGAIN(r))
    return;

  // CHECK-MESSAGES: :[[@LINE+2]]:7: warning: use ERRNO_IS_NEG_RESOURCE instead of combining ERRNO_IS_RESOURCE with a negative check
  // CHECK-FIXES: if (ERRNO_IS_NEG_RESOURCE(r))
  if (r < 0 && ERRNO_IS_RESOURCE(r))
    return;

  // CHECK-MESSAGES: :[[@LINE+2]]:7: warning: use ERRNO_IS_NEG_NOT_SUPPORTED instead of combining ERRNO_IS_NOT_SUPPORTED with a negative check
  // CHECK-FIXES: if (ERRNO_IS_NEG_NOT_SUPPORTED(r))
  if (r < 0 && ERRNO_IS_NOT_SUPPORTED(r))
    return;

  // CHECK-MESSAGES: :[[@LINE+2]]:7: warning: use ERRNO_IS_NEG_PRIVILEGE instead of combining ERRNO_IS_PRIVILEGE with a negative check
  // CHECK-FIXES: if (ERRNO_IS_NEG_PRIVILEGE(r))
  if (r < 0 && ERRNO_IS_PRIVILEGE(r))
    return;

  // CHECK-MESSAGES: :[[@LINE+2]]:7: warning: use ERRNO_IS_NEG_DISK_SPACE instead of combining ERRNO_IS_DISK_SPACE with a negative check
  // CHECK-FIXES: if (ERRNO_IS_NEG_DISK_SPACE(r))
  if (r < 0 && ERRNO_IS_DISK_SPACE(r))
    return;

  // CHECK-MESSAGES: :[[@LINE+2]]:7: warning: use ERRNO_IS_NEG_DEVICE_ABSENT instead of combining ERRNO_IS_DEVICE_ABSENT with a negative check
  // CHECK-FIXES: if (ERRNO_IS_NEG_DEVICE_ABSENT(r))
  if (r < 0 && ERRNO_IS_DEVICE_ABSENT(r))
    return;

  // CHECK-MESSAGES: :[[@LINE+2]]:7: warning: use ERRNO_IS_NEG_XATTR_ABSENT instead of combining ERRNO_IS_XATTR_ABSENT with a negative check
  // CHECK-FIXES: if (ERRNO_IS_NEG_XATTR_ABSENT(r))
  if (r < 0 && ERRNO_IS_XATTR_ABSENT(r))
    return;
}

void test_reversed_order() {
  int r = 0;

  // Test with the order reversed: ERRNO_IS_XXX(r) && r < 0
  // CHECK-MESSAGES: :[[@LINE+2]]:7: warning: use ERRNO_IS_NEG_TRANSIENT instead of combining ERRNO_IS_TRANSIENT with a negative check
  // CHECK-FIXES: if (ERRNO_IS_NEG_TRANSIENT(r))
  if (ERRNO_IS_TRANSIENT(r) && r < 0)
    return;
}

void test_with_parens() {
  int r = 0;

  // Test with parentheses around the whole expression
  // CHECK-MESSAGES: :[[@LINE+2]]:8: warning: use ERRNO_IS_NEG_TRANSIENT instead of combining ERRNO_IS_TRANSIENT with a negative check
  // CHECK-FIXES: if (ERRNO_IS_NEG_TRANSIENT(r))
  if ((r < 0 && ERRNO_IS_TRANSIENT(r)))
    return;
}

void test_no_match() {
  int r = 0;
  int s = 0;

  // Should not match: different variables
  if (r < 0 && ERRNO_IS_TRANSIENT(s))
    return;

  // Should not match: wrong comparison
  if (r > 0 && ERRNO_IS_TRANSIENT(r))
    return;

  // Should not match: wrong value
  if (r < 1 && ERRNO_IS_TRANSIENT(r))
    return;
}
