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
#include <cbmpc/api/pve_base_pke.h>
#include <cbmpc/api/pve_batch_ac.h>
#include <cbmpc/api/pve_batch_single_recipient.h>
#include <cbmpc/api/schnorr_2p.h>
#include <cbmpc/api/schnorr_mp.h>
#include <cbmpc/api/tdh2.h>
#include <cbmpc/c_api/access_structure.h>
#include <cbmpc/c_api/cmem.h>
#include <cbmpc/c_api/common.h>
#include <cbmpc/c_api/ecdsa_2p.h>
#include <cbmpc/c_api/ecdsa_mp.h>
#include <cbmpc/c_api/eddsa_2p.h>
#include <cbmpc/c_api/eddsa_mp.h>
#include <cbmpc/c_api/job.h>
#include <cbmpc/c_api/pve_base_pke.h>
#include <cbmpc/c_api/pve_batch_ac.h>
#include <cbmpc/c_api/pve_batch_single_recipient.h>
#include <cbmpc/c_api/schnorr_2p.h>
#include <cbmpc/c_api/schnorr_mp.h>
#include <cbmpc/c_api/tdh2.h>
#include <cbmpc/core/access_structure.h>
#include <cbmpc/core/bip32_path.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/buf128.h>
#include <cbmpc/core/buf256.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/job.h>
#include <cbmpc/core/macros.h>
#include <cbmpc/core/precompiled.h>
