package testutil

import (
	"os"
	"syscall"
	"testing"
)

// WithSilencedStderr redirects the process' stderr to /dev/null while fn runs.
func WithSilencedStderr(fn func()) {
	savedFD, err := syscall.Dup(2)
	if err != nil {
		fn()
		return
	}
	devnull, err := os.OpenFile(os.DevNull, os.O_WRONLY, 0)
	if err != nil {
		_ = syscall.Close(savedFD)
		fn()
		return
	}
	defer devnull.Close()
	_ = syscall.Dup2(int(devnull.Fd()), 2)
	// Ensure stderr is restored even if fn panics
	defer func() {
		_ = syscall.Dup2(savedFD, 2)
		_ = syscall.Close(savedFD)
	}()
	fn()
}

// TSilence wraps a test section and silences stderr for its duration.
func TSilence(t *testing.T, fn func(t *testing.T)) {
	t.Helper()
	WithSilencedStderr(func() { fn(t) })
}
