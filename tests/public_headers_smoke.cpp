// Compile-only smoke test: public headers must not depend on internal headers.
//
// This TU is built with include paths limited to:
//   - <repo>/include
//   - OpenSSL include dir
//
// If any public header includes <cbmpc/internal/...>, this will fail to compile.

#include <cbmpc/api/curve.h>
#include <cbmpc/api/ecdsa_2p.h>
#include <cbmpc/api/ecdsa_mp.h>
#include <cbmpc/api/eddsa_2p.h>
#include <cbmpc/api/eddsa_mp.h>
#include <cbmpc/api/hd_keyset_ecdsa_2p.h>
#include <cbmpc/api/hd_keyset_eddsa_2p.h>
#include <cbmpc/api/pve.h>
#include <cbmpc/api/pve_ac.h>
#include <cbmpc/api/pve_batch.h>
#include <cbmpc/api/schnorr_2p.h>
#include <cbmpc/api/schnorr_mp.h>
#include <cbmpc/api/tdh2.h>
#include <cbmpc/capi/access_structure.h>
#include <cbmpc/capi/cmem.h>
#include <cbmpc/capi/common.h>
#include <cbmpc/capi/ecdsa_2p.h>
#include <cbmpc/capi/ecdsa_mp.h>
#include <cbmpc/capi/eddsa_2p.h>
#include <cbmpc/capi/eddsa_mp.h>
#include <cbmpc/capi/job.h>
#include <cbmpc/capi/pve.h>
#include <cbmpc/capi/pve_ac.h>
#include <cbmpc/capi/pve_batch.h>
#include <cbmpc/capi/schnorr_2p.h>
#include <cbmpc/capi/schnorr_mp.h>
#include <cbmpc/capi/tdh2.h>
#include <cbmpc/core/access_structure.h>
#include <cbmpc/core/bip32_path.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/buf128.h>
#include <cbmpc/core/buf256.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/job.h>
#include <cbmpc/core/macros.h>
#include <cbmpc/core/precompiled.h>
