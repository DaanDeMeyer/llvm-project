.. title:: clang-tidy - systemd-errno-wrapper

systemd-errno-wrapper
=====================

Detects patterns like ``(r < 0 && ERRNO_IS_XXX(r))`` and suggests replacing
them with ``ERRNO_IS_NEG_XXX(r)``.

This check is equivalent to the ``errno-wrapper.cocci`` Coccinelle semantic
patch used in the `systemd project <https://github.com/systemd/systemd>`_.

The systemd project defines a set of ``ERRNO_IS_XXX()`` macros that check if
an errno value matches a certain category (e.g., transient errors, disconnect
errors, etc.). These macros expect a positive errno value as input.

When checking if a return value is a negative errno of a specific type, code
often uses the pattern ``r < 0 && ERRNO_IS_XXX(r)``. However, systemd also
provides ``ERRNO_IS_NEG_XXX()`` macros that combine both checks into a single
call, making the code more concise and readable.

Example
-------

.. code-block:: c

   // Before
   if (r < 0 && ERRNO_IS_TRANSIENT(r))
       return r;

   // After
   if (ERRNO_IS_NEG_TRANSIENT(r))
       return r;

Supported Macros
----------------

This check supports the following macro pairs:

- ``ERRNO_IS_TRANSIENT`` → ``ERRNO_IS_NEG_TRANSIENT``
- ``ERRNO_IS_DISCONNECT`` → ``ERRNO_IS_NEG_DISCONNECT``
- ``ERRNO_IS_ACCEPT_AGAIN`` → ``ERRNO_IS_NEG_ACCEPT_AGAIN``
- ``ERRNO_IS_RESOURCE`` → ``ERRNO_IS_NEG_RESOURCE``
- ``ERRNO_IS_NOT_SUPPORTED`` → ``ERRNO_IS_NEG_NOT_SUPPORTED``
- ``ERRNO_IS_PRIVILEGE`` → ``ERRNO_IS_NEG_PRIVILEGE``
- ``ERRNO_IS_DISK_SPACE`` → ``ERRNO_IS_NEG_DISK_SPACE``
- ``ERRNO_IS_DEVICE_ABSENT`` → ``ERRNO_IS_NEG_DEVICE_ABSENT``
- ``ERRNO_IS_XATTR_ABSENT`` → ``ERRNO_IS_NEG_XATTR_ABSENT``
